#pragma once

// https://gracicot.github.io/reflection/2018/04/03/reflection-present.html

// Reflection in C++ Part 1: The Present

namespace Reflexion::TypeIntrospectionInCpp::_1
{
    template<typename Type>
    auto FooOrBar(Type const& Value) -> decltype(Value.Foo())
    {
        std::cout << "Foo()" << std::endl;
        return Value.Foo();
    }

    template<typename Type>
    auto FooOrBar(Type const& Value) -> decltype(Value.Bar())
    {
        std::cout << "Bar()" << std::endl;
        return Value.Bar();
    }

    template<typename Type>
    auto FooAndBar(Type const& Value) -> std::common_type_t<decltype(Value.Foo()), decltype(Value.Bar())>
    {
        std::cout << "Foo(), Bar()" << std::endl;
        return Value.Foo(), Value.Bar();
    }

    void Run()
    {
        struct HasFoo { void Foo() const { } } F;
        struct HasBar { void Bar() const { } } B;
        struct HasBoth : HasFoo, HasBar { } Fb;

        FooOrBar(F);
        FooOrBar(B);
        FooAndBar(Fb); 
    }
}

namespace Reflexion::TypeIntrospectionInCpp::_2
{
    template<typename, typename = void>
    constexpr bool HasPerimeter = false;

    template<typename Type>
    constexpr bool HasPerimeter<Type, std::void_t<decltype(std::declval<Type>().Perimeter)>> = true;

    struct Foo
    {
        int Perimeter;
    };

    struct Bar
    {
    };

    static_assert(HasPerimeter<Foo>);
    static_assert(!HasPerimeter<Bar>);
}

namespace Reflexion::TypeIntrospectionInCpp::_3::ReflectingFunctionTypes
{
    template<typename ReturnType>
    auto PrintDefaultReturn(ReturnType(*)()) -> void
    {
        std::cout << ReturnType{ } << std::endl;
    }

    template<typename ReturnType, typename ...ArgTypes>
    auto PrintDefaultReturn(ReturnType(*)(ArgTypes...)) -> void
    {
        std::cout << ReturnType{ } << std::endl;
    }

    auto ReturnInt() -> int 
    {
        return 0;
    }

    struct Dummy
    {
        inline friend std::ostream& operator<<(std::ostream& Os, Dummy const&)
        {
            return Os << "Dummy";
        }
    };

    auto ReturnDummy() -> Dummy
    {
        return Dummy{ };
    }

    auto ReturnDouble(int, int, int, int) -> double
    {
        return 0.0;
    }

    auto Run() -> void
    {
        PrintDefaultReturn(ReturnInt);
        PrintDefaultReturn(ReturnDummy);
        PrintDefaultReturn(ReturnDouble);
    }
}

namespace Reflexion::TypeIntrospectionInCpp::_4::TraitsFunctions
{
    template<typename TLambda>
    struct FunctionTraits : FunctionTraits<decltype(&TLambda::operator())>
    {
    };

    template<typename TReturn, typename ...TArgs>
    struct FunctionTraits<TReturn(*)(TArgs...)>
    {
        using ResultType = TReturn;
        using ParameterTypes = std::tuple<TArgs...>;
    };

    template<typename TReturn, typename TClass, typename ...TArgs>
    struct FunctionTraits<TReturn(TClass::*)(TArgs...) const>
    {
        using ResultType = TReturn;
        using ParameterTypes = std::tuple<TArgs...>;
    };

    template<typename TFunction>
    using FunctionResultType = typename FunctionTraits<TFunction>::ResultType;

    template<typename TFunction>
    using FunctionArgumentsType = typename FunctionTraits<TFunction>::ParameterTypes;

    template<std::size_t N, typename TFunction>
    using NthArgumentType = std::tuple_element_t<N, FunctionArgumentsType<TFunction>>;

    template<typename TFunction>
    constexpr auto ArgumentsCount = std::tuple_size_v<FunctionArgumentsType<TFunction>>;

    auto ReturnString() -> std::string
    {
        using namespace std::literals;
        return ""s;
    }

    auto Run() -> void
    {
        FunctionResultType<decltype(&ReturnString)> Cadena = "Cadena";
        std::cout << Cadena << std::endl;

        FunctionResultType<decltype(&_3::ReflectingFunctionTypes::ReturnInt)> Numero = 27;
        std::cout << Numero << std::endl;

        FunctionArgumentsType<decltype(&ReturnString)> Cadena2;
        std::cout << typeid(Cadena2).name() << std::endl;

        auto Lambda([](char, int, double) -> int { return 0; });
        int(*LambdaPointer)(char, int, double) = Lambda;
        FunctionArgumentsType<decltype(LambdaPointer)> Numero2;
        std::cout << typeid(Numero2).name() << std::endl;

        FunctionArgumentsType<decltype(Lambda)> Numero3;
        std::cout << typeid(Numero3).raw_name() << std::endl;
    }
}

namespace Reflexion
{
    using TypeIntrospectionInCpp::_4::TraitsFunctions::FunctionArgumentsType;
    using TypeIntrospectionInCpp::_4::TraitsFunctions::FunctionResultType;
    using TypeIntrospectionInCpp::_4::TraitsFunctions::NthArgumentType;
    using TypeIntrospectionInCpp::_4::TraitsFunctions::FunctionTraits;
    using TypeIntrospectionInCpp::_4::TraitsFunctions::ArgumentsCount;
}

namespace Reflexion::TypeIntrospectionInCpp::_5::UsingFunctionReflection
{
    int SameFunction(std::string, double) { return 100; }

    void Run()
    {
        using TFunction = decltype(&SameFunction);

        auto Arg1 = std::tuple_element_t<0, FunctionArgumentsType<TFunction>>{ "Denis West" };
        auto Arg2 = std::tuple_element_t<1, FunctionArgumentsType<TFunction>>{ 30'000.0 };

        FunctionResultType<TFunction> Result = SameFunction(Arg1, Arg2);
        std::cout << Result << std::endl;

        auto Lambda([](int) { });
        using TLambdaArgument = std::tuple_element_t<0, FunctionArgumentsType<decltype(Lambda)>>;

        TLambdaArgument Numero = 27;
        std::cout << "Denis aprendio esto a los: " << Numero << " años\n";
    }
}

namespace Reflexion::Reification::_1
{
    template<typename TLambda, std::size_t ...S>
    constexpr auto WrapLambda(std::index_sequence<S...>, TLambda Lambda)
    {
        // A wrapper, local struct that recreate the lambda passed as parameter.
        struct Wrapper
        {
            constexpr Wrapper(TLambda L) noexcept : m_Lambda{ std::move(L) } { }

            // Reify the call operator with the exact same parameter type and return type.
            constexpr auto operator()(NthArgumentType<S, TLambda>... Args) const -> FunctionResultType<TLambda>
            {
                return m_Lambda(std::forward<NthArgumentType<S, TLambda>>(Args)...);
            }

        private:
            TLambda m_Lambda;
        };

        return Wrapper{ std::move(Lambda) };
    }

    template<typename TLambda>
    constexpr auto WrapLambda(TLambda Lambda) 
    {
        return WrapLambda(std::make_index_sequence<ArgumentsCount<TLambda>>(), std::move(Lambda));
    }

    void Run()
    {
        // static_assert(wrapped({ 4 }) == 8);
        constexpr auto Wrapped = WrapLambda([](int Value) { return Value * 2; });
        std::cout << typeid(Wrapped).name() << std::endl 
                  << Wrapped(4)             << std::endl;
    }
}

namespace Reflexion::Reification::_2::ReflectionAndReification
{
    template<typename TLambda, std::size_t ...S>
    constexpr auto MakeDeferred(std::index_sequence<S...>, TLambda Lambda)
    {
        // We create a tuple type that can store the list of arguments of the lambda 
        // We are going to store that in our callable type to cache parameters before call
        using ParameterPack = std::tuple<NthArgumentType<S, TLambda>...>;

        // We define our wrapper struct
        // We inherit privately to leverage empty base class optimization, but we could easily
        // have choose to contain a private member instead.
        struct Wrapper : private TLambda
        {
            constexpr Wrapper(TLambda L) : TLambda{ std::move(L) } { }

            // We make a bind function that take the same arguments as our lambda
            // we are going to store each argument in ...args for a later call
            void Bind(NthArgumentType<S, TLambda> ...Args)
            {
                Bound.reset();
                Bound = ParameterPack{ std::forward<NthArgumentType<S, TLambda>>(Args)... };
            }

            explicit operator bool() const
            {
                return Bound.has_value();
            }

            // We make a call operator that has the same return type as our lambda
            auto operator()() -> FunctionResultType<TLambda>
            {
                // Here we are using the stored parameters set in the `bind` function
                return TLambda::operator()(std::forward<NthArgumentType<S, TLambda>>(std::get<S>(*Bound))...);
            }

        private:
            std::optional<ParameterPack> Bound;
        };

        return Wrapper{ std::move(Lambda) };
    }

    template<typename TLambda>
    constexpr auto MakeDeferred(TLambda Lambda)
    {
        return MakeDeferred(std::make_index_sequence<ArgumentsCount<TLambda>>(), Lambda);
    }

    void Run()
    {
        // We make a deffered lambda
        auto Func = MakeDeferred(
            [](int A, std::vector<int>& B, double C)
            {
                std::cout << "A: " << A << std::endl;

                for (auto&& Value : B)
                    std::cout << ' ' << Value;

                std::cout << std::endl << C << std::endl;
            });

        auto Vector = std::vector{ 1, 2, 3, 4, 5, 42 };

        // Bind the parameters to our function
        Func.Bind(12, Vector, 5.4);

        // Call the function with bound parameters:
        Func();

        Func.Bind(27, Vector = { 10, 20, 30 }, 100.0);
        Func();
    }
}

namespace Reflexion::GenericLambdas::_1::MagicCallImplementation
{
    auto LambdaArgTypeTest() -> void
    {
        auto Lambda = [](auto){ };
        //auto FunctionType = &decltype(Lambda)::operator(); // Error
        auto FunctionType = &decltype(Lambda)::operator()<int>;
    }

    // Default case, the lambda cannot be instantiated, yields false
    template<typename, typename, typename = void>
    constexpr bool IsCallOperatorInstantiable = false;

    // Specialization if the expression `&L::template operator()<Args...>` is valid, yields true
    template<typename TLambda, typename ...TArgs>
    constexpr bool IsCallOperatorInstantiable<
        TLambda, std::tuple<TArgs...>,
        std::void_t<decltype(&TLambda::template operator()<TArgs...>)>
    > = true;

    // Declaration of our function. Must be declared to provide specializations.
    template<typename, typename, typename = void>
    struct DeducedFunctionTraitsHelper;

    // This specialization matches the first path of the pseudocode,
    // more presicely the condition `if TFunc instantiable with TArgs... then`
    template<typename TFunc, typename ...TArgs>
    struct DeducedFunctionTraitsHelper<TFunc, std::tuple<TArgs...>,
        // arguments TFunc and TArgs if TFunc is instantiable with TArgs
        std::enable_if_t<IsCallOperatorInstantiable<TFunc, std::tuple<TArgs...>>>>
        // return function_traits with function pointer
        // Returning is modelized as inheritance. We inherit (returning) the function trait with the deduced pointer to member.
        : FunctionTraits<decltype(&TFunc::template operator()<TArgs...>)>
    {
    };

    // This specialisation matches the second path of the pseudocode,
    // the `else if size of TArgs larger than 0`
    template<typename TFunc, typename TFirst, typename ...TArgs>
    struct DeducedFunctionTraitsHelper<TFunc, 
        std::tuple<TFirst, TArgs...>, // arguments TFunc and TFirst, TArgs... if not instantiable
        std::enable_if_t<!IsCallOperatorInstantiable<TFunc, std::tuple<TFirst, TArgs...>>>
    >   // return deduced_function_traits(TFunc, drop first TFirst...)
        // again, returning is modelized as inheritance. We are returning the next step of the algorithm (recursion)
        : DeducedFunctionTraitsHelper<TFunc, std::tuple<TArgs...>>
    {
    };

    // Third path of the algorithm.
    // Else return nothing, end of algorithm
    template<typename, typename, typename>
    struct DeducedFunctionTraitsHelper { };

    // We can also define some alias to ease it’s usage
    template<typename TFunc, typename ...TArgs>
    using DeducedFunctionTriats = DeducedFunctionTraitsHelper<TFunc, std::tuple<TArgs...>>;

    template<typename TFunc, typename ...TArgs>
    using DeducedFunctionResultType = typename DeducedFunctionTriats<TFunc, TArgs...>::ResultType;

    template<typename TFunc, typename ...TArgs>
    using DeducedFunctionArgumentType = typename DeducedFunctionTriats<TFunc, TArgs...>::ParameterTypes;

    template<std::size_t N, typename TFunc, typename ...TArgs>
    using DeducedNthArgumentType = std::tuple_element_t<N, DeducedFunctionArgumentType<TFunc, TArgs...>>;

    template<typename TFunc, typename ...TArgs>
    constexpr auto DeducedArgumentsCount = std::tuple_size_v<DeducedFunctionArgumentType<TFunc, TArgs...>>;
    
    // MagicCall

    template<typename Type>
    constexpr Type MagicValue()
    {
        return Type{ }; // Simple implementation that default construct the type
    }

    // We could have used decltype(auto) instead of reflection here ------v
    template<typename TLambda, typename ...TArgs, std::size_t ...S>
    constexpr auto MagicCall(std::index_sequence<S...>, TLambda Lambda, TArgs&& ...Args) -> DeducedFunctionResultType<TLambda, TArgs...>
    {
        // Call the lambda with both MagicValue and provided parameter in ...Args 
        return Lambda(MagicValue<DeducedNthArgumentType<S, TLambda, TArgs...>>()..., std::forward<TArgs>(Args)...);
    }

    template<typename TLambda, typename ...TArgs>
    constexpr auto MagicCall(TLambda Lambda, TArgs&& ...Args) -> DeducedFunctionResultType<TLambda, TArgs...>
    {
        // We generate a sequence from 0 up to the number of parameter we need to get through `MagicValue`
        auto Sequence = std::make_index_sequence<DeducedArgumentsCount<TLambda, TArgs...> - sizeof...(Args)>();
        return MagicCall(Sequence, std::move(Lambda), std::forward<TArgs>(Args)...);
    }

    template<size_t TypeId>
    struct SomeType
    {
        friend std::ostream& operator<<(std::ostream& Os, SomeType<TypeId> const&)
        {
            return Os << typeid(SomeType<TypeId>).name();
        }
    };

    void Run()
    {
        // LambdaArgTypeTest();
        
        using namespace std::literals;

#define Variable(Var) << #Var ": " << Var << std::endl

        MagicCall([](SomeType<1> S1, SomeType<2> S2, int N1, double N2, auto&& V1, auto&& V2)
            {
                std::cout Variable(S1) Variable(S2) Variable(N1) Variable(N2) Variable(V1) Variable(V2);
            }, /*Magic*/ /*Magic*/ 4, 5.4, "Str1", "Str2"sv);

#undef Variable
    }
}