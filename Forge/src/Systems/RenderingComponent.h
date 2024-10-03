#pragma once
#include "../Renderer/Renderable.h"
#include "../GameEngine/IComponent.h"
#include "../Core/Path.h"

namespace renderer
{
	class ConstantBuffer;
	class IConstantBufferImpl;
	class Renderable;
	class Material;
}

namespace forge
{
	class Path;
	class PropertiesChain;

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
		RTTI_DECLARE_POLYMORPHIC_CLASS( RenderingComponent, forge::IDataComponent );

	public:
		virtual void OnAttached( EngineInstance& engineInstance, ecs::CommandsQueue& commandsQueue ) override;

		virtual void OnDetaching( EngineInstance& engineInstance, ecs::CommandsQueue& commandsQueue ) override;

		void LoadMeshAndMaterial( const forge::Path& path );
		void SetDirty();

		void SetInteractingWithLight( Bool enabled );
		void SetDrawAsOverlayEnabled( Bool enabled );

		ecs::MutableFragmentView< forge::RenderableFragment > GetDirtyData()
		{
			return GetMutableData();
		}

	protected:
		virtual ecs::MutableFragmentView< RenderableFragment > GetMutableData() override
		{
			SetDirty();
			return DataComponent< RenderableFragment >::GetMutableData();
		}

	private:
		void OnPropertyChanged( const forge::PropertiesChain& propertiesChain );

		CallbackToken m_onShadersClearCache;
		forge::Path m_meshPath;
	};
}

