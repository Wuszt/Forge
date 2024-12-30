#include "Fpch.h"
#include "SceneEditor.h"
#include "../../External/imgui/imgui.h"
#include "../IMGUI/IMGUIMenuBar.h"
#include "SceneHierarchy.h"
#include "SceneViewport.h"
#include "SceneObjectView.h"
#include "../GameEngine/SceneObject.h"
#include "../GameEngine/RenderingManager.h"
#include "../Core/IWindow.h"
#include "../Core/Streams.h"
#include "../Core/Serializer.h"
#include "../GameEngine/SceneManager.h"
#include "../GameEngine/Scene.h"

editor::SceneEditor::SceneEditor( editor::WindowBase* parent, forge::EngineInstance& engineInstance )
	: WindowBase( engineInstance, parent, true )
{
	AddChild< SceneHierarchy >( *this );
	AddChild< SceneViewport >( *this );
	AddChild< SceneObjectView >( *this );

	m_onNewSceneToken = GetEngineInstance().GetSceneManager().RegisterOnNewScene( [ & ]()
		{
			SelectObject( {} );
		} );

	m_newSceneButton = GetMenuBar()->AddButton( { "Scene", "New" }, [ & ]()
		{
			GetEngineInstance().GetSceneManager().OpenEmptyScene();
		} );

	m_saveSceneButton = GetMenuBar()->AddButton( { "Scene", "Save" }, [ & ]()
		{
			GetEngineInstance().GetSceneManager().SaveCurrentScene();
		}, false, [ & ]{ return GetEngineInstance().GetSceneManager().HasScenePath(); } );

	m_saveAsSceneButton = GetMenuBar()->AddButton( { "Scene", "Save as..." }, [ & ]()
		{
			auto& window = GetEngineInstance().GetRenderingManager().GetWindow();
			const forge::Path savingPath = window.CreateFileDialog( forge::IWindow::FileDialogType::Save, { std::string( "fscene" ) } );
			if ( !savingPath.IsEmpty() )
			{
				GetEngineInstance().GetSceneManager().SaveScene( std::move( savingPath ) );
			}
		} );

	m_loadSceneButton = GetMenuBar()->AddButton( { "Scene", "Load" }, [ & ]()
		{
			auto& window = GetEngineInstance().GetRenderingManager().GetWindow();
			const forge::Path loadingPath = window.CreateFileDialog( forge::IWindow::FileDialogType::Open, { std::string( "fscene" ) } );

			if ( !loadingPath.IsEmpty() )
			{
				GetEngineInstance().GetSceneManager().LoadScene( std::move( loadingPath ) );
			}
		} );

	rtti::Get().VisitTypes( [ & ]( const rtti::Type& type )
		{
			if ( type.IsA< forge::SceneObject >() || type.InheritsFrom< forge::SceneObject >() )
			{
				if( !type.IsAbstract() && !type.HasMetadata( "EditorOnly" ) )
				{
					m_objectCreationButtons.emplace_back( GetMenuBar()->AddButton( { "Create Scene Object", type.GetName() }, [ & ]()
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
	if ( m_selectedObjectID != objectID )
	{
		m_selectedObjectID = objectID;
		m_onNewSelectedObject.Invoke( m_selectedObjectID );
	}
}

void editor::SceneEditor::Draw()
{
	ImGui::DockSpace( ImGui::GetID( GetName() ) );
}
