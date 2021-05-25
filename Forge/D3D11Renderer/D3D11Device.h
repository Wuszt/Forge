#pragma once
struct ID3D11Device;

class D3D11Device
{
public:
	D3D11Device( ID3D11Device* device );
	~D3D11Device();

	FORGE_INLINE ID3D11Device* const& GetDevice() const
	{
		return m_device;
	}

private:
	ID3D11Device* m_device = nullptr;
};

