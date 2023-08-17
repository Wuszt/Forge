#pragma once
#include "IPanel.h"

namespace forge
{
	class EngineInstance;
}

namespace renderer
{
	class ITexture;
}

namespace editor
{
	class SceneEditor : public IPanel
	{
	public:
		SceneEditor( forge::EngineInstance& engineInstance );
		virtual void Draw() override;

	private:
		std::unique_ptr<renderer::ITexture> m_targetTexture;
	};
}