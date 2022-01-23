#pragma once
#include "RenderingComponent.h"
#include "TransformComponent.h"
#include "../Renderer/PublicDefaults.h"

namespace renderer
{
	class IRenderer;
	class IMeshesRenderingPass;

	class IVertexShader;
	class IPixelShader;
	class IVertexBuffer;
	class IIndexBuffer;
	class IInputLayout;
	struct cbMesh;

	template< class T >
	class StaticConstantBuffer;
}

namespace systems
{
	class CamerasSystem;

	class RenderingSystem : public ECSSystem< forge::TransformComponentData, forge::RenderingComponentData >
	{
	public:

		using ECSSystem::ECSSystem;

		virtual void OnInitialize() override;

#ifdef FORGE_IMGUI_ENABLED
		virtual void OnRenderDebug() override;
#endif

		void OnBeforeDraw();

		void OnDraw();

		void OnPresent();

		void SetSamplers( const std::vector< renderer::SamplerStateFilterType >& filterTypes );

	private:
		systems::CamerasSystem* m_camerasSystem = nullptr;
		std::unique_ptr< forge::CallbackToken > m_beforeDrawToken;
		std::unique_ptr< forge::CallbackToken > m_drawToken;
		std::unique_ptr< forge::CallbackToken > m_presentToken;
		renderer::IRenderer* m_renderer;

		std::unique_ptr< renderer::IRawRenderablesPack > m_rawRenderablesPackage;
		std::unique_ptr< renderer::StaticConstantBuffer< renderer::cbCamera > > m_cameraCB;
		std::unordered_map< renderer::SamplerStateFilterType, std::unique_ptr< renderer::ISamplerState > > m_samplerStates;

		std::unique_ptr< renderer::IMeshesRenderingPass > m_opaqueRenderingPass;

#ifdef FORGE_DEBUGGING
		std::unique_ptr< forge::CallbackToken > m_clearingCacheToken;
#endif

#ifdef FORGE_IMGUI_ENABLED
		std::unique_ptr< forge::CallbackToken > m_overlayDebugToken;
		std::unique_ptr< renderer::IRenderTargetView > m_temporaryRTV;
		Float m_depthBufferDenominator = std::numeric_limits< Float >::infinity();
#endif
	};
}