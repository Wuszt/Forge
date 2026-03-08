#pragma once

namespace forge
{
	class ApplicationArgs
	{
	public:
		ApplicationArgs() = default;
		ApplicationArgs( int argc, char* argv[] );

		Bool HasArgument( std::string_view argument ) const;
		const std::string* GetArgumentValue( std::string_view argument ) const;

	private:
		struct StringViewHash
		{
			using is_transparent = void;

			size_t operator()( std::string_view sv ) const 
			{
				return std::hash<std::string_view>{}( sv );
			}
		};

		struct StringViewEqual
		{
			using is_transparent = void;

			Bool operator()( std::string_view lhs, std::string_view rhs ) const 
			{
				return lhs == rhs;
			}
		};

		std::unordered_map< std::string, std::string, StringViewHash, StringViewEqual > m_arguments;
	};
}