#pragma once
#include "IRenderingPass.h"

namespace renderer
{
	class IRenderer;
	struct ShaderDefine;

	class FullScreenRenderingPass : public IRenderingPass
	{
	public:
		FullScreenRenderingPass( IRenderer& renderer, const std::string& fullscreenEffectPath, forge::ArraySpan< renderer::ShaderDefine > shaderDefines );
		FullScreenRenderingPass( IRenderer& renderer, const std::string& fullscreenVSPath, const std::string& fullscreenEffectPath, forge::ArraySpan< renderer::ShaderDefine > shaderDefines );

		void Draw( std::vector< IShaderResourceView* > input );

		FORGE_INLINE void SetShaderDefines( forge::ArraySpan< renderer::ShaderDefine > shaderDefines )
		{
			m_shaderDefines = { shaderDefines.begin(), shaderDefines.end() };
		}

		FORGE_INLINE void SetVSName( const std::string& vsName )
		{
			m_vertexShaderName = vsName;
		}

		FORGE_INLINE void SetPSName( const std::string& psName )
		{
			m_vertexShaderName = psName;
		}

	private:
		std::string m_vertexShaderName;;
		std::string m_pixelShaderName;
		std::vector< renderer::ShaderDefine > m_shaderDefines;
	};
}

