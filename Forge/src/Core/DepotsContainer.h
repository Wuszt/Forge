#pragma once
#include "Path.h"
#include <filesystem>

namespace forge
{
	class Depot
	{
	public:
		Depot( const forge::Path& depotsAbsolutePath, const forge::Path& rootPath );

		const forge::Path& GetPath() const
		{
			return m_rootPath;
		}

	private:
		const forge::Path m_rootPath;
	};

	class DepotsContainer
	{
	public:
		DepotsContainer( const forge::Path& appDepotName );

		const Depot& GetApplicationDepot() const
		{
			return m_appDepot;
		}

		const Depot& GetEngineDepot() const
		{
			return m_engineDepot;
		}

		const forge::Path& GetDepotsPath() const
		{
			return m_depotsPath;
		}

		bool TryToGetExistingFilePath( const forge::Path& path, forge::Path& outAbsolutePath ) const
		{
			if( std::filesystem::exists( path.Get() ) )
			{
				outAbsolutePath = path;
				return true;
			}

			outAbsolutePath = GetDepotsPath();
			outAbsolutePath.Append( path );

			if ( std::filesystem::exists( outAbsolutePath.Get() ) )
			{
				return true;
			}

			outAbsolutePath = {};
			return false;
		}

	private:
		forge::Path m_depotsPath;

		const Depot m_appDepot;
		const Depot m_engineDepot;
	};
}

