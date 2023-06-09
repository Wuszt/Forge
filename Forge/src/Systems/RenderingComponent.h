#pragma once
#include "../Renderer/Renderable.h"

namespace renderer
{
	class ConstantBuffer;
	class IConstantBufferImpl;
	class Renderable;
	class Material;
}

namespace forge
{
	struct RenderableFragment : public ecs::Fragment
	{
		RTTI_DECLARE_STRUCT( RenderableFragment, ecs::Fragment );

		RenderableFragment() = default;
		RenderableFragment( RenderableFragment&) {}
		RenderableFragment( RenderableFragment&& ) = default;
		RenderableFragment& operator=( RenderableFragment&& ) = default;

		renderer::Renderable m_renderable;
	};

	struct DirtyRenderable : public ecs::Tag
	{
		RTTI_DECLARE_STRUCT( DirtyRenderable, ecs::Tag );
	};

	struct IgnoresLights : public ecs::Tag
	{
		RTTI_DECLARE_STRUCT( IgnoresLights, ecs::Tag );
	};

	struct DrawAsOverlay : public ecs::Tag
	{
		RTTI_DECLARE_STRUCT( DrawAsOverlay, ecs::Tag );
	};

	class RenderingComponent : public DataComponent< RenderableFragment >
	{
		RTTI_DECLARE_POLYMORPHIC_CLASS( RenderingComponent, forge::IComponent );

	public:
		virtual void OnAttaching( EngineInstance& engineInstance, ecs::CommandsQueue& commandsQueue ) override;
		virtual void OnAttached( EngineInstance& engineInstance, ecs::CommandsQueue& commandsQueue ) override;
		void LoadMeshAndMaterial( const std::string& path );
		void SetDirty();

		void SetInteractingWithLight( bool enabled );
		void SetDrawAsOverlayEnabled( bool enabled );

		const renderer::Renderable& GetRenderable() const
		{
			return GetData().m_renderable;
		}

		renderer::Renderable& GetDirtyRenderable()
		{
			SetDirty();
			return GetMutableData().m_renderable;
		}
	private:
		CallbackToken m_onShadersClearCache;
	};
}

