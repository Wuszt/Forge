#include "Fpch.h"
#include "D3D11PixelShader.h"
#include "D3D11Device.h"

namespace d3d11
{
	D3D11PixelShader::D3D11PixelShader( const D3D11Device& device, D3D11RenderContext& context, const std::string& path )
		: m_context( context )
	{
		auto wstr = std::wstring( path.begin(), path.end() );
		LPCWSTR wPath = wstr.c_str();
		ID3DBlob* errorMsg;
		HRESULT result = D3DCompileFromFile( wPath, 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PS", "ps_4_0", D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_DEBUG, 0, &m_buffer, &errorMsg );

		if( errorMsg )
		{
			if( result == S_OK )
			{
				FORGE_LOG_WARNING( "Pixel Shader [%s] compilation warning: %s", path.c_str(), static_cast<char*>( errorMsg->GetBufferPointer() ) );
			}
			else
			{
				FORGE_LOG_ERROR( "Pixel Shader [%s] compilation error: %s", path.c_str(), static_cast<char*>( errorMsg->GetBufferPointer() ) );
			}

			errorMsg->Release();
			FORGE_ASSERT( result == S_OK );
		}

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