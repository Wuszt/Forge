#pragma once

class IRenderer;

class IIMGUIRenderAPIAdapter
{
public:
	virtual ~IIMGUIRenderAPIAdapter() {}

	virtual void Render() = 0;

	virtual void OnNewFrame() = 0;
};

class IMGUID3D11Adapter : public IIMGUIRenderAPIAdapter
{
public:
	IMGUID3D11Adapter( const IRenderer& renderer );
	virtual void Render() override;
	virtual void OnNewFrame() override;
	virtual ~IMGUID3D11Adapter();
};