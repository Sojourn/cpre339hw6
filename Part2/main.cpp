#include <iostream>
#include <unordered_set>
#include <memory>
#include <string>
#include <sstream>
#include <locale>
#include <cassert>
#include <ctime>

class LoggerDecorator
{
public:
	virtual void logError(std::string &msg) = 0;
	virtual void logWarning(std::string &msg) = 0;
};

class Logger
{
public:
	void logWarning(std::string msg)
	{
		for(auto &decorator : _decorators)
			decorator->logWarning(msg);

		std::cout << msg << std::endl;
	}

	void logError(std::string msg)
	{
		for(auto &decorator : _decorators)
			decorator->logError(msg);

		std::cout << msg << std::endl;

	}

	Logger &operator +=(std::shared_ptr<LoggerDecorator> decorator)
	{
		_decorators.insert(decorator);
		return *this;
	}

	Logger &operator -=(std::shared_ptr<LoggerDecorator> decorator)
	{
		_decorators.erase(decorator);
		return *this;
	}

private:
	std::unordered_set<std::shared_ptr<LoggerDecorator> > _decorators;
};

class TimestampDecorator : public LoggerDecorator
{
public:
	virtual void logError(std::string &msg)
	{
		msg = prependTimestamp(msg);
	}

	virtual void logWarning(std::string &msg)
	{
		msg = prependTimestamp(msg);
	}

private:
	std::string prependTimestamp(std::string &msg)
	{
		std::stringstream ss;
		ss << time(NULL) << ": " << msg;
		return ss.str();
	}
};

class EmphasisDecorator : public LoggerDecorator
{
public:
	virtual void logError(std::string &msg)
	{
		msg = toUpper(msg);
	}

	virtual void logWarning(std::string &msg)
	{
	}

private:
	std::string toUpper(const std::string &msg)
	{
		static std::locale loc;

		std::stringstream ss;
		for(char c : msg)
			ss << std::toupper(c, loc);

		return ss.str();
	}
};

int main(int argc, char **argv)
{
	Logger logger;
	
	logger.logWarning("a warning message");
	logger.logError("an error message");

	logger += std::make_shared<EmphasisDecorator>();
	logger += std::make_shared<TimestampDecorator>();

	logger.logWarning("a warning message");
	logger.logError("an error message");

	std::system("pause");
	return 0;
}
