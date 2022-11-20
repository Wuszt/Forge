#pragma once

namespace renderer
{
	class ConstantBuffer;
	class IConstantBufferImpl;
	class Renderable;
	class Material;
}

namespace forge
{
	struct RenderingComponentData
	{
		DECLARE_STRUCT( RenderingComponentData );

		renderer::Renderable* m_renderable;
	};

	class RenderingComponent : public DataComponent< RenderingComponentData >
	{
		DECLARE_TYPE( RenderingComponent, IComponent );
	public:
		RenderingComponent();
		~RenderingComponent();

		void LoadMeshAndMaterial( const std::string& path );

		const renderer::Renderable* GetRenderable() const
		{
			return m_renderable.get();
		}

		renderer::Renderable* GetRenderable()
		{
			return m_renderable.get();
		}

	private:
		std::unique_ptr< renderer::Renderable > m_renderable; //todo: why pointer?
	};
}

