#include "Fpch.h"
#include "FragmentViews.h"
#include "Archetype.h"

void ecs::fragmentViews::internal::MarkArchetypeAsInUse( ecs::Archetype& archetype )
{
	archetype.MarkAsInUse();
}

void ecs::fragmentViews::internal::CancelArchetypeAsInUse( ecs::Archetype& archetype )
{
	archetype.CancelInUse();
}
