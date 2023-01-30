#pragma once
#include "ICamera.h"

namespace renderer
{
	class CustomCamera : public ICamera
	{
		DECLARE_POLYMORPHIC_CLASS( CustomCamera, renderer::ICamera );

	public:
		CustomCamera( Matrix projectionMatrix, Float nearPlane, Float farPlane )
			: m_projectionMatrix( projectionMatrix )
			, m_nearPlane( nearPlane )
			, m_farPlane( farPlane )
		{}

		CustomCamera();

		virtual Matrix GetInvViewMatrix() const override;
		virtual Matrix GetViewMatrix() const override;
		virtual Matrix GetViewProjectionMatrix() const override;
		virtual void SetPosition( const Vector3& pos ) override;
		virtual const Vector3& GetPosition() const override;
		virtual void SetOrientation( const Quaternion& rot ) override;
		virtual const Quaternion& GetOrientation() const override;

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
			return ICamera::CameraType::Custom;
		}

		virtual Matrix GetProjectionMatrix() const override
		{
			return m_projectionMatrix;
		}

		virtual Matrix GetInvProjectionMatrix() const override
		{
			return m_projectionMatrix.AffineInverted();
		}

		virtual void SetNearPlane( Float nearPlane )
		{
			m_nearPlane = nearPlane;
		}

		virtual void SetFarPlane( Float farPlane )
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

	private:
		Float m_nearPlane = 0.0f;
		Float m_farPlane = 0.0f;
		Transform m_transform;
		Matrix m_projectionMatrix;
	};
}

