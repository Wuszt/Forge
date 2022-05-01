#pragma once

namespace renderer
{
	class ITexture;
}

namespace forge
{
	namespace imgui
	{
		void DrawTexture( const std::string& name, renderer::ITexture& texture, Bool open = true, std::function<void()> preTextureDrawing = nullptr );
	}
}