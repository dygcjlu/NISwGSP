#ifndef COMMON_DEF_H
#define COMMON_DEF_H

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
//#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_INFO
#include "../3rd/spdlog/spdlog.h"


extern const std::string strLogFileName;

#define FUNC_RETURN_SUCCESS 0
#define FUNC_RETURN_FAILED -1
/*
#ifdef SAVE_LOG_TO_FILE
#define MY_LOG_DEBUG(logger, ...) SPDLOG_LOGGER_DEBUG(logger, __VA_ARGS__)
//#define LOG(...) LogWrapper(__FILE__, __LINE__, __VA_ARGS__)

#else

#define MY_LOG_DEBUG(logger, ...)  printf(__VA_ARGS__)

#endif
*/


std::shared_ptr<spdlog::logger> GetLoggerHandle();


#endif //COMMON_DEF_H