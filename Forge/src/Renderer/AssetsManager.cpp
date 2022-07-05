#include "Fpch.h"
#include "AssetsManager.h"
#include "../Core/DepotsContainer.h"
#include "../Core/IAssetsLoader.h"
#include "../Core/IAsset.h"

forge::AssetsManager::AssetsManager( const forge::DepotsContainer& depotsContainer )
	: m_depotsContainer( depotsContainer )
{}

forge::AssetsManager::~AssetsManager() = default;

std::shared_ptr< forge::IAsset > forge::AssetsManager::GetAssetInternal( const std::string& path )
{
	auto cachedAsset = m_assetsCache.find( path );
	if( cachedAsset != m_assetsCache.end() )
	{
		return cachedAsset->second;
	}

	std::string finalPath;
	if( !m_depotsContainer.TryToGetExistingFilePath( path, finalPath ) )
	{
		return nullptr;
	}

	Uint32 extensionStartIndex = finalPath.find_last_of( '.' ) + 1u;

	if( extensionStartIndex >= finalPath.size() )
	{
		return nullptr;
	}

	std::string extension = finalPath.substr( extensionStartIndex );

	std::shared_ptr< IAsset > loadedAsset = m_assetsLoaders[ extension ]->LoadAsset( finalPath );
	m_assetsCache.emplace( path, loadedAsset );
	return loadedAsset;
}
