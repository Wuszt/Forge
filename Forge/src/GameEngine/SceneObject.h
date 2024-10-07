#pragma once
#include "Object.h"

namespace forge
{
	class SceneObject : public forge::Object
	{
		RTTI_DECLARE_CLASS( SceneObject, forge::Object );

	protected:
		virtual void OnInit() override;
		virtual void OnDeinit() override;

	private:
		forge::CallbackToken m_onTransformParentChangedToken;
	};
}