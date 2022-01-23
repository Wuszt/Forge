#pragma once
#include "../Renderer/IShaderResourceView.h"

namespace d3d11
{
	class D3D11ShaderResourceView : public renderer::IShaderResourceView
	{
	public:
		D3D11ShaderResourceView( const D3D11Device& device, ID3D11Texture2D* texture, DXGI_FORMAT srvFormat );
		D3D11ShaderResourceView( ID3D11ShaderResourceView* srv );
		~D3D11ShaderResourceView();

		D3D11_SHADER_RESOURCE_VIEW_DESC GetDesc() const;

		ID3D11ShaderResourceView* GetTypedSRV() const
		{
			return m_srv;
		}

		virtual void* GetRawSRV() const override
		{
			return m_srv;
		}

	private:
		ID3D11ShaderResourceView* m_srv = nullptr;
	};
}

