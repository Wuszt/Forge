#include "Fpch.h"
#include "D3D11VertexShader.h"
#include "D3D11Device.h"

D3D11VertexShader::D3D11VertexShader( D3D11Context* contextPtr, const D3D11Device& device, const std::string& path )
	: m_contextPtr( contextPtr )
{
	HRESULT result;
	auto wstr = std::wstring( path.begin(), path.end() );
	LPCWSTR wPath = wstr.c_str();
	result = D3DCompileFromFile( wPath, 0, 0, "VS", "vs_4_0", D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_DEBUG, 0, &m_buffer, 0 );

	if( result == S_OK )
	{
		result = device.GetDevice()->CreateVertexShader( m_buffer->GetBufferPointer(), m_buffer->GetBufferSize(), NULL, &m_vertexShader );
	}

	FORGE_ASSERT( result == S_OK );
}

D3D11VertexShader::~D3D11VertexShader()
{
	m_buffer->Release();
	m_vertexShader->Release();
}

void D3D11VertexShader::Set()
{
	m_contextPtr->GetDeviceContext()->VSSetShader( m_vertexShader, 0, 0 );
}
