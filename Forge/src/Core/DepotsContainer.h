#pragma once
#include "Path.h"

namespace forge
{
	class Depot
	{
	public:
		Depot( const forge::Path& depotsAbsolutePath, const forge::Path& rootPath );
		forge::Path GetAbsolutePath( const forge::Path& localPath ) const;
		bool ContainsFile( const forge::Path& localPath ) const;

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

		bool TryToGetExistingFilePath( const forge::Path& localPath, forge::Path& outAbsolutePath ) const
		{
			if( m_appDepot.ContainsFile( localPath ) )
			{
				outAbsolutePath = m_appDepot.GetAbsolutePath( localPath );
				return true;
			}

			if( m_engineDepot.ContainsFile( localPath ) )
			{
				outAbsolutePath = m_engineDepot.GetAbsolutePath( localPath );
				return true;
			}

			return false;
		}

	private:
		forge::Path m_depotsPath;

		const Depot m_appDepot;
		const Depot m_engineDepot;
	};
}

