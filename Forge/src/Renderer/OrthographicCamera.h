#pragma once
#include "ICamera.h"

namespace renderer
{
	class OrthographicCamera : public ICamera
	{
		RTTI_DECLARE_POLYMORPHIC_CLASS( OrthographicCamera, renderer::ICamera );

	public:
		OrthographicCamera();
		OrthographicCamera( Float width, Float aspectRatio, Float nearPlane, Float farPlane );
		~OrthographicCamera();

		static Matrix ConstructOrthographicMatrix( const Vector2& volumeSize, Float nearPlane, Float farPlane );

		virtual Matrix GetInvViewMatrix() const override
		{
			return m_transform.ToMatrix();
		}

		virtual Matrix GetViewMatrix() const override
		{
			return m_transform.ToMatrix().OrthonormInverted();
		}

		virtual Matrix GetProjectionMatrix() const override
		{
			return ConstructOrthographicMatrix( m_volumeSize, m_nearPlane, m_farPlane );
		}

		virtual Matrix GetInvProjectionMatrix() const override
		{
			return Matrix
			{
				m_volumeSize.X / 2.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 0.0f, m_volumeSize.Y / 2.0f, 0.0f,
				0.0f, m_farPlane - m_nearPlane, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 1.0f
			};
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

		virtual void SetTransform( Transform transform ) override
		{
			m_transform = transform;
		}

		virtual const Transform& GetTransform() const override
		{
			return m_transform;
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

		Float GetAspectRatio() const
		{
			return m_volumeSize.X / m_volumeSize.Y;
		}

		virtual ICamera::CameraType GetCameraType() const override
		{
			return ICamera::CameraType::Orthographic;
		}

		void SetAspectRatio( Float aspectRatio )
		{
			m_volumeSize = { m_volumeSize.X, m_volumeSize.X / aspectRatio };
		}

		void SetWidth( Float width )
		{
			m_volumeSize = { width, width / GetAspectRatio() };
		}

		const Vector2& GetVolumeSize() const
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

