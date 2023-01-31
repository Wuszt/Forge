#include "Fpch.h"
#include "IDebuggable.h"
#include "IMGUISystem.h"

#ifdef FORGE_IMGUI_ENABLED

forge::IDebuggable::IDebuggable() = default;
forge::IDebuggable::~IDebuggable() = default;

void forge::IDebuggable::InitializeDebuggable( EngineInstance& engineInstance, const char* categoryName, const char* name )
{
	m_topBarHandle = engineInstance.GetSystemsManager().GetSystem< systems::IMGUISystem >().GetTopBar().AddButton( { "Debug", categoryName, name }, true );
	m_onClickedTopBarItemToken = m_topBarHandle->GetCallback().AddListener( [ this, &engineInstance ]()
	{
		if( m_topBarHandle->IsSelected() )
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
