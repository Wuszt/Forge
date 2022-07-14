#include "Fpch.h"
#include "AssetsManager.h"
#include "../Core/DepotsContainer.h"
#include "../Core/IAssetsLoader.h"
#include "../Core/IAsset.h"

forge::AssetsManager::AssetsManager( const forge::DepotsContainer& depotsContainer )
	: m_depotsContainer( depotsContainer )
{}

forge::AssetsManager::~AssetsManager() = default;

forge::ArraySpan< std::shared_ptr< forge::IAsset > > forge::AssetsManager::GetAssetsInternal( const std::string& path )
{
	{
		auto cachedAsset = m_assetsCache.find( path );
		if( cachedAsset != m_assetsCache.end() )
		{
			return cachedAsset->second;
		}
	}

	std::string finalPath;
	if( !m_depotsContainer.TryToGetExistingFilePath( path, finalPath ) )
	{
		return {};
	}

	Uint32 extensionStartIndex = finalPath.find_last_of( '.' ) + 1u;

	if( extensionStartIndex >= finalPath.size() )
	{
		return {};
	}

	std::string extension = finalPath.substr( extensionStartIndex );

	std::vector< std::shared_ptr< IAsset > > loadedAssets = m_assetsLoaders[ extension ]->LoadAssets( finalPath );
	m_assetsCache.emplace( path, std::move( loadedAssets ) );
	return m_assetsCache[ path ];
}
