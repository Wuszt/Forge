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
	return vec;
	return { vec.X, -vec.Z, vec.Y };
}

Quaternion FixOrientation( const Quaternion& quat )
{
	return quat;
	return { quat.i, -quat.k, quat.j, quat.r };
}

Matrix ConvertMatrix(const aiMatrix4x4 rawMatrix)
{
	Matrix result = Matrix{rawMatrix[0][0], rawMatrix[0][1], rawMatrix[0][2], rawMatrix[0][3],
				rawMatrix[1][0], rawMatrix[1][1], rawMatrix[1][2], rawMatrix[1][3],
				rawMatrix[2][0], rawMatrix[2][1], rawMatrix[2][2], rawMatrix[2][3],
				rawMatrix[3][0], rawMatrix[3][1], rawMatrix[3][2], rawMatrix[3][3]};

	result.Transpose();
	return result;
}

Matrix ConvertMatrix(const aiMatrix3x3 rawMatrix)
{
	return ConvertMatrix(aiMatrix4x4(rawMatrix));
}


void LoadBones(const aiMesh* mesh, renderer::TMPAsset& asset)
{
	for( Uint32 i = 0u; i < mesh->mNumBones; ++i )
	{
		Uint32 boneIndex = 0u;
		std::string boneName( mesh->mBones[i]->mName.data );

		if(asset.m_boneMapping.find(boneName) == asset.m_boneMapping.end())
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
		asset.m_boneInfo[boneIndex].m_boneOffset = ConvertMatrix(mesh->mBones[i]->mOffsetMatrix);

		Quaternion rot = FixOrientation(asset.m_boneInfo[boneIndex].m_boneOffset.GetRotation());
		Vector3 pos = FixOrientation(asset.m_boneInfo[boneIndex].m_boneOffset.GetTranslation());

		asset.m_boneInfo[boneIndex].m_boneOffset = Matrix(rot) * Matrix(pos);

		for(Uint32 j = 0; j < mesh->mBones[i]->mNumWeights; ++j)
		{
			Uint32 vertexID = mesh->mBones[i]->mWeights[j].mVertexId;
			Float weight = mesh->mBones[i]->mWeights[j].mWeight;
			
			asset.m_bones.resize(Math::Max<Uint32>(asset.m_bones.size(), vertexID + 1u));
			
			Bool found = false;
			for(Uint32 x = 0u; x < 10u; ++x)
			{
				if(asset.m_bones[vertexID].m_boneIDs[x] == boneIndex)
				{
					found = true;
					break;
				}

				found = asset.m_bones[vertexID].m_weights[x] == 0.0f;
				if(found)
				{
					asset.m_bones[vertexID].m_boneIDs[x] = boneIndex;
					asset.m_bones[vertexID].m_weights[x] = weight;
					break;
				}
			}

			FORGE_ASSERT(found);
		}
	}
}

Vector3 ConvertVector3(const aiVector3D& vec)
{
	return {vec.x, vec.y, vec.z};
}

void ReadNodeHierarchy( renderer::TMPAsset& asset, const aiAnimation* anim, const aiNode* node, const aiNodeAnim* parent, std::vector<Matrix> inBeetweenMatrices )
{
	std::string nodeName( node->mName.data );
	//Matrix nodeTransform = ConvertMatrix(node->mTransformation);

	//Quaternion rot = FixOrientation(nodeTransform.GetRotation());
	//Vector3 pos = FixOrientation(nodeTransform.GetTranslation());

	//nodeTransform = Matrix(rot) * Matrix(pos);

	aiNodeAnim* nodeAnim = nullptr;
	for(Uint32 i = 0u; i < anim->mNumChannels; ++i)
	{
		if(std::string(anim->mChannels[i]->mNodeName.data) == nodeName)
		{
			nodeAnim = anim->mChannels[i];
			break;
		}
	}

	Float step = 0.1f;

	Float duration = anim->mDuration;

	if( inBeetweenMatrices.empty() )
	{
		inBeetweenMatrices.resize( ceil( duration / step ) + 1 );
	}

	if( nodeAnim )
	{
		FORGE_ASSERT(nodeAnim->mNumPositionKeys > 0u);
		FORGE_ASSERT(nodeAnim->mNumRotationKeys > 0u);
		FORGE_ASSERT(nodeAnim->mNumScalingKeys > 0u);

		FORGE_ASSERT(nodeAnim->mPositionKeys[0].mTime == 0.0f);
		FORGE_ASSERT(nodeAnim->mRotationKeys[0].mTime == 0.0f);
		FORGE_ASSERT(nodeAnim->mScalingKeys[0].mTime == 0.0f);

		Uint32 x = 0u;
		for(Float time = 0.0f; time < duration; time += step)
		{			
			Matrix translationMatrix;
			for( Uint32 i = 1; i < nodeAnim->mNumPositionKeys; ++i )
			{
				if( time < nodeAnim->mPositionKeys[ i ].mTime )
				{
					Vector3 prevKey = FixOrientation( ConvertVector3( nodeAnim->mPositionKeys[ i - 1 ].mValue ) );
					Vector3 nextKey = FixOrientation( ConvertVector3( nodeAnim->mPositionKeys[ i ].mValue ) );
					float timeDiff = nodeAnim->mPositionKeys[ i ].mTime - nodeAnim->mPositionKeys[ i - 1 ].mTime;
					float t = ( time - nodeAnim->mPositionKeys[ i - 1 ].mTime ) / timeDiff;

					Vector3 result = Math::Lerp( prevKey, nextKey, t );

					translationMatrix = Matrix( result );
					break;
				}
			}

			
			Matrix orientationMatrix;
			for( Uint32 i = 1; i < nodeAnim->mNumRotationKeys; ++i )
			{
				if( time < nodeAnim->mRotationKeys[ i ].mTime )
				{
					Quaternion prevKey = ConvertMatrix( nodeAnim->mRotationKeys[ i - 1 ].mValue.GetMatrix() ).GetRotation();
					Quaternion nextKey = ConvertMatrix( nodeAnim->mRotationKeys[ i ].mValue.GetMatrix() ).GetRotation();
					float timeDiff = nodeAnim->mRotationKeys[ i ].mTime - nodeAnim->mRotationKeys[ i - 1 ].mTime;
					float t = ( time - nodeAnim->mRotationKeys[i - 1].mTime ) / timeDiff;

					Quaternion result = Quaternion::Lerp( prevKey, nextKey, t );

					orientationMatrix = Matrix( result );
					break;
				}
			}

			Matrix scaleMatrix;
			for( Uint32 i = 1; i < nodeAnim->mNumScalingKeys; ++i )
			{
				if(time < nodeAnim->mScalingKeys[ i ].mTime )
				{
					Vector3 prevKey = FixOrientation( ConvertVector3( nodeAnim->mScalingKeys[ i - 1 ].mValue) );
					Vector3 nextKey = FixOrientation( ConvertVector3( nodeAnim->mScalingKeys[ i ].mValue) );
					float timeDiff = nodeAnim->mScalingKeys[ i ].mTime - nodeAnim->mScalingKeys[ i - 1 ].mTime;
					float t = ( time - nodeAnim->mScalingKeys[ i - 1 ].mTime ) / timeDiff;

					Vector3 result = Math::Lerp( prevKey, nextKey, t);

					scaleMatrix.SetScale( result );
					break;
				}
			}

			Matrix nodeTransform = scaleMatrix * orientationMatrix * translationMatrix;
			
			if( asset.m_boneMapping.find( nodeName ) != asset.m_boneMapping.end() )
			{
				Uint32 boneIndex = asset.m_boneMapping.at( nodeName );
				std::vector< Matrix >& boneAnim = asset.m_boneInfo[ boneIndex ].m_anim;

				Matrix parentMatrix;
				if( parent )
				{
					parentMatrix = asset.m_boneInfo[ asset.m_boneMapping.at( std::string( parent->mNodeName.data ) ) ].m_anim[ boneAnim.size() ];
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
			Matrix nodeTransform = ConvertMatrix( node->mTransformation );

			Quaternion rot = FixOrientation( nodeTransform.GetRotation() );
			Vector3 pos = FixOrientation( nodeTransform.GetTranslation() );
			Matrix inBeetweenMatrix = Matrix( rot ) * Matrix( pos );

			for( Matrix& m : inBeetweenMatrices )
			{
				m = inBeetweenMatrix * m;
			}
		}
	}

	for(Uint32 i = 0u; i < node->mNumChildren; ++i)
	{
		ReadNodeHierarchy(asset, anim, node->mChildren[i], ( nodeAnim && asset.m_boneMapping.find( nodeName ) != asset.m_boneMapping.end() ) ? nodeAnim : parent, inBeetweenMatrices);
	}

	if( nodeAnim && asset.m_boneMapping.find( nodeName ) != asset.m_boneMapping.end() )
	{
		Uint32 boneIndex = asset.m_boneMapping.at( nodeName );
		std::vector< Matrix >& boneAnim = asset.m_boneInfo[ boneIndex ].m_anim;

		for( Matrix& a : boneAnim )
		{
			a = asset.m_boneInfo[ boneIndex ].m_boneOffset * a;
		}

		asset.m_boneInfo[ boneIndex ].m_finalTransformation = boneAnim[ duration * 0.5f / step ];
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

	std::vector< renderer::Shape > shapes;

	std::vector< renderer::InputPosition > poses;
	std::vector< renderer::InputTexCoord > texCoords;
	std::vector< renderer::InputNormal > normals;
	std::vector< renderer::InputBlendWeights > blendWeights;
	std::vector< renderer::InputBlendIndices > blendIndices;

	std::vector< renderer::ModelAsset::MaterialData > materialsData;
	std::shared_ptr< renderer::TMPAsset > tmpAsset = std::make_shared< renderer::TMPAsset >( path );
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	{
		Matrix fixingMatrix = Matrix::IDENTITY();
		if(scene->getGlobalSettings()->FrontAxis == ofbx::UpVector_AxisX)
		{
			fixingMatrix.Y = Vector4::EX();
		}
		else if(scene->getGlobalSettings()->FrontAxis == ofbx::UpVector_AxisZ)
		{
			fixingMatrix.Y = Vector4::EZ();
		}
		if(scene->getGlobalSettings()->UpAxis == ofbx::UpVector_AxisX)
		{
			fixingMatrix.Z = Vector4::EX();
		}
		else if(scene->getGlobalSettings()->UpAxis == ofbx::UpVector_AxisY)
		{
			fixingMatrix.Z = Vector4::EY();
		}
		if(Vector4::EX() != fixingMatrix.Y && Vector4::EX() != fixingMatrix.Z)
		{
			fixingMatrix.X = Vector4::EX();
		}
		else if(Vector4::EY() != fixingMatrix.Y && Vector4::EY() != fixingMatrix.Z)
		{
			fixingMatrix.X = Vector4::EY();
		}
		else
		{
			fixingMatrix.X = Vector4::EZ();
		}
		fixingMatrix.Z = -fixingMatrix.Z;

		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path,
			aiProcess_CalcTangentSpace |
			aiProcess_Triangulate |
			//aiProcess_ConvertToLeftHanded |
			aiProcess_JoinIdenticalVertices |
			aiProcess_SortByPType |
			aiProcess_PopulateArmatureData);

		FORGE_ASSERT(scene->mNumMeshes == 1);

		auto* mesh = scene->mMeshes[0];

		LoadBones(mesh, *tmpAsset);

		std::vector<Matrix> initMatrices(std::ceil( scene->mAnimations[ 0 ]->mDuration / 0.1f ) + 1u, fixingMatrix );
		//std::vector<Matrix> initMatrices{};
		ReadNodeHierarchy( *tmpAsset, scene->mAnimations[ 0 ], scene->mRootNode, nullptr, initMatrices );

		//Uint32 ticks = static_cast<Uint32>(scene->mAnimations[0]->mDuration + 1.0f);
		//ticks = 1u;
		//for(Uint32 i = 0u; i < scene->mAnimations[0]->mNumChannels; ++i)
		//{
		//	auto* channel = scene->mAnimations[0]->mChannels[i];

		//}

		for(auto& vertex : tmpAsset->m_bones)
		{
			renderer::InputBlendWeights tmpWeights;
			tmpWeights.m_weights[0] = vertex.m_weights[0];
			tmpWeights.m_weights[1] = vertex.m_weights[1];
			tmpWeights.m_weights[2] = vertex.m_weights[2];
			tmpWeights.m_weights[3] = vertex.m_weights[3];
			float sum = vertex.m_weights[0] + vertex.m_weights[1] + vertex.m_weights[2] + vertex.m_weights[3];

			if(0.5f > sum)
			{
				sum = sum;
			}

			blendWeights.emplace_back(tmpWeights);

			renderer::InputBlendIndices tmpIndices;
			tmpIndices.m_indices[0] = vertex.m_boneIDs[0];
			tmpIndices.m_indices[1] = vertex.m_boneIDs[1];
			tmpIndices.m_indices[2] = vertex.m_boneIDs[2];
			tmpIndices.m_indices[3] = vertex.m_boneIDs[3];
			blendIndices.emplace_back(tmpIndices);
		}

		shapes.emplace_back();

		for(Uint32 i = 0u; i < scene->mNumMaterials; ++i)
		{
			materialsData.emplace_back(renderer::ModelAsset::MaterialData{m_renderer.CreateConstantBuffer()});
			auto& materialData = materialsData.back();
			materialData.m_buffer->AddData("diffuseColor", Vector4(1.0f, 1.0f, 1.0f, 1.0f));
			materialData.m_buffer->UpdateBuffer();
		}

		Float maxX = -std::numeric_limits<Float>::infinity();
		Float minX = std::numeric_limits<Float>::infinity();

		Float maxY = -std::numeric_limits<Float>::infinity();
		Float minY = std::numeric_limits<Float>::infinity();

		Float maxZ = -std::numeric_limits<Float>::infinity();
		Float minZ = std::numeric_limits<Float>::infinity();

		for(Uint32 i = 0u; i < mesh->mNumVertices; ++i)
		{
			poses.emplace_back(FixOrientation(Vector3{mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z}));
			normals.emplace_back(FixOrientation(Vector3{mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z}));
			texCoords.emplace_back(mesh->mTextureCoords[0]->x, mesh->mTextureCoords[0]->y);

			maxX = Math::Max(maxX, poses.back().X);
			minX = Math::Min(minX, poses.back().X);

			maxY = Math::Max(maxY, poses.back().Y);
			minY = Math::Min(minY, poses.back().Y);

			maxZ = Math::Max(maxZ, poses.back().Z);
			minZ = Math::Min(minZ, poses.back().Z);
		}

		Float offset = ( maxY - minY ) * 0.5f;

		for(auto& pos : poses)
		{
			//pos.Y -= offset;
			//pos *= 100.0f;
		}

		for(Uint32 i = 0u; i < mesh->mNumFaces; ++i)
		{
			for(Uint32 x = 0u; x < mesh->mFaces[i].mNumIndices; ++x)
			{
				shapes.back().m_indices.emplace_back(mesh->mFaces[i].mIndices[x]);
			}
		}
	}

#if 0
	tmpAsset = std::make_shared< renderer::TMPAsset >(path);

	{
		tmpAsset->m_rootBone.m_matrix = Matrix::IDENTITY();
		{
			//wez porownaj z tym lumen enginem TPose'a - wszystko dziala tylko jesli pomnoze bone'y przez pose'a, ale hipsy maja wtedy podwojny translate
			//Left leg oryginalnie ma -175 obrot wokol Z, a w animacji tylko -4. Dlaczego? Co sie dzieje w lumenie?
			// Left leg hash: 11778920607986760839
			auto* animStack = scene->getAnimationStack( 0 );
			auto* animLayer = animStack->getLayer( 0 );
			auto* curve = animLayer->getCurveNode( 0 );
			for( Uint32 i = 0u; curve->getCurve( 0 ) == nullptr; ++i, curve = animLayer->getCurveNode( i ) );
			Float animLength = ofbx::fbxTimeToSeconds( curve->getCurve( 0 )->getKeyTime()[ curve->getCurve( 0 )->getKeyCount() - 1 ] );
			//FORGE_LOG( "%f", ofbx::fbxTimeToSeconds( curve->getCurve( 0 )->getKeyTime()[ curve->getCurve( 0 )->getKeyCount() - 1 ] ) );

			//{
			//	auto v = curve->getNodeLocalTransform( 15.5f );
			//	Vector3 vec = {static_cast<Float>( v.x ), static_cast<Float>( v.y ), static_cast<Float>( v.z )};
			//	FORGE_LOG(vec.ToDebugString().c_str());
			//}


	//http://www.richardssoftware.net/Home/Post/34
	//cluster is a bone, indices which it contains are indices to vertices on which has influence on
	//each vertex has few bones to interpolate between - the sum of weights is always 1
			for( Uint32 i = 0u; i < scene->getMeshCount(); ++i )
			{
				auto* mesh = scene->getMesh( i );
				auto* geometry = mesh->getGeometry();

				Uint32 indicesAmount = 0u;
				auto* skin = geometry->getSkin();


				for( Uint32 i = 0; i < skin->getClusterCount(); ++i )
				{
					std::vector< const ofbx::Object* > rawBones;
					{
						auto* link = skin->getCluster( i )->getLink();
						while( link )
						{
							rawBones.emplace_back( link );
							link = link->getParent();
						}
					}

					TMPAsset::Bone* currentBone = &tmpAsset->m_rootBone;
					for( auto it = rawBones.rbegin() + 1u; it != rawBones.rend(); ++it )
					{
						auto findIt = std::find_if( currentBone->m_children.begin(), currentBone->m_children.end(), [ it ]( const TMPAsset::Bone& bone )
						{
							return bone.m_id == ( *it )->id;
						} );

						if( findIt == currentBone->m_children.end() )
						{
							auto cast = [](const ofbx::Matrix& tr, Uint32 index) { return static_cast<Float>(tr.m[index]); };
							auto castMatrix = [&cast](const ofbx::Matrix& tr){ return Matrix(cast(tr, 0), cast(tr, 1), cast(tr, 2), cast(tr, 3), cast(tr, 4), cast(tr, 5), cast(tr, 6), cast(tr, 7), cast(tr, 8), cast(tr, 9), cast(tr, 10), cast(tr, 11), cast(tr, 12), cast(tr, 13), cast(tr, 14), cast(tr, 15));};

							auto castVector = [](const ofbx::Vec3& vec) { return Vector3{static_cast<Float>(vec.x), static_cast<Float>(vec.y), static_cast<Float>(vec.z)}; };

							Vector3 preAngles = castVector((*it)->getPreRotation()) * DEG2RAD;
							preAngles = FixOrientation(preAngles);
							Matrix pre = Matrix(Quaternion(preAngles));
							Vector3 translation = FixOrientation(castVector((*it)->getLocalTranslation()));
							Vector3 rot = FixOrientation(castVector((*it)->getLocalRotation()) * DEG2RAD );
							Vector3 rawRot = FixOrientation(castVector((*it)->getLocalRotation()));

							Matrix transformMatrix = Matrix(Quaternion(rot)) * Matrix(translation) * pre;

							const auto* rotationCurveNode = animLayer->getCurveNode( **it, "Lcl Rotation" );
							const auto* translationCurveNode = animLayer->getCurveNode( **it, "Lcl Translation" );

							const Uint32 samplesAmount = 1u;
							const Float step = animLength / static_cast<Float>( samplesAmount );
							std::vector< Matrix > anim;

							if(rotationCurveNode || translationCurveNode)
							{
								for(Uint32 i = 0; i < samplesAmount; ++i)
								{
									Float t = static_cast<Float>(i) * step;
									Matrix translationMatrix;
									translationMatrix.SetTranslation(translation);

									if(translationCurveNode)
									{
										auto raw = translationCurveNode->getNodeLocalTransform(t);
										translationMatrix.SetTranslation(FixOrientation(Vector3{static_cast<Float>(raw.x), static_cast<Float>(raw.y), static_cast<Float>(raw.z)}));
										translationMatrix = translationMatrix;
									}

									Matrix rotationMatrix{Quaternion(rot)};
									if(rotationCurveNode)
									{
										Vector3 degreesEulers = FixOrientation(castVector(rotationCurveNode->getNodeLocalTransform(t)));
										Vector3 eulers = degreesEulers * DEG2RAD;
										rotationMatrix = Matrix(Quaternion(eulers));
									}

									//anim.emplace_back(rotationMatrix * translationMatrix);
								}
							}

							currentBone->m_children.emplace_back(TMPAsset::Bone{transformMatrix, Matrix::IDENTITY(), nullptr, (*it)->id, (*it)->name, {}, anim});
							currentBone = &currentBone->m_children.back();
						}
						else
						{
							currentBone = &( *findIt );
						}
					}
				}
			}

			SetParents(tmpAsset->m_rootBone);
		}
	}

	PrintAnims(tmpAsset->m_rootBone);
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
				const ofbx::Vec3& vertex = geometry->getVertices()[ j ];

				Vector3 pos = transformMatrix.TransformPoint(Vector3{static_cast<Float>(vertex.x), static_cast<Float>(vertex.y), static_cast<Float>(vertex.z)});
				Vector3 normal = getNormalFunc(i, j);
				Vector2 uvs = getUVsFunc(j);

				Uint64 posHash = Math::CalculateHash(pos);
				Uint64 uvsHash = Math::CalculateHash(uvs);
				Uint64 normalHash = Math::CalculateHash(normal);

				auto it = uniqueVerticesIndices[posHash][uvsHash].find(normalHash);
				if(it == uniqueVerticesIndices[posHash][uvsHash].end())
				{
					it = uniqueVerticesIndices[posHash][uvsHash].emplace(uvsHash, static_cast<Uint32>(poses.size())).first;
					poses.emplace_back(pos);
					texCoords.emplace_back(uvs);
					normals.emplace_back(normal);
				}
				else
				{
					FORGE_ASSERT(pos == poses[it->second] && uvs == texCoords[it->second]);
				}

				shapes.back().m_indices.emplace_back(it->second);
			}
		}
	}
#endif

	blendWeights.resize(poses.size());
	blendIndices.resize(poses.size());

	renderer::Vertices vertices(poses, normals, texCoords, blendWeights, blendIndices);

	return { std::make_shared< renderer::ModelAsset >( path, std::make_unique< renderer::Model >( m_renderer, vertices, shapes ), std::move( materialsData ) ), tmpAsset };
}

static const char* c_handledExceptions[] = { "fbx" };
forge::ArraySpan<const char *> renderer::FBXLoader::GetHandledExtensions() const
{
	return c_handledExceptions;
}