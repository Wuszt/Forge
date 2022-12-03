#pragma once
#include "FragmentsPackage.h"
#include "Archetype.h"

namespace ecs
{
	class ECSManager : public forge::IManager
	{


		std::vector< Archetype > m_archetypes;
	};
}