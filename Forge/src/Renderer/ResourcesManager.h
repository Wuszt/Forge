#pragma once

namespace renderer
{
	class IRenderer;
	class IModelsLoader;
	class ITexture;

	struct MaterialData
	{
		std::unique_ptr< renderer::ConstantBuffer > m_buffer;
		std::string m_textureName;
	};

	class ResourcesManager
	{
	public:
		ResourcesManager( IRenderer& renderer );
		~ResourcesManager();

		struct ModelMaterialsPackage
		{
			std::shared_ptr< renderer::Model > m_model;
			std::vector< MaterialData > m_materialsData;
		};

		std::shared_ptr< const ModelMaterialsPackage > LoadModel( const std::string& path );
		std::shared_ptr< const ITexture > LoadTexture( const std::string& path );

		std::vector< std::shared_ptr< ITexture > > GetAllLoadedTextures() const
		{
			std::vector< std::shared_ptr< ITexture > > textures;
			textures.reserve( m_texturesCache.size() );
			for( auto it : m_texturesCache )
			{
				textures.emplace_back( it.second );
			}

			return textures;
		}

	private:
		std::unordered_map< std::string, std::shared_ptr< ModelMaterialsPackage > > m_modelsMaterialsCache;
		std::unordered_map< std::string, std::shared_ptr< ITexture > > m_texturesCache;
		std::unique_ptr< IModelsLoader > m_modelsLoader;
		std::unique_ptr< ITexturesLoader > m_texturesLoader;
	};
}

