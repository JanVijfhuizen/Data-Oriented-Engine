#pragma once

namespace jlb
{
	template <typename T>
	class SystemManager;

	// Base class for systems that can interact with the engine and other systems.
	template <typename T>
	class System
	{
		friend SystemManager<T>;

	protected:
		virtual void Allocate(const T& info, SystemManager<T>& systemManager) = 0;
		virtual void Free(const T& info, SystemManager<T>& systemManager) = 0;

		// Called at the start of the game. Allocate data here that is not dependent on other systems.
		virtual void Awake(const T& info, SystemManager<T>& systemManager) {}
		// Called at the start of the game, but after Awake. Allocate data here that is dependent on other systems.
		virtual void Start(const T& info, SystemManager<T>& systemManager) {}
		// Called every frame.
		virtual void Update(const T& info, SystemManager<T>& systemManager) {}
		// Called when the screen has been resized/minimized.
		virtual void OnRecreateSwapChainAssets(const T& info, SystemManager<T>& systemManager) {}
		// Called when a key has been pressed.
		void OnKeyInput(const T& info, SystemManager<T>& systemManager, int key, int action){}
		// Called when the mouse has moved / mousekey has been pressed.
		void OnMouseInput(const T& info, SystemManager<T>& systemManager, int key, int action){}
		// Called at the end of the game.
		virtual void Exit(const T& info, SystemManager<T>& systemManager){}
	};
}
