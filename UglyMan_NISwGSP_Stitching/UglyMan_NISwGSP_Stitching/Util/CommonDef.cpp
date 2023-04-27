
#include <iostream>
#include "CommonDef.h"
#include "spdlog/sinks/rotating_file_sink.h"

const std::string LOG_HANDLE = "stitch_log";

std::shared_ptr<spdlog::logger> GetLoggerHandle()
{
    static int bIsLogInit = false;
    if (!bIsLogInit)
    {
        
        try
        {

            auto file_logger = spdlog::rotating_logger_mt(LOG_HANDLE, "./log/stitch_log.log", 1024 * 1024 * 5, 1);
            spdlog::set_level(spdlog::level::debug);
            file_logger->flush_on(spdlog::level::info);
            //[2022-07-18 15:23:16.564] [yolact_logger] [info] [main.cpp:40] start control
            // spdlog::set_pattern("%+");
            spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e][%t][%l][%s:%!:%#] %v");

            SPDLOG_LOGGER_TRACE(file_logger, "Trace log is enabled!");
            SPDLOG_LOGGER_DEBUG(file_logger, "DEBUG log is enabled!");
            SPDLOG_LOGGER_INFO(file_logger, "INFO log is enabled!");
        }
        catch (const spdlog::spdlog_ex &ex)
        {
            std::cout << "Log initialization failed: " << ex.what() << std::endl;
        }
        catch (...)
        {
            std::cout << "Log initialization failed, unknown exception!" << std::endl;
        }

        bIsLogInit = true;
    }

    return spdlog::get(LOG_HANDLE);
}