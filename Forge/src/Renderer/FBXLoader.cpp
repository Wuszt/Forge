#include "Fpch.h"
#include "FBXLoader.h"
#include "../Core/IAsset.h"
#include "SkeletonAsset.h"
#include "ModelAsset.h"
#include <filesystem>
#include "../../External/openfbx/src/ofbx.h"
#include "AnimationSetAsset.h"
#include <regex>
#include "../Core/Path.h"

renderer::FBXLoader::FBXLoader( renderer::Renderer& renderer )
	: m_renderer( renderer )
{}

#define CONVERT2FORGE( rawMatrix ) Matrix{ static_cast< Float >( rawMatrix.m[ 0 ] ), static_cast< Float >( rawMatrix.m[ 1 ] ), static_cast< Float >( rawMatrix.m[ 2 ] ), static_cast< Float >( rawMatrix.m[ 3 ] ), \
	static_cast< Float >( rawMatrix.m[ 4 ] ), static_cast< Float >( rawMatrix.m[ 5 ] ), static_cast< Float >( rawMatrix.m[ 6 ] ), static_cast< Float >( rawMatrix.m[ 7 ] ), \
	static_cast< Float >( rawMatrix.m[ 8 ] ), static_cast< Float >( rawMatrix.m[ 9 ] ), static_cast< Float >( rawMatrix.m[ 10 ] ), static_cast< Float >( rawMatrix.m[ 11 ] ), \
	static_cast< Float >( rawMatrix.m[ 12 ] ), static_cast< Float >( rawMatrix.m[ 13 ] ), static_cast< Float >( rawMatrix.m[ 14 ] ), static_cast< Float >( rawMatrix.m[ 15 ] ) }

Matrix ConstructFixingMatrix( const ofbx::GlobalSettings& globalSettings )
{
	Matrix fixingMatrix = Matrix::IDENTITY();
	if ( globalSettings.FrontAxis == ofbx::UpVector_AxisX )
	{
		fixingMatrix.Y = Vector4::EX();
	}
	else if ( globalSettings.FrontAxis == ofbx::UpVector_AxisZ )
	{
		fixingMatrix.Y = Vector4::EZ();
	}
	if ( globalSettings.UpAxis == ofbx::UpVector_AxisX )
	{
		fixingMatrix.Z = Vector4::EX();
	}
	else if ( globalSettings.UpAxis == ofbx::UpVector_AxisY )
	{
		fixingMatrix.Z = Vector4::EY();
	}
	if ( Vector4::EX() != fixingMatrix.Y && Vector4::EX() != fixingMatrix.Z )
	{
		fixingMatrix.X = Vector4::EX();
	}
	else if ( Vector4::EY() != fixingMatrix.Y && Vector4::EY() != fixingMatrix.Z )
	{
		fixingMatrix.X = Vector4::EY();
	}
	else
	{
		fixingMatrix.X = Vector4::EZ();
	}

	if ( globalSettings.CoordAxis == ofbx::CoordSystem_RightHanded )
	{
		fixingMatrix.Z = -fixingMatrix.Z;
	}

	return fixingMatrix;
}

namespace
{
	struct FileHandle
	{
		FileHandle( const forge::Path& path )
		{
			fopen_s( &m_file, path.Get(), "rb" );
		}

		~FileHandle()
		{
			if ( m_file )
			{
				fclose( m_file );
			}
		}

		FILE* m_file;
	};
}

using SceneHandle = std::unique_ptr < ofbx::IScene, decltype( []( ofbx::IScene* scene ) { if ( scene ) scene->destroy(); } ) > ;
SceneHandle LoadScene( const forge::Path& path )
{
	FileHandle handle( path );

	if ( handle.m_file == nullptr )
	{
		return nullptr;
	}

	fseek( handle.m_file, 0, SEEK_END );
	Uint64 fileSize = ftell( handle.m_file );
	fseek( handle.m_file, 0, SEEK_SET );
	forge::UniqueRawPtr content( fileSize );
	fread( content.GetData(), 1, fileSize, handle.m_file );

	return SceneHandle( ofbx::load( reinterpret_cast< ofbx::u8* >( content.GetData() ), static_cast< Int32 >( fileSize ), static_cast< ofbx::u64 >( ofbx::LoadFlags::TRIANGULATE ) ) );
}

namespace
{
	struct SkeletonData
	{
		std::vector< renderer::InputBlendWeights > m_blendWeights;
		std::vector< renderer::InputBlendIndices > m_blendIndices;
		std::unordered_map< const ofbx::Object*, Uint32 > m_bonesMap;

		bool IsValid() const
		{
			return !m_blendWeights.empty();
		}
	};
}

std::shared_ptr< renderer::SkeletonAsset > LoadSkeleton( const forge::Path& path, const SceneHandle& scene, SkeletonData& skeletonData )
{
	std::vector< Matrix > bonesOffsets;

	if ( scene->getMesh( 0 )->getGeometry()->getSkin() == nullptr )
	{
		return nullptr;
	}

	for ( Uint32 i = 0u; i < static_cast< Uint32 >( scene->getMesh( 0 )->getGeometry()->getSkin()->getClusterCount() ); ++i )
	{
		const ofbx::Cluster* mainCluster = scene->getMesh( 0 )->getGeometry()->getSkin()->getCluster( i );

		Uint32 boneIndex = static_cast< Uint32 > ( bonesOffsets.size() );
		skeletonData.m_bonesMap[ mainCluster->getLink() ] = boneIndex;
		bonesOffsets.emplace_back( CONVERT2FORGE( mainCluster->getTransformMatrix() ) );

		Uint32 offset = 0u;
		for ( Uint32 s = 0u; s < static_cast< Uint32 >( scene->getMeshCount() ); ++s )
		{
			const ofbx::Cluster* cluster = scene->getMesh( s )->getGeometry()->getSkin()->getCluster( i );
			FORGE_ASSERT( cluster->getWeightsCount() == cluster->getIndicesCount() );

			for ( Uint32 j = 0; j < static_cast< Uint32 >( cluster->getWeightsCount() ); ++j )
			{
				Uint32 vertexID = offset + cluster->getIndices()[ j ];
				Float weight = static_cast< Float >( cluster->getWeights()[ j ] );

				skeletonData.m_blendWeights.resize( Math::Max< Uint32 >( static_cast< Uint32 >( skeletonData.m_blendWeights.size() ), vertexID + 1u ) );
				skeletonData.m_blendIndices.resize( Math::Max< Uint32 >( static_cast< Uint32 >( skeletonData.m_blendIndices.size() ), vertexID + 1u ) );

				Bool found = false;
				for ( Uint32 x = 0u; x < 4u; ++x )
				{
					if ( skeletonData.m_blendIndices[ vertexID ].m_indices[ x ] == boneIndex )
					{
						found = true;
						break;
					}

					found = skeletonData.m_blendWeights[ vertexID ].m_weights[ x ] == 0.0f;
					if ( found )
					{
						skeletonData.m_blendIndices[ vertexID ].m_indices[ x ] = boneIndex;
						skeletonData.m_blendWeights[ vertexID ].m_weights[ x ] = weight;
						break;
					}
				}

				//TODO: Handle more than 4 weights
				//FORGE_ASSERT( found );
			}

			offset += scene->getMesh( s )->getGeometry()->getVertexCount();
		}
	}

	for ( renderer::InputBlendWeights& weightsSet : skeletonData.m_blendWeights )
	{
		Float sum = weightsSet.m_weights[ 0 ] + weightsSet.m_weights[ 1 ] + weightsSet.m_weights[ 2 ] + weightsSet.m_weights[ 3 ];

		if ( sum == 0.0f )
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

	return std::make_shared< renderer::SkeletonAsset >( path, std::move( bonesOffsets ) );
}

Matrix GetNodeLocalTransform( const ofbx::AnimationLayer& animLayer, renderer::Animation& targetAnimation, const ofbx::Object& obj, Uint32 frame, Uint32 framesAmount, const std::unordered_map< const ofbx::Object*, Uint32 >& bonesMap )
{
	const auto& scene = animLayer.getScene();
	auto it = bonesMap.find( &obj );
	bool isBone = it != bonesMap.end();
	if ( isBone )
	{
		if ( !targetAnimation.GeyKeys( it->second ).empty() )
		{
			return targetAnimation.GetAnimationKey( it->second, frame ).ToMatrix();
		}
	}

	Matrix parentTransform = ConstructFixingMatrix( *scene.getGlobalSettings() );
	if ( ofbx::Object* parent = obj.getParent() )
	{
		parentTransform = GetNodeLocalTransform( animLayer, targetAnimation, *parent, frame, framesAmount, bonesMap );
	}

	const auto* rotationCurveNode = animLayer.getCurveNode( obj, "Lcl Rotation" );
	const auto* translationCurveNode = animLayer.getCurveNode( obj, "Lcl Translation" );

	bool hasAnim = rotationCurveNode || translationCurveNode;

	Float frameRate = targetAnimation.GetFrameRate();

	if ( hasAnim )
	{
		if ( isBone )
		{
			Matrix parentTransform = ConstructFixingMatrix( *scene.getGlobalSettings() );
			targetAnimation.GeyKeys( it->second ).resize( framesAmount );
			for ( Uint32 i = 0u; i < static_cast< Uint32 >( framesAmount ); ++i )
			{
				ofbx::Vec3 translation = { 0.0, 0.0, 0.0 };
				if ( translationCurveNode )
				{
					translation = translationCurveNode->getNodeLocalTransform( static_cast< Float >( i ) / frameRate );
				}

				ofbx::Vec3 rotation = { 0.0, 0.0, 0.0 };
				if ( rotationCurveNode )
				{
					rotation = rotationCurveNode->getNodeLocalTransform( static_cast< Float >( i ) / frameRate );
				}

				if ( ofbx::Object* parent = obj.getParent() )
				{
					parentTransform = GetNodeLocalTransform( animLayer, targetAnimation, *parent, i, framesAmount, bonesMap );
				}

				Matrix transform = CONVERT2FORGE( obj.evalLocal( translation, rotation ) ) * parentTransform;

				targetAnimation.GetAnimationKey( it->second, i ) = { transform.GetTranslation(), transform.GetRotation() };
			}

			return targetAnimation.GetAnimationKey( it->second, frame ).ToMatrix();
		}
		else
		{
			ofbx::Vec3 translation;
			if ( translationCurveNode )
			{
				translation = translationCurveNode->getNodeLocalTransform( static_cast< Float >( frame ) / frameRate );
			}

			ofbx::Vec3 rotation;
			if ( rotationCurveNode )
			{
				rotation = rotationCurveNode->getNodeLocalTransform( static_cast< Float >( frame ) / frameRate );
			}

			return CONVERT2FORGE( obj.evalLocal( translation, rotation ) ) * parentTransform;
		}
	}
	else
	{
		Matrix transform = CONVERT2FORGE( obj.getLocalTransform() ) * parentTransform;
		if ( isBone )
		{
			Vector3 translation = transform.GetTranslation();
			Quaternion rotation = transform.GetRotation();

			targetAnimation.GeyKeys( it->second ).resize( framesAmount );
			for ( Uint32 i = 0u; i < static_cast< Uint32 >( framesAmount ); ++i )
			{
				targetAnimation.GetAnimationKey( it->second, i ) = { translation, rotation };
			}
		}

		return transform;
	}

	FORGE_FATAL( "" );
	return Matrix();
}

std::shared_ptr< renderer::AnimationSetAsset > LoadAnimationSet( const SceneHandle& scene, const forge::Path& path, const SkeletonData& skeletonData, renderer::SkeletonAsset& skeleton )
{
	std::vector< renderer::Animation > animations;

	for ( Uint32 i = 0u; i < static_cast< Uint32 >( scene->getAnimationStackCount() ); ++i )
	{
		if ( scene->getAnimationStack( i )->getLayer( 1 ) )
		{
			FORGE_LOG_WARNING( "There is more than one layer in animation stack, but just one is handled!" );
		}

		const auto* animLayer = scene->getAnimationStack( i )->getLayer( 0 );

		const ofbx::AnimationCurveNode* curveNode = animLayer->getCurveNode( 0 );
		const ofbx::AnimationCurve* curve = nullptr;
		for ( Uint32 i = 1u; curveNode != nullptr && curve == nullptr; ++i )
		{
			curve = curveNode->getCurve( 0 );
			curveNode = animLayer->getCurveNode( i );
		}
		if ( curve == nullptr )
		{
			continue;
		}

		renderer::Animation& animation = animations.emplace_back( scene->getSceneFrameRate(), static_cast< Uint32 >( skeleton.GetBonesOffsets().GetSize() ) );

		Uint32 framesAmount = static_cast< Uint32 >( ofbx::fbxTimeToSeconds( curve->getKeyTime()[ curve->getKeyCount() - 1 ] ) * animation.GetFrameRate() );

		for ( Uint32 j = 0u; j < static_cast< Uint32 >( scene->getGeometry( 0 )->getSkin()->getClusterCount() ); ++j )
		{
			const ofbx::Object* link = scene->getGeometry( 0 )->getSkin()->getCluster( j )->getLink();

			GetNodeLocalTransform( *animLayer, animation, *link, 0u, framesAmount, skeletonData.m_bonesMap );
		}
	}

	return std::make_shared< renderer::AnimationSetAsset >( path, animations );
}

std::shared_ptr< renderer::ModelAsset > LoadModel( const forge::Path& path, renderer::Renderer& renderer, const SceneHandle& scene, const SkeletonData& skeletonData )
{
	if ( scene->getMeshCount() == 0u )
	{
		return nullptr;
	}

	std::vector< renderer::Shape > shapes;
	std::vector< renderer::ModelAsset::MaterialData > materialsData;

	std::vector< renderer::InputPosition > poses;
	std::vector< renderer::InputTexCoord > texCoords;
	std::vector< renderer::InputColor > colors;
	std::vector< renderer::InputNormal > normals;

	std::unordered_map< Uint64, Uint32 > uniqueVertices;
	std::vector< Uint32 > indicesToVertices;

	for ( Uint32 i = 0u; i < static_cast< Uint32 >( scene->getMeshCount() ); ++i )
	{
		auto* mesh = scene->getMesh( i );
		auto* geometry = mesh->getGeometry();

		Matrix correctingScale;
		correctingScale.SetScale( 0.01f );
		const Matrix transformMatrix = CONVERT2FORGE( mesh->getGlobalTransform() ) * correctingScale;

		Uint32 materialIndexOffset = static_cast< Uint32 >( materialsData.size() );
		for ( Uint32 i = 0; i < static_cast< Uint32 >( mesh->getMaterialCount() ); ++i )
		{
			auto* rawMaterial = mesh->getMaterial( i );
			materialsData.emplace_back( renderer::ModelAsset::MaterialData{ renderer.CreateConstantBuffer() } );
			auto& materialData = materialsData.back();
			materialData.m_buffer->AddData( "diffuseColor", Vector4( rawMaterial->getDiffuseColor().r, rawMaterial->getDiffuseColor().g, rawMaterial->getDiffuseColor().b, 1.0f ) );
			materialData.m_buffer->UpdateBuffer();

			if ( const auto* rawTexture = rawMaterial->getTexture( ofbx::Texture::TextureType::DIFFUSE ) )
			{
				char buff[ 400 ];
				rawTexture->getFileName().toString( buff );
				materialData.m_diffuseTextureName = std::filesystem::path( buff ).filename().string();
			}
		}

		Int32 currentMaterialIndex = geometry->getMaterials() ? geometry->getMaterials()[ 0 ] : 0;
		shapes.emplace_back();
		shapes.back().m_materialIndex = materialIndexOffset + currentMaterialIndex;

		for ( Uint32 i = 0u; i < geometry->getVertexCount() / 3u; ++i )
		{
			if ( geometry->getMaterials() && geometry->getMaterials()[ i ] != currentMaterialIndex )
			{
				shapes.emplace_back();
				currentMaterialIndex = geometry->getMaterials()[ i ];
				shapes.back().m_materialIndex = materialIndexOffset + currentMaterialIndex;
			}

			std::function< Vector3( Uint32, Uint32 ) > getNormalFunc;

			if ( geometry->getNormals() )
			{
				getNormalFunc = [ geometry, &transformMatrix ]( Uint32 i, Uint32 j )
				{
					const ofbx::Vec3& rawNormal = geometry->getNormals()[ j ];
					return transformMatrix.TransformVector( Vector3{ static_cast< Float >( rawNormal.x ), static_cast< Float >( rawNormal.y ), static_cast< Float >( rawNormal.z ) } );
				};
			}
			else
			{
				getNormalFunc = [ geometry, &transformMatrix ]( Uint32 i, Uint32 j )
				{
					const ofbx::Vec3& vertexA = geometry->getVertices()[ i * 3u ];
					Vector3 a = transformMatrix.TransformPoint( Vector3{ static_cast< Float >( vertexA.x ), static_cast< Float >( vertexA.y ), static_cast< Float >( vertexA.z ) } );

					const ofbx::Vec3& vertexB = geometry->getVertices()[ i * 3u + 1u ];
					Vector3 b = transformMatrix.TransformPoint( Vector3{ static_cast< Float >( vertexB.x ), static_cast< Float >( vertexB.y ), static_cast< Float >( vertexB.z ) } );

					return a.Cross( b ).Normalized();
				};
			}

			Uint64 hashes[ 3u ] = { 0u, 0u, 0u };

			Vector4 tmpColors[ 3u ] = { Vector4(), Vector4(), Vector4() };
			Vector2 tmpUVs[ 3u ] = { Vector2(), Vector2(), Vector2() };
			Vector3 tmpPoses[ 3u ] = { Vector3(), Vector3(), Vector3() };
			Vector3 tmpNormals[ 3u ] = { Vector3(), Vector3(), Vector3() };

			if ( auto* color = geometry->getColors() )
			{
				for ( Uint32 j = i * 3u; j < ( i + 1 ) * 3u; ++j )
				{
					const ofbx::Vec4& rawColor = color[ j ];
					tmpColors[ j - i * 3u ] = { static_cast< Float >( rawColor.x ), static_cast< Float >( rawColor.y ), static_cast< Float >( rawColor.z ), static_cast< Float >( rawColor.w ) };
				}
			}

			if ( auto* uvs = geometry->getUVs( 0 ) )
			{
				for ( Uint32 j = i * 3u; j < ( i + 1 ) * 3u; ++j )
				{
					const ofbx::Vec2& rawUV = uvs[ j ];
					tmpUVs[ j - i * 3u ] = { static_cast< Float >( rawUV.x ), static_cast< Float >( 1.0f - rawUV.y ) };
				}
			}

			for ( Uint32 j = i * 3u; j < ( i + 1 ) * 3u; ++j )
			{
				const ofbx::Vec3& vertex = geometry->getVertices()[ j ];
				tmpPoses[ j - i * 3u ] = transformMatrix.TransformPoint( Vector3{ static_cast< Float >( vertex.x ), static_cast< Float >( vertex.y ), static_cast< Float >( vertex.z ) } );
				tmpNormals[ j - i * 3u ] = getNormalFunc( i, j );
			}

			for ( Uint32 j = 0u; j < 3u; ++j )
			{
				Uint64 hash = Math::CalculateHash( tmpColors[ j ] );
				hash = Math::CombineHashes( hash, Math::CalculateHash( tmpUVs[ j ] ) );
				hash = Math::CombineHashes( hash, Math::CalculateHash( tmpPoses[ j ] ) );
				hash = Math::CombineHashes( hash, Math::CalculateHash( tmpNormals[ j ] ) );

				if ( uniqueVertices.find( hash ) == uniqueVertices.end() )
				{
					if ( geometry->getColors() )
					{
						colors.emplace_back( tmpColors[ j ] );
					}

					if ( geometry->getUVs( 0 ) )
					{
						texCoords.emplace_back( tmpUVs[ j ] );
					}

					poses.emplace_back( tmpPoses[ j ] );
					normals.emplace_back( tmpNormals[ j ] );

					indicesToVertices.emplace_back( i * 3u + j );
					uniqueVertices[ hash ] = static_cast< Uint32 >( poses.size() ) - 1u;
				}

				shapes.back().m_indices.emplace_back( uniqueVertices[ hash ] );
			}
		}
	}

	renderer::VerticesBuilder builder;
	builder.AddData( std::move( poses ) );
	builder.AddData( std::move( normals ) );

	if ( !texCoords.empty() )
	{
		builder.AddData( std::move( texCoords ) );
	}

	if ( !colors.empty() )
	{
		builder.AddData( std::move( colors ) );
	}

	if ( skeletonData.IsValid() )
	{
		std::vector< renderer::InputBlendWeights > blendWeights{ uniqueVertices.size() };
		std::vector< renderer::InputBlendIndices > blendIndices{ uniqueVertices.size() };

		for ( Uint32 i = 0u; i < indicesToVertices.size(); ++i )
		{
			blendWeights[ i ] = skeletonData.m_blendWeights[ indicesToVertices[ i ] ];
			blendIndices[ i ] = skeletonData.m_blendIndices[ indicesToVertices[ i ] ];
		}

		builder.AddData( blendWeights );
		builder.AddData( blendIndices );
	}

	renderer::Vertices vertices = builder.Build();

	std::unique_ptr< renderer::Model > model = std::make_unique< renderer::Model >( renderer, std::move( vertices ), shapes );
	std::shared_ptr< renderer::ModelAsset > modelAsset = std::make_shared< renderer::ModelAsset >( path, std::move( model ), std::move( materialsData ) );

	return modelAsset;
}

forge::Path GetFixedPathOfAsset( const Char* rawPath )
{
	FORGE_ASSERT( false );
	return forge::Path( rawPath );
	//return std::regex_replace( path, std::regex( "\\\\[^\\\\]*fbm\\\\" ), "\\" );
}

void CreateExternalAssets( const SceneHandle& scene )
{
	for ( Uint32 i = 0u; i < static_cast< Uint32 >( scene->getEmbeddedDataCount() ); ++i )
	{
		auto data = scene->getEmbeddedData( i );

		char path[ 400 ];
		scene->getEmbeddedFilename( i ).toString( path );
		forge::Path fixedPath = GetFixedPathOfAsset( path );
		FileHandle fileHandle( fixedPath );

		if ( std::filesystem::exists( fixedPath.Get() ) )
		{
			continue;
		}

		if ( fopen_s( &fileHandle.m_file, fixedPath.Get(), "wb" ) != 0 )
		{
			FORGE_LOG_ERROR( "Couldn't open %s to write external asset", fixedPath.Get() );
			continue;
		}

		fwrite( data.begin + 4, sizeof( ofbx::u8 ), data.end - data.begin - 4, fileHandle.m_file );
	}
}

std::vector< std::shared_ptr< forge::IAsset > > renderer::FBXLoader::LoadAssets( const forge::Path& path ) const
{
	std::vector< std::shared_ptr< forge::IAsset > > loadedAssets;

	const SceneHandle scene = LoadScene( path );

	CreateExternalAssets( scene );

	SkeletonData skeletonData;
	if ( auto skeleton = LoadSkeleton( path, scene, skeletonData ) )
	{
		loadedAssets.emplace_back( skeleton );

		if ( auto animation = LoadAnimationSet( scene, path, skeletonData, *skeleton ) )
		{
			loadedAssets.emplace_back( animation );
		}
	}

	if ( auto model = LoadModel( path, m_renderer, scene, skeletonData ) )
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