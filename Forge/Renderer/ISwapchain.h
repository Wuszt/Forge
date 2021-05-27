#pragma once
class ISwapchain
{
public:
	virtual ~ISwapchain() {}

	virtual void Present() = 0;
};

