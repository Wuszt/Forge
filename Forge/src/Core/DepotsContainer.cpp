#include "fpch.h"
#include "DepotsContainer.h"
#include <filesystem>

forge::DepotsContainer::DepotsContainer( const std::string& appDepotName )
	: m_appDepot( appDepotName )
	, m_engineDepot( "Engine" )
{}

forge::Depot::Depot( const std::string& rootPath )
	: m_rootPath( std::filesystem::current_path().string() + "\\Depots\\" + rootPath + "\\" )
{}

std::string forge::Depot::GetAbsolutePath( const std::string& localPath ) const
{
	return m_rootPath + localPath;
}

bool forge::Depot::ContainsFile( const std::string& localPath ) const
{
	return std::filesystem::exists( GetAbsolutePath( localPath ) );
}
