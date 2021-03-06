#pragma once
#ifdef FORGE_DEBUGGING
#include "ISystem.h"

namespace systems
{
	class DebugSystem : public ISystem
	{
		DECLARE_TYPE( DebugSystem, systems, ISystem );

	public:
		using ISystem::ISystem;

		virtual void OnInitialize() override;

		void DrawSphere( const Vector3& position, Float radius, const Vector4& color, Float lifetime );
		void DrawCube( const Vector3& position, const Vector3& extension, const Vector4& color, Float lifetime );
		Bool IsSystemDebugEnabled( const ISystem& system );

	private:
		void Update();

		struct DebugEntity
		{
			forge::EntityID m_entityId;
			Float m_timestamp;
		};

		std::vector< DebugEntity > m_debugEntities;
		forge::CallbackToken m_updateToken;

		forge::CallbackToken m_onRenderDebugToken;

#ifdef FORGE_IMGUI_ENABLED
		forge::CallbackToken m_fpsCounterDrawingToken;
#endif
	};
}

#endif
