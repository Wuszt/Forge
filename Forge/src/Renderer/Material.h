#pragma once

namespace renderer
{
	class IVertexShader;
	class IPixelShader;
	class IRenderer;
	class ITexture;
	enum class RenderingPass;

	template< class ShaderType >
	class ShaderPack;

	class Material
	{
	public:
		Material( renderer::IRenderer& renderer, const Model& model, std::unique_ptr< ConstantBuffer >&& buffer, const std::string& vsPath, const std::string& psPath, renderer::RenderingPass renderingPass );

		FORGE_INLINE std::shared_ptr< ShaderPack< IVertexShader > > GetVertexShader() const
		{
			return m_vertexShader;
		}

		FORGE_INLINE std::shared_ptr< ShaderPack< IPixelShader > > GetPixelShader() const
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

		void SetShaders( const std::string& vsPath, const std::string& psPath, renderer::RenderingPass renderingPass );
		void SetRenderingPass( renderer::RenderingPass renderingPass );
		void SetTexture( const std::string& path, Uint32 index );

		FORGE_INLINE renderer::RenderingPass GetRenderingPass() const
		{
			return m_renderingPass;
		}

		FORGE_INLINE const std::string& GetVertexShaderPath() const
		{
			return m_vertexShaderPath;
		}

		FORGE_INLINE const std::string& GetPixelShaderPath() const
		{
			return m_pixelShaderPath;
		}

	private:
		std::shared_ptr< ShaderPack< IVertexShader > > m_vertexShader;
		std::shared_ptr< ShaderPack< IPixelShader > > m_pixelShader;
		std::unique_ptr< renderer::ConstantBuffer > m_constantBuffer;
		std::unique_ptr< const IInputLayout > m_inputLayout;
		std::vector< std::shared_ptr< const ITexture > > m_textures; 
		IRenderer& m_renderer;

		renderer::RenderingPass m_renderingPass = static_cast< renderer::RenderingPass >( 0 );
		forge::CallbackToken m_onShadersClearCache;

		std::string m_vertexShaderPath;
		std::string m_pixelShaderPath;
	};
}

