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
		DECLARE_STRUCT( RenderableFragment, ecs::Fragment );
		REGISTER_ECS_FRAGMENT();

		RenderableFragment() = default;
		RenderableFragment( RenderableFragment&) {}
		RenderableFragment( RenderableFragment&& ) = default;
		RenderableFragment& operator=( RenderableFragment&& ) = default;

		renderer::Renderable m_renderable;
	};

	struct DirtyRenderable : public ecs::Tag
	{
		DECLARE_STRUCT( DirtyRenderable, ecs::Tag );
		REGISTER_ECS_TAG()
	};

	struct IgnoresLights : public ecs::Tag
	{
		DECLARE_STRUCT( IgnoresLights, ecs::Tag );
		REGISTER_ECS_TAG()
	};

	struct DrawAsOverlay : public ecs::Tag
	{
		DECLARE_STRUCT( DrawAsOverlay, ecs::Tag );
		REGISTER_ECS_TAG()
	};

	class RenderingComponent : public DataComponent< RenderableFragment >
	{
		DECLARE_POLYMORPHIC_CLASS( RenderingComponent, forge::IComponent );
	public:
		RenderingComponent();
		~RenderingComponent();

		virtual void OnAttach( EngineInstance& engineInstance ) override;
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

