#include "Allocation.h"

namespace jlb
{
	AllocationID::operator bool() const
	{
		return index != SIZE_MAX;
	}
}
