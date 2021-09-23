#pragma once
#include "ICamera.h"

namespace forge
{
	class PerspectiveCamera : public ICamera
	{
	public:
		PerspectiveCamera( Float aspectRatio, Float fov, Float nearPlane, Float farPlane );
		~PerspectiveCamera();

		virtual Matrix GetProjectionMatrix() const override;
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
			return ICamera::Type::Perspective;
		}

	private:
		Float m_aspectRatio;
		Float m_fov;
		Float m_nearPlane;
		Float m_farPlane;
		Transform m_transform;
	};
}

