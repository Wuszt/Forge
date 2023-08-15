#pragma once
class IPanel
{
public:
	virtual ~IPanel() = default;

protected:
	virtual void Draw() {}
};

