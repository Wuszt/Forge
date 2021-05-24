#include "Fpch.h"
#include "D3D11PixelShader.h"
#include "D3D11Device.h"

D3D11PixelShader::D3D11PixelShader( const D3D11Device& device, const std::string& path )
{
	HRESULT result;
	LPCWSTR wPath = std::wstring( path.begin(), path.end() ).c_str();
	result = D3DCompileFromFile( wPath, 0, 0, "PS", "ps_4_0", D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_DEBUG, 0, &m_buffer, 0 );

	if( result == S_OK )
	{
		result = device.GetDevice()->CreatePixelShader( m_buffer->GetBufferPointer(), m_buffer->GetBufferSize(), NULL, &m_pixelShader );
	}

	FORGE_ASSERT( result == S_OK );
}

D3D11PixelShader::~D3D11PixelShader()
{
	m_buffer->Release();
	m_pixelShader->Release();
}
