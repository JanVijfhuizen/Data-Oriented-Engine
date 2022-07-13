#include "pch.h"
#include "Systems/TurnSystem.h"
#include "JlbMath.h"
#include "Systems/ResourceManager.h"
#include "VkEngine/Systems/UIRenderSystem.h"

namespace game
{
	constexpr size_t _MAX_TICKS_PER_SECOND = 16;

	bool TurnSystem::GetIfTickEvent() const
	{
		return _tickCalled;
	}

	float TurnSystem::GetTimeLerp() const
	{
		return _lerp;
	}

	void TurnSystem::Allocate(const vke::EngineData& info)
	{
	}

	void TurnSystem::Free(const vke::EngineData& info)
	{
	}

	void TurnSystem::Update(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		const auto resourceSys = systems.GetSystem<ResourceManager>();
		const auto uiSys = systems.GetSystem<vke::UIRenderSystem>();

		const auto timelineSubTexture = resourceSys->GetSubTexture(ResourceManager::UISubTextures::timeline);

		jlb::StackArray<vke::SubTexture, 4> textureDivided{};
		vke::texture::Subdivide(timelineSubTexture, 4, textureDivided);

		jlb::StackArray<vke::SubTexture, 4> coordinatesDivided{};
		vke::SubTexture screenSpaceCoordinates{};
		screenSpaceCoordinates.lTop = glm::vec2(-visualLayout.screenSpaceWidth *.5f, visualLayout.screenYCoordinates);
		screenSpaceCoordinates.rBot = screenSpaceCoordinates.lTop;
		screenSpaceCoordinates.rBot.x *= -1;
		vke::texture::Subdivide(screenSpaceCoordinates, 4, coordinatesDivided);

		jlb::StackArray<vke::SubTexture, 4> targetTextures{};
		targetTextures[0] = textureDivided[0];
		targetTextures[1] = _paused ? textureDivided[1] : textureDivided[2];
		targetTextures[2] = vke::texture::MirrorHorizontally(textureDivided[0]);
		targetTextures[3] = textureDivided[3];

		// Draw the UI for the textures.
		for (size_t i = 0; i < 4; ++i)
		{
			vke::UIRenderTask renderTask{};
			renderTask.subTexture = targetTextures[i];
			renderTask.transform.position = vke::texture::GetCenter(coordinatesDivided[i]);
			renderTask.transform.scale = visualLayout.imageSize;
			const auto result = uiSys->TryAdd(renderTask);
			assert(result);
		}

		if (_paused)
			return;

		const float dTicksPerSecond = 1.f / _previousTicksPerSecond;

		_time += info.deltaTime * 0.01f;
		_tickCalled = false;

		while (_timePreviousTick + dTicksPerSecond < _time)
		{
			_timePreviousTick += dTicksPerSecond;
			_tickCalled = true;
		}

		_lerp = fmod(_time, dTicksPerSecond) / dTicksPerSecond;
		_previousTicksPerSecond = _ticksPerSecond;
	}

	void TurnSystem::OnKeyInput(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems, 
		const int key, const int action)
	{
		vke::GameSystem::OnKeyInput(info, systems, key, action);

		// Adjust is paused.
		if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
			_paused = !_paused;

		// Adjust turn speed.
		if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
			_ticksPerSecond /= 2;
		if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
			_ticksPerSecond *= 2;
		_ticksPerSecond = jlb::math::Clamp<size_t>(_ticksPerSecond, 1, _MAX_TICKS_PER_SECOND);
	}
}
