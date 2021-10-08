#include "Fpch.h"
#include "IShadersManager.h"

renderer::IShadersManager::IShadersManager() = default;
renderer::IShadersManager::~IShadersManager() = default;

const renderer::IVertexShader* renderer::IShadersManager::GetVertexShader( const std::string& path )
{
	auto& shader = m_vertexShaders[ path ];

	if( !shader )
	{
		shader = CreateVertexShader( path );
	}

	return shader.get();
}

const renderer::IPixelShader* renderer::IShadersManager::GetPixelShader( const std::string& path )
{
	auto& shader = m_pixelShaders[ path ];

	if( !shader )
	{
		shader = CreatePixelShader( path );
	}

	return shader.get();
}
