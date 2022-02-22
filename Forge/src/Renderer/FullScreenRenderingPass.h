#pragma once
#include "IRenderingPass.h"

namespace renderer
{
	class IRenderer;

	class FullScreenRenderingPass : public IRenderingPass
	{
	public:
		FullScreenRenderingPass( IRenderer& renderer, const std::string& fullscreenEffectPath );
		FullScreenRenderingPass( IRenderer& renderer, const std::string& fullscreenVSPath, const std::string& fullscreenEffectPath );

		void Draw( std::vector< IShaderResourceView* > input );

	private:
		const std::string m_vertexShaderName;;
		const std::string m_pixelShaderName;
	};
}

