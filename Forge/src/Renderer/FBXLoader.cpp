#include "Fpch.h"
#include "FBXLoader.h"
#include "../Core/IAsset.h"
#include "ModelAsset.h"
#include <filesystem>
#include "../../External/openfbx/src/ofbx.h"

renderer::FBXLoader::FBXLoader( renderer::IRenderer& renderer )
	: m_renderer( renderer )
{}

// TODO: IndexBuffer
std::vector< std::shared_ptr< forge::IAsset > > renderer::FBXLoader::LoadAssets( const std::string& path ) const
{
	FILE* fp;
	fopen_s( &fp, path.c_str(), "rb" );

	if( !fp ) return {};

	fseek( fp, 0, SEEK_END );
	long file_size = ftell( fp );
	fseek( fp, 0, SEEK_SET );
	auto* content = new ofbx::u8[ file_size ];
	fread( content, 1, file_size, fp );
	ofbx::IScene* scene = ofbx::load( ( ofbx::u8* )content, file_size, ( ofbx::u64 )ofbx::LoadFlags::TRIANGULATE );

	std::vector< renderer::Shape > shapes;

	std::vector< renderer::InputPosition > poses;
	std::vector< renderer::InputTexCoord > texCoords;
	std::vector< renderer::InputNormal > normals;

	std::vector< renderer::ModelAsset::MaterialData > materialsData;

	for( Uint32 i = 0u; i < scene->getMeshCount(); ++i )
	{
		auto* mesh = scene->getMesh( i );
		auto* geometry = mesh->getGeometry();
		auto rawTransform = mesh->getGlobalTransform();
		auto cast = [ &rawTransform ]( Uint32 index ) { return static_cast<Float>( rawTransform.m[ index ] ); };
		Matrix transformMatrix( cast( 0 ), cast( 1 ), cast( 2 ), cast( 3 ), cast( 4 ), cast( 5 ), cast( 6 ), cast( 7 ), cast( 8 ), cast( 9 ), cast( 10 ), cast( 11 ), cast( 12 ), cast( 13 ), cast( 14 ), cast( 15 ) );

		Matrix tmp;

		if( scene->getGlobalSettings()->FrontAxis == ofbx::UpVector_AxisX )
		{
			tmp.Y = Vector3::EX();
		}
		else if( scene->getGlobalSettings()->FrontAxis == ofbx::UpVector_AxisZ )
		{
			tmp.Y = Vector3::EZ();
		}

		if( scene->getGlobalSettings()->UpAxis == ofbx::UpVector_AxisX )
		{
			tmp.Z = Vector3::EX();
		}
		else if( scene->getGlobalSettings()->UpAxis == ofbx::UpVector_AxisY )
		{
			tmp.Z = Vector3::EY();
		}

		if( Vector3::EX() != tmp.Y && Vector3::EX() != tmp.Z )
		{
			tmp.X = Vector3::EX();
		}
		else if( Vector3::EY() != tmp.Y && Vector3::EY() != tmp.Z )
		{
			tmp.X = Vector3::EY();
		}
		else
		{
			tmp.X = Vector3::EZ();
		}

		tmp.Z = -tmp.Z;

		transformMatrix = transformMatrix * tmp;

		Uint32 materialIndexOffset = materialsData.size();
		for( Uint32 i = 0; i < mesh->getMaterialCount(); ++i )
		{
			auto* rawMaterial = mesh->getMaterial( i );
			materialsData.emplace_back( renderer::ModelAsset::MaterialData{ m_renderer.CreateConstantBuffer() } );
			auto& materialData = materialsData.back();
			materialData.m_buffer->AddData( "diffuseColor", Vector4( rawMaterial->getDiffuseColor().r, rawMaterial->getDiffuseColor().g, rawMaterial->getDiffuseColor().b, 1.0f ) );
			materialData.m_buffer->UpdateBuffer();

			if( const auto* rawTexture = rawMaterial->getTexture( ofbx::Texture::TextureType::DIFFUSE ) )
			{
				char buff[ 400 ];
				rawTexture->getFileName().toString( buff );
				materialData.m_diffuseTextureName = std::filesystem::path( buff ).filename().string();
			}
		}

		Int32 currentMaterialIndex = geometry->getMaterials() ? geometry->getMaterials()[ 0 ] : 0;
		shapes.emplace_back();
		shapes.back().m_materialIndex = materialIndexOffset + currentMaterialIndex;

		for( Uint32 i = 0u; i < geometry->getVertexCount() / 3u; ++i )
		{
			if( geometry->getMaterials() && geometry->getMaterials()[ i ] != currentMaterialIndex )
			{
				shapes.emplace_back();
				currentMaterialIndex = geometry->getMaterials()[ i ];
				shapes.back().m_materialIndex = materialIndexOffset + currentMaterialIndex;
			}

			std::function< Vector2( Uint32 ) > getUVsFunc;
			std::function< Vector3( Uint32, Uint32 ) > getNormalFunc;

			if( geometry->getNormals() )
			{
				getNormalFunc = [ geometry, &transformMatrix ]( Uint32 i, Uint32 j )
				{
					const ofbx::Vec3& rawNormal = geometry->getNormals()[ j ];
					return transformMatrix.TransformVector( Vector3{ static_cast<Float>( rawNormal.x ), static_cast<Float>( rawNormal.y ), static_cast<Float>( rawNormal.z ) } );
				};
			}
			else
			{
				getNormalFunc = [ geometry, &transformMatrix ]( Uint32 i, Uint32 j )
				{
					const ofbx::Vec3& vertexA = geometry->getVertices()[ i * 3u ];
					Vector3 a = transformMatrix.TransformPoint( Vector3{ static_cast<Float>( vertexA.x ), static_cast<Float>( vertexA.y ), static_cast<Float>( vertexA.z ) } );

					const ofbx::Vec3& vertexB = geometry->getVertices()[ i * 3u + 1u ];
					Vector3 b = transformMatrix.TransformPoint( Vector3{ static_cast<Float>( vertexB.x ), static_cast<Float>( vertexB.y ), static_cast<Float>( vertexB.z ) } );

					return a.Cross( b ).Normalized();
				};
			}

			if( auto* uvs = geometry->getUVs( 0 ) )
			{
				getUVsFunc = [ uvs ]( Uint32 j )
				{
					const ofbx::Vec2& rawUV = uvs[ j ];
					return Vector2{ static_cast<Float>( rawUV.x ), static_cast<Float>( rawUV.y ) };
				};
			}
			else
			{
				getUVsFunc = []( Uint32 j )
				{
					return Vector2();
				};
			}

			for( Uint32 j = i * 3u; j < ( i + 1 ) * 3u; ++j )
			{
				shapes.back().m_indices.emplace_back( poses.size() );
				const ofbx::Vec3& vertex = geometry->getVertices()[ j ];
				poses.emplace_back( transformMatrix.TransformPoint( Vector3{ static_cast<Float>( vertex.x ), static_cast<Float>( vertex.y ), static_cast<Float>( vertex.z ) } ) );
				normals.emplace_back( getNormalFunc( i, j ) );
				texCoords.emplace_back( getUVsFunc( j ) );
			}
		}
	}

	renderer::Vertices vertices( poses, normals, texCoords );

	return { std::make_shared< renderer::ModelAsset >( path, std::make_unique< renderer::Model >( m_renderer, vertices, shapes ), std::move( materialsData ) ) };
}

static const char* c_handledExceptions[] = { "fbx" };
forge::ArraySpan<const char *> renderer::FBXLoader::GetHandledExtensions() const
{
	return c_handledExceptions;
}