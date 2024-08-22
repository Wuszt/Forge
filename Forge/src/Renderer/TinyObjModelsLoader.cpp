#include "Fpch.h"
#include "TinyObjModelsLoader.h"
#include <filesystem>

#define TINYOBJLOADER_IMPLEMENTATION
#include "../../External/tinyobjloader/tiny_obj_loader.h"
#include <fstream>
#include "ModelAsset.h"

renderer::TinyObjModelsLoader::TinyObjModelsLoader( renderer::Renderer& renderer )
	: m_renderer( renderer )
{}

std::vector< std::shared_ptr< forge::IAsset > > renderer::TinyObjModelsLoader::LoadAssets( const forge::Path& path ) const
{
	tinyobj::ObjReaderConfig readerConfig;

	readerConfig.mtl_search_path = path.GetFolder().AsString();

	tinyobj::ObjReader reader;

	if ( !reader.ParseFromFile( path.AsString(), readerConfig ) )
	{
		return {};
	}

	Transform transform = Transform( Quaternion{ 0.0f, -FORGE_PI_HALF, 0.0f } *Quaternion{ 0.0f, 0.0f, -FORGE_PI_HALF } );
	std::vector< renderer::InputPosition > objPoses;
	auto& rawPositions = reader.GetAttrib().vertices;
	for ( Uint32 i = 0; i < rawPositions.size(); i += 3 )
	{
		objPoses.emplace_back( transform.TransformPoint( { rawPositions[ i ], rawPositions[ i + 1u ], rawPositions[ i + 2u ] } ) );
	}

	std::vector< renderer::InputTexCoord > objTexCoords;
	auto& rawTexCoords = reader.GetAttrib().texcoords;
	for ( Uint32 i = 0; i < rawTexCoords.size(); i += 2 )
	{
		objTexCoords.emplace_back( rawTexCoords[ i ], 1.0f - rawTexCoords[ i + 1u ] );
	}
	objTexCoords.resize( Math::Max( 1u, static_cast< Uint32 >( objTexCoords.size() ) ) );

	std::vector< renderer::InputNormal> objNormals;
	auto& rawNormals = reader.GetAttrib().normals;
	for ( Uint32 i = 0; i < rawNormals.size(); i += 3 )
	{
		objNormals.emplace_back( transform.TransformVector( { rawNormals[ i ], rawNormals[ i + 1u ], rawNormals[ i + 2u ] } ) );
	}
	objNormals.resize( Math::Max( 1u, static_cast< Uint32 >( objNormals.size() ) ) );

	std::vector< renderer::InputPosition > poses;
	std::vector< renderer::InputTexCoord > texCoords;
	std::vector< renderer::InputNormal > normals;

	std::unordered_map< Uint64, Uint32 > uniqueVertices;
	std::vector< renderer::Shape > shapes;
	for ( const auto& shape : reader.GetShapes() )
	{
		shapes.emplace_back( Shape{ Indices(), static_cast< Uint32 >( Math::Max( 0, shape.mesh.material_ids[ 0 ] ) ) } );
		for ( auto index : shape.mesh.indices )
		{
			Vector3 pos = objPoses[ index.vertex_index ];
			Vector2 uvs = objTexCoords[ Math::Max( 0, index.texcoord_index ) ];
			Vector3 normal = objNormals[ Math::Max( 0, index.normal_index ) ];

			Uint64 hash = Math::CalculateHash( pos );
			hash = Math::CombineHashes( hash, Math::CalculateHash( uvs ) );
			hash = Math::CombineHashes( hash, Math::CalculateHash( normal ) );

			if ( uniqueVertices.find( hash ) == uniqueVertices.end() )
			{
				poses.emplace_back( pos );
				texCoords.emplace_back( uvs );
				normals.emplace_back( normal );

				uniqueVertices[ hash ] = static_cast< Uint32 >( poses.size() ) - 1u;
			}

			shapes.back().m_indices.emplace_back( uniqueVertices[ hash ] );
		}
	}

	renderer::Vertices vertices( poses, normals, texCoords );
	auto& materials = reader.GetMaterials();

	std::vector< renderer::ModelAsset::MaterialData > materialsData;
	for ( const auto& material : reader.GetMaterials() )
	{
		std::string diffuseTexName = std::filesystem::path( material.diffuse_texname ).filename().string();
		std::string normalTexMap = std::filesystem::path( material.normal_texname ).filename().string();
		std::string alphaTexName = std::filesystem::path( material.alpha_texname ).filename().string();

		materialsData.emplace_back( renderer::ModelAsset::MaterialData{ m_renderer.CreateConstantBuffer(), diffuseTexName, normalTexMap, alphaTexName } );
		materialsData.back().m_buffer->AddData( "diffuseColor", Vector4( material.diffuse[ 0 ], material.diffuse[ 1 ], material.diffuse[ 2 ], 1.0f ) );
		materialsData.back().m_buffer->UpdateBuffer();
	}

	return { std::make_shared< renderer::ModelAsset >( path, std::make_unique< renderer::Model >( m_renderer, std::move( vertices ), shapes ), std::move( materialsData ) ) };
}

static const char* c_handledExceptions[] = { "obj" };
forge::ArraySpan< const char* > renderer::TinyObjModelsLoader::GetHandledExtensions() const
{
	return c_handledExceptions;
}
