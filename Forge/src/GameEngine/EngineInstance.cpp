#include "Fpch.h"
#include "EngineInstance.h"
#include "../Core/FPSCounter.h"
#include "../Core/IWindow.h"
#include "../Renderer/PublicDefaults.h"
#include "../Core/DepotsContainer.h"
#include "../Renderer/TinyObjModelsLoader.h"
#include "../Core/AssetsManager.h"
#include "RenderingManager.h"

forge::EngineInstance::EngineInstance( ApplicationInstance& appInstance )
	: m_appInstance( appInstance )
{
	forge::Time::Initialize();

	m_depotsContainer = std::make_unique< DepotsContainer >( appInstance.GetApplicationName() );
	m_assetsManager = std::make_unique< AssetsManager >( GetDepotsContainer() );

	m_ecsManager = std::make_unique< ecs::ECSManager >();
	m_updateManager = std::make_unique< forge::UpdateManager >();
	m_systemManager = std::make_unique< systems::SystemsManager >( *this );
	m_objectsManager = std::make_unique< forge::ObjectsManager >( *this, *m_updateManager, *m_ecsManager );

	if( m_appInstance.WithRendering() )
	{
		m_renderingManager = std::make_unique< renderer::RenderingManager >( GetDepotsContainer(), GetAssetsManager(), GetUpdateManager() );

		m_windowClosedToken = GetRenderingManager().GetWindow().RegisterEventListener( [ appPtr = &appInstance ]( const forge::IWindow::IEvent& ev )
		{
			if( ev.GetEventType() == IWindow::EventType::OnCloseRequested )
			{
				appPtr->Shutdown();
			}
		} );
	}
}

forge::EngineInstance::~EngineInstance() = default;

void forge::EngineInstance::Run()
{
	m_appInstance.Initialize( *this );

	while( !m_appInstance.ShouldShutdown() )
	{
		PC_SCOPE( "Frame" );
		forge::Time::Update();
		forge::FPSCounter::OnUpdate( forge::Time::GetDeltaTime() );

		m_appInstance.OnUpdate( *this );

		m_updateManager->Update();

		PC_FRAME_END();
	}

	m_appInstance.Deinitialize( *this );
}
