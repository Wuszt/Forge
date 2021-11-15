#pragma once

namespace renderer
{
	class IRenderer;
	class IModelsLoader;

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

		const ModelMaterialsPackage& LoadModel( const std::string& path );

	private:
		std::unordered_map< std::string, ModelMaterialsPackage > m_modelsMaterialsCache;
		std::unique_ptr< IModelsLoader > m_modelsLoader;
	};
}

