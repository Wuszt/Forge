#include "Fpch.h"
#include "D3D11PixelShader.h"
#include "D3D11Device.h"

namespace d3d11
{
	D3D11PixelShader::D3D11PixelShader( const D3D11Device& device, D3D11RenderContext& context, const std::string& path, forge::ArraySpan< const renderer::ShaderDefine > defines )
		: m_context( context )
	{
		std::vector< D3D_SHADER_MACRO > macros;
		for( const auto& define : defines )
		{
			macros.push_back( D3D_SHADER_MACRO{ define.GetName().c_str(), define.GetDefine().c_str()});
		}

		macros.push_back( { nullptr, nullptr } );

		auto wstr = std::wstring( path.begin(), path.end() );
		LPCWSTR wPath = wstr.c_str();
		ID3DBlob* errorMsg;
		HRESULT result = D3DCompileFromFile( wPath, macros.data(), D3D_COMPILE_STANDARD_FILE_INCLUDE, "PS", "ps_4_0", D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_DEBUG, 0, &m_buffer, &errorMsg );

		if( errorMsg )
		{
			std::string definesAsString;

			for( const auto& define : defines )
			{
				definesAsString += forge::String::Printf( "{ %s %s },", define.GetName().c_str(), define.GetDefine().c_str());
			}

			if( result == S_OK )
			{
				FORGE_LOG_WARNING( "Pixel Shader [%s] [%s] compilation warning: %s", path.c_str(), definesAsString.c_str(), static_cast< char* >( errorMsg->GetBufferPointer() ) );
			}
			else
			{
				FORGE_LOG_ERROR( "Pixel Shader [%s] [%s] compilation error: %s", path.c_str(), definesAsString.c_str(), static_cast< char* >( errorMsg->GetBufferPointer() ) );
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