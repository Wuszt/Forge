#pragma once

namespace renderer
{
	class IVertexShader;
	class IPixelShader;
	class IRenderer;

	class Material
	{
	public:
		Material( renderer::IRenderer& renderer, const std::string& vsPath, const std::string& psPath );

		FORGE_INLINE const renderer::IVertexShader* GetVertexShader() const
		{
			return m_vertexShader;
		}

		FORGE_INLINE const renderer::IPixelShader* GetPixelShader() const
		{
			return m_pixelShader;
		}

	private:
		const renderer::IVertexShader* m_vertexShader = nullptr;
		const renderer::IPixelShader* m_pixelShader = nullptr;
	};
}

