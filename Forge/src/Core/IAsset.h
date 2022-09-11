#pragma once
namespace forge
{
	class IAsset
	{
		DECLARE_ABSTRACT_TYPE( IAsset );
	public:
		IAsset( const std::string& path );
		virtual ~IAsset() = default;

		const std::string& GetPath() const
		{
			return m_path;
		}

	private:
		const std::string m_path;
	};
}

