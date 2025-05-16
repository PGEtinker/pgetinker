#ifndef PGETINKER_H
#define PGETINKER_H

#if defined(__EMSCRIPTEN__)
#include <emscripten.h>
#endif

// Loads a file from a URL into the emscripten virtual filesystem
static inline void pgetinker_file_resolve(const char* url, const char* mountPath)
{
    #if defined(__EMSCRIPTEN__)
    emscripten_wget(url, mountPath);
    emscripten_sleep(0);
    #endif
}


#endif // PGETINKER_H
