#pragma once

namespace forge
{
	class EngineInstance;
}

namespace editor
{
	class PanelBase
	{
	public:
		PanelBase( forge::EngineInstance& engineIntance );
		virtual ~PanelBase() = default;

		void Update();

	protected:
		virtual void Draw() = 0;
		virtual const Char* GetName() const = 0;
		virtual void OnClicked( const Vector2& pos ) {}

		Vector2 GetSize() const;

		forge::EngineInstance& GetEngineInstance() const
		{
			return m_engineInstance;
		}

	private:
		forge::EngineInstance& m_engineInstance;
		Vector2 m_currentSize;
	};
}