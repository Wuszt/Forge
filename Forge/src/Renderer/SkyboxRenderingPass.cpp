#include "Fpch.h"
#include "SkyboxRenderingPass.h"
#include "TextureAsset.h"
#include "../Core/AssetsManager.h"
#include "OrthographicCamera.h"

renderer::SkyboxRenderingPass::SkyboxRenderingPass( forge::AssetsManager& assetsManager, renderer::Renderer& renderer, std::shared_ptr< const renderer::ITexture > texture )
	: IRenderingPass( renderer )
{
	FORGE_ASSERT( texture->GetType() == renderer::ITexture::Type::TextureCube );

	m_renderable = Renderable( renderer );
	m_renderable.SetModel( assetsManager, "Models\\sphere.obj");

	m_renderable.GetMaterials()[ 0 ]->SetShaders( "Skybox.fx", "Skybox.fx", renderer::RenderingPass::Opaque );
	m_renderable.GetMaterials()[ 0 ]->SetTexture( texture, renderer::Material::TextureType::Diffuse );

	m_cameraCB = renderer.CreateStaticConstantBuffer< CBCamera >();
}

void renderer::SkyboxRenderingPass::Draw( const renderer::ICamera& camera )
{
	renderer::IRenderTargetView* views[] = { GetTargetTexture()->GetRenderTargetView() };
	GetRenderer().SetRenderTargets( views, m_depthStencilBuffer ? &m_depthStencilBuffer->GetView() : nullptr );

	renderer::PerspectiveCamera perspectiveCamera;
	Float aspectRatio = static_cast< const renderer::OrthographicCamera& >( camera ).GetAspectRatio();

	if ( camera.IsA< renderer::OrthographicCamera >() )
	{
		perspectiveCamera = renderer::PerspectiveCamera( aspectRatio, FORGE_PI / 3.0f, 1.0f, 10000.0f );
	}
	else if( camera.IsA< renderer::PerspectiveCamera >() )
	{
		perspectiveCamera = static_cast< const renderer::PerspectiveCamera& >( camera );
	}
	else
	{
		FORGE_FATAL( "Can't handle this type of camera" );
	}

	m_cameraCB->GetData().VP = perspectiveCamera.GetViewProjectionMatrix();
	m_cameraCB->GetData().CameraPosition = perspectiveCamera.GetPosition();
	m_cameraCB->GetData().CameraDirection = perspectiveCamera.GetOrientation() * Vector3::EY();
	m_cameraCB->GetData().ProjectionA = perspectiveCamera.GetProjectionMatrix()[ 1 ][ 2 ];
	m_cameraCB->GetData().ProjectionB = perspectiveCamera.GetProjectionMatrix()[ 3 ][ 2 ];
	m_cameraCB->GetData().NearPlane = perspectiveCamera.GetNearPlane();
	m_cameraCB->GetData().FarPlane = perspectiveCamera.GetFarPlane();
	m_cameraCB->UpdateBuffer();
	m_cameraCB->SetVS( renderer::VSConstantBufferType::Camera );
	m_cameraCB->SetPS( renderer::PSConstantBufferType::Camera );

	GetRenderer().SetViewportSize( GetTargetTexture()->GetTextureSize() );

	CullingMode prevMode = GetRenderer().GetCullingMode();
	GetRenderer().SetCullingMode( CullingMode::CullingFront );
	GetRenderer().Draw( m_renderable );
	GetRenderer().SetCullingMode( prevMode );
}
