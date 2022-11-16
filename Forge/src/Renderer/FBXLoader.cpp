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

Matrix Convert( const ofbx::Matrix& rawMatrix )
{
	auto cast = []( Float t ) { return static_cast< Float >( t ); };

	Matrix result = Matrix{ cast( rawMatrix.m[ 0 ] ), cast( rawMatrix.m[ 1 ] ), cast( rawMatrix.m[ 2 ] ), cast( rawMatrix.m[ 3 ] ),
				cast( rawMatrix.m[ 4 ] ), cast( rawMatrix.m[ 5 ] ), cast( rawMatrix.m[ 6 ] ), cast( rawMatrix.m[ 7 ] ),
				cast( rawMatrix.m[ 8 ] ), cast( rawMatrix.m[ 9 ] ), cast( rawMatrix.m[ 10 ] ), cast( rawMatrix.m[ 11 ] ),
				cast( rawMatrix.m[ 12 ] ), cast( rawMatrix.m[ 13 ] ), cast( rawMatrix.m[ 14 ] ), cast( rawMatrix.m[ 15 ] ) };

	return result;
}

Vector2 Convert( const ofbx::Vec2& vec )
{
	return { static_cast< Float >( vec.x ),static_cast< Float >( vec.y ) };
}

Vector3 Convert( const ofbx::Vec3& vec )
{
	return { static_cast< Float >( vec.x ),static_cast< Float >( vec.y ),static_cast< Float >( vec.z ) };
}

Vector4 Convert( const ofbx::Vec4& vec )
{
	return { static_cast< Float >( vec.x ),static_cast< Float >( vec.y ),static_cast< Float >( vec.z ),static_cast< Float >( vec.w ) };
}

struct Context
{
	Context( const ofbx::Skin& skin, const ofbx::AnimationLayer& anim, renderer::TMPAsset& asset, Matrix fixingMatrix )
		: m_skin( skin )
		, m_anim( anim )
		, m_asset( asset )
		, m_fixingMatrix( std::move( fixingMatrix ) )
	{}

	const ofbx::Skin& m_skin;
	const ofbx::AnimationLayer& m_anim;
	renderer::TMPAsset& m_asset;
	const Matrix m_fixingMatrix;

	std::unordered_map< const ofbx::Object*, Uint32 > m_bonesMap;
};

void LoadBones_OFBX( const ofbx::Mesh* mesh, Context& ctx )
{
	const ofbx::Skin* skin = mesh->getGeometry()->getSkin();
	for( Uint32 i = 0u; i < skin->getClusterCount(); ++i )
	{
		const ofbx::Cluster* cluster = skin->getCluster( i );

		Uint32 boneIndex = 0u;
		std::string boneName( cluster->name );

		boneIndex = static_cast< Uint32 >( ctx.m_asset.m_boneInfo.size() );
		ctx.m_asset.m_boneInfo.emplace_back();
		ctx.m_bonesMap[ cluster->getLink() ] = boneIndex;

		ctx.m_asset.m_boneInfo[ boneIndex ].m_boneOffset = Convert( cluster->getTransformMatrix() );

		FORGE_ASSERT( cluster->getWeightsCount() == cluster->getIndicesCount() );

		for( Uint32 j = 0; j < cluster->getWeightsCount(); ++j )
		{
			Uint32 vertexID = cluster->getIndices()[ j ];
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

	const auto* rotationCurveNode = ctx.m_anim.getCurveNode( obj, "Lcl Rotation" );
	const auto* translationCurveNode = ctx.m_anim.getCurveNode( obj, "Lcl Translation" );

	bool hasAnim = rotationCurveNode || translationCurveNode;

	Float frameRate = ctx.m_anim.getScene().getSceneFrameRate();
	if( hasAnim )
	{
		if( isBone )
		{		
			Uint32 framesAmount = ofbx::fbxTimeToSeconds( ctx.m_anim.getCurveNode( 0 )->getCurve( 0 )->getKeyTime()[ ctx.m_anim.getCurveNode( 0 )->getCurve( 0 )->getKeyCount() - 1 ] ) * frameRate;
			Matrix parentTransform = ctx.m_fixingMatrix;
			for( Uint32 i = 0u; i < static_cast< Uint32 >( framesAmount ); ++i )
			{
				ofbx::Vec3 translation;
				if( translationCurveNode )
				{
					translation = translationCurveNode->getNodeLocalTransform( static_cast< Float >( i ) / frameRate );
				}

				ofbx::Vec3 rotation;
				if( rotationCurveNode )
				{
					rotation = rotationCurveNode->getNodeLocalTransform( static_cast< Float >( i ) / frameRate );
				}

				if( ofbx::Object* parent = obj.getParent() )
				{
					parentTransform = GetNodeLocalTransform( ctx, *parent, i );
				}

				Matrix transform = Convert( obj.evalLocal( translation, rotation ) ) * parentTransform;

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

			return Convert( obj.evalLocal( translation, rotation ) ) * parentTransform;
		}
	}
	else
	{
		Matrix transform = Convert( obj.getLocalTransform() ) * parentTransform;
		if( isBone )
		{
			Vector3 translation = transform.GetTranslation();
			Quaternion rotation = transform.GetRotation();

			Uint32 framesAmount = ofbx::fbxTimeToSeconds( ctx.m_anim.getCurveNode( 0 )->getCurve( 0 )->getKeyTime()[ ctx.m_anim.getCurveNode( 0 )->getCurve( 0 )->getKeyCount() - 1 ] ) * frameRate;
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
	ctx.m_asset.m_animDuration = ofbx::fbxTimeToSeconds( ctx.m_anim.getCurveNode( 0 )->getCurve( 0 )->getKeyTime()[ ctx.m_anim.getCurveNode( 0 )->getCurve( 0 )->getKeyCount() - 1 ] );

	for( Uint32 i = 0u; i < ctx.m_skin.getClusterCount(); ++i )
	{
		const ofbx::Object* link = ctx.m_skin.getCluster( i )->getLink();
	
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
	auto* sceneG = scene;

	std::vector< renderer::Shape > shapes;

	std::vector< renderer::InputPosition > poses;
	std::vector< renderer::InputTexCoord > texCoords;
	std::vector< renderer::InputNormal > normals;
	std::vector< renderer::InputBlendWeights > blendWeights;
	std::vector< renderer::InputBlendIndices > blendIndices;

	std::vector< renderer::ModelAsset::MaterialData > materialsData;
	std::shared_ptr< renderer::TMPAsset > tmpAsset = std::make_shared< renderer::TMPAsset >( path );

	Context ctx( *scene->getMesh( 0 )->getGeometry()->getSkin(), *scene->getAnimationStack( 0 )->getLayer( 0 ), *tmpAsset, ConstructFixingMatrix( *scene->getGlobalSettings() ) );

	LoadBones_OFBX( scene->getMesh( 0 ), ctx );
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

	LoadAnimation( ctx );

	for( auto& vertex : tmpAsset->m_bones )
	{
		renderer::InputBlendWeights tmpWeights;
		tmpWeights.m_weights[ 0 ] = vertex.m_weights[ 0 ];
		tmpWeights.m_weights[ 1 ] = vertex.m_weights[ 1 ];
		tmpWeights.m_weights[ 2 ] = vertex.m_weights[ 2 ];
		tmpWeights.m_weights[ 3 ] = vertex.m_weights[ 3 ];

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

	return { std::make_shared< renderer::ModelAsset >( path, std::make_unique< renderer::Model >( m_renderer, vertices, shapes ), std::move( materialsData ) ), tmpAsset };
}

static const char* c_handledExceptions[] = { "fbx" };
forge::ArraySpan<const char*> renderer::FBXLoader::GetHandledExtensions() const
{
	return c_handledExceptions;
}