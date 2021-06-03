#pragma once

class IInput;

class IWindow
{
public:
	virtual ~IWindow() = default;

	virtual Uint32 GetWidth() const = 0;
	virtual Uint32 GetHeight() const = 0;
	virtual void Update() = 0;

	virtual IInput* GetInput() const = 0;

	static std::unique_ptr< IWindow > CreateNewWindow( Uint32 width, Uint32 height );
};

