#pragma once
#include "../Renderer/ConstantBuffer.h"
#include "D3D11Device.h"
#include "D3D11RenderContext.h"

struct ID3D11Buffer;

namespace d3d11
{
	class D3D11ConstantBufferImpl : public renderer::IConstantBufferImpl
	{
	public:
		D3D11ConstantBufferImpl( D3D11RenderContext* contextPtr, D3D11Device* device, void* data, Uint32 dataSize );
		~D3D11ConstantBufferImpl();

		virtual void SetVS( Uint32 index ) override;
		virtual void SetPS( Uint32 index ) override;

	private:
		ID3D11Buffer* m_buff;
		D3D11RenderContext* m_context;
		void* m_data;
	};
}