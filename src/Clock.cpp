#include "Clock.h"

namespace clockkitx
{
	timestamp_t Clock::getPhase(Clock& c)
	{
		return getValue() - c.getValue();
	}
}
