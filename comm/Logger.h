#include <spdlog/spdlog.h>
#include <open62541/plugin/log.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace tt
{
class Logger
{
public:
    Logger()
    {
       auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
       spdlogger = std::make_shared<spdlog::logger>("Client", console_sink);
       spdlogger->set_level(spdlog::level::debug);
       spdlog::register_logger(spdlogger);
    }

    UA_Logger getUALogger()
    {
        UA_Logger logger;
        logger.context = spdlogger.get();
        logger.log = log;
        logger.clear=nullptr;
        return logger;
    }

 private:
    static void log(void* logContext, UA_LogLevel level, UA_LogCategory, const char* msg, va_list args)
    {
        char buf[512];
        vsnprintf(buf, 512, msg, args);
        auto logger = static_cast<spdlog::logger*>(logContext);
        switch(level)
        {
            case UA_LOGLEVEL_DEBUG:
                logger->debug(buf);
                break;
            case UA_LOGLEVEL_INFO:
                logger->info(buf);
                break;
            case UA_LOGLEVEL_ERROR:
            case UA_LOGLEVEL_FATAL:
                logger->error(buf);
                break;
            case UA_LOGLEVEL_TRACE:
                logger->trace(buf);
                break;
            case UA_LOGLEVEL_WARNING:
                logger->warn(buf);
                break;
        }
    }
    std::shared_ptr<spdlog::logger> spdlogger;
};
}