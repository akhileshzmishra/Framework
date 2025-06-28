//
// Created by Akhilesh Mishra on 06/02/2025.
//

#ifndef MYFUNCTION_H
#define MYFUNCTION_H

#include <memory>
#include <type_traits>
#include <iostream>
#include <ostream>
#include <tuple>
#include <bitset>
#include <concepts>
#include <array>
#include <cmath>
#include <algorithm>
#include <AnyStorage.h>

namespace datastructure {

template <class Functor>
class BaseFunctionManager{
    static constexpr size_t MAX_SIZE = sizeof(PtrDataStorage);
    static constexpr size_t MAX_ALIGN = __alignof__(PtrDataStorage);

    static constexpr bool isLocal =
	    (std::is_trivially_copyable<Functor>::value)
	 && (sizeof(Functor) <= MAX_SIZE)
	 && (__alignof__(Functor) <= MAX_ALIGN);

    typedef std::integral_constant<bool, isLocal> LocalStorage;
public:

    static void create(AnyData& data, Functor&& functor){
        if constexpr(isLocal){
            new (data.asVoidPtr()) Functor(std::forward<Functor>(functor));
        }
        else{
            *data.asPtr<Functor*>() = new Functor(std::forward<Functor>(functor));
        }
    }

    static void clone(AnyData& dest, AnyData& source){
        if constexpr(isLocal){
            new (dest.asPtr<Functor>()) Functor(source.asPtr<Functor>());
        }
        else{
            *dest.asPtr<Functor*>() = new Functor(**source.asPtr<Functor*>());
        }
    }

    static void destroy(AnyData& dest){
        if constexpr(isLocal){
            dest.asPtr<Functor>()->~Functor();
        }
        else{
            delete *dest.asPtr<Functor*>();
        }
    }

    static Functor* getPointer(const AnyData& source)
	{
        if constexpr (isLocal)
        {
            return const_cast<Functor*>(static_cast<const Functor*>(source.asPtr<Functor>()));
        }
        else {
            return const_cast<Functor*>(static_cast<const Functor*>(*source.asPtr<Functor*>()));
        }
	}

};

template <class Signature, class Functor>
class FunctionManager;

template <class ReturnType, class Functor, class...Args>
class FunctionManager<ReturnType(Args...), Functor>: public BaseFunctionManager<Functor>{
    typedef BaseFunctionManager<Functor> Base;
public:
    static ReturnType invoke(const AnyData& data, Args... args){
        return std::invoke(*Base::getPointer(data), std::forward<Args>(args)...);
    }
};


template <class Class, class Member, class ReturnType, class...Args>
class FunctionManager<ReturnType(Args...), Member Class::*>: public BaseFunctionManager<Member Class::*>{
    typedef Member Class::* Functor;
    typedef BaseFunctionManager<Functor> Base;
public:
    static ReturnType invoke(const AnyData& data, Args... args){
        return std::invoke(*Base::getPointer(data), std::forward<Args>(args)...);
    }
};


template <class ReturnType, class...Args>
class MyFunction;

template <class ReturnType, class...Args>
class MyFunction<ReturnType(Args...)>{
    AnyData d_data{};
    using InvokerType = ReturnType (*)(const AnyData&, Args...);
    using DestroyType = void(*)(AnyData& );
    template<typename Functor>
	using Handler
	  = FunctionManager<ReturnType(Args...), Functor>;

    InvokerType d_invoker = nullptr;
    DestroyType d_destroyer = nullptr;
public:
    template<class Functor>
        requires std::is_invocable_v<Functor, Args...>
    explicit MyFunction(Functor f){
        Handler<Functor>::create(d_data, std::forward<Functor>(f));
        d_invoker = Handler<Functor>::invoke;
        d_destroyer = Handler<Functor>::destroy;
        std::cout <<"Constructor" << std::endl;
    }

    ReturnType operator()(Args... args){
        return std::invoke(d_invoker, d_data, args...);
    }

    ~MyFunction(){
        std::invoke(d_destroyer, d_data);
        std::cout <<"Destructor" << std::endl;
    }

    template<class Functor>
        requires std::is_invocable_v<Functor, Args...>
    explicit MyFunction(const MyFunction<Functor>& rhs) {
        d_invoker = rhs.d_invoker;
        d_destroyer = rhs.d_destroyer;
        Handler<Functor>::clone(d_data, rhs.d_data);
    }
};



}

#endif //MYFUNCTION_H
