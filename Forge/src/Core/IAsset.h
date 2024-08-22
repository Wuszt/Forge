#pragma once
#include "Path.h"
namespace forge
{
	class IAsset
	{
		RTTI_DECLARE_ABSTRACT_CLASS( IAsset );
	public:
		IAsset( const forge::Path& path );
		virtual ~IAsset() = default;

		const forge::Path& GetPath() const
		{
			return m_path;
		}

	private:
		const forge::Path m_path;
	};
}