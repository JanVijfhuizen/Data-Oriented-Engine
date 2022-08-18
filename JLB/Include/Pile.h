#pragma once

namespace jlb
{
	template <typename T>
	struct Pile final
	{
		void Add(const T& instance, float weight = 1);
		[[nodiscard]] operator T() const;

	private:
		T _value{};
		float _weight = 0;
	};

	template <typename T>
	void Pile<T>::Add(const T& instance, const float weight)
	{
		_value += instance * weight;
		_weight += weight;
	}

	template <typename T>
	Pile<T>::operator T() const
	{
		return _value / _weight;
	}
}
