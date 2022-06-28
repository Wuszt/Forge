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
		IMGUISystem();
		~IMGUISystem();

		virtual void OnInitialize() override;

		FORGE_INLINE forge::CallbackToken AddOverlayListener( const forge::Callback<>::TFunc& func )
		{
			return m_overlayCallback.AddListener( func );
		}

		FORGE_INLINE void AddSystemToDebug( const ISystem& system, Bool* enabled )
		{
			m_systemsToDebug.push_back( { system, enabled } );
		}

	private:
		void DrawOverlay();
		void DrawTopBar();

		std::unique_ptr< forge::IMGUIInstance > m_imguiInstance;
		
		struct SystemDebugEntry
		{
			const ISystem& m_system;
			Bool* m_enabled;
		};

		std::vector< SystemDebugEntry > m_systemsToDebug;

		forge::CallbackToken m_preUpdateToken;
		forge::CallbackToken m_updateToken;
		forge::CallbackToken m_postRenderingToken;

		forge::Callback<> m_overlayCallback;

		Bool m_imguiDemoEnabled = false;
	};
}

#endif