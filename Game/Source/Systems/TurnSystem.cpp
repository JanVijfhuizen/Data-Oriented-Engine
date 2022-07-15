#include "pch.h"
#include "Systems/TurnSystem.h"
#include "Curve.h"
#include "JlbMath.h"
#include "Systems/ResourceManager.h"
#include "Systems/TextRenderHandler.h"
#include "VkEngine/Systems/EntityRenderSystem.h"
#include "VkEngine/Systems/UIRenderSystem.h"

namespace game
{
	bool TurnSystem::GetIfTickEvent() const
	{
		return _tickCalled;
	}

	float TurnSystem::GetTickLerp() const
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
		_tickCalled = false;

		const auto resourceSys = systems.GetSystem<ResourceManager>();
		const auto entitySys = systems.GetSystem<vke::EntityRenderSystem>();
		const auto uiSys = systems.GetSystem<vke::UIRenderSystem>();

		const auto timelineSubTexture = resourceSys->GetSubTexture(ResourceManager::UISubTextures::timeline);

		// Divide timeline texture.
		jlb::StackArray<vke::SubTexture, 4> textureDivided{};
		vke::texture::Subdivide(timelineSubTexture, 4, textureDivided);

		const auto& cameraPixelSize = entitySys->camera.pixelSize;

		// Calculate screen space coordinates.
		jlb::StackArray<vke::SubTexture, 5> coordinatesDivided{};
		vke::SubTexture screenSpaceCoordinates{};
		screenSpaceCoordinates.lTop = glm::vec2(-cameraPixelSize * 
			(vke::PIXEL_SIZE_ENTITY + visuals.padding) * 2.5f, visuals.screenYCoordinates);
		screenSpaceCoordinates.rBot = screenSpaceCoordinates.lTop;
		screenSpaceCoordinates.rBot.x *= -1;
		vke::texture::Subdivide(screenSpaceCoordinates, 5, coordinatesDivided);

		// Define textures used.
		jlb::StackArray<vke::SubTexture, 4> targetTextures{};
		targetTextures[0] = textureDivided[0];
		targetTextures[1] = _paused ? textureDivided[1] : textureDivided[2];
		targetTextures[2] = vke::texture::MirrorHorizontally(textureDivided[0]);
		targetTextures[3] = textureDivided[3];

		// Smooth animation when turn action is triggered.
		for (size_t i = 0; i < 5; ++i)
		{
			auto& keyLerp = _keyVerticalLerps[i];
			keyLerp += info.deltaTime / visuals.onPressedAnimDuration;
			keyLerp = jlb::math::Min<float>(1, keyLerp);
		}

		const float scale = cameraPixelSize * vke::PIXEL_SIZE_ENTITY;
		auto curveOvershoot = jlb::CreateCurveOvershooting();
		auto curveDecelerate = jlb::CreateCurveDecelerate();

		// Draw time multiplier.
		{
			const auto textRenderSys = systems.GetSystem<TextRenderHandler>();

			const float eval = jlb::DoubleCurveEvaluate(_keyVerticalLerps[4], curveOvershoot, curveDecelerate);
			const float offset = -eval * visuals.onPressedTimeVerticalOffsetMultiplier;

			TextRenderTask textRenderTask{};
			textRenderTask.origin = vke::texture::GetCenter(coordinatesDivided[4]);
			textRenderTask.origin.y += offset;
			textRenderTask.text = "x";
			auto result = textRenderSys->TryAdd(textRenderTask);
			assert(result != SIZE_MAX);

			const char* stringLiterals[]
			{
				"1", "2", "4", "8", "16"
			};

			size_t index = 0;
			size_t ticks = 1;

			while (ticks != _ticksPerSecond)
			{
				++index;
				ticks *= 2;
			}

			assert(index < sizeof stringLiterals / sizeof(const char*));
			textRenderTask.text = stringLiterals[index];
			textRenderTask.appendIndex = result;
			result = textRenderSys->TryAdd(textRenderTask);
			assert(result != SIZE_MAX);
		}
		
		// Draw the UI for the textures.
		for (size_t i = 0; i < 4; ++i)
		{
			vke::UIRenderTask renderTask{};
			renderTask.subTexture = targetTextures[i];
			renderTask.transform.position = vke::texture::GetCenter(coordinatesDivided[i]);
			renderTask.transform.scale = scale;
			const float eval = jlb::DoubleCurveEvaluate(_keyVerticalLerps[i], curveOvershoot, curveDecelerate);
			renderTask.transform.scale *= 1.f + eval * (visuals.onPressedSizeMultiplier - 1);
			const auto result = uiSys->TryAdd(renderTask);
			assert(result != SIZE_MAX);
		}

		const float dTicksPerSecond = 1.f / static_cast<float>(_previousTicksPerSecond);

		_time += _paused ? 0 : info.deltaTime * 0.001f;
		_lerp = fmodf(_time, dTicksPerSecond) / dTicksPerSecond;
		
		if (_time > dTicksPerSecond)
		{
			_time = fmodf(_time, dTicksPerSecond);
			_tickCalled = true;
			_previousTicksPerSecond = _ticksPerSecond;
		}
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
			_time = 1.f / static_cast<float>(_ticksPerSecond) + 1e-5f;
			_keyVerticalLerps[3] = 0;
		}

		// Adjust turn speed.
		if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
		{
			_ticksPerSecond /= 2;
			_keyVerticalLerps[0] = 0;
			_keyVerticalLerps[4] = 0;
		}
			
		if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
		{
			_ticksPerSecond *= 2;
			_keyVerticalLerps[2] = 0;
			_keyVerticalLerps[4] = 0;
		}
			
		_ticksPerSecond = jlb::math::Clamp<size_t>(_ticksPerSecond, 1, _maxTicksPerSecond);
	}
}
