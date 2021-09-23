#pragma once

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

		std::unique_ptr< forge::CallbackToken > m_preUpdateToken;
		std::unique_ptr< forge::CallbackToken > m_updateToken;
		std::unique_ptr< forge::CallbackToken > m_postRenderingToken;

		forge::Callback<> m_overlayCallback;
	};
}

