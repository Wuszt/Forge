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

D3D11_DEPTH_STENCIL_DESC GetDesc( renderer::DepthStencilComparisonFunc comparisonFunc, Bool writeEnabled )
{
	D3D11_DEPTH_STENCIL_DESC desc;
	desc.DepthEnable = true;
	desc.DepthWriteMask = writeEnabled ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
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

	return desc;
}

d3d11::D3D11DepthStencilState::D3D11DepthStencilState( d3d11::D3D11Device& device, d3d11::D3D11RenderContext& context, renderer::DepthStencilComparisonFunc comparisonFunc )
	: m_device( device )
	, m_context( context )
{
	D3D11_DEPTH_STENCIL_DESC desc = GetDesc( comparisonFunc, true );

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

void d3d11::D3D11DepthStencilState::EnableWrite( Bool enable )
{
	D3D11_DEPTH_STENCIL_DESC desc;
	m_depthStencilState->GetDesc( &desc );

	desc.DepthWriteMask = enable ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
	FORGE_ASSURE( m_device.GetDevice()->CreateDepthStencilState( &desc, &m_depthStencilState ) == S_OK );

	D3D11_DEPTH_STENCIL_DESC currentStateDesc;
	ID3D11DepthStencilState* currentState = nullptr;
	m_context.GetDeviceContext()->OMGetDepthStencilState( &currentState, nullptr );
	currentState->GetDesc( &currentStateDesc );
	Bool isSet = currentStateDesc.BackFace.StencilDepthFailOp == desc.BackFace.StencilDepthFailOp
		&& currentStateDesc.BackFace.StencilFailOp == desc.BackFace.StencilFailOp
		&& currentStateDesc.BackFace.StencilFunc == desc.BackFace.StencilFunc
		&& currentStateDesc.BackFace.StencilPassOp == desc.BackFace.StencilPassOp
		&& currentStateDesc.DepthEnable == desc.DepthEnable
		&& currentStateDesc.DepthFunc == desc.DepthFunc
		&& currentStateDesc.FrontFace.StencilDepthFailOp == desc.FrontFace.StencilDepthFailOp
		&& currentStateDesc.FrontFace.StencilFailOp == desc.FrontFace.StencilFailOp
		&& currentStateDesc.FrontFace.StencilFunc == desc.FrontFace.StencilFunc
		&& currentStateDesc.FrontFace.StencilPassOp == desc.FrontFace.StencilPassOp
		&& currentStateDesc.StencilEnable == desc.StencilEnable
		&& currentStateDesc.StencilReadMask == desc.StencilReadMask
		&& currentStateDesc.StencilWriteMask == desc.StencilWriteMask;

	if( isSet )
	{
		Set();
	}
}
