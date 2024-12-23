#pragma once

namespace forge
{
	class StartupArguments
	{
	public:
		StartupArguments( Int32 argc, Char* argv[] );

		Bool Contains( const Char* key ) const;
		const Char* GetValue( const Char* key ) const;

	private:
		struct Argument
		{
			std::string m_key;
			std::string m_value;
		};

		std::vector< Argument > m_args;
	};
}
