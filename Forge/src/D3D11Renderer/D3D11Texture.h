#pragma once
#include "../Renderer/ITexture.h"
#include "D3D11ShaderResourceView.h"

struct ID3D11Texture2D;
struct D3D11_TEXTURE2D_DESC;

namespace d3d11
{
	class D3D11ShaderResourceView;
	class D3D11Device;
	class D3D11RenderContext;
	class D3D11Swapchain;

	class D3D11Texture : public renderer::ITexture
	{
	public:
		D3D11Texture( const D3D11Device& device, const D3D11RenderContext& context, ID3D11Texture2D& texture, DXGI_FORMAT srvFormat = DXGI_FORMAT_UNKNOWN );
		D3D11Texture( const D3D11Device& device, const D3D11RenderContext& context, const D3D11_TEXTURE2D_DESC& desc, DXGI_FORMAT srvFormat = DXGI_FORMAT_UNKNOWN );
		D3D11Texture( const D3D11Device& device, const D3D11RenderContext& context, Uint32 width, Uint32 height, Flags flags, Format format, renderer::ITexture::Type textureType, Format srvFormat = ITexture::Format::Unknown );
		
		~D3D11Texture();

		virtual void Resize( const Vector2& size );

		ID3D11Texture2D* GetTexture() const
		{
			return m_texture;
		}

		virtual const D3D11ShaderResourceView* GetShaderResourceView() const override
		{
			return m_srv.get();
		}

		virtual D3D11RenderTargetView* GetRenderTargetView() const override
		{
			return m_rtv.get();
		}

		virtual forge::Callback< const Vector2& >& GetOnResizedCallback() override
		{
			return m_onResizedCallback;
		}

		virtual Vector2 GetSize() const override;

		virtual renderer::ITexture::Type GetType() const override
		{
			return m_textureType;
		}

	protected:
		void SetTexture( ID3D11Texture2D& texture )
		{
			m_texture = &texture;
		}

		void CreateViewsIfRequired( Uint32 flags, Uint32 srvFormat );

		void ReleaseResources()
		{
			m_texture->Release();
			m_texture = nullptr;
			m_srv = nullptr;
			m_rtv = nullptr;
		}

		void InvokeResizeCallback( const Vector2& size ) const
		{
			m_onResizedCallback.Invoke( size );
		}

	private:
		const D3D11Device& m_device;
		const D3D11RenderContext& m_context;
		ID3D11Texture2D* m_texture = nullptr;
		std::unique_ptr< D3D11ShaderResourceView > m_srv;
		std::unique_ptr< D3D11RenderTargetView > m_rtv;
		forge::Callback< const Vector2& > m_onResizedCallback;
		renderer::ITexture::Type m_textureType;
	};
}
