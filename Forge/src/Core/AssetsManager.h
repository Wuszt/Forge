#pragma once

namespace forge
{
	class IRenderer;
	class IAsset;
	class IAssetsLoader;
	class DepotsContainer;

	class AssetsManager
	{
	public:
		AssetsManager( const forge::DepotsContainer& depotsContainer );
		~AssetsManager();

		template< class T >
		std::vector < std::shared_ptr< T > > GetLoadedAssetsOfType() const
		{
			std::vector< std::shared_ptr< T > > result;
			for( const auto& pair : m_assetsCache )
			{
				for( auto asset : pair.second )
				{
					if( asset->IsA< T >() )
					{
						result.emplace_back( std::dynamic_pointer_cast< T >( asset ) );
					}
				}			
			}

			return result;
		}

		template< class T >
		std::shared_ptr< T > GetAsset( const std::string& path )
		{
			for( auto asset : GetAssetsInternal( path ) )
			{
				std::shared_ptr< T > result = std::dynamic_pointer_cast< T >( asset );
				if( result )
				{
					return result;
				}
			}

			return nullptr;
		}

		template< class T, class... Args >
		void AddAssetsLoader( Args&&... args )
		{
			std::shared_ptr< IAssetsLoader > loader = std::make_shared< T >( std::forward< Args >( args )... );
			for( auto extension : loader->GetHandledExtensions() )
			{
				FORGE_ASSERT( m_assetsLoaders.find( extension ) == m_assetsLoaders.end() );
				m_assetsLoaders.emplace( extension, loader );
			}
		}

	private:
		forge::ArraySpan< std::shared_ptr< forge::IAsset > > GetAssetsInternal( const std::string& path );

		std::unordered_map< std::string, std::vector< std::shared_ptr< IAsset > > > m_assetsCache;
		std::unordered_map< std::string, std::shared_ptr< IAssetsLoader > > m_assetsLoaders;
		const forge::DepotsContainer& m_depotsContainer;
	};
}

