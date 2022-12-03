#pragma once

namespace forge
{
	class IDebuggable
	{
#ifdef FORGE_IMGUI_ENABLED
	public:
		template< class T >
		void InitializeDebuggable( EngineInstance& engineInstance )
		{
			const char* category = "Other";
			if( T::GetTypeStatic().InheritsFrom< systems::ISystem >() )
			{
				category = "Systems";
			}

			const char* name = T::GetTypeStatic().GetName();

			Uint32 classNameStart = static_cast< Uint32 >( std::string( name ).find_last_of( "::" ) );
			
			if( classNameStart == static_cast< Uint32 >( std::string::npos ) )
			{
				classNameStart = 0u;
			}
			else
			{
				classNameStart += 1u;
			}

			InitializeDebuggable( engineInstance, category, name + classNameStart );
		}

	protected:
		virtual void OnRenderDebug() = 0;

	private:
		void InitializeDebuggable( EngineInstance& engineInstance, const char* categoryName, const char* name );

		std::shared_ptr< imgui::TopBarItem > m_topBarHandle;
		forge::CallbackToken m_onClickedTopBarItemToken;
		forge::CallbackToken m_onRenderDebugToken;
#endif
	};
}