#pragma once

#ifdef FORGE_IMGUI_ENABLED
#include "ISystem.h"

namespace forge
{
	class IMGUIInstance;
}

namespace systems
{
	class IMGUISystem : public ISystem
	{
	public:
		IMGUISystem( forge::EngineInstance& engineInstance );

		virtual void OnInitialize() override;

		FORGE_INLINE forge::CallbackToken AddOverlayListener( const forge::Callback<>::TFunc& func )
		{
			return m_overlayCallback.AddListener( func );
		}

	private:
		void DrawOverlay();

		std::unique_ptr< forge::IMGUIInstance > m_imguiInstance;

		forge::CallbackToken m_preUpdateToken;
		forge::CallbackToken m_updateToken;
		forge::CallbackToken m_postRenderingToken;
		forge::CallbackToken m_onWindowClosedToken;

		forge::Callback<> m_overlayCallback;
	};
}

#endif