#pragma once

namespace game
{
	class GameState;
	template <typename T>
	class SystemManager;

	template <typename T>
	class System
	{
		friend SystemManager<T>;

	protected:
		virtual void Allocate(const T& info, SystemManager<T>& systemManager) = 0;
		virtual void Free(const T& info, SystemManager<T>& systemManager) = 0;

		virtual void Awake(const T& info, SystemManager<T>& systemManager) {};
		virtual void Start(const T& info, SystemManager<T>& systemManager) {};
		virtual void Update(const T& info, SystemManager<T>& systemManager) {};
	};
}
