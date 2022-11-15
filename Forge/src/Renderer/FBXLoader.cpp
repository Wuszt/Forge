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

void LoadBones_OFBX( const ofbx::Mesh* mesh, renderer::TMPAsset& asset )
{
	const ofbx::Skin* skin = mesh->getGeometry()->getSkin();
	for( Uint32 i = 0u; i < skin->getClusterCount(); ++i )
	{
		const ofbx::Cluster* cluster = skin->getCluster( i );

		Uint32 boneIndex = 0u;
		std::string boneName( cluster->name );

		if( asset.m_boneMapping.find( boneName ) == asset.m_boneMapping.end() )
		{
			boneIndex = static_cast< Uint32 >( asset.m_boneInfo.size() );
			asset.m_boneInfo.emplace_back();
			asset.m_boneMapping.emplace( boneName, 0u );
		}
		else
		{
			boneIndex = asset.m_boneMapping.at( boneName );
		}

		asset.m_boneMapping.at( boneName ) = boneIndex;
		asset.m_boneInfo[ boneIndex ].m_boneOffset = Convert( cluster->getTransformMatrix() );

		FORGE_ASSERT( cluster->getWeightsCount() == cluster->getIndicesCount() );

		for( Uint32 j = 0; j < cluster->getWeightsCount(); ++j )
		{
			Uint32 vertexID = cluster->getIndices()[ j ];
			Float weight = cluster->getWeights()[ j ];

			asset.m_bones.resize( Math::Max<Uint32>( asset.m_bones.size(), vertexID + 1u ) );

			Bool found = false;
			for( Uint32 x = 0u; x < 10u; ++x )
			{
				if( asset.m_bones[ vertexID ].m_boneIDs[ x ] == boneIndex )
				{
					found = true;
					break;
				}

				found = asset.m_bones[ vertexID ].m_weights[ x ] == 0.0f;
				if( found )
				{
					asset.m_bones[ vertexID ].m_boneIDs[ x ] = boneIndex;
					asset.m_bones[ vertexID ].m_weights[ x ] = weight;
					break;
				}
			}

			FORGE_ASSERT( found );
		}
	}
}

void ReadNodeHierarchy_OFBX( renderer::TMPAsset& asset, const ofbx::AnimationLayer* anim, const ofbx::Object* node, const ofbx::Object* parent, std::vector<Matrix> inBeetweenMatrices )
{
	std::string nodeName( node->name );

	auto* skin = node->getScene().getMesh( 0 )->getGeometry()->getSkin();

	const ofbx::Object* link = nullptr;
	for( Uint32 i = 0u; i < skin->getClusterCount(); ++i )
	{
		if( std::string( skin->getCluster( i )->getLink()->name) == nodeName )
		{
			link = skin->getCluster( i )->getLink();
			break;
		}
	}

	const auto* rotationCurveNode = anim->getCurveNode( link ? *link : *node, "Lcl Rotation" );
	const auto* translationCurveNode = anim->getCurveNode( link ? *link : *node, "Lcl Translation" );
	const auto* scaleCurveNode = anim->getCurveNode( link ? *link : *node, "Lcl Scaling" );

	Float step = 0.01f;
	Float duration = ofbx::fbxTimeToSeconds( anim->getCurveNode( 57 )->getCurve( 0 )->getKeyTime()[ anim->getCurveNode( 57 )->getCurve( 0 )->getKeyCount() - 1 ] );
	duration *= 30.0f;
	if( inBeetweenMatrices.empty() )
	{
		inBeetweenMatrices.resize( ceil( duration / step ) + 1 );
	}

	bool hasAnim = rotationCurveNode || translationCurveNode || scaleCurveNode;
	auto convertTime = []( auto keyTime ) { return ofbx::fbxTimeToSeconds( keyTime ) * 30.0f; };
	if( hasAnim )
	{
		auto sampleLerp = [&convertTime]( const ofbx::AnimationCurve* curve, Float time )
		{
			for( Uint32 i = 1; i < curve->getKeyCount(); ++i )
			{
				if( time < convertTime( curve->getKeyTime()[ i ] ) )
				{
					Float prevKey = curve->getKeyValue()[ i - 1 ];
					Float nextKey = curve->getKeyValue()[ i ];
					Float timeDiff = convertTime( curve->getKeyTime()[ i ] ) - convertTime( curve->getKeyTime()[ i - 1 ] );
					Float t = ( time - convertTime( curve->getKeyTime()[ i - 1 ] ) ) / timeDiff;

					return Math::Lerp( prevKey, nextKey, t );
				}
			}
		};

		auto sample = [&convertTime]( const ofbx::AnimationCurve* curve, Float time )
		{
			for( Uint32 i = 1; i < curve->getKeyCount(); ++i )
			{
				if( time < convertTime( curve->getKeyTime()[ i ] ) )
				{
					Float prevKey = curve->getKeyValue()[ i - 1 ];
					Float nextKey = curve->getKeyValue()[ i ];

					return std::make_pair( prevKey, nextKey );
				}
			}

			return std::pair< Float, Float >();
		};

		Uint32 x = 0u;
		Float time = 0.0f;
		for( Float time = 0.0f; time < duration; time += step )
		{

			Matrix translationMatrix;
			if( translationCurveNode )
			{
				Vector3 result = { sampleLerp( translationCurveNode->getCurve( 0 ), time ), sampleLerp( translationCurveNode->getCurve( 1 ), time ), sampleLerp( translationCurveNode->getCurve( 2 ), time ) };
				translationMatrix = Matrix( result );
			}

			Matrix orientationMatrix;
			if( rotationCurveNode )
			{
				Vector3 eulers = { sampleLerp( rotationCurveNode->getCurve( 0 ), time ), sampleLerp( rotationCurveNode->getCurve( 1 ), time ), sampleLerp( rotationCurveNode->getCurve( 2 ), time ) };

				Matrix prevM = Convert( ( link ? *link : *node ).evalLocal( { 0.0f,0.0f,0.0f }, { eulers.X, eulers.Y, eulers.Z } ) );

				orientationMatrix = prevM;
			}

			Matrix nodeTransform = orientationMatrix * translationMatrix;

			if( asset.m_boneMapping.find( nodeName ) != asset.m_boneMapping.end() )
			{
				Uint32 boneIndex = asset.m_boneMapping.at( nodeName );
				std::vector< Matrix >& boneAnim = asset.m_boneInfo[ boneIndex ].m_anim;

				Matrix parentMatrix;
				if( parent )
				{
					parentMatrix = asset.m_boneInfo[ asset.m_boneMapping.at( std::string( parent->name ) ) ].m_anim[ boneAnim.size() ];
				}

				nodeTransform = nodeTransform * inBeetweenMatrices[ boneAnim.size() ] * parentMatrix;
				boneAnim.emplace_back( nodeTransform );
			}
			else
			{
				inBeetweenMatrices[ x++ ] = nodeTransform * inBeetweenMatrices[ x ];
			}
		}

		if( asset.m_boneMapping.find( nodeName ) != asset.m_boneMapping.end() )
		{
			inBeetweenMatrices.clear();
		}
	}
	else
	{
		if( asset.m_boneMapping.find( nodeName ) == asset.m_boneMapping.end() )
		{
			Matrix nodeTransform = Convert( node->getLocalTransform() );

			Quaternion rot = nodeTransform.GetRotation();
			Vector3 pos = nodeTransform.GetTranslation();
			Matrix inBeetweenMatrix = Matrix( rot ) * Matrix( pos );

			for( Matrix& m : inBeetweenMatrices )
			{
				m = inBeetweenMatrix * m;
			}
		}
	}

	for( Uint32 i = 0u; i < node->getScene().getAllObjectCount(); ++i )
	{
		auto* child = node->getScene().getAllObjects()[ i ];
		if( child->getParent() == node && std::string( child->name ) != nodeName )
		{
			ReadNodeHierarchy_OFBX(asset, anim, child, ( hasAnim && asset.m_boneMapping.find( nodeName ) != asset.m_boneMapping.end() ) ? node : parent, inBeetweenMatrices );
		}
	}

	if( hasAnim && asset.m_boneMapping.find( nodeName ) != asset.m_boneMapping.end() )
	{
		Uint32 boneIndex = asset.m_boneMapping.at( nodeName );
		std::vector< Matrix >& boneAnim = asset.m_boneInfo[ boneIndex ].m_anim;

		for( Matrix& a : boneAnim )
		{
			a = asset.m_boneInfo[ boneIndex ].m_boneOffset * a;
		}

		asset.m_boneInfo[ boneIndex ].m_finalTransformation = boneAnim[ 0 ];
	}

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

	Matrix fixingMatrix = Matrix::IDENTITY();
	if( scene->getGlobalSettings()->FrontAxis == ofbx::UpVector_AxisX )
	{
		fixingMatrix.Y = Vector4::EX();
	}
	else if( scene->getGlobalSettings()->FrontAxis == ofbx::UpVector_AxisZ )
	{
		fixingMatrix.Y = Vector4::EZ();
	}
	if( scene->getGlobalSettings()->UpAxis == ofbx::UpVector_AxisX )
	{
		fixingMatrix.Z = Vector4::EX();
	}
	else if( scene->getGlobalSettings()->UpAxis == ofbx::UpVector_AxisY )
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
	fixingMatrix.Z = -fixingMatrix.Z;

	std::vector< renderer::Shape > shapes;

	std::vector< renderer::InputPosition > poses;
	std::vector< renderer::InputTexCoord > texCoords;
	std::vector< renderer::InputNormal > normals;
	std::vector< renderer::InputBlendWeights > blendWeights;
	std::vector< renderer::InputBlendIndices > blendIndices;

	std::vector< renderer::ModelAsset::MaterialData > materialsData;
	std::shared_ptr< renderer::TMPAsset > tmpAsset = std::make_shared< renderer::TMPAsset >( path );

	LoadBones_OFBX( scene->getMesh( 0 ), *tmpAsset );
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

	auto* animStack = scene->getAnimationStack( 0 );
	auto* animLayer = animStack->getLayer( 0 );

	Float animLength = ofbx::fbxTimeToSeconds( animLayer->getCurveNode( 57 )->getCurve( 0 )->getKeyTime()[ animLayer->getCurveNode( 57 )->getCurve( 0 )->getKeyCount() - 1 ] );
	animLength *= 30.0f;
	std::vector<Matrix> initMatrices( std::ceil( animLength / 0.01f ) + 1u, fixingMatrix );
	ReadNodeHierarchy_OFBX( *tmpAsset, animLayer, scene->getRoot(), nullptr, initMatrices );

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