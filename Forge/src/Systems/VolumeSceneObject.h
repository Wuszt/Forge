#pragma once
#include "../GameEngine/SceneObject.h"

namespace forge
{
	class VolumeSceneObject : public SceneObject
	{
		RTTI_DECLARE_CLASS( VolumeSceneObject, forge::SceneObject );

	public:
		virtual void OnInit( ObjectInitData& initData ) override;

	private:
		forge::CallbackToken m_updateToken;
	};
}