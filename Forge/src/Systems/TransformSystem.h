#pragma once
#include "TransformComponent.h"

namespace systems
{
	class TransformSystem : public ISystem
	{
		DECLARE_POLYMORPHIC_CLASS( TransformSystem, systems::ISystem );

	private:
		virtual void OnInitialize();
		void Update();

		forge::CallbackToken m_updateToken;
	};
}

