#pragma once

namespace renderer
{
	class IRenderer;
	class IModelsLoader;
	class ITexture;

	class ResourcesManager
	{
	public:
		ResourcesManager( IRenderer& renderer );
		~ResourcesManager();

		struct ModelMaterialsPackage
		{
			std::shared_ptr< renderer::Model > m_model;
			std::vector< std::unique_ptr< renderer::ConstantBuffer > > m_materialsData;
		};

		std::shared_ptr< const ModelMaterialsPackage > LoadModel( const std::string& path );
		std::shared_ptr< const ITexture > LoadTexture( const std::string& path );

	private:
		std::unordered_map< std::string, std::shared_ptr< ModelMaterialsPackage > > m_modelsMaterialsCache;
		std::unordered_map< std::string, std::shared_ptr< ITexture > > m_texturesCache;
		std::unique_ptr< IModelsLoader > m_modelsLoader;
		std::unique_ptr< ITexturesLoader > m_texturesLoader;
	};
}

