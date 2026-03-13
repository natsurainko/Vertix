#ifndef VERTIX_ENGINE_API_H
#define VERTIX_ENGINE_API_H

#ifdef Vertix_Engine_EXPORTS
#  define VERTIX_ENGINE_API __declspec(dllexport)
#else
#  define VERTIX_ENGINE_API __declspec(dllimport)
#endif

#endif /* VERTIX_ENGINE_API_H */