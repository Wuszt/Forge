#pragma once

namespace forge
{
	class PropertiesChain
	{
		RTTI_DECLARE_STRUCT( PropertiesChain );

	public:
		void Add( const rtti::Property& property )
		{
			m_chain.push_back( &property );
		}

		forge::ConstArraySpan< const rtti::Property* > Get() const
		{
			return forge::ConstArraySpan< const rtti::Property* >( m_chain );
		}

	private:
		std::vector< const rtti::Property* > m_chain;
	};
}