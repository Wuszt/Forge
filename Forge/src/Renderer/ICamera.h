#pragma once
class ICamera
{
public:
	virtual ~ICamera();

	virtual Matrix GetViewMatrix() const = 0;
	virtual Matrix GetProjectionMatrix() const = 0;
	virtual Matrix GetViewProjectionMatrix() const = 0;

	virtual void SetPosition( const Vector3& pos ) = 0;
	virtual const Vector3& GetPosition() const = 0;

	virtual void SetOrientation( const Quaternion& rot ) = 0;
	virtual const Quaternion& GetOrientation() const = 0;

	virtual void SetTransform( Transform transform ) = 0;
	virtual const Transform& GetTransform() const = 0;
};

