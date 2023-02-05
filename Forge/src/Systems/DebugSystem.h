#pragma once
#ifdef FORGE_DEBUGGING

#ifdef FORGE_IMGUI_ENABLED
namespace ecs
{
	class ECSDebug;
}
#endif

namespace systems
{
	class DebugSystem : public ISystem
	{
		DECLARE_POLYMORPHIC_CLASS( DebugSystem, systems::ISystem );

	public:
		DebugSystem();
		virtual ~DebugSystem();

		virtual void OnInitialize() override;

		void DrawSphere( const Vector3& position, Float radius, const Vector4& color, Float lifetime );
		void DrawCube( const Vector3& position, const Vector3& extension, const Vector4& color, Float lifetime );
		void DrawLine( const Vector3& start, const Vector3& end, Float thickness, const Vector4& color, Float lifetime );
		Bool IsSystemDebugEnabled( const ISystem& system );

	private:
		void Update();

		struct DebugObject
		{
			forge::ObjectID m_objectId;
			Float m_timestamp;
		};

		std::vector< DebugObject > m_debugObjects;
		forge::CallbackToken m_updateToken;

		forge::CallbackToken m_onRenderDebugToken;

#ifdef FORGE_IMGUI_ENABLED
		forge::CallbackToken m_fpsCounterDrawingToken;
		std::unique_ptr< ecs::ECSDebug > m_ecsDebug;
#endif
	};
}
#endif
