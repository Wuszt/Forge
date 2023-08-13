#include "Fpch.h"
#include "IRenderingPass.h"

renderer::IRenderingPass::IRenderingPass( Renderer& renderer )
	: m_renderer( renderer )
{}

renderer::IRenderingPass::~IRenderingPass() = default;

void renderer::IRenderingPass::ClearTargetTexture()
{
	if( m_targetTexture )
	{
		m_targetTexture->GetRenderTargetView()->Clear();
	}
}

void renderer::IRenderingPass::SetTargetTexture( ITexture& targetTexture )
{
	m_targetTexture = &targetTexture;
	m_onTargetTextureResized = m_targetTexture->GetOnResizedCallback().AddListener( [ & ]( const Vector2& size ) { OnTargetTextureResized( size ); } );
}

renderer::IMeshesRenderingPass::IMeshesRenderingPass( Renderer& renderer )
	: IRenderingPass( renderer )
{
	m_cameraCB = renderer.CreateStaticConstantBuffer< CBCamera >();
}

renderer::IMeshesRenderingPass::~IMeshesRenderingPass() = default;

void renderer::IMeshesRenderingPass::OnBeforeDraw( const renderer::ICamera& camera, const LightingData* lightingData )
{
	AdjustViewportSize();
	UpdateCameraConstantBuffer( camera );
}

void renderer::IMeshesRenderingPass::ClearTargetTexture()
{
	IRenderingPass::ClearTargetTexture();

	if( m_depthStencilBuffer )
	{
		m_depthStencilBuffer->GetView().Clear();
	}
}

renderer::IDepthStencilView& renderer::IMeshesRenderingPass::GetDepthStencilView() const
{
	return m_depthStencilBuffer->GetView( m_dsvIndex );
}

void renderer::IMeshesRenderingPass::AdjustViewportSize()
{
	Vector2 resolution = GetTargetTexture() ? GetTargetTexture()->GetSize() : Vector2::ZEROS();
	
	if( GetDepthStencilBuffer() )
	{
		resolution.X = Math::Max( resolution.X, GetDepthStencilBuffer()->GetTexture()->GetSize().X );
		resolution.Y = Math::Max( resolution.Y, GetDepthStencilBuffer()->GetTexture()->GetSize().Y );
	}

	GetRenderer().SetViewportSize( resolution );
}

void renderer::IMeshesRenderingPass::UpdateCameraConstantBuffer( const renderer::ICamera& camera )
{
	m_cameraCB->GetData().VP = camera.GetViewProjectionMatrix();
	m_cameraCB->GetData().CameraPosition = camera.GetPosition();
	m_cameraCB->GetData().CameraDirection = camera.GetOrientation() * Vector3::EY();
	m_cameraCB->GetData().ProjectionA = camera.GetProjectionMatrix()[ 1 ][ 2 ];
	m_cameraCB->GetData().ProjectionB = camera.GetProjectionMatrix()[ 3 ][ 2 ];
	m_cameraCB->GetData().NearPlane = camera.GetNearPlane();
	m_cameraCB->GetData().FarPlane = camera.GetFarPlane();
	m_cameraCB->UpdateBuffer();
	m_cameraCB->SetVS( renderer::VSConstantBufferType::Camera );
	m_cameraCB->SetPS( renderer::PSConstantBufferType::Camera );
}

const renderer::ShaderDefine renderer::IMeshesRenderingPass::c_ambientLightDefine{ "__AMBIENT_LIGHT__" };
const renderer::ShaderDefine renderer::IMeshesRenderingPass::c_pointLightDefine{ "__POINT_LIGHT__" };
const renderer::ShaderDefine renderer::IMeshesRenderingPass::c_spotLightDefine{ "__SPOT_LIGHT__" };
const renderer::ShaderDefine renderer::IMeshesRenderingPass::c_directionalLightDefine{ "__DIRECTIONAL_LIGHT__" };