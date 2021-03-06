#pragma once

namespace renderer
{
	class ICamera
	{
	public:
		enum class Type
		{
			Perspective,
			Orthographic,
			Custom
		};

		virtual ~ICamera() {}

		virtual Matrix GetInvViewMatrix() const = 0;
		virtual Matrix GetViewMatrix() const = 0;
		virtual Matrix GetInvProjectionMatrix() const = 0;
		virtual Matrix GetProjectionMatrix() const = 0;
		virtual Matrix GetViewProjectionMatrix() const = 0;

		virtual void SetPosition( const Vector3& pos ) = 0;
		virtual const Vector3& GetPosition() const = 0;

		virtual void SetOrientation( const Quaternion& rot ) = 0;
		virtual const Quaternion& GetOrientation() const = 0;

		virtual void SetTransform( Transform transform ) = 0;
		virtual const Transform& GetTransform() const = 0;

		virtual void SetNearPlane( Float nearPlane ) = 0;
		virtual void SetFarPlane( Float farPlane ) = 0;

		virtual Float GetNearPlane() const = 0;
		virtual Float GetFarPlane() const = 0;

		virtual Type GetType() const = 0;

		Bool HasNonLinearDepth() const
		{
			switch( GetType() )
			{
			case Type::Perspective:
				return true;
			case Type::Orthographic:
				return false;
			default:
				FORGE_FATAL( "Improper type" );
				return false;
			}
		}
	};
}
