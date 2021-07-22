#pragma once
class IDepthStencilBuffer
{
public:
	virtual ~IDepthStencilBuffer() {}
	virtual void Clear() = 0;
};

