#pragma once
#include "ICamera.h"

namespace renderer
{
	class OrthographicCamera : public ICamera
	{
	public:
		OrthographicCamera( const Vector2& volumeSize, Float nearPlane, Float farPlane );
		~OrthographicCamera();

		static Matrix ConstructOrthographicMatrix( const Vector2& volumeSize, Float nearPlane, Float farPlane );

		FORGE_INLINE virtual Matrix GetInvViewMatrix() const override
		{
			return m_transform.ToMatrix();
		}

		FORGE_INLINE virtual Matrix GetViewMatrix() const override
		{
			return m_transform.ToMatrix().OrthonormInverted();
		}

		FORGE_INLINE virtual Matrix GetProjectionMatrix() const override
		{
			return ConstructOrthographicMatrix( m_volumeSize, m_nearPlane, m_farPlane );
		}

		FORGE_INLINE virtual Matrix GetInvProjectionMatrix() const override
		{
			return Matrix
			{
				m_volumeSize.X / 2.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 0.0f, m_volumeSize.Y / 2.0f, 0.0f,
				0.0f, m_farPlane - m_nearPlane, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 1.0f
			};
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

		FORGE_INLINE virtual void SetTransform( Transform transform ) override
		{
			m_transform = transform;
		}

		FORGE_INLINE virtual const Transform& GetTransform() const override
		{
			return m_transform;
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


		FORGE_INLINE virtual Type GetType() const override
		{
			return Type::Orthographic;
		}

		FORGE_INLINE void SetVolumeSize( const Vector2& volumeSize )
		{
			m_volumeSize = volumeSize;
		}

		FORGE_INLINE const Vector2& GetVolumeSize() const
		{
			return m_volumeSize;
		}

	private:
		Vector2 m_volumeSize;
		Float m_nearPlane;
		Float m_farPlane;
		Transform m_transform;
	};
}

