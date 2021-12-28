#pragma once
#include "RenderingComponent.h"
#include "TransformComponent.h"
#include "../Renderer/PublicDefaults.h"

namespace renderer
{
	class IRenderer;

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
	};
}