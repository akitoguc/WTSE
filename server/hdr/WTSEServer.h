#pragma once

#ifdef WTSESERVER_EXPORTS
#define WTSESERVER_API __declspec(dllexport)
#else
#define WTSESERVER_API __declspec(dllimport)
#endif
