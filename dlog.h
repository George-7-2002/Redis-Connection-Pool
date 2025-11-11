#ifndef LOG_H
#define LOG_H
#include <vector>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/async.h>
#include "spdlog/sinks/stdout_color_sinks.h"
#ifndef SPDLOG_TRACE_ON
#define SPDLOG_TRACE_ON
#endif

#ifndef SPDLOG_DEBUG_ON
#define SPDLOG_DEBUG_ON
#endif

// #define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE // Must define this macro to output file name and line number

/**
 * @brief A singleton wrapper for the spdlog library to provide a global logger.
 */
class DLog
{
public:
    
    static  DLog* GetInstance()
    {
        static DLog dlogger;
        return &dlogger;
    }
    

    std::shared_ptr<spdlog::logger> getLogger()
    {
        return log_;
    }
    
    /**
     * @brief Sets the global logging level (e.g., 'trace', 'debug', 'info').
     */
    // trace debug info warn err critical off
    static void SetLevel(char *log_level);
    // {
    //     if(strcmp(log_level, "trace") == 0) {
    //         level_ =  spdlog::level::trace;
    //     }else if(strcmp(log_level, "debug") == 0) {
    //         level_ =  spdlog::level::debug;
    //     }else if(strcmp(log_level, "info") == 0) {
    //         level_ =  spdlog::level::info;
    //     }else if(strcmp(log_level, "warn") == 0) {
    //         level_ =  spdlog::level::warn;
    //     }else if(strcmp(log_level, "err") == 0) {
    //         level_ =  spdlog::level::err;
    //     }else if(strcmp(log_level, "critical") == 0) {
    //         level_ =  spdlog::level::critical;
    //     }else if(strcmp(log_level, "off") == 0) {
    //         level_ =  spdlog::level::off;
    //     } else {
    //         printf("level: %s is invalid\n", log_level);
    //     }
    // }
private:
    /**
     * @brief Private constructor. Configures sinks (console, daily file)
     * and registers the logger.
     */
    DLog(/* args */) {
        // spdlog::set_pattern("[%Y-%m-%d %H:%M:%S %e] [%n] [%l] [process %P] [thread %t] %v");
        std::vector<spdlog::sink_ptr> sinkList;
    #if 1  // Log to console
        auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        consoleSink->set_level(level_);
        //consoleSink->set_pattern("[multi_sink_example] [%^%l%$] %v");
        //consoleSink->set_pattern("[%m-%d %H:%M:%S.%e][%^%L%$]  %v");
        // consoleSink->set_pattern("%Y-%m-%d %H:%M:%S [%l] [%t] - <%s>|<%#>|<%!>,%v");
        // consoleSink->set_pattern("[%Y-%m-%d %H:%M:%S %e] [%n]  [thread %t] %v");
        consoleSink->set_pattern("[%Y-%m-%d %H:%M:%S.%e][thread %t][%@,%!][%l] : %v");
        sinkList.push_back(consoleSink);
    #endif
        // Log to file
        auto dailySink = std::make_shared<spdlog::sinks::daily_file_sink_mt>("logs/daily.log", 23, 59);
        dailySink->set_level(level_);
        // dailySink->set_pattern("[%Y-%m-%d %H:%M:%S %e] [%n] [%l] [process %P] [thread %t] %v");
        dailySink->set_pattern("[%Y-%m-%d %H:%M:%S.%e][thread %t][%@,%!][%l] : %v");
        sinkList.push_back(dailySink);

        log_ = std::make_shared<spdlog::logger>("both", begin(sinkList), end(sinkList));
        //register it if you need to access it globally
        spdlog::register_logger(log_);
 
  
        spdlog::flush_every(std::chrono::seconds(1));
    }
    ~DLog() { }

private:
    std::shared_ptr<spdlog::logger> log_;
    // static spdlog::level::level_enum level_ = spdlog::level::info;
    static spdlog::level::level_enum level_;
};

// --- Logging macros that call the global logger instance ---
#define LogTrace(...) SPDLOG_LOGGER_CALL(DLog::GetInstance()->getLogger().get(), spdlog::level::trace, __VA_ARGS__)
#define LogDebug(...) SPDLOG_LOGGER_CALL(DLog::GetInstance()->getLogger().get(), spdlog::level::debug, __VA_ARGS__)
#define LogInfo(...) SPDLOG_LOGGER_CALL(DLog::GetInstance()->getLogger().get(), spdlog::level::info, __VA_ARGS__)
#define LogWarn(...) SPDLOG_LOGGER_CALL(DLog::GetInstance()->getLogger().get(), spdlog::level::warn, __VA_ARGS__)
#define LogError(...) SPDLOG_LOGGER_CALL(DLog::GetInstance()->getLogger().get(), spdlog::level::err, __VA_ARGS__)
#define LogCritical(...) SPDLOG_LOGGER_CALL(DLog::GetInstance()->getLogger().get(), spdlog::level::critical, __VA_ARGS__)
#endif
