#pragma once
namespace forge
{
	class GUID
	{
		RTTI_DECLARE_CLASS( GUID );

	public:
		GUID() = default;
		
		Bool IsValid() const
		{
			return *this != GUID();
		}

		Bool operator==( const GUID& other ) const
		{
			return m_A == other.m_A
				&& m_B == other.m_B
				&& m_C == other.m_C
				&& m_D == other.m_D;
		}

		Bool operator!=( const GUID& other ) const
		{
			return !( *this == other );
		}

		static GUID Generate();

		Uint64 GetHash() const;

	private:
		Int32 m_A = 0;
		Int32 m_B = 0;
		Int32 m_C = 0;
		Int32 m_D = 0;
	};
}

namespace std 
{
	template <>
	struct hash< forge::GUID > 
	{
		size_t operator() ( const forge::GUID& guid ) const { return guid.GetHash(); }
	};
}