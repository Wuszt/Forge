#pragma once
#include "../Renderer/IDepthStencilBuffer.h"

namespace d3d11
{
	class D3D11Texture;
	class D3D11DepthStencilView;

	class D3D11DepthStencilBuffer : public renderer::IDepthStencilBuffer
	{
	public:
		D3D11DepthStencilBuffer( const D3D11Device& device, const D3D11RenderContext& context, Uint32 width, Uint32 height, Bool cubeTexture );

		virtual std::shared_ptr< renderer::ITexture > GetTexture() const override;

		virtual renderer::IDepthStencilView& GetView( Uint32 index = 0u ) const override;

		void CreateDepthStencilViews();
		void CreateTexture( Uint32 width, Uint32 height, Bool cubeTexture );

		virtual void Resize( Uint32 width, Uint32 height ) override;

	private:
		std::shared_ptr< D3D11Texture > m_texture;
		std::vector< std::shared_ptr< D3D11DepthStencilView > > m_dsvs;
		const D3D11Device& m_device;
		const D3D11RenderContext& m_context;
	};
}
