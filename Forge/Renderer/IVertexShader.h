#pragma once
class IVertexShader
{
public:
	virtual ~IVertexShader() {}

	virtual void Set() = 0;
};

