#ifndef MYFUNCTION_H
#define MYFUNCTION_H

#include <AnyStorage.h>

#include <algorithm>
#include <array>
#include <bitset>
#include <cmath>
#include <concepts>
#include <iostream>
#include <memory>
#include <ostream>
#include <tuple>
#include <type_traits>

#include "DebugPrint.h"

namespace datastructure {

template <class T>
struct StripSignature;

template <class R, bool NoExcept, class... A>
struct StripSignature<R (*) (A...) noexcept (NoExcept)> {
    using type = R (A...);
};

template <typename R, typename ClassOrType, bool NoExcept, typename... A>
struct StripSignature<R (ClassOrType::*) (A...) noexcept (NoExcept)> {
    using type = R (A...);
};

template <typename R, typename ClassOrType, bool NoExcept, typename... A>
struct StripSignature<R (ClassOrType::*) (A...) & noexcept (NoExcept)> {
    using type = R (A...);
};

template <typename R, typename ClassOrType, bool NoExcept, typename... A>
struct StripSignature<R (ClassOrType::*) (A...) const noexcept (NoExcept)> {
    using type = R (A...);
};

template <typename R, typename ClassOrType, bool NoExcept, typename... A>
struct StripSignature<R (ClassOrType::*) (A...) const & noexcept (NoExcept)> {
    using type = R (A...);
};

enum class ManageStorageEnum { CreateStorage, CloneStorage, DeleteStorage, MoveStorage, Invalid };

template <class Functor, int Size>
class BaseFunctionManager {
    static constexpr size_t MAX_SIZE = sizeof (AnyData<Size>);
    static constexpr size_t MAX_ALIGN = alignof (AnyData<Size>);

    static constexpr bool isLocal = (std::is_trivially_copyable_v<Functor>) &&
                                    (sizeof (Functor) <= MAX_SIZE) &&
                                    (alignof (Functor) <= MAX_ALIGN);

   public:
    static void manageStorage (AnyData<Size>& destination, AnyData<Size>& source,
                               ManageStorageEnum type) {
        switch (type) {
            case ManageStorageEnum::CloneStorage:
                clone (destination, source);
                break;
            case ManageStorageEnum::MoveStorage:
                move (destination, source);
                break;
            case ManageStorageEnum::DeleteStorage:
                destroy (destination);
                break;
            default:
                throw std::invalid_argument ("Invalid managed storage type");
        }
    }

    static void create (AnyData<Size>& destination, Functor&& source) {
        DebugPrint::printLine (
            "Create storage function, islocal=", isLocal, ", size=", sizeof (Functor),
            ", sizeof buffer=", sizeof (AnyData<Size>), ", align=", alignof (Functor),
            ", copyable=", std::is_trivially_copyable_v<Functor>);
        if constexpr (isLocal) {
            new (destination.template asPtr<Functor> ()) Functor (std::forward<Functor> (source));
        } else {
            *destination.template asPtr<Functor*> () = new Functor (std::forward<Functor> (source));
        }
    }

    static Functor* getPointer (AnyData<Size>& source) {
        if constexpr (isLocal) {
            return source.template asPtr<Functor> ();
        } else {
            return *source.template asPtr<Functor*> ();
        }
    }

    template <class ReturnType, class... Args>
    static ReturnType invoke (AnyData<Size>& source, Args&&... args) {
        return std::invoke (*getPointer (source), std::forward<Args> (args)...);
    }

   private:
    static void clone (AnyData<Size>& dest, AnyData<Size>& source) {
        DebugPrint::printLine ("Clone storage function, islocal=", isLocal);
        if constexpr (isLocal) {
            new (dest.template asPtr<Functor> ()) Functor (*source.template asPtr<Functor> ());
        } else {
            *dest.template asPtr<Functor*> () = new Functor (**source.template asPtr<Functor*> ());
        }
    }

    static void move (AnyData<Size>& dest, AnyData<Size>& source) {
        DebugPrint::printLine ("Move storage function, islocal=", isLocal);
        // Proper care must be taken to ensure that delete is not called on source in case of local
        // storage.
        if constexpr (isLocal) {
            new (dest.template asPtr<Functor> ())
                Functor (std::move (*source.template asPtr<Functor> ()));
        } else {
            *dest.template asPtr<Functor*> () = *source.template asPtr<Functor*> ();
            *source.template asPtr<Functor*> () = nullptr;
        }
    }

    static void destroy (AnyData<Size>& dest) {
        DebugPrint::printLine ("Destroy storage function, islocal=", isLocal);
        if constexpr (isLocal) {
            dest.template asPtr<Functor> ()->~Functor ();
        } else {
            delete *dest.template asPtr<Functor*> ();
        }
    }
};


template <class Functor>
class MyFunction;

template <class ReturnType, class... Args>
class MyFunction<ReturnType (Args...)> {
    static constexpr const int DataSize = 256;
    AnyData<DataSize> d_data{};
    using InvokerType = ReturnType (*) (AnyData<DataSize>&, Args...);
    using ManagerType = void (*) (AnyData<DataSize>&, AnyData<DataSize>&, ManageStorageEnum);

    template <typename Functor>
    using Handler = BaseFunctionManager<Functor, DataSize>;

    template <typename F>
    using Stored = std::decay_t<F>;

    InvokerType d_invoker = nullptr;
    ManagerType d_manager = nullptr;

   public:
    template <std::invocable<Args...> Functor>
    explicit MyFunction (Functor&& f) {
        using F = Stored<Functor>;
        Handler<F>::create (d_data, std::forward<F> (f));
        d_invoker = Handler<F>::template invoke<ReturnType, Args...>;
        d_manager = Handler<F>::manageStorage;
        DebugPrint::printLine ("Constructor");
    }

    explicit MyFunction (nullptr_t) {}

    MyFunction () = default;

    ReturnType operator() (Args... args) {
        if (d_invoker) {
            return std::invoke (d_invoker, d_data, args...);
        }
        throw std::invalid_argument ("Invalid managed function");
    }

    ~MyFunction () {
        DebugPrint::printLine ("Destructor");
        if (d_manager) {
            DebugPrint::printLine ("Destroyed");
            std::invoke (d_manager, d_data, d_data, ManageStorageEnum::DeleteStorage);
        }
    }

    MyFunction (const MyFunction& rhs) {
        d_invoker = rhs.d_invoker;
        d_manager = rhs.d_manager;
        std::invoke (d_manager, d_data, rhs.d_data, ManageStorageEnum::CloneStorage);
        DebugPrint::printLine ("Copy Constructor");
    }

    MyFunction (MyFunction&& rhs) noexcept {
        d_invoker = rhs.d_invoker;
        d_manager = rhs.d_manager;

        std::invoke (d_manager, d_data, rhs.d_data, ManageStorageEnum::MoveStorage);
        rhs.d_invoker = nullptr;
        rhs.d_manager = nullptr;
        DebugPrint::printLine ("Move Constructor");
    }

    MyFunction& operator= (const MyFunction& rhs) {
        if (this != &rhs) {
            std::invoke (d_manager, d_data, rhs.d_data, ManageStorageEnum::DeleteStorage);
            d_invoker = rhs.d_invoker;
            d_manager = rhs.d_manager;
            std::invoke (d_manager, d_data, rhs.d_data, ManageStorageEnum::CloneStorage);
        }
        DebugPrint::printLine ("Copy Assignment Operator");
        return *this;
    }

    MyFunction& operator= (MyFunction&& rhs) noexcept {
        if (this != &rhs) {
            std::invoke (d_manager, d_data, rhs.d_data, ManageStorageEnum::DeleteStorage);
            d_invoker = rhs.d_invoker;
            d_manager = rhs.d_manager;
            rhs.d_invoker = nullptr;
            rhs.d_manager = nullptr;
            std::invoke (d_manager, d_data, rhs.d_data, ManageStorageEnum::MoveStorage);
            DebugPrint::printLine ("Move Assignment Operator");
        }
        return *this;
    }
};

// Deduction logic for function pointer.
template <class ReturnType, class... Args>
MyFunction (ReturnType (*) (Args...)) -> MyFunction<ReturnType (Args...)>;

// Deduction logic for lambda functions
template <class Functor>
MyFunction (Functor) -> MyFunction<typename StripSignature<decltype (&Functor::operator())>::type>;

}  // namespace datastructure

#endif  // MYFUNCTION_H
