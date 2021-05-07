#include "Fpch.h"
#include "Matrix.h"

void Matrix::SetRotationX( Float rad )
{
	Float sin = Math::Sin( rad );
	Float cos = Math::Cos( rad );

	X.X = 1.0f;
	X.Y = 0.0f;
	X.Z = 0.0f;

	Y.X = 0.0f;
	Y.Y = cos;
	Y.Z = sin;

	Z.X = 0.0f;
	Z.Y = -sin;
	Z.Z = cos;
}

void Matrix::SetRotationY( Float rad )
{
	Float sin = Math::Sin( rad );
	Float cos = Math::Cos( rad );

	X.X = 0.0f;
	X.X = cos;
	X.Z = -sin;

	Y.X = 0.0f;
	Y.Y = 1.0f;
	Y.Z = 0.0f;

	Z.X = sin;
	Z.Y = 0.0f;
	Z.Z = cos;
}

void Matrix::SetRotationZ( Float rad )
{
	Float sin = Math::Sin( rad );
	Float cos = Math::Cos( rad );

	X.X = cos;
	X.Y = sin;
	X.Z = 0.0f;

	Y.X = -sin;
	Y.Y = cos;
	Y.Z = 0.0f;

	Z.X = 0.0f;
	Z.Y = 0.0f;
	Z.Z = 1.0f;
}
