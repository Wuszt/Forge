#include "Fpch.h"
#include "ResourcesManager.h"
#include "TinyObjModelsLoader.h"

renderer::ResourcesManager::ResourcesManager( IRenderer& renderer )
{
	m_modelsLoader = std::make_unique< renderer::TinyObjModelsLoader >( renderer );
}

renderer::ResourcesManager::~ResourcesManager() = default;

const renderer::ResourcesManager::ModelMaterialsPackage& renderer::ResourcesManager::LoadModel( const std::string& path )
{
	auto result = m_modelsMaterialsCache.find( path );
	if( result == m_modelsMaterialsCache.end() )
	{
		ModelMaterialsPackage package;
		package.m_model = m_modelsLoader->LoadModel( path, &package.m_materialsData );
		return m_modelsMaterialsCache.emplace( path, std::move( package ) ).first->second;
	}

	return result->second;
}
