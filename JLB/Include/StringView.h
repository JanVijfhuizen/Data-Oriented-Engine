#pragma once

namespace jlb
{
	/// <summary>
	/// String class that can ONLY handle string literals.<br>
	/// This means that the contents of the string cannot be modified after creation.
	/// </summary>
	class StringView final
	{
	public:
		StringView() = default;
		// ReSharper disable once CppNonExplicitConvertingConstructor
		StringView(const char* strLit);

		/// <summary>
		/// Returns the pointer to the string literal.
		/// </summary>
		[[nodiscard]] const char* GetData() const;
		// Get the length of the string.
		[[nodiscard]] size_t GetLength() const;

		bool operator==(const StringView& other) const;
		bool operator==(const char* other) const;
		bool operator!=(const StringView& other) const;
		bool operator!=(const char* other) const;

		operator const char* () const;

	private:
		const char* _strLit = nullptr;
	};
}