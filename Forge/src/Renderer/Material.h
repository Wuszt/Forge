#pragma once

#include "IShader.h"
#include "IRenderingResource.h"
#include "../Core/Path.h"

namespace renderer
{
	class IInputLayout;
	class IVertexShader;
	class IPixelShader;
	class Renderer;
	class ITexture;
	class ConstantBuffer;
	class Model;
	enum class RenderingPass;

	template< class ShaderType >
	class ShaderPack;

	class Material : public IRenderingResource
	{
		RTTI_DECLARE_CLASS( Material );

	public:
		enum class TextureType
		{
			Diffuse,
			Normal,
			Alpha,
			Count
		};

		Material( renderer::Renderer& renderer, const Model& model, std::unique_ptr< ConstantBuffer >&& buffer, const forge::Path& vsPath, const forge::Path& psPath, renderer::RenderingPass renderingPass );
		Material();
		Material( Material&& );
		~Material();

		std::shared_ptr< ShaderPack< IVertexShader > > GetVertexShader() const
		{
			return m_vertexShader;
		}

		std::shared_ptr< ShaderPack< IPixelShader > > GetPixelShader() const
		{
			return m_pixelShader;
		}

		const renderer::ConstantBuffer* GetConstantBuffer() const
		{
			return m_constantBuffer.get();
		}

		renderer::ConstantBuffer* GetConstantBuffer()
		{
			return m_constantBuffer.get();
		}

		const renderer::IInputLayout* GetInputLayout() const
		{
			return m_inputLayout.get();
		}

		forge::ArraySpan< const std::shared_ptr< const ITexture > > GetTextures() const
		{
			return m_textures;
		}

		void SetShaders( const forge::Path& vsPath, const forge::Path& psPath, renderer::RenderingPass renderingPass );
		void SetRenderingPass( renderer::RenderingPass renderingPass );
		void SetTexture( std::shared_ptr< const ITexture > texture, Material::TextureType textureType );

		renderer::RenderingPass GetRenderingPass() const
		{
			return m_renderingPass;
		}

		const forge::Path& GetVertexShaderPath() const
		{
			return m_vertexShaderPath;
		}

		const forge::Path& GetPixelShaderPath() const
		{
			return m_pixelShaderPath;
		}

	private:
		std::shared_ptr< ShaderPack< IVertexShader > > m_vertexShader;
		std::shared_ptr< ShaderPack< IPixelShader > > m_pixelShader;
		std::unique_ptr< renderer::ConstantBuffer > m_constantBuffer;
		std::unique_ptr< const IInputLayout > m_inputLayout;
		Renderer* m_renderer = nullptr;

		std::vector< renderer::ShaderDefine > m_shadersDefines;

		std::shared_ptr< const ITexture > m_textures[ static_cast< Uint32 > ( TextureType::Count ) ] = { nullptr };

		renderer::RenderingPass m_renderingPass = static_cast< renderer::RenderingPass >( 0 );
		forge::CallbackToken m_onShadersClearCache;

		forge::Path m_vertexShaderPath;
		forge::Path m_pixelShaderPath;
	};
}

