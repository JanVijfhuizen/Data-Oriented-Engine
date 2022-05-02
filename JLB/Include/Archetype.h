#pragma once
#include "Vector.h"
#include "Tuple.h"

namespace jlb
{
	/// <summary>
	/// Class used to store and specify a type of entities.
	/// </summary>
	template <typename Info, typename ...Components>
	class Archetype : public Vector<Tuple<Components...>>
	{
	public:
		using Entity = Tuple<Components...>;

		// Define the resources required from the systems.
		virtual void DefineResourceUsage(Info& info) = 0;
		// Updates all entities in this archetype.
		void Update(Info& info);

	protected:
		// Update a single entity.
		virtual void OnUpdate(Info& info, Components&...) = 0;

	private:
		struct ProxyInfo final
		{
			void(Archetype<Info, Components...>::* func)(Info& info, Components&...);
			Info* info;
			Archetype<Info, Components...>* obj;
		};

		static void ProxyOnUpdate(ProxyInfo& info, Components&... components);
	};

	template <typename Info, typename ... Components>
	void Archetype<Info, Components...>::Update(Info& info)
	{
		ProxyInfo proxyInfo{};
		proxyInfo.func = &Archetype<Info, Components...>::OnUpdate;
		proxyInfo.info = &info;
		proxyInfo.obj = this;

		for (auto& entity : *this)
			jlb::Apply(ProxyOnUpdate, entity, proxyInfo);
	}

	template <typename Info, typename ... Components>
	void Archetype<Info, Components...>::ProxyOnUpdate(ProxyInfo& info, Components&... components)
	{
		(info.obj->*info.func)(*info.info, components...);
	}
}
