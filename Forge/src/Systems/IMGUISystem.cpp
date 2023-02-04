#include "Fpch.h"

#ifdef FORGE_IMGUI_ENABLED
#include "IMGUISystem.h"
#include "../IMGUI/PublicDefaults.h"
#include "../IMGUI/IMGUIInstance.h"
#include "../Core/IWindow.h"

IMPLEMENT_TYPE( systems::IMGUISystem );

systems::IMGUISystem::IMGUISystem() = default;
systems::IMGUISystem::~IMGUISystem() = default;

void systems::IMGUISystem::OnInitialize()
{
	m_imguiInstance = std::make_unique< forge::IMGUIInstance >( GetEngineInstance().GetWindow(), GetEngineInstance().GetRenderer() );
	m_preUpdateToken = GetEngineInstance().GetUpdateManager().RegisterUpdateFunction( forge::UpdateManager::BucketType::FrameStart, [ & ]()
	{
		m_imguiInstance->OnNewFrame();
	} );

	m_updateToken = GetEngineInstance().GetUpdateManager().RegisterUpdateFunction( forge::UpdateManager::BucketType::Update, [ & ]()
	{
		DrawOverlay();
		m_topBar.Draw();
	} );

	m_postRenderingToken = GetEngineInstance().GetUpdateManager().RegisterUpdateFunction( forge::UpdateManager::BucketType::PostRendering, [ & ]()
	{
		m_imguiInstance->Render();
	} );
}

void systems::IMGUISystem::DrawOverlay()
{
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
	const float PAD = 10.0f;
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImVec2 work_pos = viewport->WorkPos;
	ImVec2 work_size = viewport->WorkSize;
	ImVec2 window_pos, window_pos_pivot;
	window_pos.x = work_pos.x + PAD;
	window_pos.y = work_pos.y + PAD;
	window_pos_pivot.x = 0.0f;
	window_pos_pivot.y = 0.0f;
	ImGui::SetNextWindowPos( window_pos, ImGuiCond_Always, window_pos_pivot );
	window_flags |= ImGuiWindowFlags_NoMove;

	ImGui::SetNextWindowBgAlpha( 0.35f );
	Bool tmp = true;
	if( ImGui::Begin( "Overlay", &tmp, window_flags ) )
	{
		m_overlayCallback.Invoke();
	}

	ImGui::End();
}

imgui::TopBarItemHandle imgui::TopBar::AddButton( forge::ArraySpan< const char* > path, Bool selectable )
{
	std::shared_ptr< Menu > currentParent = nullptr;
	std::vector< std::weak_ptr< Element > >* children = &m_rootElements;
	
	forge::ArraySpan< const char* > purePath = { path, path.GetSize() - 1u };

	auto sortElements = []( std::weak_ptr< Element > a, std::weak_ptr< Element > b )
	{
		if( auto hardA = a.lock() )
		{
			if( auto hardB = b.lock() )
			{
				return std::strcmp( hardA->GetName(), hardB->GetName() ) < 0;
			}
		}

		return false;
	};

	for( const char* name : purePath )
	{
		auto it = std::find_if( children->begin(), children->end(), [ name ]( std::weak_ptr< Element > element )
		{
			if( auto locked = element.lock() )
			{
				return locked->GetName() == name;
			}

			return false;
		} );

		if( it == children->end() )
		{
			auto tmp = std::make_shared< Menu >( name, currentParent );
			children->emplace_back( tmp );
			std::sort( children->begin(), children->end(), sortElements );
			currentParent = tmp;
			children = &currentParent->GetChildren();
		}
		else
		{
			if( auto child = it->lock() )
			{
				auto currentParent = std::dynamic_pointer_cast<Menu>( child );
				FORGE_ASSERT( currentParent );
				children = &currentParent->GetChildren();
			}		
		}
	}

	const char* itemName = *path.back();
	FORGE_ASSERT( std::find_if( children->begin(), children->end(), [ itemName ]( std::weak_ptr< Element > element )
	{
		if( auto locked = element.lock() )
		{
			return locked->GetName() == itemName;
		}
		return false;
	} ) == children->end() );

	std::shared_ptr< TopBarItem > item = std::make_shared< TopBarItem >( itemName, currentParent, selectable );
	children->emplace_back( item );
	std::sort( children->begin(), children->end(), sortElements );

	return item;
}

void DrawElements( std::vector< std::weak_ptr< imgui::TopBar::Element > >& elements )
{
	for( auto it = elements.begin(); it != elements.end(); )
	{
		if( auto element = it->lock() )
		{
			if( auto item = std::dynamic_pointer_cast< imgui::TopBarItem > ( element ) )
			{
				Bool isSelected = item->IsSelected();
				if( ImGui::MenuItem( item->GetName(), nullptr, &isSelected ) )
				{
					item->GetCallback().Invoke();
				}
			}
			else
			{
				auto menu = std::dynamic_pointer_cast< imgui::TopBar::Menu >( element );
				if( ImGui::BeginMenu( menu->GetName() ) )
				{
					DrawElements( menu->GetChildren() );
					ImGui::EndMenu();
				}
			}
			++it;
		}
		else
		{
			it = forge::utils::RemoveReorder( elements, it );
		}
	}
}

void imgui::TopBar::Draw()
{
	if( ImGui::BeginMainMenuBar() )
	{
		DrawElements( m_rootElements );
		ImGui::EndMainMenuBar();
	}
}
#endif