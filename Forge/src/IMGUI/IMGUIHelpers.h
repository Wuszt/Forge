#pragma once

namespace renderer
{
	class ITexture;
}

namespace forge
{
	namespace imgui
	{
		void DrawFoldableTextureView( const std::string& name, const renderer::ITexture& texture, Bool open = true, std::function<void()> preTextureDrawing = nullptr );

		static ImVec2 CastToImGui( const Vector2& vec )
		{
			return ImVec2(vec.X, vec.Y);
		}

		static Vector2 CastToForge( const ImVec2& vec )
		{
			return Vector2( vec.x, vec.y );
		}

		Bool InputText( const Char* label, std::string& text );

		std::string PrefixLabel( const Char* label );
	}
}