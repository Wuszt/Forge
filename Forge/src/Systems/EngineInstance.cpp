#include "Fpch.h"
#include "EngineInstance.h"
#include "../Core/FPSCounter.h"
#include "../Core/IWindow.h"
#include "../Renderer/PublicDefaults.h"

forge::EngineInstance::EngineInstance( ApplicationInstance& appInstance )
	: m_appInstance( appInstance )
{
	forge::Time::Initialize();

	m_systemManager = std::make_unique< systems::SystemsManager >( *this );
	m_entitiesManager = std::make_unique< forge::EntitiesManager >( *this );
	m_updateManager = std::make_unique< forge::UpdateManager >( *this );

	m_systemManager->Initialize();
	m_entitiesManager->Initialize();
	m_updateManager->Initialize();

	if( m_appInstance.WithRendering() )
	{
		const Uint32 width = 1600;
		const Uint32 height = 900;
		m_window = forge::IWindow::CreateNewWindow( width, height );
		m_renderer = renderer::IRenderer::CreateRenderer( *m_window, renderer::RendererType::D3D11 );

		m_windowUpdateToken = std::make_unique< CallbackToken >( GetUpdateManager().RegisterUpdateFunction( UpdateManager::BucketType::PostUpdate, [ & ]() { m_window->Update(); } ) );
	}

	m_windowClosedToken = std::make_unique< CallbackToken >( m_window->RegisterEventListener( [ appPtr = &appInstance ]( const forge::IWindow::IEvent& ev )
	{
		if( ev.GetEventType() == IWindow::EventType::OnWindowClosed )
		{
			appPtr->Shutdown();
		}
	}) );
}

forge::EngineInstance::~EngineInstance()
{
	m_windowClosedToken = nullptr;
	m_windowUpdateToken = nullptr;

	m_systemManager->Deinitialize();
	m_entitiesManager->Deinitialize();
	m_updateManager->Deinitialize();

	m_systemManager = nullptr;
	m_entitiesManager = nullptr;
	m_updateManager = nullptr;
	m_renderer = nullptr;
	m_window = nullptr;
}

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
