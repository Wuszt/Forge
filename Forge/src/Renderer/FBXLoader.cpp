#include "Fpch.h"
#include "FBXLoader.h"
#include "../Core/IAsset.h"
#include "ModelAsset.h"
#include <filesystem>
#include "../../External/openfbx/src/ofbx.h"
#include <math.h>

renderer::FBXLoader::FBXLoader( renderer::IRenderer& renderer )
	: m_renderer( renderer )
{}

#define CONVERT2FORGE( rawMatrix ) Matrix{ static_cast< Float >( rawMatrix.m[ 0 ] ), static_cast< Float >( rawMatrix.m[ 1 ] ), static_cast< Float >( rawMatrix.m[ 2 ] ), static_cast< Float >( rawMatrix.m[ 3 ] ), \
	static_cast< Float >( rawMatrix.m[ 4 ] ), static_cast< Float >( rawMatrix.m[ 5 ] ), static_cast< Float >( rawMatrix.m[ 6 ] ), static_cast< Float >( rawMatrix.m[ 7 ] ), \
	static_cast< Float >( rawMatrix.m[ 8 ] ), static_cast< Float >( rawMatrix.m[ 9 ] ), static_cast< Float >( rawMatrix.m[ 10 ] ), static_cast< Float >( rawMatrix.m[ 11 ] ), \
	static_cast< Float >( rawMatrix.m[ 12 ] ), static_cast< Float >( rawMatrix.m[ 13 ] ), static_cast< Float >( rawMatrix.m[ 14 ] ), static_cast< Float >( rawMatrix.m[ 15 ] ) }

struct Context
{
	Context( const ofbx::IScene& scene, const ofbx::AnimationLayer& anim, renderer::TMPAsset& asset, Matrix fixingMatrix )
		: m_scene( scene )
		, m_asset( asset )
		, m_fixingMatrix( std::move( fixingMatrix ) )
	{}

	const ofbx::IScene& m_scene;;
	renderer::TMPAsset& m_asset;
	const Matrix m_fixingMatrix;

	std::unordered_map< const ofbx::Object*, Uint32 > m_bonesMap;
};

void LoadBones_OFBX( Context& ctx )
{
	for( Uint32 i = 0u; i < ctx.m_scene.getMesh( 0 )->getGeometry()->getSkin()->getClusterCount(); ++i )
	{
		const ofbx::Cluster* mainCluster = ctx.m_scene.getMesh( 0 )->getGeometry()->getSkin()->getCluster( i );

		Uint32 boneIndex = 0u;

		boneIndex = static_cast< Uint32 >( ctx.m_asset.m_boneInfo.size() );
		ctx.m_asset.m_boneInfo.emplace_back();
		ctx.m_bonesMap[ mainCluster->getLink() ] = boneIndex;

		ctx.m_asset.m_boneInfo[ boneIndex ].m_boneOffset = CONVERT2FORGE( mainCluster->getTransformMatrix() );

		Uint32 offset = 0u;
		for( Uint32 s = 0u; s < ctx.m_scene.getMeshCount(); ++s )
		{
			const ofbx::Cluster* cluster = ctx.m_scene.getMesh( s )->getGeometry()->getSkin()->getCluster(i);
			FORGE_ASSERT( cluster->getWeightsCount() == cluster->getIndicesCount() );
			
			for( Uint32 j = 0; j < cluster->getWeightsCount(); ++j )
			{			
				Uint32 vertexID = offset + cluster->getIndices()[j];
				Float weight = cluster->getWeights()[ j ];

				ctx.m_asset.m_bones.resize( Math::Max<Uint32>( ctx.m_asset.m_bones.size(), vertexID + 1u ) );

				Bool found = false;
				for( Uint32 x = 0u; x < 10u; ++x )
				{
					if( ctx.m_asset.m_bones[ vertexID ].m_boneIDs[ x ] == boneIndex )
					{
						found = true;
						break;
					}

					found = ctx.m_asset.m_bones[ vertexID ].m_weights[ x ] == 0.0f;
					if( found )
					{
						ctx.m_asset.m_bones[ vertexID ].m_boneIDs[ x ] = boneIndex;
						ctx.m_asset.m_bones[ vertexID ].m_weights[ x ] = weight;
						break;
					}
				}

				FORGE_ASSERT( found );
			}

			offset += ctx.m_scene.getMesh( s )->getGeometry()->getVertexCount();
		}
	}
}

Matrix GetNodeLocalTransform( Context& ctx, const ofbx::Object& obj, Uint32 frame )
{
	auto it = ctx.m_bonesMap.find( &obj );
	bool isBone = it != ctx.m_bonesMap.end();
	if( isBone )
	{
		if( !ctx.m_asset.m_boneInfo[ it->second ].m_translationAnim.empty() )
		{
			return Matrix( ctx.m_asset.m_boneInfo[ it->second ].m_rotationAnim[ frame ] ) * Matrix( ctx.m_asset.m_boneInfo[ it->second ].m_translationAnim[ frame ] );
		}	
	}

	Matrix parentTransform = ctx.m_fixingMatrix;
	if( ofbx::Object* parent = obj.getParent() )
	{
		parentTransform = GetNodeLocalTransform( ctx, *parent, frame );
	}

	const auto& anim = *ctx.m_scene.getAnimationStack( 0 )->getLayer( 0 );

	const auto* rotationCurveNode = anim.getCurveNode( obj, "Lcl Rotation" );
	const auto* translationCurveNode = anim.getCurveNode( obj, "Lcl Translation" );

	bool hasAnim = rotationCurveNode || translationCurveNode;

	Float frameRate = anim.getScene().getSceneFrameRate();
	if( hasAnim )
	{
		if( isBone )
		{		
			Uint32 framesAmount = ofbx::fbxTimeToSeconds( anim.getCurveNode( 0 )->getCurve( 0 )->getKeyTime()[ anim.getCurveNode( 0 )->getCurve( 0 )->getKeyCount() - 1 ] ) * frameRate;
			Matrix parentTransform = ctx.m_fixingMatrix;
			for( Uint32 i = 0u; i < static_cast< Uint32 >( framesAmount ); ++i )
			{
				ofbx::Vec3 translation = { 0.0, 0.0, 0.0 };
				if( translationCurveNode )
				{
					translation = translationCurveNode->getNodeLocalTransform( static_cast< Float >( i ) / frameRate );
				}

				ofbx::Vec3 rotation = { 0.0, 0.0, 0.0 };
				if( rotationCurveNode )
				{
					rotation = rotationCurveNode->getNodeLocalTransform( static_cast< Float >( i ) / frameRate );
				}

				if( ofbx::Object* parent = obj.getParent() )
				{
					parentTransform = GetNodeLocalTransform( ctx, *parent, i );
				}

				Matrix transform = CONVERT2FORGE( obj.evalLocal( translation, rotation ) ) * parentTransform;

				ctx.m_asset.m_boneInfo[ it->second ].m_translationAnim.emplace_back( transform.GetTranslation() );
				ctx.m_asset.m_boneInfo[ it->second ].m_rotationAnim.emplace_back( transform.GetRotation() );
			}

			return Matrix( ctx.m_asset.m_boneInfo[ it->second ].m_rotationAnim[ frame ] ) * Matrix( ctx.m_asset.m_boneInfo[ it->second ].m_translationAnim[ frame ] );
		}
		else
		{
			ofbx::Vec3 translation;
			if( translationCurveNode )
			{
				translation = translationCurveNode->getNodeLocalTransform( static_cast< Float >( frame ) / frameRate );
			}

			ofbx::Vec3 rotation;
			if( rotationCurveNode )
			{
				rotation = rotationCurveNode->getNodeLocalTransform( static_cast< Float >( frame ) / frameRate );
			}

			return CONVERT2FORGE( obj.evalLocal( translation, rotation ) ) * parentTransform;
		}
	}
	else
	{
		Matrix transform = CONVERT2FORGE( obj.getLocalTransform() ) * parentTransform;
		if( isBone )
		{
			Vector3 translation = transform.GetTranslation();
			Quaternion rotation = transform.GetRotation();

			Uint32 framesAmount = ofbx::fbxTimeToSeconds( anim.getCurveNode( 0 )->getCurve( 0 )->getKeyTime()[ anim.getCurveNode( 0 )->getCurve( 0 )->getKeyCount() - 1 ] ) * frameRate;
			for( Uint32 i = 0u; i < static_cast< Uint32 >( framesAmount ); ++i )
			{
				ctx.m_asset.m_boneInfo[ it->second ].m_translationAnim.emplace_back( translation );
				ctx.m_asset.m_boneInfo[ it->second ].m_rotationAnim.emplace_back( rotation );
			}		
		}

		return transform;
	}

	FORGE_FATAL("");
	return Matrix();
}

void LoadAnimation( Context& ctx )
{
	const auto& anim = *ctx.m_scene.getAnimationStack( 0 )->getLayer( 0 );
	ctx.m_asset.m_animDuration = ofbx::fbxTimeToSeconds( anim.getCurveNode( 0 )->getCurve( 0 )->getKeyTime()[ anim.getCurveNode( 0 )->getCurve( 0 )->getKeyCount() - 1 ] );

	for( Uint32 i = 0u; i < ctx.m_scene.getGeometry( 0 )->getSkin()->getClusterCount(); ++i )
	{
		const ofbx::Object* link = ctx.m_scene.getGeometry( 0 )->getSkin()->getCluster(i)->getLink();
	
		GetNodeLocalTransform( ctx, *link, 0u );
	}
}

Matrix ConstructFixingMatrix( const ofbx::GlobalSettings& globalSettings )
{
	Matrix fixingMatrix = Matrix::IDENTITY();
	if( globalSettings.FrontAxis == ofbx::UpVector_AxisX )
	{
		fixingMatrix.Y = Vector4::EX();
	}
	else if( globalSettings.FrontAxis == ofbx::UpVector_AxisZ )
	{
		fixingMatrix.Y = Vector4::EZ();
	}
	if( globalSettings.UpAxis == ofbx::UpVector_AxisX )
	{
		fixingMatrix.Z = Vector4::EX();
	}
	else if( globalSettings.UpAxis == ofbx::UpVector_AxisY )
	{
		fixingMatrix.Z = Vector4::EY();
	}
	if( Vector4::EX() != fixingMatrix.Y && Vector4::EX() != fixingMatrix.Z )
	{
		fixingMatrix.X = Vector4::EX();
	}
	else if( Vector4::EY() != fixingMatrix.Y && Vector4::EY() != fixingMatrix.Z )
	{
		fixingMatrix.X = Vector4::EY();
	}
	else
	{
		fixingMatrix.X = Vector4::EZ();
	}

	if( globalSettings.CoordAxis == ofbx::CoordSystem_RightHanded )
	{
		fixingMatrix.Z = -fixingMatrix.Z;
	}

	return fixingMatrix;
}

ofbx::IScene* LoadScene( const std::string& path )
{
	struct FileHandle
	{
		FileHandle( const std::string& path )
		{
			fopen_s( &m_file, path.c_str(), "rb" );
		}

		~FileHandle()
		{
			if( m_file )
			{
				fclose( m_file );
			}
		}

		FILE* m_file;
	};

	FileHandle handle( path );

	if( handle.m_file == nullptr )
	{
		return nullptr;
	}

	fseek( handle.m_file, 0, SEEK_END );
	long file_size = ftell( handle.m_file );
	fseek( handle.m_file, 0, SEEK_SET );
	auto* content = new ofbx::u8[ file_size ];
	fread( content, 1, file_size, handle.m_file );
	return ofbx::load( ( ofbx::u8* )content, file_size, ( ofbx::u64 )ofbx::LoadFlags::TRIANGULATE );
}

std::vector< std::shared_ptr< forge::IAsset > > renderer::FBXLoader::LoadAssets( const std::string& path ) const
{
	std::vector< renderer::Shape > shapes;

	std::vector< renderer::InputPosition > poses;
	std::vector< renderer::InputTexCoord > texCoords;
	std::vector< renderer::InputNormal > normals;
	std::vector< renderer::InputBlendWeights > blendWeights;
	std::vector< renderer::InputBlendIndices > blendIndices;

	std::vector< renderer::ModelAsset::MaterialData > materialsData;
	std::shared_ptr< renderer::TMPAsset > tmpAsset = std::make_shared< renderer::TMPAsset >( path );

	const auto* scene = LoadScene( path );

	for( Uint32 i = 0u; i < scene->getMeshCount(); ++i )
	{
		auto* mesh = scene->getMesh( i );
		auto* geometry = mesh->getGeometry();

		auto rawTransform = mesh->getGlobalTransform();
		auto cast = [&rawTransform]( Uint32 index ) { return static_cast< Float >( rawTransform.m[ index ] ); };
		Matrix transformMatrix( cast( 0 ), cast( 1 ), cast( 2 ), cast( 3 ), cast( 4 ), cast( 5 ), cast( 6 ), cast( 7 ), cast( 8 ), cast( 9 ), cast( 10 ), cast( 11 ), cast( 12 ), cast( 13 ), cast( 14 ), cast( 15 ) );

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

		std::unordered_map< Uint64, std::unordered_map< Uint64, std::unordered_map< Uint64, Uint32 > > > uniqueVerticesIndices;
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
				getNormalFunc = [geometry, &transformMatrix]( Uint32 i, Uint32 j )
				{
					const ofbx::Vec3& rawNormal = geometry->getNormals()[ j ];
					return transformMatrix.TransformVector( Vector3{ static_cast< Float >( rawNormal.x ), static_cast< Float >( rawNormal.y ), static_cast< Float >( rawNormal.z ) } );
				};
			}
			else
			{
				getNormalFunc = [geometry, &transformMatrix]( Uint32 i, Uint32 j )
				{
					const ofbx::Vec3& vertexA = geometry->getVertices()[ i * 3u ];
					Vector3 a = transformMatrix.TransformPoint( Vector3{ static_cast< Float >( vertexA.x ), static_cast< Float >( vertexA.y ), static_cast< Float >( vertexA.z ) } );

					const ofbx::Vec3& vertexB = geometry->getVertices()[ i * 3u + 1u ];
					Vector3 b = transformMatrix.TransformPoint( Vector3{ static_cast< Float >( vertexB.x ), static_cast< Float >( vertexB.y ), static_cast< Float >( vertexB.z ) } );

					return a.Cross( b ).Normalized();
				};
			}

			if( auto* uvs = geometry->getUVs( 0 ) )
			{
				getUVsFunc = [uvs]( Uint32 j )
				{
					const ofbx::Vec2& rawUV = uvs[ j ];
					return Vector2{ static_cast< Float >( rawUV.x ), static_cast< Float >( rawUV.y ) };
				};
			}
			else
			{
				getUVsFunc = []( Uint32 j )
				{
					return Vector2();
				};
			}

			//for( Uint32 j = i * 3u; j < ( i + 1 ) * 3u; ++j )
			//{
			//	const ofbx::Vec3& vertex = geometry->getVertices()[ j ];

			//	Vector3 pos = transformMatrix.TransformPoint( Vector3{ static_cast< Float >( vertex.x ), static_cast< Float >( vertex.y ), static_cast< Float >( vertex.z ) } );
			//	Vector3 normal = getNormalFunc( i, j );
			//	Vector2 uvs = getUVsFunc( j );

			//	pos.X = Math::Round( pos.X, 2u );
			//	pos.Y = Math::Round( pos.Y, 2u );
			//	pos.Z = Math::Round( pos.Z, 2u );

			//	normal.X = Math::Round( normal.X, 2u );
			//	normal.Y = Math::Round( normal.Y, 2u );
			//	normal.Z = Math::Round( normal.Z, 2u );

			//	uvs.X = Math::Round( uvs.X, 2u );
			//	uvs.Y = Math::Round( uvs.Y, 2u );

			//	Uint64 posHash = Math::CalculateHash( pos );
			//	Uint64 uvsHash = Math::CalculateHash( uvs );
			//	Uint64 normalHash = Math::CalculateHash( normal );

			//	auto it = uniqueVerticesIndices[ posHash ][ uvsHash ].find( normalHash );
			//	if( it == uniqueVerticesIndices[ posHash ][ uvsHash ].end() )
			//	{
			//		it = uniqueVerticesIndices[ posHash ][ uvsHash ].emplace( uvsHash, static_cast< Uint32 >( poses.size() ) ).first;
			//		poses.emplace_back( pos );
			//		texCoords.emplace_back( uvs );
			//		normals.emplace_back( normal );
			//	}
			//	else
			//	{
			//		FORGE_ASSERT( pos == poses[ it->second ] && uvs == texCoords[ it->second ] );
			//	}

			//	shapes.back().m_indices.emplace_back( it->second );
			//}

			for( Uint32 j = i * 3u; j < ( i + 1 ) * 3u; ++j )
			{
				const ofbx::Vec3& vertex = geometry->getVertices()[ j ];
				Vector3 pos = transformMatrix.TransformPoint( Vector3{ static_cast< Float >( vertex.x ), static_cast< Float >( vertex.y ), static_cast< Float >( vertex.z ) } );
				poses.emplace_back( pos );
				texCoords.emplace_back( getUVsFunc( j ) );
				normals.emplace_back( getNormalFunc( i, j ) );

				shapes.back().m_indices.emplace_back( poses.size() - 1 );
			}
		}
	}
	Context ctx( *scene, *scene->getAnimationStack( 0 )->getLayer( 0 ), *tmpAsset, ConstructFixingMatrix( *scene->getGlobalSettings() ) );
	LoadBones_OFBX( ctx );

	LoadAnimation( ctx );

	for( auto& vertex : tmpAsset->m_bones )
	{
		Float sum = vertex.m_weights[ 0 ] + vertex.m_weights[ 1 ] + vertex.m_weights[ 2 ] + vertex.m_weights[ 3 ];
		if( sum == 0.0f )
		{
			sum = 1.0f;
			vertex.m_weights[ 0 ] = 1.0f;
		}

		renderer::InputBlendWeights tmpWeights;
		tmpWeights.m_weights[ 0 ] = vertex.m_weights[ 0 ] / sum;
		tmpWeights.m_weights[ 1 ] = vertex.m_weights[ 1 ] / sum;
		tmpWeights.m_weights[ 2 ] = vertex.m_weights[ 2 ] / sum;
		tmpWeights.m_weights[ 3 ] = vertex.m_weights[ 3 ] / sum;
		blendWeights.emplace_back( tmpWeights );

		renderer::InputBlendIndices tmpIndices;
		tmpIndices.m_indices[ 0 ] = vertex.m_boneIDs[ 0 ];
		tmpIndices.m_indices[ 1 ] = vertex.m_boneIDs[ 1 ];
		tmpIndices.m_indices[ 2 ] = vertex.m_boneIDs[ 2 ];
		tmpIndices.m_indices[ 3 ] = vertex.m_boneIDs[ 3 ];
		blendIndices.emplace_back( tmpIndices );
	}

	blendWeights.resize( poses.size() );
	blendIndices.resize( poses.size() );

	renderer::Vertices vertices( poses, normals, texCoords, blendWeights, blendIndices );

	std::unique_ptr< renderer::Model > model = std::make_unique< renderer::Model >( m_renderer, vertices, shapes );
	std::shared_ptr< renderer::ModelAsset > modelAsset = std::make_shared< renderer::ModelAsset >( path, std::move( model ), std::move( materialsData ) );

	return { modelAsset, tmpAsset };
}

static const char* c_handledExceptions[] = { "fbx" };
forge::ArraySpan<const char*> renderer::FBXLoader::GetHandledExtensions() const
{
	return c_handledExceptions;
}