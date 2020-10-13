#include "PhaseLockedClock.h"
#include "Exceptions.h"
#include <iostream>
#include <cstdlib>

namespace clockkitx
{
	PhaseLockedClock::PhaseLockedClock(Clock& primary, Clock& reference)
		: primaryClock_(primary),
		  referenceClock_(reference),
		  variableFrequencyClock_(primary),
		  updateInterval_(1000000),
		  inSync_(false),
		  primaryFrequencyAvg_(1000000),
		  phasePanic_(5000),
		  updatePanic_(5000000),
		  lastUpdate_(0)
	{
		thread_.setPriority(Poco::Thread::PRIO_HIGHEST);
		thread_.start(this);
	}


	PhaseLockedClock::~PhaseLockedClock()
	{
		isStopThread_ = true;
		thread_.join();
	}


	timestamp_t PhaseLockedClock::getValue()
	{
		if (!inSync_)
			throw ClockException("PhaseLockedClock not in sync");

		try
		{
			Poco::ScopedLock<Poco::Mutex> lock(mutex_);
			const timestamp_t time = variableFrequencyClock_.getValue();
			return time;
		}
		catch (const ClockException&)
		{
			//cout << "PhaseLockedClock picked up exception, now out of sync." << endl;
			inSync_ = false;
			throw;
		}
	}


	bool PhaseLockedClock::isSynchronized() const
	{
		return inSync_;
	}

	int PhaseLockedClock::getOffset()
	{
		if (!inSync_)
			throw ClockException("PhaseLockedClock not in sync");
		
		Poco::ScopedLock<Poco::Mutex> lock(mutex_);
		const int offset = static_cast<int>(thisPhase_);
		
		return offset;
	}


	void PhaseLockedClock::setPhasePanic(timestamp_t phasePanic)
	{
		phasePanic_ = phasePanic;
	}

	void PhaseLockedClock::setUpdatePanic(timestamp_t updatePanic)
	{
		updatePanic_ = updatePanic;
	}

	void PhaseLockedClock::run()
	{
		//cout << "thread running" << endl;

		while (!isStopThread_)
		{
			update();

			// todo:
			// put in a timestamp for the last successfull update
			// if it has been a while, we declare this clock out of sync.

			const double variance = updateInterval_ / 10.0;
			const double base = updateInterval_ - (variance / 2.0);
			const double random = (rand() % static_cast<int>(variance * 1000)) / 1000.0;
			const int sleep_ms = static_cast<int>((base + random) / 1000);

			Poco::Thread::sleep(sleep_ms);
		}
	}

	void PhaseLockedClock::update()
	{
		if (inSync_ && (primaryClock_.getValue() - lastUpdate_) > updatePanic_)
		{
			//cout << "last update too long ago." << endl;
			inSync_ = false;
		}

		if (!inSync_)
		{
			//cout << "CLOCK OUT OF SYNC" << endl;
			setClock();
			if (inSync_)
			{
				if (updatePhase())
					lastUpdate_ = primaryClock_.getValue();
			}
			return;
		}

		const bool phaseUpdated = updatePhase();
		if (!phaseUpdated)
		{
			//cout << "PHASE UPDATE FAILED" << endl;
			return;
		}

		const bool clockUpdated = updateClock();
		if (!clockUpdated)
		{
			//cout << "CLOCK UPDATE FAILED" << endl;
			return;
		}

		// mark a timestamp for sucessfull update.
		lastUpdate_ = primaryClock_.getValue();
	}


	bool PhaseLockedClock::updatePhase()
	{
		try
		{
			timestamp_t phase, variableValue, primaryValue;
			
			{
				Poco::ScopedLock<Poco::Mutex> lock(mutex_);
				phase = referenceClock_.getPhase(variableFrequencyClock_);
				variableValue = variableFrequencyClock_.getValue();
				primaryValue = primaryClock_.getValue();
			}

			lastPhase_ = thisPhase_;
			lastVariableValue_ = thisVariableValue_;
			lastPrimaryValue_ = thisPrimaryValue_;

			thisPhase_ = phase;
			thisVariableValue_ = variableValue;
			thisPrimaryValue_ = primaryValue;

			//cout << "detected phase: " << ((int)phase) << endl;

			return true;
		}
		catch (const ClockException&)
		{
			//cout << "PLC handling clock exception: " << e.getMessage() << endl;
			return false;
		}
	}


	bool PhaseLockedClock::updateClock()
	{
		// calculate the elapsed time in seconds on the reference clock
		const timestamp_t lastReferenceValue = lastVariableValue_ + lastPhase_;
		const timestamp_t thisReferenceValue = thisVariableValue_ + thisPhase_;
		const double referenceElapsed = thisReferenceValue - lastReferenceValue;

		// find the primary clock's frequency (filter for noise)
		const double primaryTicks = thisPrimaryValue_ - lastPrimaryValue_;
		const double primaryFrequency = 1e6 * primaryTicks / referenceElapsed;
		primaryFrequencyAvg_ += (primaryFrequency - primaryFrequencyAvg_) * 0.1;

		// cout << "primary clock frequency average: " << ((int)primaryFrequencyAvg_) << endl;

		// if the phase is too high, we declare the clock out of sync
		if ((thisPhase_ > phasePanic_) || (thisPhase_ < (-1 * phasePanic_)))
		{
			//cout << "phase too high" << endl;
			inSync_ = false;
			return false;
		}

		// calculate the adjustment for the variable clock's frequency
		const double phaseDiff = thisPhase_ * 0.1;
		const double frequencyDiff = 1000000 - primaryFrequencyAvg_;
		const double variableClockFrequency = 1000000 + (frequencyDiff + phaseDiff);

		// cout << "using frequency: " << ((int) variableClockFrequency) << endl;

		{
			Poco::ScopedLock<Poco::Mutex> lock(mutex_);
			variableFrequencyClock_.setFrequency(static_cast<int>(variableClockFrequency));
		}

		return true;
	}

	void PhaseLockedClock::setClock()
	{
		Poco::ScopedLock<Poco::Mutex> lock(mutex_);

		try
		{
			variableFrequencyClock_.setValue(referenceClock_.getValue());
			inSync_ = true;
			//cout << "CLOCK IN SYNC" << endl;
		}
		catch (const ClockException&)
		{
			//cout << "exception while resetting to the reference clock" << endl;
			inSync_ = false;
		}
	}
}
