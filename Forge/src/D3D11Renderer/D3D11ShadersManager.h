#pragma once
#include "../Renderer/IShadersManager.h"

namespace renderer
{
	template< class ShaderType >
	class ShaderPack;
}

namespace d3d11
{
	class D3D11RenderContext;
	class D3D11Device;

	class D3D11ShadersManager : public renderer::IShadersManager
	{
	public:
		D3D11ShadersManager( D3D11Device& device, D3D11RenderContext& context );
	protected:
		virtual std::unique_ptr< renderer::IVertexShader > CreateVertexShader( const std::string& path, forge::ArraySpan< const renderer::ShaderDefine > defines ) const override;
		virtual std::unique_ptr< renderer::IPixelShader > CreatePixelShader( const std::string& path, forge::ArraySpan< const renderer::ShaderDefine > defines ) const override;

	private:
		D3D11Device& m_device;
		D3D11RenderContext& m_context;
	};
}

