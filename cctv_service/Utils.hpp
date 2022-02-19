#ifndef __UTILS_HPP__
#define __UTILS_HPP__
#include <windows.h>
#include "Log.hpp"


#ifndef SAFE_DELETE
#define SAFE_DELETE(x) if(x) delete x
#endif

#ifndef CCTV_LOG
using namespace cctv;

#define CCTV_LOG_HLV(logLevel, fmt,...) OutputLog(logLevel, _T(__FUNCTION__), __LINE__, fmt, ##__VA_ARGS__)
#define CCTV_LOG_ERROR(fmt,...)				OutputLog(LOG_ERROR, _T(__FUNCTION__), __LINE__, fmt, ##__VA_ARGS__)
#define CCTV_LOG_WARN(fmt,...)				OutputLog(LOG_WARN, _T(__FUNCTION__), __LINE__, fmt, ##__VA_ARGS__)
#define CCTV_LOG_DEBUG(fmt,...)				OutputLog(LOG_DEBUG, _T(__FUNCTION__), __LINE__, fmt, ##__VA_ARGS__)
#define CCTV_LOG_INFO(fmt,...)				OutputLog(LOG_INFO, _T(__FUNCTION__), __LINE__, fmt, ##__VA_ARGS__)
#define CCTV_LOG(fmt,...)				OutputLog(LOG_DEBUG, _T(__FUNCTION__), __LINE__, fmt, ##__VA_ARGS__)
#endif


#endif