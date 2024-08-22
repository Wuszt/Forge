#include "Fpch.h"
#include "AssetsManager.h"
#include "../Core/DepotsContainer.h"
#include "../Core/IAssetsLoader.h"
#include "../Core/IAsset.h"

forge::AssetsManager::AssetsManager( const forge::DepotsContainer& depotsContainer )
	: m_depotsContainer( depotsContainer )
{}

forge::AssetsManager::~AssetsManager() = default;

forge::ArraySpan< std::shared_ptr< forge::IAsset > > forge::AssetsManager::GetAssetsInternal( const forge::Path& path )
{
	{
		auto cachedAsset = m_assetsCache.find( path );
		if( cachedAsset != m_assetsCache.end() )
		{
			return cachedAsset->second;
		}
	}

	forge::Path finalPath;
	if( !m_depotsContainer.TryToGetExistingFilePath( path, finalPath ) )
	{
		return {};
	}

	const std::string extension = finalPath.GetExtension();

	if ( extension.empty() )
	{
		return {};
	}

	auto loader = m_assetsLoaders.find( extension );

	if( loader == m_assetsLoaders.end() )
	{
		FORGE_LOG_WARNING( "There is no loader which would handle %s extension", extension.c_str() );
		return {};
	}

	StopWatch sw;
	std::vector< std::shared_ptr< IAsset > > loadedAssets = loader->second->LoadAssets( finalPath );
	FORGE_LOG( "Loaded %s in %.4f seconds", finalPath.Get(), sw.GetDuration());
	 
	if( loadedAssets.empty() )
	{
		return {};
	}

	m_assetsCache.emplace( path, std::move( loadedAssets ) );
	return m_assetsCache[ path ];
}
