#pragma once

namespace renderer
{
	struct MaterialData;

	class IModelsLoader
	{
	public:
		IModelsLoader( const forge::DepotsContainer& depotsContainer, IRenderer& renderer );
		virtual ~IModelsLoader();

		virtual std::shared_ptr< renderer::Model > LoadModel( const std::string& path, std::vector< MaterialData >* materialsData = nullptr ) = 0;

	protected:
		FORGE_INLINE IRenderer& GetRenderer() const
		{
			return m_renderer;
		}

		const forge::DepotsContainer& m_depotsContainer;

	private:
		IRenderer& m_renderer;
	};
}

