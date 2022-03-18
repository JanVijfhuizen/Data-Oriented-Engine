#include "pch.h"
#include "WindowHandler.h"

int main()
{
	WindowHandler windowHandler{};
	{
		const WindowHandler::Info windowCreateInfo{};
		windowHandler.Construct(windowCreateInfo);
	}

	bool quit = false;
	while(!quit)
	{
		windowHandler.BeginFrame(quit);
	}
}
