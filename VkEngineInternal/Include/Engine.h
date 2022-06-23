#pragma once

namespace jlb
{
	class StackAllocator;
}

namespace vke
{
	/// <summary>
	/// A data-oriented engine that focuses on optimizing memory performance.
	/// </summary>
	class Engine final
	{
	public:
		/// <summary>
		/// Runs the game via the engine.
		/// </summary>
		/// <returns>0 if successful, otherwise returns a Vulkan error.</returns>
		static int Run();
	};
}