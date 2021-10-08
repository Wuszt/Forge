#pragma once

namespace renderer
{
	class IInputLayout
	{
	public:
		virtual ~IInputLayout() = default;

		virtual void Set() const = 0;
	};
}
