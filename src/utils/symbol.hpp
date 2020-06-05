#pragma once

#include <napi.h>
#include <type_traits>
#include "dl.hpp"

namespace flac_bindings {

    extern Library* libFlac;
    extern Napi::ObjectReference module;

    template<typename SymbolType>
    class Symbol {
        const char* symbolName = nullptr;

        inline void throwIfLibFlacIsNotLoaded() {
            if(!libFlac) {
                throw Napi::Error::New(module.Env(), "libFLAC has not been loaded yet");
            }
        }

    public:

        Symbol(const char* symbolName): symbolName(symbolName) {}

        class Binded {
            Library* library;
            const char* symbolName;

            friend class Symbol;

            Binded(Library* library, const char* symbolName): library(library), symbolName(symbolName) {}

        public:
            template<
                class... ArgTypes,
                typename FuncType = std::remove_pointer_t<SymbolType>,
                typename ResultType = std::result_of_t<SymbolType(ArgTypes...)>,
                typename std::enable_if<
                    std::is_pointer<SymbolType>::value && std::is_function<FuncType>::value,
                    int
                >::type = 0
            >
            inline ResultType operator()(ArgTypes&&... args) {
                return library->getSymbolAddress<SymbolType>(symbolName)(std::forward<ArgTypes>(args)...);
            }

            inline SymbolType& operator*() {
                static_assert(!std::is_function<std::remove_pointer_t<SymbolType>>::value, "Function type is not valid");
                return *library->getSymbolAddress<SymbolType*>(symbolName);
            }

            inline SymbolType& operator->() {
                return *(*this);
            }

            inline operator SymbolType&() {
                return *(*this);
            }
        };

        template<
            class... ArgTypes,
            typename FuncType = std::remove_pointer_t<SymbolType>,
            typename ResultType = std::result_of_t<SymbolType(ArgTypes...)>,
            typename std::enable_if<
                std::is_pointer<SymbolType>::value && std::is_function<FuncType>::value,
                int
            >::type = 0
        >
        ResultType operator()(ArgTypes&&... args) {
            throwIfLibFlacIsNotLoaded();
            return (*this)(libFlac)(std::forward<ArgTypes>(args)...);
        }

        SymbolType& operator*() {
            throwIfLibFlacIsNotLoaded();
            return *(*this)(libFlac);
        }

        SymbolType& operator->() {
            throwIfLibFlacIsNotLoaded();
            return *(*this)(libFlac);
        }

        operator SymbolType&() {
            throwIfLibFlacIsNotLoaded();
            return *(*this)(libFlac);
        }

        inline Binded operator()(Library* library) {
            return Binded(library, symbolName);
        }
    };

    template<typename T, typename std::enable_if<std::is_function<T>::value, int>::type = 0>
    using FunctionSymbol = Symbol<std::add_pointer_t<T>>;

}
