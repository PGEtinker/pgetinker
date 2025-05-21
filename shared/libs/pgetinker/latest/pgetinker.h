#ifndef PGETINKER_H
#define PGETINKER_H

#if defined(__EMSCRIPTEN__)
#include <emscripten.h>
#include <iostream>
#endif

// Loads a file from a URL into the emscripten virtual filesystem
static inline void pgetinker_file_resolve(const char* url, const char* mountPath)
{
    #if defined(__EMSCRIPTEN__)
    emscripten_wget(url, mountPath);
    emscripten_sleep(0);
    #endif
}

#if defined(__EMSCRIPTEN__)
namespace {
    struct AutoRun {
        AutoRun() {
            std::cout << "INFO: Using Escripten " << __EMSCRIPTEN_major__ << "." << __EMSCRIPTEN_minor__ << "." << __EMSCRIPTEN_tiny__ << "\n";
        }
    };

    static AutoRun autoRunInstance;
}
#endif


#endif // PGETINKER_H
