#ifndef ALOTZ_LOG_H
#define ALOTZ_LOG_H

#include <string>
#include <stdint.h>
#include <memory>
#include <list>
#include <sstream>
#include <fstream>
#include <vector>
#include <stdarg.h>
#include <map>
#include "util.h"
#include "singleton.h"
#include "thread.h"

#define ALOTZ_LOG_LEVEL(logger, level) \
    if(logger->getLevel() <= level) \
        alotz::LogEventWrap(alotz::LogEvent::ptr(new alotz::LogEvent(logger, level, \
                        __FILE__, __LINE__, 0, alotz::GetThreadId(),\
                alotz::GetFiberId(), time(0), alotz::Thread::GetName()))).getSS()

#define ALOTZ_LOG_DEBUG(logger) ALOTZ_LOG_LEVEL(logger, alotz::LogLevel::DEBUG)
#define ALOTZ_LOG_INFO(logger) ALOTZ_LOG_LEVEL(logger, alotz::LogLevel::INFO)
#define ALOTZ_LOG_WARN(logger) ALOTZ_LOG_LEVEL(logger, alotz::LogLevel::WARN)
#define ALOTZ_LOG_ERROR(logger) ALOTZ_LOG_LEVEL(logger, alotz::LogLevel::ERROR)
#define ALOTZ_LOG_FATAL(logger) ALOTZ_LOG_LEVEL(logger, alotz::LogLevel::FATAL)

#define ALOTZ_LOG_FMT_LEVEL(logger, level, fmt, ...) \
    if(logger->getLevel() <= level) \
        alotz::LogEventWrap(alotz::LogEvent::ptr(new alotz::LogEvent(logger, level, \
                        __FILE__, __LINE__, 0, alotz::GetThreadId(),\
                alotz::GetFiberId(), time(0), alotz::Thread::GetName()))).getEvent()->format(fmt, __VA_ARGS__)

#define ALOTZ_LOG_FMT_DEBUG(logger, fmt, ...) ALOTZ_LOG_FMT_LEVEL(logger, alotz::LogLevel::DEBUG, fmt, __VA_ARGS__)
#define ALOTZ_LOG_FMT_INFO(logger, fmt, ...)  ALOTZ_LOG_FMT_LEVEL(logger, alotz::LogLevel::INFO, fmt, __VA_ARGS__)
#define ALOTZ_LOG_FMT_WARN(logger, fmt, ...)  ALOTZ_LOG_FMT_LEVEL(logger, alotz::LogLevel::WARN, fmt, __VA_ARGS__)
#define ALOTZ_LOG_FMT_ERROR(logger, fmt, ...) ALOTZ_LOG_FMT_LEVEL(logger, alotz::LogLevel::ERROR, fmt, __VA_ARGS__)
#define ALOTZ_LOG_FMT_FATAL(logger, fmt, ...) ALOTZ_LOG_FMT_LEVEL(logger, alotz::LogLevel::FATAL, fmt, __VA_ARGS__)

#define ALOTZ_LOG_ROOT() alotz::LoggerMgr::GetInstance()->getRoot()
#define ALOTZ_LOG_NAME(name) alotz::LoggerMgr::GetInstance()->getLogger(name)

namespace alotz {

class Logger;
class LoggerManager;

class LogLevel {
public:
    enum Level {
        UNKNOW = 0,
        DEBUG = 1,
        INFO = 2,
        WARN = 3,
        ERROR = 4,
        FATAL = 5
    };

    static const char* ToString(LogLevel::Level level);
    static LogLevel::Level fromString(const std::string& str);
};

class LogEvent {
public:
    typedef std::shared_ptr<LogEvent> ptr;
    LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level
            ,const char* file, int32_t m_line, uint32_t elapse
            , uint32_t thread_id, uint32_t fiber_id, uint64_t time
            , const std::string& thread_name);

    const char* getFile() const { return m_file;}
    int32_t getLine() const { return m_line;}
    uint32_t getElapse() const { return m_elapse;}
    uint32_t getThreadId() const { return m_threadId;}
    uint32_t getFiberId() const { return m_fiberId;}
    uint64_t getTime() const { return m_time;}
    const std::string& getThreadName() const { return m_threadName; }
    std::string getContent() const { return m_ss.str();}
    std::shared_ptr<Logger> getLogger() const { return m_logger;}
    LogLevel::Level getLevel() const { return m_level;}

    std::stringstream& getSS() { return m_ss;}
    void format(const char* fmt, ...);
    void format(const char* fmt, va_list al);
private:
    const char* m_file = nullptr;  //?????????
    int32_t m_line = 0;            //??????
    uint32_t m_elapse = 0;         //???????????????????????????????????????
    uint32_t m_threadId = 0;       //??????id
    uint32_t m_fiberId = 0;        //??????id
    uint64_t m_time = 0;           //?????????
    std::string m_threadName;
    std::stringstream m_ss;

    std::shared_ptr<Logger> m_logger;
    LogLevel::Level m_level;
};

class LogEventWrap {
public:
    LogEventWrap(LogEvent::ptr e);
    ~LogEventWrap();
    LogEvent::ptr getEvent() const { return m_event;}
    std::stringstream& getSS();
private:
    LogEvent::ptr m_event;
};

class LogFormatter {
public:
    typedef std::shared_ptr<LogFormatter> ptr;
    LogFormatter(const std::string& pattern);

    //%t    %thread_id %m%n
    std::string format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);
public:
    class FormatItem {
    public:
        typedef std::shared_ptr<FormatItem> ptr;
        virtual ~FormatItem() {}
        virtual void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
    };

    void init();

    bool isError() const { return m_error; }
    const std::string getPattern() const { return m_pattern; }
private:
    std::string m_pattern;
    std::vector<FormatItem::ptr> m_items;
    bool m_error = false;
};

class LogAppender {
friend class Logger;
public:
    typedef std::shared_ptr<LogAppender> ptr;
    typedef SpinLock MutexType;
    virtual ~LogAppender() {}

    virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
    virtual std::string toYamlString() = 0;

    void setFormatter(LogFormatter::ptr val);
    LogFormatter::ptr getFormatter();

    LogLevel::Level getLevel() const { return m_level;}
    void setLevel(LogLevel::Level val) { m_level = val;}
protected:
    LogLevel::Level m_level = LogLevel::DEBUG;
    bool m_hasFormatter = false;
    MutexType m_mutex;
    LogFormatter::ptr m_formatter;
};

class Logger : public std::enable_shared_from_this<Logger> {
friend class LoggerManager;
public:
    typedef std::shared_ptr<Logger> ptr;
    typedef SpinLock MutexType;

    Logger(const std::string& name = "root");
    void log(LogLevel::Level level, LogEvent::ptr event);

    void debug(LogEvent::ptr event);
    void info(LogEvent::ptr event);
    void warn(LogEvent::ptr event);
    void error(LogEvent::ptr event);
    void fatal(LogEvent::ptr event);

    void addAppender(LogAppender::ptr appender);
    void delAppender(LogAppender::ptr appender);
    void clearAppenders();
    LogLevel::Level getLevel() const { return m_level;}
    void setLevel(LogLevel::Level val) { m_level = val;}

    const std::string& getName() const { return m_name;}

    void setFormatter(LogFormatter::ptr val);
    void setFormatter(const std::string& val);
    LogFormatter::ptr getFormatter();

    std::string toYamlString();
private:
    std::string m_name;                     //????????????
    LogLevel::Level m_level;                //????????????
    MutexType m_mutex;
    std::list<LogAppender::ptr> m_appenders;//Appender??????
    LogFormatter::ptr m_formatter;
    Logger::ptr m_root;
};

class StdoutLogAppender : public LogAppender {
public:
    typedef std::shared_ptr<StdoutLogAppender> ptr;
    void log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override;
    std::string toYamlString() override;
};

class FileLogAppender : public LogAppender {
public:
    typedef std::shared_ptr<FileLogAppender> ptr;
    FileLogAppender(const std::string& filename);
    void log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override;
    std::string toYamlString() override;

    //?????????????????????????????????????????????true
    bool reopen();
private:
    std::string m_filename;
    std::ofstream m_filestream;
    uint64_t m_lastTime = 0;
};

class LoggerManager {
public:
    typedef SpinLock MutexType;

    LoggerManager();
    Logger::ptr getLogger(const std::string& name);

    void init();
    Logger::ptr getRoot() const { return m_root;}

    std::string toYamlString();
private:
    MutexType m_mutex;
    std::map<std::string, Logger::ptr> m_loggers;
    Logger::ptr m_root;
};

typedef alotz::Singleton<LoggerManager> LoggerMgr;

}

#endif
