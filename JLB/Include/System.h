#pragma once

namespace jlb
{
	template <typename T>
	class Systems;
	template <typename T>
	class SystemManager;

	// Base class for systems that can interact with the engine and other systems.
	template <typename T>
	class System
	{
		friend SystemManager<T>;

	protected:
		virtual void Allocate(const T& info){}
		virtual void Free(const T& info){}

		// Called at the start of the game. Allocate data here that is not dependent on other systems.
		virtual void Awake(const T& info, const Systems<T> systems) {}
		// Called at the start of the game, but after Awake. Allocate data here that is dependent on other systems.
		virtual void Start(const T& info, const Systems<T> systems) {}
		// Called at the very beginning of the frame.
		virtual void BeginFrame(const T& info, const Systems<T> systems) {}
		// Called every frame before Update.
		virtual void PreUpdate(const T& info, const Systems<T> systems) {}
		// Called every frame.
		virtual void Update(const T& info, const Systems<T> systems) {}
		// Called every frame after Update.
		virtual void PostUpdate(const T& info, const Systems<T> systems){}
		// Called at the very end of the frame.
		virtual void EndFrame(const T& info, const Systems<T> systems) {}
		// Called when the screen has been resized/minimized.
		virtual void OnRecreateSwapChainAssets(const T& info, const Systems<T> systems) {}
		// Called when a key has been pressed.
		virtual void OnKeyInput(const T& info, const Systems<T> systems, int key, int action){}
		// Called when the mouse has moved / mousekey has been pressed.
		virtual void OnMouseInput(const T& info, const Systems<T> systems, int key, int action){}
		// Called when the mouse scroll is used.
		virtual void OnScrollInput(const T& info, const Systems<T> systems, float xOffset, float yOffset){}
		// Called at the end of the game.
		virtual void Exit(const T& info, const Systems<T> systems){}
	};
}
