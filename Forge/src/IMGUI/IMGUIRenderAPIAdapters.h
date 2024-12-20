#pragma once

namespace renderer
{
	class Renderer;

	class IIMGUIRenderAPIAdapter
	{
	public:
		virtual ~IIMGUIRenderAPIAdapter() {}

		virtual void Render() = 0;

		virtual void OnNewFrame() = 0;
	};

}

namespace d3d11
{
	class IMGUID3D11Adapter : public renderer::IIMGUIRenderAPIAdapter
	{
	public:
		IMGUID3D11Adapter( renderer::Renderer& renderer );
		virtual void Render() override;
		virtual void OnNewFrame() override;
		virtual ~IMGUID3D11Adapter();

	private:
		renderer::Renderer& m_renderer;
	};
}