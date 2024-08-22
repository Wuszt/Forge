#include "fpch.h"
#include "DepotsContainer.h"
#include <filesystem>
#include "Path.h"

forge::DepotsContainer::DepotsContainer( const forge::Path& appDepotName )
	: m_appDepot( GetDepotsPath(), appDepotName )
	, m_engineDepot( GetDepotsPath(), forge::Path( "Engine" ) )
	, m_depotsPath( std::filesystem::current_path().parent_path().parent_path().string() + "\\Depots" )
{}

forge::Depot::Depot( const forge::Path& depotsAbsolutePath, const forge::Path& rootPath )
	: m_rootPath( depotsAbsolutePath.AsString() + rootPath.AsString() )
{}

forge::Path forge::Depot::GetAbsolutePath( const forge::Path& localPath ) const
{
	forge::Path result = m_rootPath;
	result.Append( localPath );
	return result;
}

bool forge::Depot::ContainsFile( const forge::Path& localPath ) const
{
	return std::filesystem::exists( GetAbsolutePath( localPath ).Get() );
}
