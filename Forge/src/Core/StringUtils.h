#pragma once

namespace forge
{
	class String
	{
		String() = delete;

	public:
		static std::string Printf( const char* txt, ... );
		static std::string Printf( const char* txt, const va_list& argsList );
	};
}

