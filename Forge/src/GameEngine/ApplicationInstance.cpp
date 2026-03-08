#include "Fpch.h"
#include "ApplicationInstance.h"

forge::ApplicationInstance::ApplicationInstance( std::string applicationName, ApplicationArgs args )
	: m_applicationName( std::move( applicationName ) )
	, m_applicationArgs( std::move( args ) )
{}
