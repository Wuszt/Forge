#include "Fpch.h"
#include "TinyObjModelsLoader.h"
#include "ResourcesManager.h"
#include <filesystem>

#define TINYOBJLOADER_IMPLEMENTATION
#include "../../External/tinyobjloader/tiny_obj_loader.h"
#include <fstream>

std::shared_ptr< renderer::Model > renderer::TinyObjModelsLoader::LoadModel( const std::string& path, std::vector< renderer::MaterialData >* materialsData )
{
	const std::string folderPath = "Resources/Models/";
	const std::string objFinalPath = folderPath + path;

	std::string warning;
	std::string error;

	tinyobj::ObjReaderConfig readerConfig;

	if( materialsData )
	{
		readerConfig.mtl_search_path = folderPath;
	}

	tinyobj::ObjReader reader;

	if( !reader.ParseFromFile( objFinalPath, readerConfig ) )
	{
		return nullptr;
	}

	Transform transform = Transform( Quaternion{ 0.0f, -FORGE_PI_HALF, 0.0f } * Quaternion{ 0.0f, 0.0f, -FORGE_PI_HALF } );

	std::vector< renderer::InputPosition > objPoses;
	auto& rawPositions = reader.GetAttrib().vertices;
	for( Uint32 i = 0; i < rawPositions.size(); i += 3 )
	{
		objPoses.emplace_back( transform.TransformPoint( { rawPositions[ i ], rawPositions[ i + 1u ], rawPositions[ i + 2u ] } ) );
	}

	std::vector< renderer::InputTexCoord > objTexCoords;
	auto& rawTexCoords = reader.GetAttrib().texcoords;
	for( Uint32 i = 0; i < rawTexCoords.size(); i += 2 )
	{
		objTexCoords.emplace_back( rawTexCoords[ i ], 1.0f - rawTexCoords[ i + 1 ] );
	}
	objTexCoords.resize( Math::Max( 1u, static_cast< Uint32 >( objTexCoords.size() ) ) );

	std::vector< renderer::InputPosition > poses;
	std::vector< renderer::InputTexCoord > texCoords;

	std::unordered_map< Uint64, std::unordered_map< Uint64, Uint32 > > uniqueVerticesIndices;
	std::vector< renderer::Shape > shapes;
	for( const auto& shape : reader.GetShapes() )
	{
		//FORGE_ASSERT( std::count( shape.mesh.material_ids.begin(), shape.mesh.material_ids.end(), shape.mesh.material_ids[ 0 ] ) == shape.mesh.material_ids.size() );
		shapes.emplace_back( Shape{ Indices(), static_cast<Uint32>( Math::Max( 0, shape.mesh.material_ids[ 0 ] ) ) } );
		for( auto index : shape.mesh.indices )
		{
			Vector3 pos = objPoses[ index.vertex_index ];
			Vector2 uvs = objTexCoords[ Math::Max( 0, index.texcoord_index ) ];

			Uint64 posHash = Math::CalculateHash( pos );
			Uint64 uvsHash = Math::CalculateHash( uvs );

			auto it = uniqueVerticesIndices[ posHash ].find( uvsHash );
			if( it == uniqueVerticesIndices[ posHash ].end() )
			{
				it = uniqueVerticesIndices[ posHash ].emplace( uvsHash, static_cast< Uint32 >( poses.size() ) ).first;
				poses.emplace_back( pos );
				texCoords.emplace_back( uvs );
			}
			else
			{
				FORGE_ASSERT( pos == poses[ it->second ] && uvs == texCoords[ it->second ] );
			}

			shapes.back().m_indices.emplace_back( it->second );
		}
	}

	renderer::Vertices vertices( poses, texCoords );

	Math::Random rng;
	if( materialsData )
	{
		auto& materials = reader.GetMaterials();
		for( const auto& material : reader.GetMaterials() )
		{
			materialsData->emplace_back( MaterialData{ GetRenderer().CreateConstantBuffer(), std::experimental::filesystem::v1::path( material.diffuse_texname ).filename().string() } );
			materialsData->back().m_buffer->AddData( "diffuseColor", Vector4( material.diffuse[ 0 ], material.diffuse[ 1 ], material.diffuse[ 2 ], 1.0f ) );
			materialsData->back().m_buffer->UpdateBuffer();
		}
	}
	else
	{
		materialsData->resize(1);
	}

	auto result = std::make_shared< renderer::Model >( GetRenderer(), vertices, shapes );

	return result;
}
