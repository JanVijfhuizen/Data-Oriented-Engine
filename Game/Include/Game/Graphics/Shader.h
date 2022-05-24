#pragma once

namespace game 
{
	struct Shader final 
	{
		VkShaderModule frag;
		VkShaderModule vert;
	};
}