#include "Fpch.h"
#include "SceneEditor.h"
#include "../../External/imgui/imgui.h"
#include "../Systems/SceneRenderingSystem.h"
#include "../GameEngine/EngineInstance.h"
#include "../GameEngine/SystemsManager.h"
#include "../GameEngine/RenderingManager.h"
#include "../Renderer/Renderer.h"
#include "../IMGUI/IMGUIHelpers.h"
#include "../Renderer/ITexture.h"
#include "../Renderer/IShaderResourceView.h"

editor::SceneEditor::SceneEditor( forge::EngineInstance& engineInstance )
	: IPanel( engineInstance )
{
	m_targetTexture = GetEngineInstance().GetRenderingManager().GetRenderer().CreateTexture( 512u, 512u, 
		renderer::ITexture::Flags::BIND_RENDER_TARGET | renderer::ITexture::Flags::BIND_SHADER_RESOURCE, renderer::ITexture::Format::R8G8B8A8_UNORM, renderer::ITexture::Type::Texture2D, renderer::ITexture::Format::R8G8B8A8_UNORM);

	systems::SceneRenderingSystem& sceneRenderingSystem = GetEngineInstance().GetSystemsManager().GetSystem<systems::SceneRenderingSystem>();
	sceneRenderingSystem.SetTargetTexture(m_targetTexture.get());
}

void editor::SceneEditor::Draw()
{
	ImGui::Begin("SceneEditor");
	if ( m_targetTexture->GetSize() != forge::imgui::CastToForge( ImGui::GetWindowSize() ) )
	{
		m_targetTexture->Resize( forge::imgui::CastToForge( ImGui::GetWindowSize() ) );
	}

	ImGui::Image( m_targetTexture->GetShaderResourceView()->GetRawSRV(), forge::imgui::CastToImGui( m_targetTexture->GetSize() ) );
	ImGui::End();
}
