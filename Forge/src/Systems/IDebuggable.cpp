#include "Fpch.h"
#include "IDebuggable.h"
#include "IMGUISystem.h"
#include "../IMGUI/IMGUIMenuBar.h"

#ifdef FORGE_IMGUI_ENABLED

forge::IDebuggable::IDebuggable() = default;
forge::IDebuggable::~IDebuggable() = default;

void forge::IDebuggable::InitializeDebuggable( EngineInstance& engineInstance, const char* categoryName, const char* name )
{
	m_menuBarHandle = engineInstance.GetSystemsManager().GetSystem< systems::IMGUISystem >().GetMenuBar().AddButton( { "Debug", categoryName, name }, true );
	m_onClickedTopBarItemToken = m_menuBarHandle->GetCallback().AddListener( [ this, &engineInstance ]()
	{
		if( m_menuBarHandle->IsSelected() )
		{
			m_onRenderDebugToken = engineInstance.GetUpdateManager().RegisterUpdateFunction( forge::UpdateManager::BucketType::PreRendering, [ this ]()
			{
				OnRenderDebug();
			} );
		}
		else
		{
			m_onRenderDebugToken.Unregister();
		}
	} );
}
#endif
