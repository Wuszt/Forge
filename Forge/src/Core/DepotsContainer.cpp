#include "fpch.h"
#include "DepotsContainer.h"
#include <filesystem>

forge::DepotsContainer::DepotsContainer( const std::string& appDepotName )
	: m_appDepot( GetDepotsPath(), appDepotName )
	, m_engineDepot( GetDepotsPath(), "Engine" )
	, m_depotsPath( std::filesystem::current_path().parent_path().parent_path().string() + "\\Depots" )
{}

forge::Depot::Depot( const std::string& depotsAbsolutePath, const std::string& rootPath )
	: m_rootPath( depotsAbsolutePath + "\\" + rootPath + "\\")
{}

std::string forge::Depot::GetAbsolutePath( const std::string& localPath ) const
{
	return m_rootPath + localPath;
}

bool forge::Depot::ContainsFile( const std::string& localPath ) const
{
	return std::filesystem::exists( GetAbsolutePath( localPath ) );
}
