#include "Fpch.h"
#include "FBXLoader.h"
#include "../Core/IAsset.h"
#include "ModelAsset.h"
#include <filesystem>
#include "../../External/openfbx/src/ofbx.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

renderer::FBXLoader::FBXLoader( renderer::IRenderer& renderer )
	: m_renderer( renderer )
{}

Vector3 FixOrientation( const Vector3& vec )
{
	return { vec.X, -vec.Z, vec.Y };
}

Quaternion FixOrientation( const Quaternion& quat )
{
	return { quat.i, -quat.k, quat.j, quat.r };
}

void SetParents( renderer::TMPAsset::Bone& bone )
{
	for( auto& it : bone.m_children )
	{
		it.m_parent = &bone;
		SetParents(it);
	}
}

aiBone* GetBone( const std::vector<aiBone*>& bones, aiNode* node )
{
	auto found = std::find_if( bones.begin(), bones.end(), [ node ]( aiBone* bone )
	{
		return bone->mName == node->mName;
	} );

	if( found != bones.end() )
	{
		return *found;
	}

	return nullptr;
}

renderer::TMPAsset::Bone* FindBone( renderer::TMPAsset::Bone& rootBone, const std::string& name )
{
	if( rootBone.m_name == name )
	{
		return &rootBone;
	}
	else
	{
		for( auto& child : rootBone.m_children )
		{
			if( auto* found = FindBone( child, name ) )
			{
				return found;
			}
		}

		return nullptr;
	}
}

Matrix ConvertMatrix( const aiMatrix4x4 rawMatrix )
{
	Matrix result = Matrix{ rawMatrix[ 0 ][ 0 ], rawMatrix[ 0 ][ 1 ], rawMatrix[ 0 ][ 2 ], rawMatrix[ 0 ][ 3 ],
				rawMatrix[ 1 ][ 0 ], rawMatrix[ 1 ][ 1 ], rawMatrix[ 1 ][ 2 ], rawMatrix[ 1 ][ 3 ],
				rawMatrix[ 2 ][ 0 ], rawMatrix[ 2 ][ 1 ], rawMatrix[ 2 ][ 2 ], rawMatrix[ 2 ][ 3 ],
				rawMatrix[ 3 ][ 0 ], rawMatrix[ 3 ][ 1 ], rawMatrix[ 3 ][ 2 ], rawMatrix[ 3 ][ 3 ] };

	result.Transpose();
	return result;
}

void PopulateBones( renderer::TMPAsset::Bone& rootBone, const std::vector<aiBone*>& bones, aiBone* bone )
{
	std::vector<aiNode*> nodesHierarchy;

	aiNode* node = bone->mNode;
	while( node )
	{
		nodesHierarchy.emplace_back( node );
		node = node->mParent;
	}

	renderer::TMPAsset::Bone* currentBone = &rootBone;
	Matrix matrix = Matrix::IDENTITY();
	for( auto it = nodesHierarchy.rbegin() + 1; it != nodesHierarchy.rend(); ++it )
	{
		Matrix tmp = ConvertMatrix( ( *it )->mTransformation );
		Vector3 scale;
		Vector3 translation;
		Quaternion rot;
		tmp.Decompose(scale, rot, translation);
		Matrix scaleMat;
		scaleMat.SetScale(FixOrientation(scale));
		tmp = Matrix(FixOrientation(rot)) * Matrix(FixOrientation(translation));
		//tmp =  Matrix(FixOrientation(translation)) * Matrix(FixOrientation(rot));
		matrix = matrix * tmp;
		if( auto* itBone = GetBone( bones, *it ) )
		{
			auto found = std::find_if( currentBone->m_children.begin(), currentBone->m_children.end(), [ it ]( const renderer::TMPAsset::Bone& child ) { return child.m_name == (*it)->mName.C_Str(); } );
			if( found == currentBone->m_children.end() )
			{
				currentBone->m_children.emplace_back();
				found = std::prev(currentBone->m_children.end());
				found->m_name = itBone->mName.C_Str();
				found->m_matrix = matrix;
				found->m_offsetMatrix;
			}

			matrix = Matrix::IDENTITY();
			currentBone = &*found;
		}
	}


	currentBone = currentBone;
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

	std::vector< renderer::Shape > shapes;

	std::vector< renderer::InputPosition > poses;
	std::vector< renderer::InputTexCoord > texCoords;
	std::vector< renderer::InputNormal > normals;

	std::vector< renderer::ModelAsset::MaterialData > materialsData;

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


	std::shared_ptr< renderer::TMPAsset > tmpAsset = std::make_shared< renderer::TMPAsset >( path );
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile( path,
			aiProcess_CalcTangentSpace |
			aiProcess_Triangulate |
			//aiProcess_ConvertToLeftHanded |
			aiProcess_JoinIdenticalVertices |
			aiProcess_SortByPType |
			aiProcess_PopulateArmatureData );

		for(Uint32 i = 0; i < scene->mMetaData->mNumProperties; ++i)
		{
			auto& key = scene->mMetaData->mKeys[i];
			auto& type = scene->mMetaData->mValues[i].mType;
			if(type == AI_INT32)
			{
				FORGE_LOG("%s : %d", key.C_Str(), *(int*)scene->mMetaData->mValues[i].mData);
			}
			else if(type == AI_FLOAT)
			{
				FORGE_LOG("%s : %.2f", key.C_Str(), *(float*)scene->mMetaData->mValues[i].mData);
			}
			else if(type == AI_AIVECTOR3D)
			{
				Vector3 vec = *(Vector3*)scene->mMetaData->mValues[i].mData;
				FORGE_LOG("%s : [%.2f, %.2f, %.2f]", key.C_Str(), vec.X, vec.Y, vec.Z);
			}
			else if(type == AI_AISTRING)
			{
				FORGE_LOG("%s : %s", key.C_Str(), ((aiString*)scene->mMetaData->mValues[i].mData)->C_Str());
			}
		}

		for( Uint32 meshIndex = 0u; meshIndex < scene->mNumMeshes; ++meshIndex )
		{
			auto* mesh = scene->mMeshes[ meshIndex ];

			std::vector<aiBone*> bones;
			for( Uint32 boneIndex = 0u; boneIndex < mesh->mNumBones; ++boneIndex )
			{
				bones.emplace_back(mesh->mBones[boneIndex]);
			}

			tmpAsset->m_rootBone.m_name = "RootNode";
			//tmpAsset->m_rootBone.m_matrix = fixingMatrix;
			//tmpAsset->m_rootBone.m_matrix = 
			//{
			//	Vector4::EX(),
			//	Vector4::EZ(),
			//	Vector4::EY(),
			//	Vector4::EW()
			//};

			for( Uint32 boneIndex = 0u; boneIndex < mesh->mNumBones; ++boneIndex )
			{
				auto* bone = mesh->mBones[ boneIndex ];
				PopulateBones( tmpAsset->m_rootBone, bones, bone );
			}

			Uint32 ticks = static_cast< Uint32 >( scene->mAnimations[ 0 ]->mDuration + 1.0f );

			for( Uint32 i = 0u; i < scene->mAnimations[ 0 ]->mNumChannels; ++i )
			{
				auto* channel = scene->mAnimations[0]->mChannels[i];
				if( auto* bone = FindBone( tmpAsset->m_rootBone, std::string( channel->mNodeName.C_Str() ) ) )
				{
					for( Uint32 w = 0u; w < ticks; ++w )
					{
						for( Uint32 j = 0u; j < channel->mNumPositionKeys; ++j )
						{
							if( channel->mPositionKeys[ j ].mTime >= w )
							{
								Vector3 pos = { channel->mPositionKeys[ j ].mValue.x, channel->mPositionKeys[ j ].mValue.y, channel->mPositionKeys[ j ].mValue.z };
								Quaternion quat = { channel->mRotationKeys[ j ].mValue.x, channel->mRotationKeys[ j ].mValue.y, channel->mRotationKeys[ j ].mValue.z, channel->mRotationKeys[ j ].mValue.w };
								Vector3 scale = { channel->mScalingKeys[ j ].mValue.x, channel->mScalingKeys[ j ].mValue.y, channel->mScalingKeys[ j ].mValue.z };

								Vector3 rot = quat.ToEulerAngles() * RAD2DEG;

								pos = FixOrientation( pos );
								quat = FixOrientation( quat );
								scale = FixOrientation( scale );
								Matrix posMatrix;
								posMatrix.SetTranslation( pos );
								Matrix rotMatrix( quat );
								Matrix scaleMatrix;
								scaleMatrix.SetScale(scale);
								//bone->m_animation.emplace_back( posMatrix * rotMatrix );
								bone->m_animation.emplace_back(rotMatrix * posMatrix);
								break;
							}
							else
							{
								FORGE_ASSERT(true);
							}
						}
					}
				}
			}

			SetParents(tmpAsset->m_rootBone);
		}
	}

	{
	//	tmpAsset->m_rootBone.m_matrix = Matrix::IDENTITY();
	//	{
	//		//wez porownaj z tym lumen enginem TPose'a - wszystko dziala tylko jesli pomnoze bone'y przez pose'a, ale hipsy maja wtedy podwojny translate
	//		//Left leg oryginalnie ma -175 obrot wokol Z, a w animacji tylko -4. Dlaczego? Co sie dzieje w lumenie?
	//		// Left leg hash: 11778920607986760839
	//		auto* animStack = scene->getAnimationStack( 0 );
	//		auto* animLayer = animStack->getLayer( 0 );
	//		auto* curve = animLayer->getCurveNode( 0 );
	//		for( Uint32 i = 0u; curve->getCurve( 0 ) == nullptr; ++i, curve = animLayer->getCurveNode( i ) );
	//		Float animLength = ofbx::fbxTimeToSeconds( curve->getCurve( 0 )->getKeyTime()[ curve->getCurve( 0 )->getKeyCount() - 1 ] );
	//		//FORGE_LOG( "%f", ofbx::fbxTimeToSeconds( curve->getCurve( 0 )->getKeyTime()[ curve->getCurve( 0 )->getKeyCount() - 1 ] ) );

	//		//{
	//		//	auto v = curve->getNodeLocalTransform( 15.5f );
	//		//	Vector3 vec = {static_cast<Float>( v.x ), static_cast<Float>( v.y ), static_cast<Float>( v.z )};
	//		//	FORGE_LOG(vec.ToDebugString().c_str());
	//		//}


	////http://www.richardssoftware.net/Home/Post/34
	////cluster is a bone, indices which it contains are indices to vertices on which has influence on
	////each vertex has few bones to interpolate between - the sum of weights is always 1
	//		for( Uint32 i = 0u; i < scene->getMeshCount(); ++i )
	//		{
	//			auto* mesh = scene->getMesh( i );
	//			auto* geometry = mesh->getGeometry();

	//			Uint32 indicesAmount = 0u;
	//			auto* skin = geometry->getSkin();


	//			for( Uint32 i = 0; i < skin->getClusterCount(); ++i )
	//			{
	//				std::vector< const ofbx::Object* > rawBones;
	//				{
	//					auto* link = skin->getCluster( i )->getLink();
	//					while( link )
	//					{
	//						rawBones.emplace_back( link );
	//						link = link->getParent();
	//					}
	//				}

	//				TMPAsset::Bone* currentBone = &tmpAsset->m_rootBone;
	//				for( auto it = rawBones.rbegin() + 1u; it != rawBones.rend(); ++it )
	//				{
	//					auto findIt = std::find_if( currentBone->m_children.begin(), currentBone->m_children.end(), [ it ]( const TMPAsset::Bone& bone )
	//					{
	//						return bone.m_id == ( *it )->id;
	//					} );

	//					if( findIt == currentBone->m_children.end() )
	//					{
	//						auto tr = ( *it )->getLocalTransform();
	//						auto cast = [ &tr ]( Uint32 index ) { return static_cast<Float>( tr.m[ index ] ); };
	//						Matrix transformMatrix0( cast( 0 ), cast( 1 ), cast( 2 ), cast( 3 ), cast( 4 ), cast( 5 ), cast( 6 ), cast( 7 ), cast( 8 ), cast( 9 ), cast( 10 ), cast( 11 ), cast( 12 ), cast( 13 ), cast( 14 ), cast( 15 ) );
	//						Matrix transformMatrix;
	//						transformMatrix.SetTranslation( FixOrientation( transformMatrix0.GetTranslation() ) );
	//						transformMatrix = Matrix( FixOrientation( transformMatrix0.GetRotation() ) ) * transformMatrix;

	//						const auto* rotationCurveNode = animLayer->getCurveNode( **it, "Lcl Rotation" );
	//						const auto* translationCurveNode = animLayer->getCurveNode( **it, "Lcl Translation" );

	//						const Uint32 samplesAmount = 1u;
	//						const Float step = animLength / static_cast<Float>( samplesAmount );
	//						std::vector< Matrix > anim;
	//						for( Uint32 i = 0; i < samplesAmount; ++i )
	//						{
	//							Float t = static_cast<Float>( i ) * step;
	//							Matrix translationMatrix;
	//							translationMatrix.SetTranslation( FixOrientation( transformMatrix0.GetTranslation() ) );

	//							if( translationCurveNode )
	//							{
	//								auto raw = translationCurveNode->getNodeLocalTransform( t );
	//								translationMatrix.SetTranslation( FixOrientation( Vector3{ static_cast<Float>( raw.x ), static_cast<Float>( raw.y ), static_cast<Float>( raw.z ) } ) );
	//							}

	//							Matrix rotationMatrix( Quaternion( transformMatrix.ToEulerAngles() ) );
	//							if( rotationCurveNode )
	//							{
	//								auto raw = rotationCurveNode->getNodeLocalTransform( t );
	//								Vector3 rawEulers = { DEG2RAD * static_cast<Float>( raw.x ), DEG2RAD * static_cast<Float>( raw.y ), DEG2RAD * static_cast<Float>( raw.z ) };
	//								rotationMatrix = Matrix( Quaternion( FixOrientation( rawEulers ) ) );
	//							}

	//							anim.emplace_back( rotationMatrix * translationMatrix );
	//						}

	//						currentBone->m_children.emplace_back( TMPAsset::Bone{ transformMatrix, currentBone, ( *it )->id, ( *it )->name, {}, anim } );
	//						currentBone = &currentBone->m_children.back();
	//					}
	//					else
	//					{
	//						currentBone = &( *findIt );
	//					}
	//				}
	//			}
	//		}
	//	}
	}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	for( Uint32 i = 0u; i < scene->getMeshCount(); ++i )
	{
		auto* mesh = scene->getMesh( i );
		auto* geometry = mesh->getGeometry();

		auto rawTransform = mesh->getGlobalTransform();
		auto cast = [ &rawTransform ]( Uint32 index ) { return static_cast<Float>( rawTransform.m[ index ] ); };
		Matrix transformMatrix( cast( 0 ), cast( 1 ), cast( 2 ), cast( 3 ), cast( 4 ), cast( 5 ), cast( 6 ), cast( 7 ), cast( 8 ), cast( 9 ), cast( 10 ), cast( 11 ), cast( 12 ), cast( 13 ), cast( 14 ), cast( 15 ) );

		transformMatrix = transformMatrix * fixingMatrix;

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

	return { std::make_shared< renderer::ModelAsset >( path, std::make_unique< renderer::Model >( m_renderer, vertices, shapes ), std::move( materialsData ) ), tmpAsset };
}

static const char* c_handledExceptions[] = { "fbx" };
forge::ArraySpan<const char *> renderer::FBXLoader::GetHandledExtensions() const
{
	return c_handledExceptions;
}