#pragma once

namespace renderer
{
	class ICamera
	{
		RTTI_DECLARE_ABSTRACT_CLASS( ICamera );
	public:
		enum class CameraType
		{
			Perspective,
			Orthographic,
			Custom
		};

		ICamera();
		virtual ~ICamera() = 0;

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

		virtual CameraType GetCameraType() const = 0;

		Bool HasNonLinearDepth() const
		{
			switch( GetCameraType() )
			{
			case CameraType::Perspective:
				return true;
			case CameraType::Orthographic:
				return false;
			default:
				FORGE_FATAL( "Improper type" );
				return false;
			}
		}
	};
}
