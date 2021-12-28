#include "Fpch.h"
#include "D3D11VertexShader.h"
#include "D3D11Device.h"

namespace d3d11
{
	D3D11VertexShader::D3D11VertexShader( D3D11RenderContext& context, const D3D11Device& device, const std::string& path )
		: m_context( context )
	{
		auto wstr = std::wstring( path.begin(), path.end() );
		LPCWSTR wPath = wstr.c_str();
		FORGE_ASSURE( D3DCompileFromFile( wPath, 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VS", "vs_4_0", D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_DEBUG, 0, &m_buffer, 0 ) == S_OK );

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
