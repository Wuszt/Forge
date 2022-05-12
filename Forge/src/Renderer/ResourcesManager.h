#pragma once

namespace renderer
{
	class IRenderer;
	class IModelsLoader;
	class ITexture;

	struct MaterialData
	{
		std::unique_ptr< renderer::ConstantBuffer > m_buffer;
		std::string m_diffuseTextureName;
		std::string m_normalTextureName;
		std::string m_alphaTextureName;
	};

	class ResourcesManager
	{
	public:
		ResourcesManager( const forge::DepotsContainer& depotsContainer, IRenderer& renderer );
		~ResourcesManager();

		struct ModelMaterialsPackage
		{
			std::shared_ptr< renderer::Model > m_model;
			std::vector< MaterialData > m_materialsData;
		};

		std::shared_ptr< const ModelMaterialsPackage > LoadModel( const std::string& path );
		std::shared_ptr< const ITexture > LoadTexture( const std::string& path );

		const std::unordered_map< std::string, std::shared_ptr< ITexture > >& GetAllLoadedTextures() const
		{
			return m_texturesCache;
		}

	private:
		std::unordered_map< std::string, std::shared_ptr< ModelMaterialsPackage > > m_modelsMaterialsCache;
		std::unordered_map< std::string, std::shared_ptr< ITexture > > m_texturesCache;
		std::unique_ptr< IModelsLoader > m_modelsLoader;
		std::unique_ptr< ITexturesLoader > m_texturesLoader;
	};
}

