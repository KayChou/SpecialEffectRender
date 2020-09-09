#ifndef log4cpphelper_h
#define log4cpphelper_h

#include <string>
#include <stdarg.h>

//using std::string;

#include "singleton.h"

#include <log4cpp/Appender.hh>
#include <log4cpp/Category.hh>
#include <log4cpp/Appender.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/Layout.hh>
#include <log4cpp/BasicLayout.hh>
#include <log4cpp/Priority.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/RollingFileAppender.hh>
#include <log4cpp/PatternLayout.hh>
#include <log4cpp/NDC.hh>

#define LOG_INFO_ARGS(formatStr, ...) {\
        singleton::Singleton<Log4cppHelper>::Instance().LogInfo(log4cpp::Priority::INFO, formatStr, __VA_ARGS__);  \
    }

#define LOG_NOTICE_ARGS(formatStr, ...) {\
        singleton::Singleton<Log4cppHelper>::Instance().LogInfo(log4cpp::Priority::NOTICE, formatStr, __VA_ARGS__);  \
    }

#define LOG_WARN_ARGS(formatStr, ...) {\
        singleton::Singleton<Log4cppHelper>::Instance().LogInfo(log4cpp::Priority::WARN, formatStr, __VA_ARGS__);  \
    }

#define LOG_ERROR_ARGS(formatStr, ...) {\
        singleton::Singleton<Log4cppHelper>::Instance().LogInfo(log4cpp::Priority::ERROR, formatStr, __VA_ARGS__);  \
    }

#define LOG_CRIT_ARGS(formatStr, ...) {\
        singleton::Singleton<Log4cppHelper>::Instance().LogInfo(log4cpp::Priority::CRIT, formatStr, __VA_ARGS__);  \
    }

#define LOG_ALTER_ARGS(formatStr, ...) {\
        singleton::Singleton<Log4cppHelper>::Instance().LogInfo(log4cpp::Priority::ALERT, formatStr, __VA_ARGS__);  \
    }

#define LOG_INFO(msg)	\
	LOG_INFO_ARGS("  %-48s| %s {%s} [%d]", msg, __FILE__, __FUNCTION__,  __LINE__);

#define LOG_NOTICE(msg) \
	LOG_NOTICE_ARGS("%-48s| %s {%s} [%d]", msg, __FILE__, __FUNCTION__, __LINE__);

#define LOG_WARN(msg) \
	LOG_WARN_ARGS("  %-48s| %s {%s} [%d]", msg, __FILE__, __FUNCTION__, __LINE__);

#define LOG_ERROR(msg) \
	LOG_ERROR_ARGS(" %-48s| %s {%s} [%d]", msg, __FILE__, __FUNCTION__, __LINE__);

#define LOG_CRIT(msg) \
	LOG_CRIT_ARGS("  %-48s| %s {%s} [%d]", msg, __FILE__, __FUNCTION__, __LINE__);

#define LOG_ALERT(msg) \
	LOG_ALTER_ARGS(" %-48s| %s {%s} [%d]", msg, __FILE__, __FUNCTION__, __LINE__);

class Log4cppHelper : public singleton::Singleton<Log4cppHelper>
{

public:
	~Log4cppHelper();

	void LogInfo(log4cpp::Priority::Value priority,
		const char* stringFormat,
		...);

private:
	Log4cppHelper();
	Log4cppHelper(const Log4cppHelper &);
	Log4cppHelper& operator=(const Log4cppHelper &);

private:
	friend singleton::Singleton<Log4cppHelper>;
};

#endif // log4cpphelper_h