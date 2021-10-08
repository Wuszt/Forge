#pragma once
#include "../Renderer/IShadersManager.h"

namespace d3d11
{
	class D3D11RenderContext;
	class D3D11Device;

	class D3D11ShadersManager : public renderer::IShadersManager
	{
	public:
		D3D11ShadersManager( D3D11Device& device, D3D11RenderContext& context );
	protected:
		virtual std::unique_ptr< const renderer::IVertexShader > CreateVertexShader( const std::string& path ) const override;
		virtual std::unique_ptr< const renderer::IPixelShader > CreatePixelShader( const std::string& path ) const override;

	private:
		D3D11Device& m_device;
		D3D11RenderContext& m_context;
	};
}

