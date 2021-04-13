#include <sstream>
#include <windows.h>

#define MY_LOG(fmt, ...) do { \
char buf[2048] = { 0 }; \
sprintf(buf, fmt, __VA_ARGS__); \
std::stringstream ss; \
ss << "[np_logs] " << __FUNCTION__ << ", " << __FILE__ << "(" << __LINE__ << "): " << buf; \
OutputDebugString(ss.str().c_str());} \
while(0)
