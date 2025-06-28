#ifndef MYANY_H_INCLUDE
#define MYANY_H_INCLUDE
#include <type_traits>
#include <AnyStorage.h>

namespace datastructure {

class Any {
    enum class ManagerEnum {
        Clone,
        Move,
        Destroy
    };
    template <typename T>
    class Handler {
        static const size_t MAX_ALIGN = alignof(T);
        static const size_t MAX_SIZE = sizeof(AnyData);
        using NativeType = std::remove_reference_t<T>;

        static constexpr bool isLocal =
            (sizeof(T) <= MAX_SIZE) &&
            (alignof(T) <= MAX_ALIGN) &&
            std::is_trivially_copyable_v<T>;
    public:
        template <typename... Args>
        static void create(AnyData* storage, Args&&... args) {
            if constexpr (isLocal) {
                new (storage) T(std::forward<Args>(args)...);
            }
            else {
                *storage->asPtr<T*> () = new T(std::forward<Args>(args)...);
            }
        }

        static void managerLocal(ManagerEnum me, AnyData* destination,  const AnyData* source, AnyData* storage) {
            switch (me) {
                case ManagerEnum::Clone:
                    new (destination) T(*source->asPtr<T>());
                break;
                case ManagerEnum::Move:
                    new (destination) T(*storage->asPtr<T>());
                break;
                case ManagerEnum::Destroy:
                    destination->asPtr<T>()->~T();
                break;
                default:
                    throw std::bad_cast();

            }
        }

        static void managerNonLocal(ManagerEnum me, AnyData* destination,  const AnyData* source, AnyData* storage) {
            switch (me) {
                case ManagerEnum::Clone:
                    *destination->asPtr<T*>() = new T(**source->asPtr<T*>());
                break;
                case ManagerEnum::Move:
                    *destination->asPtr<T*>() = new T(**storage->asPtr<T*>());
                break;
                case ManagerEnum::Destroy:
                    delete *destination->asPtr<T*> ();
                break;
                default:
                    throw std::bad_cast();

            }
        }

       static void manager(ManagerEnum me, AnyData* destination,  const AnyData* source, AnyData* storage){
           if constexpr (isLocal) {
               managerLocal (me, destination, source, storage);
           }
           else {
               managerNonLocal (me, destination, source, storage);
           }
       }

        static consteval bool local() {
            return isLocal;
        }
    };

    template <typename T>
    using NativeType = std::remove_reference_t<T>;

    AnyData d_data{};
    void (*d_manager)(ManagerEnum, AnyData*, const AnyData*, AnyData*) = nullptr;
    bool d_local = false;
public:
    Any() = default;

    ~Any() {
        if (d_manager) {
            d_manager(ManagerEnum::Destroy, &d_data, nullptr, nullptr);
        }
    }

    template <typename T>
    requires (!std::is_same_v<std::remove_reference_t<std::remove_cv_t<T>>, AnyData>)
    explicit Any (T&& rhs) {
        Handler<NativeType<T>>::create(&d_data, std::forward<T>(rhs));
        d_manager = &Handler<NativeType<T>>::manager;
        d_local = Handler<NativeType<T>>::local();
    }

    Any (Any&& rhs)  noexcept {
        d_manager = rhs.d_manager;
        d_local = rhs.d_local;
        d_manager(ManagerEnum::Move, &d_data, nullptr, &rhs.d_data);
    }

    Any (const Any& rhs) {
        d_manager = rhs.d_manager;
        d_local = rhs.d_local;
        d_manager(ManagerEnum::Clone, &d_data, &rhs.d_data, nullptr);
    }


    Any& operator = (const Any& rhs) {
        if (&rhs != this) {
            if (d_manager) {
                d_manager(ManagerEnum::Destroy, &d_data, nullptr, nullptr);
            }
            d_manager = rhs.d_manager;
            d_local = rhs.d_local;
            d_manager(ManagerEnum::Clone, &d_data, &rhs.d_data, nullptr);
        }
        return *this;
    }

    Any& operator = (Any&& rhs)  noexcept {
        if (&rhs != this) {
            if (d_manager) {
                d_manager(ManagerEnum::Destroy, &d_data, nullptr, nullptr);
            }
            d_manager = rhs.d_manager;
            d_local = rhs.d_local;
            d_manager(ManagerEnum::Move, &d_data, nullptr, &rhs.d_data);
        }
        return *this;
    }

    template <typename T>
    Any& operator = (T&& rhs) {
        if (d_manager) {
            d_manager(ManagerEnum::Destroy, &d_data, nullptr, nullptr);
        }
        Handler<NativeType<T>>::create(&d_data, std::forward<T>(rhs));
        d_local = Handler<NativeType<T>>::local();
        d_manager = &Handler<NativeType<T>>::manager;
        return *this;
    }

    friend bool operator == (const Any& lhs, const Any& rhs) {
        return lhs.d_manager == rhs.d_manager;
    }

    template <typename T>
    friend T& AnyCast(Any& any) {
        if (any.d_local) {
            return *any.d_data.asPtr<T> ();
        }
        return **any.d_data.asPtr<T*>();
    }

    template <typename T>
    friend const T& AnyCast(const Any& any) {
        if (any.d_local) {
            return *any.d_data.asPtr<T> ();
        }
        return **any.d_data.asPtr<T*>();
    }

};




}

#endif //MYANY_H
