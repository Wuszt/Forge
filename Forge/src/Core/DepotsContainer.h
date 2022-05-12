#pragma once

namespace forge
{
	class Depot
	{
	public:
		Depot( const std::string& rootPath );
		std::string GetAbsolutePath( const std::string& localPath ) const;
		bool ContainsFile( const std::string& localPath ) const;

	private:
		const std::string m_rootPath;
	};

	class DepotsContainer
	{
	public:
		DepotsContainer( const std::string& appDepotName );

		const Depot& GetApplicationDepot() const
		{
			return m_appDepot;
		}

		const Depot& GetEngineDepot() const
		{
			return m_engineDepot;
		}

		bool TryToGetExistingFilePath( const std::string& localPath, std::string& outAbsolutePath ) const
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
		const Depot m_appDepot;
		const Depot m_engineDepot;
	};
}

