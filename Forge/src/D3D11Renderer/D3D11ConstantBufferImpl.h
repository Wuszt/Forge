#pragma once
#include "../Renderer/ConstantBuffer.h"

struct ID3D11Buffer;

namespace d3d11
{
	class D3D11Device;
	class D3D11RenderContext;

	class D3D11ConstantBufferImpl : public renderer::IConstantBufferImpl
	{
	public:
		D3D11ConstantBufferImpl( D3D11Device& device, D3D11RenderContext& contextPtr );
		~D3D11ConstantBufferImpl();

		virtual void SetVS( Uint32 index ) override;
		virtual void SetPS( Uint32 index ) override;
		virtual void CreateBuffer( Uint32 dataSize ) override;
		virtual void UpdateBuffer( void* data ) override;

		ID3D11Buffer* GetBuffer() const
		{
			return m_buff;
		}

	private:
		ID3D11Buffer* m_buff;
		D3D11RenderContext& m_context;
		D3D11Device& m_device;
		Uint32 m_dataSize = 0u;
	};
}