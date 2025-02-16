#include "Fpch.h"
#include "IMGUIHelpers.h"
#include "imgui.h"
#include "../Renderer/ITexture.h"
#include "../Renderer/IShaderResourceView.h"

void forge::imgui::DrawFoldableTextureView( const std::string& name, const renderer::ITexture& texture, Bool open /*= true*/, std::function<void()> preTextureDrawing /*= nullptr */ )
{
	if ( ImGui::TreeNodeEx( name.c_str(), open ? ImGuiTreeNodeFlags_DefaultOpen : 0 ) )
	{
		if ( preTextureDrawing )
		{
			preTextureDrawing();
		}

		const Vector2& size = texture.GetSize();
		Float contentWidth = ImGui::GetWindowWidth() - ImGui::GetCursorPosX();
		const ImVec2 textureSize = ImVec2( contentWidth, contentWidth * size.Y / size.X );
		ImGui::Image( reinterpret_cast< ImTextureID >( texture.GetShaderResourceView()->GetRawSRV() ), textureSize );
		if ( ImGui::IsItemHovered() && ImGui::IsMouseDown( ImGuiMouseButton_Right ) )
		{
			ImVec2 pos = ImGui::GetCursorScreenPos();
			ImGuiIO& io = ImGui::GetIO();
			ImGui::BeginTooltip();
			Float region_sz = 32.0f;
			Float region_x = io.MousePos.x - pos.x - region_sz * 0.5f;
			Float region_y = textureSize.y - Math::Abs( io.MousePos.y - pos.y - region_sz * 0.5f );
			Float zoom = 4.0f;
			Math::Clamp( 0.0f, textureSize.x - region_sz, region_x );
			Math::Clamp( 0.0f, textureSize.y - region_sz, region_y );
			ImVec2 uv0 = ImVec2( ( region_x ) / textureSize.x, ( region_y ) / textureSize.y );
			ImVec2 uv1 = ImVec2( ( region_x + region_sz ) / textureSize.x, ( region_y + region_sz ) / textureSize.y );
			ImGui::Image( reinterpret_cast< ImTextureID >( texture.GetShaderResourceView()->GetRawSRV() ), ImVec2( region_sz * zoom, region_sz * zoom ), uv0, uv1 );
			ImGui::EndTooltip();
		}
		ImGui::TreePop();
	}
}

Bool forge::imgui::InputText( const Char* label, std::string& text )
{
	struct ResizeCallback
	{
		static Int32 Callback( ImGuiInputTextCallbackData* data )
		{
			if ( data->EventFlag == ImGuiInputTextFlags_CallbackResize )
			{
				std::string* str = static_cast< std::string* >( data->UserData );
				FORGE_ASSERT( str->data() == data->Buf );
				str->resize( data->BufTextLen );
				data->Buf = str->data();
			}

			return 0;
		}
	};

	return ImGui::InputText( label, text.data(), text.size() + 1, ImGuiInputTextFlags_CallbackResize, &ResizeCallback::Callback, &text );
}

std::string forge::imgui::PrefixLabel( const Char* label )
{
	float width = ImGui::CalcItemWidth();

	float x = ImGui::GetCursorPosX();
	ImGui::SetNextItemWidth( width * 0.5f );
	ImGui::Text( label );
	ImGui::SameLine();
	ImGui::SetCursorPosX( x + width * 0.5f + ImGui::GetStyle().ItemInnerSpacing.x );
	ImGui::SetNextItemWidth( -1 );

	return forge::String::Printf( "##%s", label );
}
