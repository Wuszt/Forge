#pragma once
#include "RenderingComponent.h"
#include "TransformComponent.h"

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

	class RenderingSystem : public ECSSystem< systems::ArchetypeDataTypes< forge::TransformComponentData, forge::RenderingComponentData > >
	{
		DECLARE_TYPE( RenderingSystem, systems, IECSSystem );

	public:
		RenderingSystem();
		~RenderingSystem();

		virtual void OnInitialize() override;

		void OnBeforeDraw();

		void OnDraw();

		void OnPresent();

		void SetSamplers( const forge::ArraySpan< const renderer::SamplerStateFilterType >& filterTypes );

		Vector2 GetRenderingResolution();

#ifdef FORGE_DEBUGGING
		virtual const std::string& GetDebugFriendlyName() const { static std::string name = "Rendering System"; return name; }
#endif

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

		std::unique_ptr< renderer::RawRenderablesPacks > m_rawRenderablesPacks;
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
		forge::CallbackToken m_overlayDebugToken;
		std::unique_ptr< renderer::ITexture > m_temporaryTexture;
		Float m_depthBufferDenominator = std::numeric_limits< Float >::infinity();
#endif
	};
}