#pragma once

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

#include <cassert>
#include <string>
#include <stdexcept>
#include <unordered_map>

namespace flac_bindings {

    class Library {
    private:
    #ifdef WIN32
        HMODULE lib = NULL;
    #else
        void* lib = NULL;
    #endif

        std::unordered_map<std::string, void*> cache;

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
            return Library::load(filename, LIBRARY_EXTENSION);
        }

        static Library* load(const std::string &filename, const std::string &ext) {
            std::string f(filename);
            if(f[0] != '/') {
                f += "." + ext;
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
            void* ptr = nullptr;
            std::string symbolStr = symbol;
            auto it = cache.find(symbolStr);
            if(it != cache.end()) {
                ptr = it->second;
            } else {
    #ifdef WIN32
                GetLastError();
                ptr = GetProcAddress(lib, symbol);
    #else
                dlerror();
                ptr = dlsym(lib, symbol);
    #endif
                auto errorMessage = getLastError();
                if(!errorMessage.empty()) {
                    fprintf(stderr, "Could not load symbol %s: %s\n", symbol, errorMessage.c_str());
                    assert(ptr != nullptr);
                }
                cache[symbolStr] = ptr;
            }

            return reinterpret_cast<Type>(ptr);
        }

        std::string getLastError() {
            std::string error;
    #ifdef WIN32
            LPVOID lpMsgBuf;
            DWORD dw = GetLastError();

            FormatMessage(
                FORMAT_MESSAGE_ALLOCATE_BUFFER |
                FORMAT_MESSAGE_FROM_SYSTEM |
                FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL,
                dw,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPTSTR) &lpMsgBuf,
                0, NULL );
            error = (char*) lpMsgBuf;
            LocalFree(lpMsgBuf);
    #else
            const char* ptr = dlerror();
            error = ptr != nullptr ? ptr : "";
    #endif
            return error;
        }
    };

}
