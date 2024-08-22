#pragma once
#include "IRenderingPass.h"
#include "../Core/Path.h"

namespace renderer
{
	class Renderer;
	class ShaderDefine;

	class FullScreenRenderingPass : public IRenderingPass
	{
	public:
		FullScreenRenderingPass( Renderer& renderer, const forge::Path& fullscreenEffectPath, forge::ArraySpan< renderer::ShaderDefine > shaderDefines );
		FullScreenRenderingPass( Renderer& renderer, const forge::Path& fullscreenVSPath, const forge::Path& fullscreenEffectPath, forge::ArraySpan< renderer::ShaderDefine > shaderDefines );

		void Draw( forge::ArraySpan< const IShaderResourceView* > input );

		void SetShaderDefines( forge::ArraySpan< renderer::ShaderDefine > shaderDefines )
		{
			m_shaderDefines = { shaderDefines.begin(), shaderDefines.end() };
		}

		void SetVSPath( const forge::Path& vsPath )
		{
			m_vertexShaderPath = vsPath;
		}

		void SetPSPath( const forge::Path& psPath )
		{
			m_pixelShaderPath = psPath;
		}

	private:
		void AdjustViewportSize();

		forge::Path m_vertexShaderPath;;
		forge::Path m_pixelShaderPath;
		std::vector< renderer::ShaderDefine > m_shaderDefines;
	};
}

