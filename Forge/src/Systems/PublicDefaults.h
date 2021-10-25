#pragma once

#include "EngineInstance.h"
#include "SystemsManager.h"
#include "ISystem.h"
#include "EntitiesManager.h"
#include "EntityID.h"
#include "Entity.h"
#include "IComponent.h"
#include "UpdateManager.h"
#include "ApplicationInstance.h"

#ifdef FORGE_DEBUGGING
#include "../Systems/DebugSystem.h"
#endif

#ifdef FORGE_IMGUI_ENABLED
#include "../Systems/IMGUISystem.h"
#endif