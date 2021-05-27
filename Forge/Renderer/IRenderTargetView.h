#pragma once
class IRenderTargetView
{
public:
	virtual ~IRenderTargetView() {}

	virtual void Set() = 0;
	virtual void Clear( const Vector4& bgColor = { 0.0f, 0.0f, 0.0f, 0.0f } ) = 0;
};

