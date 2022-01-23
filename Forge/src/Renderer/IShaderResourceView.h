#pragma once

namespace renderer
{
	class IShaderResourceView
	{
	public:
		virtual ~IShaderResourceView() = default;
		virtual void* GetRawSRV() const = 0;
	};
}

