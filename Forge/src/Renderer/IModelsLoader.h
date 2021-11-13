#pragma once

namespace renderer
{
	class IModelsLoader
	{
	public:
		IModelsLoader( IRenderer& renderer );
		virtual ~IModelsLoader();

		virtual std::shared_ptr< renderer::Model > LoadModel( const std::string& path, std::vector< std::unique_ptr< renderer::ConstantBuffer > >* materialsData = nullptr ) = 0;

	protected:
		FORGE_INLINE IRenderer& GetRenderer() const
		{
			return m_renderer;
		}

	private:
		IRenderer& m_renderer;
	};
}

