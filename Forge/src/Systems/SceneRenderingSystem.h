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
	class Renderer;
	class IMeshesRenderingPass;
	class FullScreenRenderingPass;
	class ShadowMapsGenerator;
	class SkyboxRenderingPass;

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

	class SceneRenderingSystem : public ISystem, public forge::IDebuggable
	{
		RTTI_DECLARE_STRUCT( SceneRenderingSystem, systems::ISystem );

	public:
		SceneRenderingSystem();
		SceneRenderingSystem( SceneRenderingSystem&& );
		~SceneRenderingSystem();

		virtual void OnInitialize() override;

		void OnBeforeDraw();

		void OnDraw();

		Vector2 GetRenderingResolution();

		void SetSkyboxTexture( std::shared_ptr< const renderer::ITexture > texture );

		void SetTargetTexture( renderer::ITexture* texture );

		forge::CallbackToken RegisterOnRenderingResolutionChange( std::function< void( const Vector2& ) > func )
		{
			return m_onChangedRenderingResolution.AddListener( std::move( func ) );
		}

#ifdef FORGE_IMGUI_ENABLED
		virtual void OnRenderDebug() override;
#endif

	private:
		void Initialize( renderer::ITexture& targetTexture );
		void Deinitialize();

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
		forge::CallbackToken m_onTargetTextureResize;
		renderer::Renderer* m_renderer;

		std::vector< std::unique_ptr< renderer::ISamplerState > > m_samplerStates;

		std::unique_ptr< renderer::IMeshesRenderingPass > m_opaqueRenderingPass;
		std::unique_ptr< renderer::IMeshesRenderingPass > m_transparentRenderingPass;
		std::unique_ptr< renderer::SkyboxRenderingPass > m_skyboxRenderingPass;

		std::unique_ptr< renderer::IDepthStencilBuffer > m_depthStencilBuffer;
		std::unique_ptr< renderer::IDepthStencilState > m_depthStencilState;

		std::unique_ptr< renderer::ITexture > m_intermediateTexture;
		renderer::ITexture* m_targetTexture = nullptr;

		std::unique_ptr< renderer::ShadowMapsGenerator > m_shadowMapsGenerator;

		RenderingMode m_renderingMode = RenderingMode::Deferred;

		Float m_renderingResolutionScale = 1.0f;

		std::unique_ptr< renderer::IBlendState > m_transparencyBlendState;

		forge::Callback< const Vector2& > m_onChangedRenderingResolution;

#ifdef FORGE_IMGUI_ENABLED
		void CacheDepthBufferForDebug();
		std::shared_ptr< imgui::TopBarItem > m_topBarButton;
		forge::CallbackToken m_topBarButtonToken;
		forge::CallbackToken m_overlayDebugToken;
		std::unique_ptr< renderer::ITexture > m_depthBufferDebugTexture;
		Float m_depthBufferDenominator = std::numeric_limits< Float >::infinity();
		Bool m_debugForceWireFrame = false;
#endif
	};
}