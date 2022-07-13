#pragma once
#include "StringView.h"
#include "SubTexture.h"

namespace vke
{
	struct Texture;

	namespace texture
	{
		struct TextureAtlasPartition final
		{
			size_t width = 1;
			jlb::StringView path{};
		};

		constexpr VkFormat DEFAULT_FORMAT = VK_FORMAT_R8G8B8A8_SRGB;
		constexpr VkImageLayout DEFAULT_LAYOUT = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		/// <summary>
		/// Combines multiple images/textures into one big texture atlas, and stores it on the drive.<br>
		/// This is mostly used during development, since there is no need to generate the texture atlas at runtime.<br>
		/// This assumes the textures have a fixed height (nodeResolution), and can only stretch on the vertical axis,<br>
		/// and the length is only allowed to be a multiplicative of nodeResolution.
		/// </summary>
		/// <param name="texturePath">Path to store the texture atlas.</param>
		/// <param name="subtexturePath">Path to store the sub textures.</param>
		/// <param name="partitions">The shape and file name of the images/textures to add.</param>
		/// <param name="nodeResolution">The size of a single node, and the height of all the images.</param>
		/// <param name="atlasWidth">The width of the to be created atlas.</param>
		void GenerateAtlas(const EngineData& info, jlb::StringView texturePath, jlb::StringView subtexturePath,
			jlb::ArrayView<TextureAtlasPartition> partitions,
			size_t nodeResolution, size_t atlasWidth);
		// Loads the sub textures to be used in conjection with a generated atlas.
		void LoadAtlasSubTextures(jlb::StringView coordsPath, jlb::ArrayView<SubTexture> outSubTextures);
		// Divide a sub texture into multiple parts, useful for things like animations.
		void Subdivide(SubTexture subTexture, size_t amount, jlb::ArrayView<SubTexture> outSubTextures);
		// Returns the center of the sub texture.
		[[nodiscard]] glm::vec2 GetCenter(SubTexture subTexture);
		[[nodiscard]] SubTexture MirrorHorizontally(SubTexture subTexture);

		[[nodiscard]] Texture Load(const EngineData& info, jlb::StringView path);
		void Free(const EngineData& info, const Texture& texture);
	}
}
