#pragma once
class IInputLayout
{
public:
	virtual ~IInputLayout() = default;

	virtual void Set() = 0;
};

