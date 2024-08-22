#pragma once

namespace forge
{
	class IAsset;
	class Path;

	class IAssetsLoader
	{
	public:
		virtual ~IAssetsLoader() = default;
		virtual std::vector< std::shared_ptr< forge::IAsset > > LoadAssets( const forge::Path& path ) const = 0;

		virtual forge::ArraySpan< const char* > GetHandledExtensions() const = 0;
	};
}

