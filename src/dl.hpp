#ifndef DL
#define DL

#ifdef WIN32
#define WIN32_MEAN_AND_LEAN
#include <windows.h>
#define LIBRARY_EXTENSION "dll"
#else
#include <dlfcn.h>
#if defined(__APPLE__) && defined(__MACH__)
#define LIBRARY_EXTENSION "dylib"
#else
#define LIBRARY_EXTENSION "so"
#endif
#endif

#include <string>
#include <stdexcept>

class Library {
private:
#ifdef WIN32
    HMODULE lib = NULL;
#else
    void* lib = NULL;
#endif

    Library(
#ifdef WIN32
        HMODULE lib
#else
        void* lib
#endif
    ) {
        this->lib = lib;
    }

public:
    static Library* load(const std::string &filename) {
        std::string f(filename);
        if(f[0] != '/') {
            f += LIBRARY_EXTENSION;
        }

#ifdef WIN32
        HMODULE lib = LoadLibrary(f.c_str() + 3); //delete the lib part (libFLAC -> FLAC)
        if(lib == NULL) {
            lib = LoadLibrary(f.c_str()); //now not
            if(lib == NULL) {
                return nullptr;
            }
        }
#else
        void* lib = dlopen(f.c_str(), RTLD_LOCAL | RTLD_LAZY);
        if(lib == NULL) {
            return nullptr;
        }
#endif
        return new Library(lib);
    }

    ~Library() {
        if(lib != NULL) {
#ifdef WIN32
            FreeLibrary(lib);
#else
            dlclose(lib);
#endif
        }
    }

    template<typename Type>
    Type getSymbolAddress(const char* symbol) {
        void* ptr;
#ifdef WIN32
        GetLastError();
        ptr = GetProcAddress(lib, symbol);
#else
        dlerror();
        ptr = dlsym(lib, symbol);
#endif
        return reinterpret_cast<Type>(ptr);
    }

    std::string getLastError() {
        std::string error;
#ifdef WIN32
        // http://stackoverflow.com/questions/3006229/get-a-text-from-the-error-code-returns-from-the-getlasterror-function
        DWORD   dwLastError = GetLastError();
        TCHAR   lpBuffer[256] = "?";
        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
                     NULL,
                     dwLastError,
                     MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),
                     lpBuffer,
                     DWORD(strlen(lpBuffer)-1),
                     NULL);
        error = lpBuffer;
#else
        error = dlerror();
#endif
        return error;
    }
};

#endif
