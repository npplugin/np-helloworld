#include <sstream>
#include <windows.h>

#define MY_LOG(msg) do {std::stringstream ss; ss << "[np_logs] " << __FUNCTION__ << ", " << __FILE__ << "(" << __LINE__ << "): " << msg; OutputDebugString(ss.str().c_str());} while(0)
