#include "Fpch.h"
#include "SceneObject.h"
#include "../Systems/TransformComponent.h"
#include "SceneManager.h"
#include "Scene.h"

RTTI_IMPLEMENT_TYPE( forge::SceneObject );

void forge::SceneObject::OnInit()
{
	Super::OnInit();
	AddComponent< forge::TransformComponent >();

	auto& sceneManager = GetEngineInstance().GetSceneManager();

	sceneManager.GetScene().AttachToScene( GetObjectID() );
	m_onTransformParentChangedToken = GetComponent< forge::TransformComponent >()->RegisterOnTransformParentChanged( [ this, &sceneManager ]()
		{
			if ( GetComponent< forge::TransformComponent >()->GetParent().IsValid() )
			{
				sceneManager.GetScene().DetachFromScene( GetObjectID() );
			}
			else
			{
				sceneManager.GetScene().AttachToScene( GetObjectID() );
			}
		} );
}

void forge::SceneObject::OnDeinit()
{
	Super::OnDeinit();

	m_onTransformParentChangedToken.Unregister();
	GetEngineInstance().GetSceneManager().GetScene().DetachFromScene( GetObjectID() );
}
