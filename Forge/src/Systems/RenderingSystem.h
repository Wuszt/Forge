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
	class ITexture;
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

		void OnBeforeDraw();

		void OnDraw();

		void OnPresent();

		void SetSamplers( const forge::ArraySpan< const renderer::SamplerStateFilterType >& filterTypes );

	protected:
#ifdef FORGE_IMGUI_ENABLED
		virtual void OnRenderDebug() override;
#endif

	private:
		systems::CamerasSystem* m_camerasSystem = nullptr;
		forge::CallbackToken m_beforeDrawToken;
		forge::CallbackToken m_drawToken;
		forge::CallbackToken m_presentToken;
		renderer::IRenderer* m_renderer;

		std::unique_ptr< renderer::IRawRenderablesPack > m_rawRenderablesPackage;
		std::unique_ptr< renderer::StaticConstantBuffer< renderer::cbCamera > > m_cameraCB;
		std::unordered_map< renderer::SamplerStateFilterType, std::unique_ptr< renderer::ISamplerState > > m_samplerStates;

		std::unique_ptr< renderer::IMeshesRenderingPass > m_opaqueRenderingPass;

#ifdef FORGE_DEBUGGING
		forge::CallbackToken m_clearingCacheToken;
#endif

#ifdef FORGE_IMGUI_ENABLED
		forge::CallbackToken m_overlayDebugToken;
		std::unique_ptr< renderer::ITexture > m_temporaryTexture;
		Float m_depthBufferDenominator = std::numeric_limits< Float >::infinity();
#endif
	};
}