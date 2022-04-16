#include "Fpch.h"
#include "OrthographicCamera.h"

renderer::OrthographicCamera::OrthographicCamera( const Vector2& volumeSize, Float nearPlane, Float farPlane )
	: m_volumeSize( volumeSize )
	, m_nearPlane( nearPlane )
	, m_farPlane( farPlane )
{}

renderer::OrthographicCamera::~OrthographicCamera() = default;

Matrix renderer::OrthographicCamera::ConstructOrthographicMatrix( const Vector2& volumeSize, Float nearPlane, Float farPlane )
{
	return Matrix
	(
		2.0f / volumeSize.X, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f / ( farPlane - nearPlane ), 0.0f,
		0.0f, 2.0f / volumeSize.Y, 0.0f, 0.0f,
		0.0f, 0.0f, nearPlane / (nearPlane - farPlane), 1.0f
	);
}
