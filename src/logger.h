#ifndef _LOGGER_H
#define _LOGGER_H

#include <string>
#include <memory>

#define LOG_INFO(message) mrtp::Logger::get().info(message);
#define LOG_ERROR(message) mrtp::Logger::get().error(message);
#define LOG_DEBUG(message) mrtp::Logger::get().debug(message);
#define LOG_WARNING(message) mrtp::Logger::get().warning(message);


namespace mrtp {

enum class LogLevel
{
    ERROR = 0,
    WARNING = 1,
    INFO = 2,
    DEBUG = 3
};


enum class LogFormatterType
{
    DEFAULT,
    EASY,
    DUMMY
};


class LogFormatter
{
public:
    LogFormatter() = default;

    virtual ~LogFormatter() = default;

    virtual void error(const std::string&) = 0;
    virtual void warning(const std::string&) = 0;
    virtual void info(const std::string&) = 0;
    virtual void debug(const std::string&) = 0;
};


class Logger
{
public:
    static Logger& get();

    void error(const std::string&);
    void warning(const std::string&);
    void info(const std::string&);
    void debug(const std::string&);

    void set_config(LogLevel, std::shared_ptr<LogFormatter>);

private:
    Logger();
    ~Logger() = default;

    LogLevel level_;
    std::shared_ptr<LogFormatter> formatter_;
};


std::shared_ptr<LogFormatter> create_log_formatter(LogFormatterType);

}

#endif // _LOGGER_H
