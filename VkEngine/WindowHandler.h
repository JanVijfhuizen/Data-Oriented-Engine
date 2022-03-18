#pragma once
#include "StringView.h"

class WindowHandler final
{
public:
	struct Info final
	{
		glm::ivec2 resolution{800, 600};
		jlb::StringView stringView;
	};

	void Construct(const Info& info);
	void Cleanup();

	[[nodiscard]] VkSurfaceKHR CreateSurface(VkInstance instance);
	void BeginFrame(bool& outQuit) const;
	bool QueryHasResized();

private:
	GLFWwindow* _window = nullptr;
	bool _resized = false;
};
