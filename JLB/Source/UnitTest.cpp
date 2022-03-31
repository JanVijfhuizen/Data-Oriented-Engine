#include "UnitTest.h"
#include "LinearAllocator.h"
#include <cstdlib>
#include <cassert>
#include "Array.h"
#include "Vector.h"
#include "StringView.h"
#include <iostream>
#include "Stack.h"
#include "HashMap.h"
#include "Heap.h"
#include "Tuple.h"
#include "StackArray.h"

namespace jlb
{
	void UnitTest::Run()
	{
		// Test linear allocator malloc/free.
		for (size_t i = 0; i < 25; ++i)
		{
			const size_t size = 512 + rand() % 512;
			LinearAllocator allocator{ size };
			allocator.Malloc(64 + rand() % 128);
			const size_t remainingStart = allocator.GetAvailableMemorySpace();

			for (size_t j = 0; j < 25; ++j)
			{
				const size_t remaining = allocator.GetAvailableMemorySpace();
				const size_t depth = 2 + rand() % 2;

				for (size_t k = 0; k < depth; ++k)
					allocator.Malloc(rand() % 128);

				for (size_t k = 0; k < depth; ++k)
					allocator.Free();

				assert(remaining == allocator.GetAvailableMemorySpace());
			}

			assert(remainingStart == allocator.GetAvailableMemorySpace());
			allocator.Free();
		}

		// Test linear allocator new/delete.
		for (size_t i = 0; i < 25; ++i)
		{
			const size_t size = 2056 + rand() % 512;
			LinearAllocator allocator{ size };

			struct TestStruct final
			{
				float f;
				int i;
				bool b;
			};

			allocator.New<TestStruct>(1 + rand() % 10);
			const size_t remainingStart = allocator.GetAvailableMemorySpace();

			for (size_t j = 0; j < 25; ++j)
			{
				const size_t remaining = allocator.GetAvailableMemorySpace();
				const size_t depth = 2 + rand() % 2;

				for (size_t k = 0; k < depth; ++k)
					allocator.New<TestStruct>(rand() % 16);

				for (size_t k = 0; k < depth; ++k)
					allocator.Free();

				assert(remaining == allocator.GetAvailableMemorySpace());
			}

			assert(remainingStart == allocator.GetAvailableMemorySpace());
			allocator.Free();
		}

		// Test array view.
		{
			struct TestStruct final
			{
				float f;
				int i;
				bool b;
			} str{};

			str.i = 14;
			str.b = true;

			for (size_t i = 0; i < 25; ++i)
			{
				LinearAllocator allocator{ 1024 };
				Array<TestStruct> array{};

				array.Allocate(allocator, 5 + rand() % 20, str);
				for (size_t j = 0; j < array.GetLength(); ++j)
				{
					assert(array[j].i == str.i);
					assert(array[j].b == str.b);
				}
			}

			LinearAllocator allocator{ 1024 };
			Array<TestStruct> array{};
			array.Allocate(allocator, 12 + rand() % 12, str);

			for (auto& test : array)
			{
				assert(test.i == str.i);
				assert(test.b == str.b);
			}

			for (int i = 0; i < 25; ++i)
			{
				Array<TestStruct> a{};
				a.Allocate(allocator, 6 + rand() % 12);

				Array<TestStruct> b{};
				b.AllocateAndCopy(allocator, a.GetLength(), a.GetData());

				allocator.Free();
				allocator.Free();
			}

			Array<int> arr{};
			arr.Allocate(allocator, 4);
			arr[1] = 4;
			arr[2] = 6;
			arr.Swap(1, 2);
			assert(arr[1] == 6 && arr[2] == 4);
		}

		{
			LinearAllocator allocator{ 1024 };
			Vector<int> vec{};
			vec.Allocate(allocator, 12);
			vec.SetCount(2);
			vec[0] = 2;
			vec[1] = 4;
			vec.Add(6);
			vec[3] = 8;

			int n = 0;
			for (auto& i : vec)
			{
				n++;
				assert(n <= 3);
				assert(i != 8);
				assert(n * 2 == i);
			}
		}

		// Strings.
		{
			jlb::StringView string = "hello";
			jlb::StringView s2 = "bye";
			jlb::StringView s3 = "hello";

			assert(string != s2);
			assert(s3 == string);
			assert(s2 == "bye");
			std::cout << s2 << std::endl;
		}

		// Stacks.
		{
			LinearAllocator allocator{ 1024 };
			Stack<int> stack{};
			stack.Allocate(allocator, 12);

			int i = 2;

			stack.Push(i);
			stack.Push(3);

			stack.Pop();

			assert(stack.Peek() == i);
		}

		// Hashmap.
		{
			LinearAllocator allocator{ 1024 };

			struct TestStruct final
			{
				int i = -1;

				bool operator ==(TestStruct& other) const
				{
					return i == other.i;
				}
			} t;
			t.i = 5;

			TestStruct u{};
			u.i = 6;

			HashMap<TestStruct> hashMap;
			hashMap.Allocate(allocator, 24);
			hashMap.hasher = [](TestStruct& str)
			{
				return static_cast<size_t>(str.i);
			};

			assert(!hashMap.Contains(t));
			hashMap.Insert(t);
			assert(hashMap.Contains(t));
			assert(hashMap.GetCount() == 1);
			hashMap.Insert(t);
			assert(hashMap.Contains(t));
			assert(hashMap.GetCount() == 1);
			hashMap.Erase(t);
			assert(!hashMap.Contains(t));
			assert(!hashMap.Contains(u));
			hashMap.Insert(u);
			assert(!hashMap.Contains(t));

			for (size_t i = 0; i < 14; ++i)
			{
				TestStruct str{};
				str.i = rand() % 250;
				if (str.i == t.i)
					continue;
				hashMap.Insert(str);
			}

			hashMap.Insert(t);
			assert(hashMap.Contains(t));
		}

		// Heap.
		{
			LinearAllocator allocator{ 1024 };
			struct TestStruct final
			{
				int i = -1;

				bool operator ==(TestStruct& other) const
				{
					return i == other.i;
				}
			} t;
			t.i = 5;

			TestStruct u{};
			u.i = 6;

			Heap<TestStruct> heap;
			heap.Allocate(allocator, 8);
			heap.hasher = [](TestStruct& str)
			{
				return static_cast<size_t>(str.i);
			};

			heap.Insert(u);
			heap.Insert(t);
			assert(heap.GetCount() == 2);
			assert(heap.Peek().i == t.i);
			heap.Clear();
			assert(heap.GetCount() == 0);
		}

		// Tuple.
		{
			struct TestStruct final
			{
				int i;
				bool b;
				float f;
			};

			using Enemy = Tuple<int, float, TestStruct>;
			Enemy tuple{};
			
			Get<0>(tuple) = 5;
			Get<2>(tuple).i = 6;
			assert(Get<0>(tuple) == 5);
			assert(Get<2>(tuple).i == 6);
		}

		// ECS-like.
		{
			LinearAllocator allocator{ 1024 };

			struct Transform
			{
				float rotation;
			};

			struct Renderer
			{
				
			};

			struct Creature
			{
				
			};

			struct PlayerBehaviour
			{
				enum
				{
					Transform,
					Renderer,
					Creature,
					This
				};
			};

			struct EnemyBehaviour
			{
				enum
				{
					Transform,
					Renderer,
					Creature,
					This
				};
			};

			struct Light
			{
				
			};

			using Player = Tuple<Transform, Renderer, Creature, PlayerBehaviour>;
			using Enemy = Tuple<Transform, Renderer, Creature, EnemyBehaviour>;
			using PointLight = Tuple<Transform, Light>;

			Player player{};
			Get<PlayerBehaviour::Transform>(player).rotation += 5;

			Vector<Enemy> enemies{};
			enemies.Allocate(allocator, 10);
			auto& enemy = enemies.Add();
			Get<EnemyBehaviour::Transform>(enemy).rotation -= 5;
		}

		// ArrayView
		{
			LinearAllocator allocator{ 1024 };

			Array<float> array{};
			array.Allocate(allocator, 4);
			ArrayView<float> view = array;
			view = { array.GetData(), 4 };

			ArrayView<float> v2 = view;
		}

		// Stack array.
		{
			StackArray<float, 4> array;

			int i = 4;
			for (auto array1 : array)
			{
				i--;
			}

			assert(i == 0);

			ArrayView<float> v = array;
		}
	}
}
