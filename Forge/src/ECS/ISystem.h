#pragma once

namespace ecs
{
	class ISystem
	{
	public:
		ISystem() = default;
		virtual ~ISystem() = default;

		virtual void OnAttach() {}
		virtual void Update() {}
		virtual void OnDetach() {}
	};
}

