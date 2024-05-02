#pragma once
#include "PanelBase.h"

namespace forge
{
	class ObjectID;
}

namespace editor
{
	class HierarchyView : public PanelBase
	{
	public:
		using SelectObjectFunc = std::function< void( forge::ObjectID ) >;
		using GetSelectedFunc = std::function< forge::ObjectID() >;

		HierarchyView( Bool withMenuBar, forge::EngineInstance& engineInstance, SelectObjectFunc selectObjectFunc, GetSelectedFunc getSelectedFunc )
			: PanelBase( withMenuBar, engineInstance )
			, m_selectObjectFunc( std::move( selectObjectFunc ) )
			, m_getSelectedObjectFunc( std::move( getSelectedFunc ) )
		{}

	protected:
		void Draw() override;
		const Char* GetName() const override { return "Hierarchy"; }

	private:
		SelectObjectFunc m_selectObjectFunc;
		GetSelectedFunc m_getSelectedObjectFunc;
	};
}

