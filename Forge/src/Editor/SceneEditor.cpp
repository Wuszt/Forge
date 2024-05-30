#include "Fpch.h"
#include "SceneEditor.h"
#include "../../External/imgui/imgui.h"
#include "../IMGUI/IMGUIMenuBar.h"
#include "SceneHierarchy.h"
#include "SceneViewport.h"
#include "SceneObjectView.h"

editor::SceneEditor::SceneEditor( editor::WindowBase* parent, forge::EngineInstance& engineInstance )
	: WindowBase( engineInstance, parent, true )
{
	AddChild< SceneHierarchy >( *this );
	AddChild< SceneViewport >( *this );
	AddChild< SceneObjectView >( *this );

	rtti::Get().VisitTypes( [ & ]( const rtti::Type& type )
		{
			if ( type.IsA< forge::Object >() || type.InheritsFrom< forge::Object >() )
			{
				const char editorNamespace[] = "editor::";
				constexpr Uint32 editorNamespaceLength = sizeof( editorNamespace ) - 1;

				if ( std::strncmp( type.GetName(), editorNamespace, editorNamespaceLength ) != 0 )
				{
					auto buttonHandle = GetMenuBar()->AddButton( { "Create Object", type.GetName() }, false );
					auto onCreationHandle = buttonHandle->GetCallback().AddListener( [ & ]()
						{
							const forge::Object::Type& asObjectType = static_cast< const forge::Object::Type& >( type );
							GetEngineInstance().GetObjectsManager().RequestCreatingObject( asObjectType );
						});
					m_objectCreationHandles.push_back( { buttonHandle, std::move( onCreationHandle ) } );
				}
			}

			return rtti::VisitOutcome::Continue;
		} );
}

editor::SceneEditor::~SceneEditor() = default;

void editor::SceneEditor::SelectObject( forge::ObjectID objectID )
{
	m_selectedObjectID = objectID;
	m_onNewSelectedObject.Invoke( m_selectedObjectID );
}

void editor::SceneEditor::Draw()
{
	ImGui::DockSpace( ImGui::GetID( GetName() ) );
}
