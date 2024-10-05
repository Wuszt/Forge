#include "Fpch.h"
#include "IMGUIMenuBar.h"

#ifdef FORGE_IMGUI_ENABLED
imgui::MenuBarItemHandle imgui::MenuBar::AddButton( forge::ArraySpan< const char* > path, std::function<void()> onClickedFunc, Bool selectable )
{
	std::shared_ptr< Menu > currentParent = nullptr;
	std::vector< std::weak_ptr< Element > >* children = &m_rootElements;

	forge::ArraySpan< const char* > purePath = { path, path.GetSize() - 1u };

	for ( const char* name : purePath )
	{
		auto it = std::find_if( children->begin(), children->end(), [ name ]( std::weak_ptr< Element > element )
			{
				if ( auto locked = element.lock() )
				{
					return locked->GetName() == name;
				}

				return false;
			} );

		if ( it == children->end() )
		{
			auto tmp = std::make_shared< Menu >( name, currentParent );
			children->emplace_back( tmp );
			currentParent = tmp;
			children = &currentParent->GetChildren();
		}
		else
		{
			if ( auto child = it->lock() )
			{
				auto currentParent = std::dynamic_pointer_cast< Menu >( child );
				FORGE_ASSERT( currentParent );
				children = &currentParent->GetChildren();
			}
		}
	}

	const char* itemName = *path.back();
	FORGE_ASSERT( std::find_if( children->begin(), children->end(), [ itemName ]( std::weak_ptr< Element > element )
		{
			if ( auto locked = element.lock() )
			{
				return locked->GetName() == itemName;
			}
			return false;
		} ) == children->end() );

	std::shared_ptr< MenuBarItem > item = std::make_shared< MenuBarItem >( itemName, currentParent, std::move( onClickedFunc ), selectable );
	children->emplace_back( item );

	return item;
}

void DrawElements( std::vector< std::weak_ptr< imgui::MenuBar::Element > >& elements )
{
	for ( auto it = elements.begin(); it != elements.end(); )
	{
		if ( auto element = it->lock() )
		{
			if ( auto item = std::dynamic_pointer_cast< imgui::MenuBarItem > ( element ) )
			{
				Bool isSelected = item->IsSelected();
				if ( ImGui::MenuItem( item->GetName(), nullptr, &isSelected ) )
				{
					item->GetCallback().Invoke();
				}
			}
			else
			{
				auto menu = std::dynamic_pointer_cast< imgui::MenuBar::Menu >( element );
				if ( ImGui::BeginMenu( menu->GetName() ) )
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

void imgui::MenuBar::Draw()
{
	if ( m_isMainMenu )
	{
		if ( ImGui::BeginMainMenuBar() )
		{
			DrawElements( m_rootElements );
			ImGui::EndMainMenuBar();
		}
	}
	else if ( ImGui::BeginMenuBar() )
	{
		DrawElements( m_rootElements );
		ImGui::EndMenuBar();
	}
}
#endif