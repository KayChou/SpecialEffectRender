#include "log.h"
#include "timstamp.h"

#define LOG_FILE_NAME "render"

Log4cppHelper::Log4cppHelper(const Log4cppHelper &)
{}

Log4cppHelper& Log4cppHelper::operator=(const Log4cppHelper &)
{
	return *this;
}

Log4cppHelper::Log4cppHelper()
{
	log4cpp::Appender *appender = new log4cpp::RollingFileAppender("default",
		string(LOG_FILE_NAME) + //string(".log")
		Timestamp::now().toFormattedString(
			Timestamp::FILENAME_STYLE) + string(".log")
	);
	log4cpp::PatternLayout *patternlayout = new log4cpp::PatternLayout();
	patternlayout->setConversionPattern("%d [%p] %m%n");
	appender->setLayout(patternlayout);

	log4cpp::Category& log = log4cpp::Category::getInstance(
		std::string(LOG_FILE_NAME));
	log.addAppender(appender);
}

Log4cppHelper::~Log4cppHelper()
{
	log4cpp::Category::shutdown();
}

void Log4cppHelper::LogInfo(log4cpp::Priority::Value priority,
	const char* stringFormat,
	...)
{
	va_list va;

	va_start(va, stringFormat);
	log4cpp::Category::getInstance(std::string(LOG_FILE_NAME)).logva(
		priority,
		stringFormat,
		va);

	va_end(va);
}
