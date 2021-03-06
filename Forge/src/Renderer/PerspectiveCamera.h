#pragma once
#include "ICamera.h"

namespace renderer
{
	class PerspectiveCamera : public ICamera
	{
	public:
		PerspectiveCamera( Float aspectRatio, Float fov, Float nearPlane, Float farPlane );
		~PerspectiveCamera();

		FORGE_INLINE virtual Matrix GetProjectionMatrix() const override
		{
			return ConstructProjectionMatrix( m_aspectRatio, m_fov, m_nearPlane, m_farPlane );
		}

		FORGE_INLINE virtual Matrix GetInvProjectionMatrix() const override
		{
			return GetProjectionMatrix().AffineInverted();
		}

		FORGE_INLINE virtual Matrix GetInvViewMatrix() const override
		{
			return m_transform.ToMatrix();
		}

		FORGE_INLINE virtual Matrix GetViewMatrix() const override
		{
			return m_transform.ToMatrix().OrthonormInverted();
		}

		FORGE_INLINE virtual Matrix GetViewProjectionMatrix() const override
		{
			return GetViewMatrix() * GetProjectionMatrix();
		}

		FORGE_INLINE virtual void SetPosition( const Vector3& pos ) override
		{
			m_transform.SetPosition( pos );
		}

		FORGE_INLINE virtual const Vector3& GetPosition() const override
		{
			return m_transform.GetPosition3();
		}

		FORGE_INLINE virtual void SetOrientation( const Quaternion& rot ) override
		{
			m_transform.SetOrientation( rot );
		}

		FORGE_INLINE virtual const Quaternion& GetOrientation() const override
		{
			return m_transform.GetOrientation();
		}

		static Matrix ConstructProjectionMatrix( Float aspectRatio, Float fov, Float nearPlane, Float farPlane );

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
			return ICamera::Type::Perspective;
		}

		FORGE_INLINE virtual void SetNearPlane( Float nearPlane ) override
		{
			m_nearPlane = nearPlane;
		}

		FORGE_INLINE virtual void SetFarPlane( Float farPlane ) override
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

		FORGE_INLINE void SetAspectRatio( Float ratio )
		{
			m_aspectRatio = ratio;
		}

		FORGE_INLINE void SetFOV( Float fov )
		{
			m_fov = fov;
		}

		FORGE_INLINE Float GetAspectRatio() const
		{
			return m_aspectRatio;
		}

		FORGE_INLINE Float GetFOV() const
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

