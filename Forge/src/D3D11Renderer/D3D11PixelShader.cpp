#include "Fpch.h"
#include "D3D11PixelShader.h"
#include "D3D11Device.h"

namespace d3d11
{
	D3D11PixelShader::D3D11PixelShader( D3D11RenderContext& context, const D3D11Device& device, const std::string& path )
		: m_context( context )
	{
		auto wstr = std::wstring( path.begin(), path.end() );
		LPCWSTR wPath = wstr.c_str();
		FORGE_ASSURE( D3DCompileFromFile( wPath, 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PS", "ps_4_0", D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_DEBUG, 0, &m_buffer, 0 ) == S_OK );

		FORGE_ASSURE( device.GetDevice()->CreatePixelShader( m_buffer->GetBufferPointer(), m_buffer->GetBufferSize(), NULL, &m_pixelShader ) == S_OK );
	}

	D3D11PixelShader::~D3D11PixelShader()
	{
		m_buffer->Release();
		m_pixelShader->Release();
	}

	void D3D11PixelShader::Set() const
	{
		m_context.GetDeviceContext()->PSSetShader( m_pixelShader, 0, 0 );
	}
}