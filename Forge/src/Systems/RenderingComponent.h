#pragma once

namespace renderer
{
	class ConstantBuffer;
	class IConstantBufferImpl;
	class IMesh;
	class Renderable;
	class Material;
}

namespace forge
{
	struct RenderingComponentData
	{
		renderer::Renderable* m_renderable;
	};

	class RenderingComponent : public DataComponent< RenderingComponentData >
	{
	public:
		using DataComponent::DataComponent;

		virtual void OnAttach( EngineInstance& engineInstance ) override;

		FORGE_INLINE const renderer::Renderable* GetRenderable() const
		{
			return m_renderable.get();
		}

		FORGE_INLINE renderer::Renderable* GetRenderable()
		{
			return m_renderable.get();
		}

	private:
		std::unique_ptr< renderer::Renderable > m_renderable;
	};
}

