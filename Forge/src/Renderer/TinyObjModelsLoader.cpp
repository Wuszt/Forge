#include "Fpch.h"
#include "TinyObjModelsLoader.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "../../External/tinyobjloader/tiny_obj_loader.h"

std::shared_ptr< renderer::Model > renderer::TinyObjModelsLoader::LoadModel( const std::string& path, std::vector< std::unique_ptr< renderer::ConstantBuffer > >* materialsData )
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

	FORGE_ASSURE( reader.ParseFromFile( objFinalPath, readerConfig ) );

	std::vector< renderer::InputPosition > poses;
	for( Uint32 i = 0; i < reader.GetAttrib().vertices.size(); i += 3 )
	{
		poses.emplace_back( reader.GetAttrib().vertices[ i ], reader.GetAttrib().vertices[ i + 1u ], reader.GetAttrib().vertices[ i + 2u ] );
	}

	renderer::Vertices vertices( poses );
	std::vector< renderer::Shape > shapes;

	for( const auto& shape : reader.GetShapes() )
	{
		shapes.emplace_back( Shape{ Indices(), static_cast< Uint32 >( shape.mesh.material_ids[ 0 ] ) } );
		for( auto index : shape.mesh.indices )
		{
			shapes.back().m_indices.emplace_back( index.vertex_index );
		}
	}
	Math::Random rng;
	if( materialsData )
	{
		auto& materials = reader.GetMaterials();
		for( const auto& material : reader.GetMaterials() )
		{
			materialsData->emplace_back( GetRenderer().CreateConstantBuffer() );
			materialsData->back()->AddData( "diffuseColor", Vector4( material.diffuse[ 0 ], material.diffuse[ 1 ], material.diffuse[ 2 ], 1.0f ) );
			materialsData->back()->UpdateBuffer();
		}
	}

	auto result = std::make_shared< renderer::Model >( GetRenderer(), vertices, shapes );

	return result;
}
