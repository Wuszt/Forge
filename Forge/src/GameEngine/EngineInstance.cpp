#include "Fpch.h"
#include "EngineInstance.h"
#include "../Core/FPSCounter.h"
#include "../Core/IWindow.h"
#include "../Renderer/PublicDefaults.h"
#include "../Core/DepotsContainer.h"
#include "../Renderer/TinyObjModelsLoader.h"
#include "../Core/AssetsManager.h"

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
		const Uint32 width = 1600;
		const Uint32 height = 900;
		m_window = forge::IWindow::CreateNewWindow( width, height );
		m_renderer = renderer::IRenderer::CreateRenderer( GetDepotsContainer(), GetAssetsManager(), *m_window, renderer::RendererType::D3D11 );

		m_windowUpdateToken = GetUpdateManager().RegisterUpdateFunction( UpdateManager::BucketType::PostUpdate, [ & ]() { m_window->Update(); } );
		m_windowClosedToken = m_window->RegisterEventListener( [ appPtr = &appInstance ]( const forge::IWindow::IEvent& ev )
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

		if( m_window )
		{
			m_window->Update();
		}

		m_appInstance.OnUpdate( *this );

		m_updateManager->Update();

		PC_FRAME_END();
	}

	m_appInstance.Deinitialize( *this );
}
