#include "Fpch.h"
#include "ResourcesManager.h"
#include "TinyObjModelsLoader.h"
#include "ITexturesLoader.h"

renderer::ResourcesManager::ResourcesManager( IRenderer& renderer )
{
	m_modelsLoader = std::make_unique< renderer::TinyObjModelsLoader >( renderer );
	m_texturesLoader = renderer.CreateTexturesLoader();
}

renderer::ResourcesManager::~ResourcesManager() = default;

std::shared_ptr< const renderer::ResourcesManager::ModelMaterialsPackage > renderer::ResourcesManager::LoadModel( const std::string& path )
{
	auto result = m_modelsMaterialsCache.find( path );
	if( result == m_modelsMaterialsCache.end() )
	{
		std::shared_ptr< ModelMaterialsPackage > package = std::make_shared< ModelMaterialsPackage >();
		package->m_model = m_modelsLoader->LoadModel( path, &package->m_materialsData );

		if( !package->m_model )
		{
			return nullptr;
		}

		return m_modelsMaterialsCache.emplace( path, std::move( package ) ).first->second;
	}

	return result->second;
}

std::shared_ptr< const renderer::ITexture > renderer::ResourcesManager::LoadTexture( const std::string& path )
{
	auto result = m_texturesCache.find( path );
	if( result == m_texturesCache.end() )
	{
		auto texture = m_texturesLoader->LoadTexture( path );
		return m_texturesCache.emplace( path, texture ).first->second;
	}

	return result->second;	
}
