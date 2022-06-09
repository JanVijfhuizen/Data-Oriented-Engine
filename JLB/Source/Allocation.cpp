#include "Allocation.h"

namespace jlb
{
	AllocationID::operator bool() const
	{
		return id != SIZE_MAX;
	}
}
