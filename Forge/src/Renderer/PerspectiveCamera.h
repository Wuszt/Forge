#pragma once
#include "ICamera.h"

namespace renderer
{
	class PerspectiveCamera : public ICamera
	{
		RTTI_DECLARE_POLYMORPHIC_CLASS( PerspectiveCamera, renderer::ICamera );

	public:
		PerspectiveCamera();
		PerspectiveCamera( Float aspectRatio, Float fov, Float nearPlane, Float farPlane );
		~PerspectiveCamera();

		virtual Matrix GetProjectionMatrix() const override
		{
			return ConstructProjectionMatrix( m_aspectRatio, m_fov, m_nearPlane, m_farPlane );
		}

		virtual Matrix GetInvProjectionMatrix() const override
		{
			return GetProjectionMatrix().AffineInverted();
		}

		virtual Matrix GetInvViewMatrix() const override
		{
			return m_transform.ToMatrix();
		}

		virtual Matrix GetViewMatrix() const override
		{
			return m_transform.ToMatrix().OrthonormInverted();
		}

		virtual Matrix GetViewProjectionMatrix() const override
		{
			return GetViewMatrix() * GetProjectionMatrix();
		}

		virtual void SetPosition( const Vector3& pos ) override
		{
			m_transform.SetPosition( pos );
		}

		virtual const Vector3& GetPosition() const override
		{
			return m_transform.GetPosition3();
		}

		virtual void SetOrientation( const Quaternion& rot ) override
		{
			m_transform.SetOrientation( rot );
		}

		virtual const Quaternion& GetOrientation() const override
		{
			return m_transform.GetOrientation();
		}

		static Matrix ConstructProjectionMatrix( Float aspectRatio, Float fov, Float nearPlane, Float farPlane );

		virtual void SetTransform( Transform transform ) override
		{
			m_transform = transform;
		}

		virtual const Transform& GetTransform() const override
		{
			return m_transform;
		}

		virtual ICamera::CameraType GetCameraType() const override
		{
			return ICamera::CameraType::Perspective;
		}

		virtual void SetNearPlane( Float nearPlane ) override
		{
			m_nearPlane = nearPlane;
		}

		virtual void SetFarPlane( Float farPlane ) override
		{
			m_farPlane = farPlane;
		}

		virtual Float GetNearPlane() const override
		{
			return m_nearPlane;
		}

		virtual Float GetFarPlane() const override
		{
			return m_farPlane;
		}

		void SetAspectRatio( Float ratio )
		{
			m_aspectRatio = ratio;
		}

		void SetFOV( Float fov )
		{
			m_fov = fov;
		}

		Float GetAspectRatio() const
		{
			return m_aspectRatio;
		}

		Float GetFOV() const
		{
			return m_fov;
		}

	private:
		Float m_aspectRatio;
		Float m_fov;
		Float m_nearPlane;
		Float m_farPlane;
		Transform m_transform;
	};
}

