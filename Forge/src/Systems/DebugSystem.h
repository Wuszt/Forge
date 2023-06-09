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
		RTTI_DECLARE_POLYMORPHIC_CLASS( DebugSystem, systems::ISystem );

	public:
		DebugSystem();
		DebugSystem(DebugSystem&&);
		~DebugSystem();
		virtual void OnInitialize() override;

		void DrawSphere( const Vector3& position, Float radius, const Vector4& color, Bool wireFrame, Bool overlay, Float lifetime );
		void DrawCube( const Vector3& position, const Vector3& extension, const Vector4& color, Bool wireFrame, Bool overlay, Float lifetime );
		void DrawLine( const Vector3& start, const Vector3& end, Float thickness, const Vector4& color, Bool overlay, Float lifetime );
		void DrawCone( const Vector3& top, const Vector3& base, Float angle, const Vector4& color, Bool wireFrame, Bool overlay, Float lifetime );

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

		struct ObjectCreationRequest
		{
			std::function< void( forge::Object* ) > m_initFunc;
			Float m_timestamp = 0.0f;
		};

		std::vector< ObjectCreationRequest > m_objectsCreationRequests;

#ifdef FORGE_IMGUI_ENABLED
		forge::CallbackToken m_fpsCounterDrawingToken;
		std::unique_ptr< ecs::ECSDebug > m_ecsDebug;
#endif
	};
}
#endif
