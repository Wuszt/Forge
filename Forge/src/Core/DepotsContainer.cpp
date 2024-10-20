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
