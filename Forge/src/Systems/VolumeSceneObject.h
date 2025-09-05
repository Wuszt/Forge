#pragma once
#include "../GameEngine/SceneObject.h"

namespace forge
{
	class VolumeSceneObject : public SceneObject
	{
		RTTI_DECLARE_CLASS( VolumeSceneObject, forge::SceneObject );

	public:
		virtual void OnInit( ObjectInitData& initData ) override;

	protected:
		virtual LinearColor GetBoundsColor() const
		{
			return LinearColor( 0.75f, 0.75f, 0.0f, 1.0f );
		}

	private:
		forge::CallbackToken m_updateToken;
	};
}