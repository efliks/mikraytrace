#include <iostream>

#include "logger.h"

#ifdef USE_EASYLOGGING
#include <easylogging++.h>
#endif


namespace mrtp {

#ifdef USE_EASYLOGGING

INITIALIZE_EASYLOGGINGPP
class EasyLogFormatter : public LogFormatter
{
public:
    EasyLogFormatter() = default;
    ~EasyLogFormatter() override = default;

    void debug(const std::string& message) override
    {
        LOG(DEBUG) << message;
    }

    void info(const std::string& message) override
    {
        LOG(INFO) << message;
    }

    void warning(const std::string& message) override
    {
        LOG(WARNING) << message;
    }

    void error(const std::string& message) override
    {
        LOG(ERROR) << message;
    }
};
#endif  // USE_EASYLOGGING

class DefaultLogFormatter : public LogFormatter
{
public:
    DefaultLogFormatter() = default;
    ~DefaultLogFormatter() override = default;

    void debug(const std::string& message) override
    {
        std::cout << "DEBUG: " << message << std::endl;
    }

    void info(const std::string& message) override
    {
        std::cout << "INFO: " << message << std::endl;
    }

    void warning(const std::string& message) override
    {
        std::cout << "WARNING: " << message << std::endl;
    }

    void error(const std::string& message) override
    {
        std::cout << "ERROR: " << message << std::endl;
    }
};


class DummyLogFormatter : public LogFormatter
{
public:
    DummyLogFormatter() = default;
    ~DummyLogFormatter() override = default;

    void debug(const std::string& message) override
    {
    }

    void info(const std::string& message) override
    {
    }

    void warning(const std::string& message) override
    {
    }

    void error(const std::string& message) override
    {
    }
};


void Logger::debug(const std::string& message)
{
    if (level_ >= LogLevel::DEBUG) {
        formatter_->debug(message);
    }
}


void Logger::info(const std::string& message)
{
    if (level_ >= LogLevel::INFO) {
        formatter_->info(message);
    }
}


void Logger::warning(const std::string& message)
{
    if (level_ >= LogLevel::WARNING) {
        formatter_->warning(message);
    }
}


void Logger::error(const std::string& message)
{
    if (level_ >= LogLevel::ERROR) {
        formatter_->error(message);
    }
}


Logger& Logger::get()
{
    static Logger instance;
    return instance;
}


Logger::Logger()
    : level_(LogLevel::DEBUG)
    , formatter_(std::shared_ptr<LogFormatter>(new DefaultLogFormatter()))
{
}


void Logger::set_config(LogLevel log_level, std::shared_ptr<LogFormatter> formatter)
{
    level_ = log_level;
    formatter_ = formatter;
}


std::shared_ptr<LogFormatter> create_log_formatter(LogFormatterType formatter_type)
{
#ifdef USE_EASYLOGGING
    if (formatter_type == LogFormatterType::EASY)
        return std::shared_ptr<LogFormatter>(new EasyLogFormatter());
#endif
    if (formatter_type == LogFormatterType::DEFAULT)
        return std::shared_ptr<LogFormatter>(new DefaultLogFormatter());

    return std::shared_ptr<LogFormatter>(new DummyLogFormatter());
}

}
