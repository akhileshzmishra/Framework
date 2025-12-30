#ifndef MYANY_H_INCLUDE
#define MYANY_H_INCLUDE
#include <type_traits>
#include <AnyStorage.h>

namespace datastructure {

enum class ManagerEnum { Clone, Move, Destroy, GetPointer };

template <size_t Size, typename T>
class Handler {
    static const size_t MAX_ALIGN = alignof (SmallBufferOptimizationStorage<Size>);
    static const size_t MAX_SIZE = sizeof (SmallBufferOptimizationStorage<Size>);
    using NativeType = std::remove_reference_t<T>;

    static constexpr bool isLocal =
        (sizeof (T) <= MAX_SIZE) && (alignof (T) <= MAX_ALIGN) && std::is_trivially_copyable_v<T>;

   public:
    template <typename... Args>
    static void create (SmallBufferOptimizationStorage<Size>& storage, Args&&... args) {
        if constexpr (isLocal) {
            new (storage.template asPtr<T> ()) T (std::forward<Args> (args)...);
        } else {
            *storage.template asPtr<T*> () = new T (std::forward<Args> (args)...);
        }
    }

    static void* manager (ManagerEnum me, SmallBufferOptimizationStorage<Size>& destination,
                          SmallBufferOptimizationStorage<Size>& source) {
        void* returnValue = nullptr;
        switch (me) {
            case ManagerEnum::Clone:
                clone (destination, source);
                break;
            case ManagerEnum::Move:
                move (destination, source);
                break;
            case ManagerEnum::Destroy:
                destroy (destination);
                break;
            case ManagerEnum::GetPointer:
                returnValue = getPointer (destination);
                break;
            default:
                throw std::bad_cast ();
        }
        return returnValue;
    }

   private:
    static void* getPointer (SmallBufferOptimizationStorage<Size>& storage) {
        if constexpr (isLocal) {
            return static_cast<void*> (storage.template asPtr<T> ());
        }
        return static_cast<void*> (*storage.template asPtr<T*> ());
    }

    static void destroy (SmallBufferOptimizationStorage<Size>& storage) {
        if constexpr (isLocal) {
            storage.template asPtr<T> ()->~T ();
        } else {
            delete *storage.template asPtr<T*> ();
        }
    }

    static void clone (SmallBufferOptimizationStorage<Size>& destination,
                       SmallBufferOptimizationStorage<Size>& source) {
        if constexpr (isLocal) {
            new (destination.template asPtr<T> ()) T (*source.template asPtr<T> ());
        } else {
            *destination.template asPtr<T*> () = new T (**source.template asPtr<T*> ());
        }
    }

    static void move (SmallBufferOptimizationStorage<Size>& destination,
                      SmallBufferOptimizationStorage<Size>& source) {
        if constexpr (isLocal) {
            new (destination.template asPtr<T> ()) T (std::move (*source.template asPtr<T> ()));
        } else {
            *destination.template asPtr<T*> () = *source.template asPtr<T*> ();
            *source.template asPtr<T*> () = nullptr;
        }
    }
};

template <size_t Size = 16>
class TypeFreeData {
    mutable SmallBufferOptimizationStorage<Size> d_data{};
    using ManagerType = void* (*)(ManagerEnum, SmallBufferOptimizationStorage<Size>&,
                                  SmallBufferOptimizationStorage<Size>&);
    ManagerType d_manager = nullptr;

   public:
    TypeFreeData () = default;

    ~TypeFreeData () {
        if (d_manager) {
            d_manager (ManagerEnum::Destroy, d_data, d_data);
        }
    }

    template <typename T>
        requires (!std::is_same_v<T, TypeFreeData>)
    explicit TypeFreeData (T&& value) {
        using Type = typename std::decay<T>::type;
        using Handler = Handler<Size, Type>;
        d_manager = Handler::manager;
        Handler::create (d_data, value);
    }

    TypeFreeData (TypeFreeData&& rhs) noexcept {
        d_manager = rhs.d_manager;
        if (rhs.d_manager) {
            rhs.d_manager = nullptr;
            if (d_manager) {
                std::invoke (d_manager, ManagerEnum::Move, d_data, rhs.d_data);
            }
        }
    }

    TypeFreeData (const TypeFreeData& rhs) {
        d_manager = rhs.d_manager;
        if (d_manager) {
            std::invoke (d_manager, ManagerEnum::Clone, d_data, rhs.d_data);
        }
    }

    TypeFreeData& operator= (const TypeFreeData& rhs) {
        if (&rhs != this) {
            if (d_manager) {
                std::invoke (d_manager, ManagerEnum::Destroy, d_data, rhs.d_data);
            }
            d_manager = rhs.d_manager;
            if (d_manager) {
                std::invoke (d_manager, ManagerEnum::Clone, d_data, rhs.d_data);
            }
        }
        return *this;
    }

    TypeFreeData& operator= (TypeFreeData&& rhs) noexcept {
        if (&rhs != this) {
            if (d_manager) {
                std::invoke (d_manager, ManagerEnum::Destroy, d_data, rhs.d_data);
            }
            d_manager = rhs.d_manager;
            std::invoke (d_manager, ManagerEnum::Move, d_data, rhs.d_data);
            rhs.d_manager = nullptr;
        }
        return *this;
    }

    template <typename T>
        requires (!std::is_same_v<T, TypeFreeData>)
    TypeFreeData& operator= (T&& value) {
        if (d_manager) {
            std::invoke (d_manager, ManagerEnum::Destroy, d_data, d_data);
        }
        using Type = typename std::decay<T>::type;
        using Handler = Handler<Size, Type>;
        d_manager = Handler::manager;
        Handler::create (d_data, value);
        return *this;
    }

    friend bool operator== (const TypeFreeData& lhs, const TypeFreeData& rhs) {
        return lhs.d_manager == rhs.d_manager;
    }

    template <typename T>
    friend T& AnyCast (TypeFreeData& any) {
        if (!any.d_manager) {
            throw std::bad_cast ();
        }
        auto managerOfTType = Handler<Size, std::decay_t<T>>::manager;
        if (managerOfTType != any.d_manager) {
            throw std::bad_cast ();
        }
        return *static_cast<T*> (
            std::invoke (any.d_manager, ManagerEnum::GetPointer, any.d_data, any.d_data));
    }

    template <typename T>
    friend const T& AnyCast (const TypeFreeData& any) {
        if (!any.d_manager) {
            throw std::bad_cast ();
        }
        auto managerOfTType = Handler<Size, std::decay_t<T>>::manager;
        if (managerOfTType != any.d_manager) {
            throw std::bad_cast ();
        }
        return *static_cast<T*> (
            std::invoke (any.d_manager, ManagerEnum::GetPointer, any.d_data, any.d_data));
    }
};

using Any = TypeFreeData<>;
using Any32 = TypeFreeData<32>;

}  // namespace datastructure

#endif //MYANY_H
