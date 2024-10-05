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
				if( !type.HasMetadata( "EditorOnly" ) )
				{
					m_objectCreationButtons.emplace_back( GetMenuBar()->AddButton( { "Create Object", type.GetName() }, [ & ]()
						{
							const forge::Object::Type& asObjectType = static_cast< const forge::Object::Type& >( type );
							GetEngineInstance().GetObjectsManager().RequestCreatingObject( asObjectType );
						}, false ) );
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
