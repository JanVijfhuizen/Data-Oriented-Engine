#include "pch.h"
#include "Systems/TurnSystem.h"
#include "Curve.h"
#include "JlbMath.h"
#include "JlbString.h"
#include "Systems/ResourceSystem.h"
#include "Systems/TextRenderHandler.h"
#include "VkEngine/Graphics/RenderConventions.h"
#include "VkEngine/Systems/UIRenderSystem.h"

namespace game
{
	size_t TurnSystem::GetTickIndex() const
	{
		return _tickIndex;
	}

	bool TurnSystem::GetIfBeginTickEvent() const
	{
		return _beginTickCalled;
	}

	bool TurnSystem::GetIfEndTickEvent() const
	{
		return _endTickCalled;
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

	void TurnSystem::Pause()
	{
		_paused = true;
	}

	void TurnSystem::PauseAtEndOfTick()
	{
		_pauseAtEndOfTick = _paused ? _pauseAtEndOfTick : true;
	}

	void TurnSystem::SkipToNextTick()
	{
		if(_paused)
		{
			_forwardToNextTick = _time > 1.f / static_cast<float>(_previousTicksPerSecond);
			_pauseAtEndOfTick = true;
			_paused = false;
			PressKey(1);
			PressKey(3);
		}
	}

	void TurnSystem::PreUpdate(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems)
	{
		vke::GameSystem::PreUpdate(info, systems);
		
		const auto resourceSys = systems.Get<ResourceSystem>();
		const auto uiSys = systems.Get<vke::UIRenderSystem>();

		const auto& cameraPixelSize = uiSys->camera.pixelSize;
		const float scale = cameraPixelSize * vke::PIXEL_SIZE_ENTITY;

		// Divide timeline texture.
		{
			const auto timelineSubTexture = resourceSys->GetSubTexture(ResourceSystem::UISubTextures::timeline);
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
				const auto textRenderSys = systems.Get<TextRenderHandler>();

				const float eval = jlb::DoubleCurveEvaluate(_keyVerticalLerps[4], curveOvershoot, curveDecelerate);
				const float offset = -eval * visuals.onPressedTimeVerticalOffsetMultiplier;

				TextRenderJob textRenderJob{};
				textRenderJob.origin = vke::texture::GetCenter(coordinatesDivided[4]);
				textRenderJob.origin.y += offset;
				textRenderJob.text = "x";
				textRenderJob.padding = -8;
				auto result = textRenderSys->TryAdd(info, textRenderJob);
				assert(result != SIZE_MAX);

				jlb::String string{};
				string.AllocateFromNumber(*info.dumpAllocator, _ticksPerSecond);

				textRenderJob.text = string.GetStringView();
				textRenderJob.appendIndex = result;
				result = textRenderSys->TryAdd(info, textRenderJob);
				assert(result != SIZE_MAX);
			}

			// Draw the UI for the textures.
			for (size_t i = 0; i < 4; ++i)
			{
				vke::UIRenderJob renderJob{};
				renderJob.subTexture = targetTextures[i];
				renderJob.position = vke::texture::GetCenter(coordinatesDivided[i]);
				renderJob.scale = glm::vec2(scale);
				const float eval = jlb::DoubleCurveEvaluate(_keyVerticalLerps[i], curveOvershoot, curveDecelerate);
				renderJob.scale *= 1.f + eval * (visuals.onPressedSizeMultiplier - 1);
				const auto result = uiSys->TryAdd(info, renderJob);
				assert(result != SIZE_MAX);
			}
		}

		const float dTicksPerSecond = 1.f / static_cast<float>(_previousTicksPerSecond);

		_time += _paused ? 0 : info.deltaTime * 0.001f;
		_lerp = fmodf(_time, dTicksPerSecond) / dTicksPerSecond;

		// Draw the timer itself.
		{
			const auto timerSubTexture = resourceSys->GetSubTexture(ResourceSystem::UISubTextures::timer);
			const auto timerArrowSubTexture = resourceSys->GetSubTexture(ResourceSystem::UISubTextures::timerArrow);

			vke::UIRenderJob renderJob{};
			renderJob.subTexture = timerArrowSubTexture;
			renderJob.position.y = visuals.screenYCoordinates + scale;
			const float lerp = 1.f - jlb::math::Clamp<float>(_lerp, 0, 1);
			const float maxWidth = (scale - cameraPixelSize) * 3;
			renderJob.position.x = lerp * maxWidth * 2 - maxWidth;
			renderJob.scale = glm::vec2(scale);
			auto result = uiSys->TryAdd(info, renderJob);
			assert(result != SIZE_MAX);

			renderJob.subTexture = timerSubTexture;
			renderJob.scale = glm::vec2(scale * 8, scale);
			renderJob.position.x = 0;
			result = uiSys->TryAdd(info, renderJob);
			assert(result != SIZE_MAX);
		}

		_beginTickCalled = false;
		_endTickCalled = false;

		if (_time > dTicksPerSecond)
		{
			_endTickCalled = !_tickEnded;
			_tickEnded = true;

			if(!_forwardToNextTick)
			{
				if (_pauseAtEndOfTick)
				{
					_paused = true;
					_pauseAtEndOfTick = false;
				}

				if (_paused)
				{
					_lerp = 1;
					_time = dTicksPerSecond + 1e-5f;
					return;
				}
			}

			if (_endTickCalled)
				return;

			// Reset turn.
			_time = fmodf(_time, dTicksPerSecond);
			_beginTickCalled = true;
			_previousTicksPerSecond = _ticksPerSecond;
			_forwardToNextTick = false;
			_tickEnded = false;
			++_tickIndex;
		}
	}

	void TurnSystem::OnKeyInput(const vke::EngineData& info, const jlb::Systems<vke::EngineData> systems, 
		const int key, const int action)
	{
		vke::GameSystem::OnKeyInput(info, systems, key, action);

		if (_pauseAtEndOfTick)
			return;
			
		// Go to the next tick.
		if (key == GLFW_KEY_UP && action == GLFW_PRESS)
			SkipToNextTick();

		// Adjust is paused.
		if (key == GLFW_KEY_DOWN && action == GLFW_PRESS && !_pauseAtEndOfTick)
		{
			_paused = !_paused;
			PressKey(1);
		}

		// Adjust turn speed.
		if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
		{
			_ticksPerSecond /= 2;
			PressKey(0);
			PressKey(4);
		}
			
		if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
		{
			_ticksPerSecond *= 2;
			PressKey(2);
			PressKey(4);
		}
			
		_ticksPerSecond = jlb::math::Clamp<size_t>(_ticksPerSecond, 1, _maxTicksPerSecond);
	}

	void TurnSystem::PressKey(const size_t index)
	{
		_keyVerticalLerps[index] = 0;
	}
}
