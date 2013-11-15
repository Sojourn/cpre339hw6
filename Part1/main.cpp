#include <vector>
#include <functional>
#include <iostream>
#include <thread>
#include <memory>
#include <cassert>

template<class Arg>
class Delegate
{
public:
	// Register a callback function with the signature void(Arg).
	template<class Func>
	Delegate &operator+=(Func func)
	{
		_callbacks.push_back(func);
		return *this;
	}

	// Invoke all registered callbacks with the supplied argument.
	void operator()(Arg arg)
	{
		for(auto &callback : _callbacks)
			callback(arg);
	}

private:
	std::vector<std::function<void (Arg)> > _callbacks;
};

class Timer
{
public:
	Delegate<Timer*> startEvent;
	Delegate<Timer*> tickEvent;
	Delegate<Timer*> stopEvent;

	Timer() : _running(false) {}

	// Run the following timer stages:
	// 1. Signals the start event once
	// 2. Signals the tick event once per second until stop is called
	// 3. Signals the stop event once
	void run()
	{
		assert(!_running);
		_running = true;

		startEvent(this);
		while(_running)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			tickEvent(this);
		}

		stopEvent(this);
	}

	void stop()
	{
		assert(_running);
		_running = false;
	}

private:
	bool _running;
};

// Returns a function which calls stop on the timer after
// being invoked repeatedly.
std::function<void (Timer*)> waitFunction(size_t ticks)
{
	return [=](Timer *timer) mutable {
		if(ticks == 0)
			timer->stop();
		else
			ticks--;
	};
}

int main(int argc, char **argv)
{
	Timer timer;

	timer.startEvent += [](Timer*) {
		std::cout << "Timer started" << std::endl;
	};

	timer.tickEvent += waitFunction(5);

	timer.tickEvent += [](Timer*) {
		std::cout << ".";
	};

	timer.stopEvent += [](Timer*) {
		std::cout << std::endl << "Timer finished" << std::endl;
	};

	timer.run();
	return 0;
}