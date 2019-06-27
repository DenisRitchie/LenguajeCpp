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

