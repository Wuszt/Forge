#include "Fpch.h"
#include "D3D11Device.h"

namespace d3d11
{
	D3D11Device::D3D11Device( ID3D11Device* device )
		: m_device( device )
	{}

	D3D11Device::~D3D11Device()
	{
		m_device->Release();
	}
}