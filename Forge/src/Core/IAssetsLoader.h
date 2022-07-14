#pragma once

namespace forge
{
	class IAsset;

	class IAssetsLoader
	{
	public:
		virtual ~IAssetsLoader() = default;
		virtual std::vector< std::shared_ptr< forge::IAsset > > LoadAssets( const std::string& path ) const = 0;

		virtual forge::ArraySpan< const char* > GetHandledExtensions() const = 0;
	};
}

