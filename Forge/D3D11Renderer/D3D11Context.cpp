#include "Fpch.h"
#include "D3D11Context.h"
#include "D3D11RenderTargetView.h"

D3D11Context::D3D11Context( ID3D11DeviceContext* deviceContext )
{
	m_deviceContext = deviceContext;
}

D3D11Context::~D3D11Context()
{
	m_deviceContext->Release();
}

void D3D11Context::SetRenderTarget( const D3D11RenderTargetView& renderTargetView )
{
	m_deviceContext->OMSetRenderTargets( 1, &renderTargetView.GetRenderTargetView(), NULL );
}

void D3D11Context::SetVertexShader( const D3D11VertexShader& vertexShader )
{
	m_deviceContext->VSSetShader( vertexShader.GetShader(), 0, 0 );
}

void D3D11Context::SetPixelShader( const D3D11PixelShader& pixelShader )
{
	m_deviceContext->PSSetShader( pixelShader.GetShader(), 0, 0 );
}
