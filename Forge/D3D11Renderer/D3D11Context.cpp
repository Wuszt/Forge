#include "Fpch.h"
#include "D3D11Context.h"
#include "D3D11RenderTargetView.h"
#include "D3D11VertexBuffer.h"
#include "D3D11IndexBuffer.h"
#include "D3D11InputLayout.h"

D3D11Context::D3D11Context( ID3D11DeviceContext* deviceContext )
{
	m_deviceContext = deviceContext;
}

D3D11Context::~D3D11Context()
{
	m_deviceContext->Release();
}

void D3D11Context::Draw( Uint32 indexCount, Uint32 offset )
{
	m_deviceContext->DrawIndexed( indexCount, offset, 0 );
}
