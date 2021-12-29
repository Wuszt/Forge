#pragma once

namespace renderer
{
	class IVertexShader;
	class IPixelShader;
	class IRenderer;
	class ITexture;

	class Material
	{
	public:
		Material( renderer::IRenderer& renderer, const Model& model, std::unique_ptr< ConstantBuffer >&& buffer, const std::string& vsPath, const std::string& psPath );

		FORGE_INLINE const renderer::IVertexShader* GetVertexShader() const
		{
			return m_vertexShader;
		}

		FORGE_INLINE const renderer::IPixelShader* GetPixelShader() const
		{
			return m_pixelShader;
		}

		FORGE_INLINE const renderer::ConstantBuffer* GetConstantBuffer() const
		{
			return m_constantBuffer.get();
		}

		FORGE_INLINE renderer::ConstantBuffer* GetConstantBuffer()
		{
			return m_constantBuffer.get();
		}

		FORGE_INLINE const renderer::IInputLayout* GetInputLayout() const
		{
			return m_inputLayout.get();
		}

		FORGE_INLINE Uint32 GetTexturesAmount() const
		{
			return static_cast< Uint32 >( m_textures.size() );
		}

		FORGE_INLINE const ITexture* GetTexture( Uint32 index ) const
		{
			if( index >= GetTexturesAmount() )
			{
				return nullptr;
			}

			return m_textures[ index ].get();
		}

		void SetVertexShader( const std::string& path );
		void SetPixelShader( const std::string& path );
		void SetTexture( const std::string& path, Uint32 index );

#ifdef FORGE_DEBUGGING
		const std::string& GetVertexShaderPath() const
		{
			return m_debugVertexShaderPath;
		}

		const std::string& GetPixelShaderPath() const
		{
			return m_debugPixelShaderPath;
		}
#endif

	private:
		const renderer::IVertexShader* m_vertexShader = nullptr;
		const renderer::IPixelShader* m_pixelShader = nullptr;
		std::unique_ptr< renderer::ConstantBuffer > m_constantBuffer;
		std::unique_ptr< const IInputLayout > m_inputLayout;
		std::vector< std::shared_ptr< const ITexture > > m_textures; 
		IRenderer& m_renderer;

#ifdef FORGE_DEBUGGING
		std::string m_debugVertexShaderPath;
		std::string m_debugPixelShaderPath;
#endif
	};
}

