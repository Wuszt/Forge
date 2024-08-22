#include "Fpch.h"
#include "D3D11VertexShader.h"
#include "D3D11Device.h"
#include "../Core/Path.h"

namespace d3d11
{
	D3D11VertexShader::D3D11VertexShader( D3D11Device& device, D3D11RenderContext& context, const forge::Path& path, forge::ArraySpan< const renderer::ShaderDefine > defines )
		: m_context( context )
	{
		std::vector< D3D_SHADER_MACRO > macros;
		for( const auto& define : defines )
		{
			macros.push_back( { define.GetName().c_str(), define.GetDefine().c_str() } );
		}

		macros.push_back( { nullptr, nullptr } );

		const auto wstr = std::wstring( path.AsString().begin(), path.AsString().end() );
		LPCWSTR wPath = wstr.c_str();
		ID3DBlob* errorMsg;
		HRESULT result = D3DCompileFromFile( wPath, macros.data(), D3D_COMPILE_STANDARD_FILE_INCLUDE, "VS", "vs_4_0", D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_DEBUG, 0, &m_buffer, &errorMsg );

		if( errorMsg )
		{
			std::string definesAsString;

			for( const auto& define : defines )
			{
				definesAsString += forge::String::Printf("{ %s %s },", define.GetName().c_str(), define.GetDefine().c_str() );
			}

			if( result == S_OK )
			{
				FORGE_LOG_WARNING( "Vertex Shader [%s] [%s] compilation warning: %s", path.Get(), definesAsString.c_str(), static_cast<char*>( errorMsg->GetBufferPointer() ) );
			}
			else
			{
				FORGE_LOG_ERROR( "Vertex Shader [%s] [%s] compilation error: %s", path.Get(), definesAsString.c_str(), static_cast<char*>( errorMsg->GetBufferPointer() ) );
			}

			errorMsg->Release();
			FORGE_ASSERT( result == S_OK );
		}

		FORGE_ASSURE( device.GetDevice()->CreateVertexShader( m_buffer->GetBufferPointer(), m_buffer->GetBufferSize(), NULL, &m_vertexShader ) == S_OK );
	}

	D3D11VertexShader::~D3D11VertexShader()
	{
		m_buffer->Release();
		m_vertexShader->Release();
	}

	void D3D11VertexShader::Set() const
	{
		m_context.GetDeviceContext()->VSSetShader( m_vertexShader, 0, 0 );
	}
}
