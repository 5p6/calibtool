#ifndef CONFIG_H
#define CONFIG_H




#ifdef BUILD_SHARED
#define CALIB_API __declspec(dllexport)
#else
#define CALIB_API
#endif


#endif