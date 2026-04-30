#ifndef VERTIX_API_H
#define VERTIX_API_H

#ifdef Vertix_EXPORTS
#  define VERTIX_API __declspec(dllexport)
#else
#  define VERTIX_API __declspec(dllimport)
#endif

#endif /* VERTIX_API_H */