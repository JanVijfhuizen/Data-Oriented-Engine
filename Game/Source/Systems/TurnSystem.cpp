#include "pch.h"
#include "Systems/TurnSystem.h"
#include "Curve.h"
#include "JlbMath.h"
#include "Systems/ResourceManager.h"
#include "Systems/TextRenderHandler.h"
#include "VkEngine/Graphics/RenderConventions.h"
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

	size_t TurnSystem::GetTicksPerSecond() const
	{
		return _ticksPerSecond;
	}

	bool TurnSystem::GetIsPaused() const
	{
		return _paused;
	}

	void TurnSystem::PauseAtEndOfTick()
	{
		_pauseAtEndOfTick = true;
	}

	void TurnSystem::PreUpdate(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		vke::GameSystem::PreUpdate(info, systems);

		_tickCalled = false;

		const auto resourceSys = systems.GetSystem<ResourceManager>();
		const auto uiSys = systems.GetSystem<vke::UIRenderSystem>();

		const auto& cameraPixelSize = uiSys->camera.pixelSize;
		const float scale = cameraPixelSize * vke::PIXEL_SIZE_ENTITY;

		// Divide timeline texture.
		{
			const auto timelineSubTexture = resourceSys->GetSubTexture(ResourceManager::UISubTextures::timeline);
			jlb::StackArray<vke::SubTexture, 4> textureDivided{};
			vke::texture::Subdivide(timelineSubTexture, 4, textureDivided);

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
				auto result = textRenderSys->TryAdd(info, textRenderTask);
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
				result = textRenderSys->TryAdd(info, textRenderTask);
				assert(result != SIZE_MAX);
			}

			// Draw the UI for the textures.
			for (size_t i = 0; i < 4; ++i)
			{
				vke::UIRenderTask renderTask{};
				renderTask.subTexture = targetTextures[i];
				renderTask.position = vke::texture::GetCenter(coordinatesDivided[i]);
				renderTask.scale = glm::vec2(scale);
				const float eval = jlb::DoubleCurveEvaluate(_keyVerticalLerps[i], curveOvershoot, curveDecelerate);
				renderTask.scale *= 1.f + eval * (visuals.onPressedSizeMultiplier - 1);
				const auto result = uiSys->TryAdd(info, renderTask);
				assert(result != SIZE_MAX);
			}
		}

		const float dTicksPerSecond = 1.f / static_cast<float>(_previousTicksPerSecond);

		_time += _paused ? 0 : info.deltaTime * 0.001f;
		_lerp = fmodf(_time, dTicksPerSecond) / dTicksPerSecond;

		// Draw the timer itself.
		{
			const auto timerSubTexture = resourceSys->GetSubTexture(ResourceManager::UISubTextures::timer);
			const auto timerArrowSubTexture = resourceSys->GetSubTexture(ResourceManager::UISubTextures::timerArrow);

			vke::UIRenderTask renderTask{};
			renderTask.subTexture = timerArrowSubTexture;
			renderTask.position.y = visuals.screenYCoordinates + vke::PIXEL_SIZE_ENTITY * cameraPixelSize;
			renderTask.position.x = scale * ((1.f - _lerp) * 8.f - 4.f);
			renderTask.scale = glm::vec2(scale);
			auto result = uiSys->TryAdd(info, renderTask);
			assert(result != SIZE_MAX);

			renderTask.subTexture = timerSubTexture;
			renderTask.scale = glm::vec2(scale * 8, scale);
			renderTask.position.x = 0;
			result = uiSys->TryAdd(info, renderTask);
			assert(result != SIZE_MAX);
		}
		
		if (_time > dTicksPerSecond)
		{
			if (_pauseAtEndOfTick && !_forwardToNextTick)
			{
				_paused = true;
				_pauseAtEndOfTick = false;
				_lerp = 1;
				_time = dTicksPerSecond + 1e-5f;
				return;
			}

			if (!_paused || _forwardToNextTick)
			{
				_time = fmodf(_time, dTicksPerSecond);
				_tickCalled = true;
				_previousTicksPerSecond = _ticksPerSecond;
				_forwardToNextTick = false;
			}
			else
			{
				_lerp = 1;
				_time = dTicksPerSecond + 1e-5f;
			}
		}
	}

	void TurnSystem::OnKeyInput(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems, 
		const int key, const int action)
	{
		vke::GameSystem::OnKeyInput(info, systems, key, action);

		if (_pauseAtEndOfTick)
			return;

		// Adjust is paused.
		if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
		{
			_paused = !_paused;
			_keyVerticalLerps[1] = 0;
		}
			
		// Go to the next tick.
		if (key == GLFW_KEY_UP && action == GLFW_PRESS && _paused)
		{
			_forwardToNextTick = true;
			_pauseAtEndOfTick = true;
			_paused = false;
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
