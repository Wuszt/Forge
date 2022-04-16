#pragma once
#include "ICamera.h"

namespace renderer
{
	class CustomCamera : public ICamera
	{
	public:
		CustomCamera( Matrix projectionMatrix, Float nearPlane, Float farPlane )
			: m_projectionMatrix( projectionMatrix )
			, m_nearPlane( nearPlane )
			, m_farPlane( farPlane )
		{}

		virtual Matrix GetInvViewMatrix() const override;
		virtual Matrix GetViewMatrix() const override;
		virtual Matrix GetViewProjectionMatrix() const override;
		virtual void SetPosition( const Vector3& pos ) override;
		virtual const Vector3& GetPosition() const override;
		virtual void SetOrientation( const Quaternion& rot ) override;
		virtual const Quaternion& GetOrientation() const override;

		FORGE_INLINE virtual void SetTransform( Transform transform ) override
		{
			m_transform = transform;
		}

		FORGE_INLINE virtual const Transform& GetTransform() const override
		{
			return m_transform;
		}

		FORGE_INLINE virtual ICamera::Type GetType() const override
		{
			return ICamera::Type::Custom;
		}

		FORGE_INLINE virtual Matrix GetProjectionMatrix() const override
		{
			return m_projectionMatrix;
		}

		FORGE_INLINE virtual Matrix GetInvProjectionMatrix() const override
		{
			return m_projectionMatrix.AffineInverted();
		}

		FORGE_INLINE virtual void SetNearPlane( Float nearPlane )
		{
			m_nearPlane = nearPlane;
		}

		FORGE_INLINE virtual void SetFarPlane( Float farPlane )
		{
			m_farPlane = farPlane;
		}

		FORGE_INLINE virtual Float GetNearPlane() const override
		{
			return m_nearPlane;
		}

		FORGE_INLINE virtual Float GetFarPlane() const override
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

