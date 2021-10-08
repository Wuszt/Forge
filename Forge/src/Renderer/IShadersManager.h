#pragma once

namespace renderer
{
	class IShadersManager
	{
	public:
		IShadersManager();
		~IShadersManager();

		const IVertexShader* GetVertexShader( const std::string& path );
		const IPixelShader* GetPixelShader( const std::string& path );

	protected:
		virtual std::unique_ptr< const IVertexShader > CreateVertexShader( const std::string& path ) const = 0;
		virtual std::unique_ptr< const IPixelShader > CreatePixelShader( const std::string& path ) const = 0;

	private:
		std::unordered_map< std::string, std::unique_ptr< const IVertexShader > > m_vertexShaders;
		std::unordered_map< std::string, std::unique_ptr< const IPixelShader > > m_pixelShaders;
	};
}

