#include "Fpch.h"
#include "FBXLoader.h"
#include "../Core/IAsset.h"
#include "SkeletonAsset.h"
#include "ModelAsset.h"
#include <filesystem>
#include "../../External/openfbx/src/ofbx.h"
#include <math.h>
#include "AnimationSetAsset.h"

renderer::FBXLoader::FBXLoader( renderer::IRenderer& renderer )
	: m_renderer( renderer )
{}

#define CONVERT2FORGE( rawMatrix ) Matrix{ static_cast< Float >( rawMatrix.m[ 0 ] ), static_cast< Float >( rawMatrix.m[ 1 ] ), static_cast< Float >( rawMatrix.m[ 2 ] ), static_cast< Float >( rawMatrix.m[ 3 ] ), \
	static_cast< Float >( rawMatrix.m[ 4 ] ), static_cast< Float >( rawMatrix.m[ 5 ] ), static_cast< Float >( rawMatrix.m[ 6 ] ), static_cast< Float >( rawMatrix.m[ 7 ] ), \
	static_cast< Float >( rawMatrix.m[ 8 ] ), static_cast< Float >( rawMatrix.m[ 9 ] ), static_cast< Float >( rawMatrix.m[ 10 ] ), static_cast< Float >( rawMatrix.m[ 11 ] ), \
	static_cast< Float >( rawMatrix.m[ 12 ] ), static_cast< Float >( rawMatrix.m[ 13 ] ), static_cast< Float >( rawMatrix.m[ 14 ] ), static_cast< Float >( rawMatrix.m[ 15 ] ) }

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

using SceneHandle = std::unique_ptr < ofbx::IScene, decltype( []( ofbx::IScene* scene ) { if( scene ) scene->destroy(); } ) > ;
SceneHandle LoadScene( const std::string& path )
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
	Uint64 fileSize = ftell( handle.m_file );
	fseek( handle.m_file, 0, SEEK_SET );
	forge::RawSmartPtr content( fileSize );
	fread( content.GetData(), 1, fileSize, handle.m_file );

	return SceneHandle( ofbx::load( reinterpret_cast< ofbx::u8* >( content.GetData() ), fileSize, static_cast< ofbx::u64 >( ofbx::LoadFlags::TRIANGULATE ) ) );
}

std::shared_ptr< renderer::SkeletonAsset > LoadSkeleton( const std::string& path, const SceneHandle& scene, std::unordered_map< const ofbx::Object*, Uint32 >& outBonesMap )
{
	std::vector< renderer::InputBlendWeights > blendWeights;
	std::vector< renderer::InputBlendIndices > blendIndices;
	std::vector< Matrix > bonesOffsets;

	for( Uint32 i = 0u; i < scene->getMesh( 0 )->getGeometry()->getSkin()->getClusterCount(); ++i )
	{
		const ofbx::Cluster* mainCluster = scene->getMesh( 0 )->getGeometry()->getSkin()->getCluster( i );

		Uint32 boneIndex = static_cast< Uint32> ( bonesOffsets.size() );
		outBonesMap[ mainCluster->getLink() ] = boneIndex;
		bonesOffsets.emplace_back( CONVERT2FORGE( mainCluster->getTransformMatrix() ) );

		Uint32 offset = 0u;
		for( Uint32 s = 0u; s < scene->getMeshCount(); ++s )
		{
			const ofbx::Cluster* cluster = scene->getMesh( s )->getGeometry()->getSkin()->getCluster(i);
			FORGE_ASSERT( cluster->getWeightsCount() == cluster->getIndicesCount() );
			
			for( Uint32 j = 0; j < cluster->getWeightsCount(); ++j )
			{			
				Uint32 vertexID = offset + cluster->getIndices()[j];
				Float weight = cluster->getWeights()[ j ];

				blendWeights.resize( Math::Max< Uint32 >( blendWeights.size(), vertexID + 1u ) );
				blendIndices.resize( Math::Max< Uint32 >( blendIndices.size(), vertexID + 1u ) );

				Bool found = false;
				for( Uint32 x = 0u; x < 4u; ++x )
				{
					if( blendIndices[ vertexID ].m_indices[ x ] == boneIndex )
					{
						found = true;
						break;
					}

					found = blendWeights[ vertexID ].m_weights[ x ] == 0.0f;
					if( found )
					{
						blendIndices[ vertexID ].m_indices[ x ] = boneIndex;
						blendWeights[ vertexID ].m_weights[ x ] = weight;
						break;
					}
				}

				//TODO: Handle more than 4 weights
				//FORGE_ASSERT( found );
			}

			offset += scene->getMesh( s )->getGeometry()->getVertexCount();
		}
	}

	for( renderer::InputBlendWeights& weightsSet : blendWeights )
	{
		Float sum = weightsSet.m_weights[ 0 ] + weightsSet.m_weights[ 1 ] + weightsSet.m_weights[ 2 ] + weightsSet.m_weights[ 3 ];

		if( sum == 0.0f )
		{
			weightsSet.m_weights[ 0 ] = 1.0f;
		}
		else
		{
			weightsSet.m_weights[ 0 ] /= sum;
			weightsSet.m_weights[ 1 ] /= sum;
			weightsSet.m_weights[ 2 ] /= sum;
			weightsSet.m_weights[ 3 ] /= sum;
		}
	}

	return std::make_shared< renderer::SkeletonAsset >( path, std::move( blendWeights ), std::move( blendIndices ), bonesOffsets );
}

Matrix GetNodeLocalTransform( const ofbx::AnimationLayer& animLayer, renderer::Animation& targetAnimation, const ofbx::Object& obj, Uint32 frame, const std::unordered_map< const ofbx::Object*, Uint32 >& bonesMap )
{
	const auto& scene = animLayer.getScene();
	auto it = bonesMap.find( &obj );
	bool isBone = it != bonesMap.end();
	if( isBone )
	{
		if( !targetAnimation.GeyKeys( it->second ).empty() )
		{
			return targetAnimation.GetAnimationKey( it->second, frame ).ToMatrix();
		}
	}

	Matrix parentTransform = ConstructFixingMatrix( *scene.getGlobalSettings() );
	if( ofbx::Object* parent = obj.getParent() )
	{
		parentTransform = GetNodeLocalTransform( animLayer, targetAnimation, *parent, frame, bonesMap );
	}

	const auto* rotationCurveNode = animLayer.getCurveNode( obj, "Lcl Rotation" );
	const auto* translationCurveNode = animLayer.getCurveNode( obj, "Lcl Translation" );

	bool hasAnim = rotationCurveNode || translationCurveNode;

	if( hasAnim )
	{
		Float frameRate = targetAnimation.GetFrameRate();
		if( isBone )
		{		
			Uint32 framesAmount = ofbx::fbxTimeToSeconds( animLayer.getCurveNode( 0 )->getCurve( 0 )->getKeyTime()[ animLayer.getCurveNode( 0 )->getCurve( 0 )->getKeyCount() - 1 ] ) * frameRate;
			Matrix parentTransform = ConstructFixingMatrix( *scene.getGlobalSettings() );
			targetAnimation.GeyKeys( it->second ).resize( framesAmount );
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
					parentTransform = GetNodeLocalTransform( animLayer, targetAnimation, *parent, i, bonesMap );
				}

				Matrix transform = CONVERT2FORGE( obj.evalLocal( translation, rotation ) ) * parentTransform;

				targetAnimation.GetAnimationKey( it->second, i ) = { transform.GetTranslation(), transform.GetRotation() };
			}

			return targetAnimation.GetAnimationKey( it->second, frame ).ToMatrix();
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

			Uint32 framesAmount = ofbx::fbxTimeToSeconds( animLayer.getCurveNode( 0 )->getCurve( 0 )->getKeyTime()[ animLayer.getCurveNode( 0 )->getCurve( 0 )->getKeyCount() - 1 ] ) * targetAnimation.GetFrameRate();
			for( Uint32 i = 0u; i < static_cast< Uint32 >( framesAmount ); ++i )
			{
				targetAnimation.GetAnimationKey( it->second, i ) = { translation, rotation };
			}		
		}

		return transform;
	}

	FORGE_FATAL("");
	return Matrix();
}

std::shared_ptr< renderer::AnimationSetAsset > LoadAnimationSet( const SceneHandle& scene, const std::string& path, const std::unordered_map< const ofbx::Object*, Uint32 >& bonesMap, renderer::SkeletonAsset& skeleton )
{
	std::vector< renderer::Animation > animations;

	for( Uint32 i = 0u; i < scene->getAnimationStackCount(); ++i )
	{
		renderer::Animation& animation = animations.emplace_back( scene->getSceneFrameRate(), skeleton.m_bonesOffsets.size() );

		const auto* animLayer = scene->getAnimationStack( i )->getLayer( 0 );

		if( scene->getAnimationStack( i )->getLayer( 1 ) )
		{
			FORGE_LOG_WARNING( "There is more than one layer in animation stack, but just one is handled!" );
		}

		for( Uint32 j = 0u; j < scene->getGeometry( 0 )->getSkin()->getClusterCount(); ++j )
		{
			const ofbx::Object* link = scene->getGeometry( 0 )->getSkin()->getCluster( j )->getLink();

			GetNodeLocalTransform( *animLayer, animation, *link, 0u, bonesMap );
		}
	}

	return std::make_shared< renderer::AnimationSetAsset >( path, animations );
}

std::shared_ptr< renderer::ModelAsset > LoadModel( const std::string& path, renderer::IRenderer& renderer, const SceneHandle & scene, renderer::SkeletonAsset& skeleton )
{
	if( scene->getMeshCount() == 0u )
	{
		return nullptr;
	}

	std::vector< renderer::Shape > shapes;
	std::vector< renderer::ModelAsset::MaterialData > materialsData;

	std::vector< renderer::InputPosition > poses;
	std::vector< renderer::InputTexCoord > texCoords;
	std::vector< renderer::InputNormal > normals;

	for( Uint32 i = 0u; i < scene->getMeshCount(); ++i )
	{
		auto* mesh = scene->getMesh( i );
		auto* geometry = mesh->getGeometry();

		Matrix transformMatrix = CONVERT2FORGE( mesh->getGlobalTransform() );

		Uint32 materialIndexOffset = materialsData.size();
		for( Uint32 i = 0; i < mesh->getMaterialCount(); ++i )
		{
			auto* rawMaterial = mesh->getMaterial( i );
			materialsData.emplace_back( renderer::ModelAsset::MaterialData{ renderer.CreateConstantBuffer() } );
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

	renderer::Vertices vertices( poses, normals, texCoords, skeleton.m_blendWeights, skeleton.m_blendIndices );

	std::unique_ptr< renderer::Model > model = std::make_unique< renderer::Model >( renderer, vertices, shapes);
	std::shared_ptr< renderer::ModelAsset > modelAsset = std::make_shared< renderer::ModelAsset >( path, std::move( model ), std::move( materialsData ) );

	return modelAsset;
}

std::vector< std::shared_ptr< forge::IAsset > > renderer::FBXLoader::LoadAssets( const std::string& path ) const
{
	std::vector< std::shared_ptr< forge::IAsset > > loadedAssets;

	SceneHandle scene = LoadScene( path );

	std::unordered_map< const ofbx::Object*, Uint32 > bonesMap;
	auto skeleton = LoadSkeleton( path, scene, bonesMap );
	loadedAssets.emplace_back( skeleton );

	if( auto animation = LoadAnimationSet( scene, path, bonesMap, *skeleton ) )
	{
		loadedAssets.emplace_back( animation );
	}	

	if( auto model = LoadModel( path, m_renderer, scene, *skeleton ) )
	{
		loadedAssets.emplace_back( model );
	}

	return loadedAssets;
}

static const char* c_handledExceptions[] = { "fbx" };
forge::ArraySpan<const char*> renderer::FBXLoader::GetHandledExtensions() const
{
	return c_handledExceptions;
}