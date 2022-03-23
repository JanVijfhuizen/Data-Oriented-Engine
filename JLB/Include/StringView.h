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
		/// <returns>Pointer to the string literal.</returns>
		[[nodiscard]] const char* GetData() const;

		bool operator==(StringView& other);
		bool operator==(const char* other);
		bool operator!=(StringView& other);
		bool operator!=(const char* other);

		operator const char* () const;

	private:
		const char* _strLit = nullptr;
	};
}