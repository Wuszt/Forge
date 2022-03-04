#include "Fpch.h"
#include "D3D11DepthStencilState.h"
#include "D3D11Device.h"
 
D3D11_COMPARISON_FUNC GetAsD3D11ComparisonFunc( renderer::DepthStencilComparisonFunc comparisonFunc )
{
	switch( comparisonFunc )
	{
	case renderer::DepthStencilComparisonFunc::COMPARISON_NEVER:
		return D3D11_COMPARISON_NEVER;
	case renderer::DepthStencilComparisonFunc::COMPARISON_LESS:
		return D3D11_COMPARISON_LESS;
	case renderer::DepthStencilComparisonFunc::COMPARISON_EQUAL:
		return D3D11_COMPARISON_EQUAL;
	case renderer::DepthStencilComparisonFunc::COMPARISON_LESS_EQUAL:
		return D3D11_COMPARISON_LESS_EQUAL;
	case renderer::DepthStencilComparisonFunc::COMPARISON_GREATER:
		return D3D11_COMPARISON_GREATER;
	case renderer::DepthStencilComparisonFunc::COMPARISON_NOT_EQUAL:
		return D3D11_COMPARISON_NOT_EQUAL;
	case renderer::DepthStencilComparisonFunc::COMPARISON_GREATER_EQUAL:
		return D3D11_COMPARISON_GREATER_EQUAL;
	case renderer::DepthStencilComparisonFunc::COMPARISON_ALWAYS:
		return D3D11_COMPARISON_ALWAYS;
	default:
		FORGE_FATAL( "Unknown type" );
		return D3D11_COMPARISON_ALWAYS;
	}
}

d3d11::D3D11DepthStencilState::D3D11DepthStencilState( d3d11::D3D11Device& device, d3d11::D3D11RenderContext& context, renderer::DepthStencilComparisonFunc comparisonFunc )
	: m_context( context )
{
	D3D11_DEPTH_STENCIL_DESC desc;
	desc.DepthEnable = true;
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	desc.DepthFunc = GetAsD3D11ComparisonFunc( comparisonFunc );

	desc.StencilEnable = false;
	desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

	desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;

	desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;

	FORGE_ASSURE( device.GetDevice()->CreateDepthStencilState( &desc, &m_depthStencilState ) == S_OK );
}

d3d11::D3D11DepthStencilState::~D3D11DepthStencilState()
{
	m_depthStencilState->Release();
}

void d3d11::D3D11DepthStencilState::Set()
{
	m_context.GetDeviceContext()->OMSetDepthStencilState( m_depthStencilState, 0 );
}

void d3d11::D3D11DepthStencilState::Clear()
{
	m_context.GetDeviceContext()->OMSetDepthStencilState( nullptr, 0 );
}
