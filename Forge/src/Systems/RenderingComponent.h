#pragma once

namespace renderer
{
	class ConstantBuffer;
	class IConstantBufferImpl;
}

namespace forge
{
	struct RenderingComponentData
	{
		renderer::IConstantBufferImpl* m_constantBufferImplementation;
	};

	class RenderingComponent : public DataComponent< RenderingComponentData >
	{
	public:
		using DataComponent::DataComponent;

		virtual void OnAttach( EngineInstance& engineInstance ) override;

		renderer::ConstantBuffer* GetConstantBuffer() const
		{
			return m_constantBuffer.get();
		}

	private:
		std::unique_ptr< renderer::ConstantBuffer > m_constantBuffer;
	};
}

