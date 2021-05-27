#pragma once
class IPixelShader
{
public:
	~IPixelShader() {}

	virtual void Set() = 0;
};

