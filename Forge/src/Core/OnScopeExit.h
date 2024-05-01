#pragma once

#define CONCAT(X, Y) CONCAT_INNER(X, Y)
#define CONCAT_INNER(X,Y) X##Y

#define ON_SCOPE_EXIT( Action ) forge::ActionOnScopeExit CONCAT( _scopeExit, __COUNTER__ ) ( [&]() { Action } )

namespace forge
{
	template< class T >
	class ActionOnScopeExit
	{
	public:
		ActionOnScopeExit( T func )
			: m_func( std::move( func ) )
		{}

		~ActionOnScopeExit()
		{
			m_func();
		}

	private:
		T m_func;
	};
}