#include "Fpch.h"
#include "D3D11SamplerState.h"
#include <d3d11.h>
#include "D3D11Device.h"

D3D11_FILTER ToD3D11Filter( renderer::SamplerStateFilterType filterType )
{
	switch( filterType )
	{
	case renderer::SamplerStateFilterType::MIN_MAG_MIP_POINT:
		return D3D11_FILTER_MIN_MAG_MIP_POINT;
	case renderer::SamplerStateFilterType::MIN_MAG_POINT_MIP_LINEAR:
		return D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
	case renderer::SamplerStateFilterType::MIN_POINT_MAG_LINEAR_MIP_POINT:
		return D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
	case renderer::SamplerStateFilterType::MIN_POINT_MAG_MIP_LINEAR:
		return D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
	case renderer::SamplerStateFilterType::MIN_LINEAR_MAG_MIP_POINT:
		return D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
	case renderer::SamplerStateFilterType::MIN_LINEAR_MAG_POINT_MIP_LINEAR:
		return D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
	case renderer::SamplerStateFilterType::MIN_MAG_LINEAR_MIP_POINT:
		return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	case renderer::SamplerStateFilterType::MIN_MAG_MIP_LINEAR:
		return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	case renderer::SamplerStateFilterType::ANISOTROPIC:
		return D3D11_FILTER_ANISOTROPIC;
	case renderer::SamplerStateFilterType::COMPARISON_MIN_MAG_MIP_POINT:
		return D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
	case renderer::SamplerStateFilterType::COMPARISON_MIN_MAG_POINT_MIP_LINEAR:
		return D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR;
	case renderer::SamplerStateFilterType::COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT:
		return D3D11_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT;
	case renderer::SamplerStateFilterType::COMPARISON_MIN_POINT_MAG_MIP_LINEAR:
		return D3D11_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR;
	case renderer::SamplerStateFilterType::COMPARISON_MIN_LINEAR_MAG_MIP_POINT:
		return D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT;
	case renderer::SamplerStateFilterType::COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
		return D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
	case renderer::SamplerStateFilterType::COMPARISON_MIN_MAG_LINEAR_MIP_POINT:
		return D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	case renderer::SamplerStateFilterType::COMPARISON_MIN_MAG_MIP_LINEAR:
		return D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	case renderer::SamplerStateFilterType::COMPARISON_ANISOTROPIC:
		return D3D11_FILTER_COMPARISON_ANISOTROPIC;
	case renderer::SamplerStateFilterType::MINIMUM_MIN_MAG_MIP_POINT:
		return D3D11_FILTER_MINIMUM_MIN_MAG_MIP_POINT;
	case renderer::SamplerStateFilterType::MINIMUM_MIN_MAG_POINT_MIP_LINEAR:
		return D3D11_FILTER_MINIMUM_MIN_MAG_POINT_MIP_LINEAR;
	case renderer::SamplerStateFilterType::MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT:
		return D3D11_FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT;
	case renderer::SamplerStateFilterType::MINIMUM_MIN_POINT_MAG_MIP_LINEAR:
		return D3D11_FILTER_MINIMUM_MIN_POINT_MAG_MIP_LINEAR;
	case renderer::SamplerStateFilterType::MINIMUM_MIN_LINEAR_MAG_MIP_POINT:
		return D3D11_FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT;
	case renderer::SamplerStateFilterType::MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
		return D3D11_FILTER_MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
	case renderer::SamplerStateFilterType::MINIMUM_MIN_MAG_LINEAR_MIP_POINT:
		return D3D11_FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT;
	case renderer::SamplerStateFilterType::MINIMUM_MIN_MAG_MIP_LINEAR:
		return D3D11_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR;
	case renderer::SamplerStateFilterType::MINIMUM_ANISOTROPIC:
		return D3D11_FILTER_MINIMUM_ANISOTROPIC;
	case renderer::SamplerStateFilterType::MAXIMUM_MIN_MAG_MIP_POINT:
		return D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_POINT;
	case renderer::SamplerStateFilterType::MAXIMUM_MIN_MAG_POINT_MIP_LINEAR:
		return D3D11_FILTER_MAXIMUM_MIN_MAG_POINT_MIP_LINEAR;
	case renderer::SamplerStateFilterType::MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT:
		return D3D11_FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT;
	case renderer::SamplerStateFilterType::MAXIMUM_MIN_POINT_MAG_MIP_LINEAR:
		return D3D11_FILTER_MAXIMUM_MIN_POINT_MAG_MIP_LINEAR;
	case renderer::SamplerStateFilterType::MAXIMUM_MIN_LINEAR_MAG_MIP_POINT:
		return D3D11_FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT;
	case renderer::SamplerStateFilterType::MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
		return D3D11_FILTER_MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
	case renderer::SamplerStateFilterType::MAXIMUM_MIN_MAG_LINEAR_MIP_POINT:
		return D3D11_FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT;
	case renderer::SamplerStateFilterType::MAXIMUM_MIN_MAG_MIP_LINEAR:
		return D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR;
	case renderer::SamplerStateFilterType::MAXIMUM_ANISOTROPIC:
		return D3D11_FILTER_MAXIMUM_ANISOTROPIC;
	default:
		FORGE_FATAL( "Unknown type");
	}

	return D3D11_FILTER_MIN_MAG_MIP_POINT;
}

D3D11_COMPARISON_FUNC ToD3D11ComparisonType( renderer::SamplerStateComparisonType comparisonType )
{
	switch( comparisonType )
	{
	case renderer::SamplerStateComparisonType::NEVER:
		return D3D11_COMPARISON_NEVER;
	case renderer::SamplerStateComparisonType::LESS:
		return D3D11_COMPARISON_LESS;
	case renderer::SamplerStateComparisonType::EQUAL:
		return D3D11_COMPARISON_EQUAL;
	case renderer::SamplerStateComparisonType::LESS_EQUAL:
		return D3D11_COMPARISON_LESS_EQUAL;
	case renderer::SamplerStateComparisonType::GREATER:
		return D3D11_COMPARISON_GREATER;
	case renderer::SamplerStateComparisonType::NOT_EQUAL:
		return D3D11_COMPARISON_NOT_EQUAL;
	case renderer::SamplerStateComparisonType::GREATER_EQUAL:
		return D3D11_COMPARISON_GREATER_EQUAL;
	case renderer::SamplerStateComparisonType::ALWAYS:
		return D3D11_COMPARISON_ALWAYS;
	default:
		FORGE_FATAL( "Unknown type" );
	}

	return D3D11_COMPARISON_NEVER;
}

d3d11::D3D11SamplerState::D3D11SamplerState( const D3D11Device& device, renderer::SamplerStateFilterType filterType, renderer::SamplerStateComparisonType comparisonType )
{
	D3D11_SAMPLER_DESC samplerDesc;

	samplerDesc.Filter = ToD3D11Filter( filterType );
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = ToD3D11ComparisonType( comparisonType );
	samplerDesc.BorderColor[ 0 ] = 0;
	samplerDesc.BorderColor[ 1 ] = 0;
	samplerDesc.BorderColor[ 2 ] = 0;
	samplerDesc.BorderColor[ 3 ] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	FORGE_ASSURE( device.GetDevice()->CreateSamplerState( &samplerDesc, &m_samplerState ) == S_OK );
}
