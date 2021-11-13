#pragma once

namespace renderer
{
	class IVertexShader;
	class IPixelShader;
	class IRenderer;

	class Material
	{
	public:
		Material( renderer::IRenderer& renderer, const Model& model, std::unique_ptr< ConstantBuffer >&& buffer, const std::string& vsPath, const std::string& psPath );

		FORGE_INLINE const renderer::IVertexShader* GetVertexShader() const
		{
			return m_vertexShader;
		}

		FORGE_INLINE const renderer::IPixelShader* GetPixelShader() const
		{
			return m_pixelShader;
		}

		const renderer::ConstantBuffer* GetConstantBuffer() const
		{
			return m_constantBuffer.get();
		}

		renderer::ConstantBuffer* GetConstantBuffer()
		{
			return m_constantBuffer.get();
		}

		const renderer::IInputLayout* GetInputLayout() const
		{
			return m_inputLayout.get();
		}

	private:
		const renderer::IVertexShader* m_vertexShader = nullptr;
		const renderer::IPixelShader* m_pixelShader = nullptr;
		std::unique_ptr< renderer::ConstantBuffer > m_constantBuffer;
		std::unique_ptr< const IInputLayout > m_inputLayout;
	};
}

