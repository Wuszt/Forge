#pragma once
#include "RenderingComponent.h"
#include "TransformComponent.h"
#include "IDebuggable.h"

#ifdef FORGE_IMGUI_ENABLED
#include "IMGUISystem.h"
namespace imgui
{
	class TopBarItem;
}
#endif

namespace renderer
{
	class IRenderer;
	class IMeshesRenderingPass;
	class FullScreenRenderingPass;
	class ShadowMapsGenerator;

	class IVertexShader;
	class IPixelShader;
	class IVertexBuffer;
	class IIndexBuffer;
	class IInputLayout;
	class ITexture;
	struct cbMesh;
	class IDepthStencilState;
	class RawRenderablesPacks;
	class ISamplerState;
	class IDepthStencilBuffer;
	class IBlendState;

	enum class SamplerStateFilterType;

	struct cbCamera;
	template< class T >
	class StaticConstantBuffer;
}

namespace systems
{
	class CamerasSystem;

	class RenderingSystem : public ISystem, public forge::IDebuggable
	{
		DECLARE_STRUCT( RenderingSystem, systems::ISystem );

	public:
		RenderingSystem();
		~RenderingSystem();

		virtual void OnInitialize() override;

		void OnBeforeDraw();

		void OnDraw();

		void OnPresent();

		Vector2 GetRenderingResolution();

#ifdef FORGE_IMGUI_ENABLED
		virtual void OnRenderDebug() override;
#endif

	private:
		enum class RenderingMode
		{
			Forward,
			Deferred
		};

		void SetRenderingMode( RenderingMode renderingMode );
		void UpdateRenderingResolution( Float scale );

		systems::CamerasSystem* m_camerasSystem = nullptr;
		forge::CallbackToken m_beforeDrawToken;
		forge::CallbackToken m_drawToken;
		forge::CallbackToken m_presentToken;
		renderer::IRenderer* m_renderer;

		std::vector< std::unique_ptr< renderer::ISamplerState > > m_samplerStates;

		std::unique_ptr< renderer::IMeshesRenderingPass > m_opaqueRenderingPass;
		std::unique_ptr< renderer::IMeshesRenderingPass > m_overlayRenderingPass;
		std::unique_ptr< renderer::IMeshesRenderingPass > m_transparentRenderingPass;

		std::unique_ptr< renderer::IDepthStencilBuffer > m_depthStencilBuffer;
		std::unique_ptr< renderer::IDepthStencilState > m_depthStencilState;

		std::unique_ptr< renderer::ITexture > m_targetTexture;

		std::unique_ptr< renderer::ShadowMapsGenerator > m_shadowMapsGenerator;

		RenderingMode m_renderingMode = RenderingMode::Deferred;
		forge::CallbackToken m_windowCallbackToken;

		Float m_renderingResolutionScale = 1.0f;

		std::unique_ptr< renderer::IBlendState > m_transparencyBlendState;

#ifdef FORGE_IMGUI_ENABLED
		void OnBeforeDrawDebug();
		std::shared_ptr< imgui::TopBarItem > m_topBarButton;
		forge::CallbackToken m_topBarButtonToken;
		forge::CallbackToken m_overlayDebugToken;
		std::unique_ptr< renderer::ITexture > m_depthBufferDebugTexture;
		Float m_depthBufferDenominator = std::numeric_limits< Float >::infinity();
#endif
	};
}