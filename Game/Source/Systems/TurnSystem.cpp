#include "pch.h"
#include "Systems/TurnSystem.h"
#include "Curve.h"
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

		// Divide timeline texture.
		jlb::StackArray<vke::SubTexture, 4> textureDivided{};
		vke::texture::Subdivide(timelineSubTexture, 4, textureDivided);

		// Calculate screen space coordinates.
		jlb::StackArray<vke::SubTexture, 4> coordinatesDivided{};
		vke::SubTexture screenSpaceCoordinates{};
		screenSpaceCoordinates.lTop = glm::vec2(-visuals.screenSpaceWidth *.5f, visuals.screenYCoordinates);
		screenSpaceCoordinates.rBot = screenSpaceCoordinates.lTop;
		screenSpaceCoordinates.rBot.x *= -1;
		vke::texture::Subdivide(screenSpaceCoordinates, 4, coordinatesDivided);

		// Define textures used.
		jlb::StackArray<vke::SubTexture, 4> targetTextures{};
		targetTextures[0] = textureDivided[0];
		targetTextures[1] = _paused ? textureDivided[1] : textureDivided[2];
		targetTextures[2] = vke::texture::MirrorHorizontally(textureDivided[0]);
		targetTextures[3] = textureDivided[3];

		// Smooth animation when turn action is triggered.
		for (size_t i = 0; i < 4; ++i)
		{
			auto& keyLerp = _keyVerticalLerps[i];
			keyLerp += info.deltaTime / visuals.onPressedAnimDuration;
			keyLerp = jlb::math::Min<float>(1, keyLerp);
		}

		auto curveOvershoot = jlb::CreateCurveOvershooting();
		auto curveDecelerate = jlb::CreateCurveDecelerate();
		
		// Draw the UI for the textures.
		for (size_t i = 0; i < 4; ++i)
		{
			vke::UIRenderTask renderTask{};
			renderTask.subTexture = targetTextures[i];
			renderTask.transform.position = vke::texture::GetCenter(coordinatesDivided[i]);
			renderTask.transform.scale = visuals.imageSize;
			const float eval = jlb::DoubleCurveEvaluate(_keyVerticalLerps[i], curveOvershoot, curveDecelerate);
			renderTask.transform.position.y -=  eval * visuals.onPressedMaxVerticalOffset;
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
		if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
		{
			_paused = !_paused;
			_keyVerticalLerps[1] = 0;
		}
			
		// Go to the next tick.
		if (key == GLFW_KEY_UP && action == GLFW_PRESS)
		{
			_timePreviousTick = 0;
			_keyVerticalLerps[3] = 0;
		}
			

		// Adjust turn speed.
		if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
		{
			_ticksPerSecond /= 2;
			_keyVerticalLerps[0] = 0;
		}
			
		if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
		{
			_ticksPerSecond *= 2;
			_keyVerticalLerps[2] = 0;
		}
			
		_ticksPerSecond = jlb::math::Clamp<size_t>(_ticksPerSecond, 1, _MAX_TICKS_PER_SECOND);
	}
}
