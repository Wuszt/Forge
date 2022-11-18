#pragma once

#include "IShader.h"

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

		enum class TextureType
		{
			Diffuse,
			Normal,
			Alpha,
			Count
		};

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

		FORGE_INLINE forge::ArraySpan< const std::shared_ptr< const ITexture > > GetTextures() const
		{
			return m_textures;
		}

		void SetShaders( const std::string& vsPath, const std::string& psPath, renderer::RenderingPass renderingPass );
		void SetRenderingPass( renderer::RenderingPass renderingPass );
		void SetTexture( std::shared_ptr< const ITexture > texture, Material::TextureType textureType );

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
		IRenderer& m_renderer;

		std::vector< renderer::ShaderDefine > m_shadersDefines;

		std::shared_ptr< const ITexture > m_textures[ static_cast< Uint32 > ( TextureType::Count ) ] = { nullptr };

		renderer::RenderingPass m_renderingPass = static_cast< renderer::RenderingPass >( 0 );
		forge::CallbackToken m_onShadersClearCache;

		std::string m_vertexShaderPath;
		std::string m_pixelShaderPath;
	};
}

