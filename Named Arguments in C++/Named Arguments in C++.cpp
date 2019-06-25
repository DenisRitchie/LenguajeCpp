#include <iostream>
#include <string>
#include <tuple>
#include <algorithm>
#include <type_traits>
#include <utility>
#include <functional>
#include <stdio.h>
#include <vector>

template<typename TValue, typename TParameter>
class NamedType
{
public:
    explicit NamedType(const TValue& value) : m_value{ value } { }
    explicit NamedType(TValue&& value) : m_value{ std::move(value) } { }
    TValue& get() { return m_value; }
    const TValue& get() const { return m_value; }

    struct Argument
    {
        template<typename UnderlyingType>
        inline NamedType operator=(UnderlyingType&& value) const
        {
            return NamedType(std::forward<UnderlyingType>(value));
        }

        Argument() = default;
        Argument(const Argument&) = delete;
        Argument(Argument&&) = delete;
        Argument& operator=(const Argument&) = delete;
        Argument& operator=(Argument&&) = delete;
    };

private:
    TValue m_value;
};

struct WidthTag { };
struct HeightTag { };
using Width = NamedType<double, WidthTag>;
using Height = NamedType<double, HeightTag>;
static const Width::Argument width;
static const Height::Argument height;

class Rectangle
{
public:
    Rectangle(Width width, Height height) : m_width{ width }, m_height{ height }
    {
    }

    friend std::ostream& operator<<(std::ostream& os, const Rectangle& r)
    {
        return os << "(" << r.m_width.get() << ", " << r.m_height.get() << ")";
    }

private:
    Width m_width;
    Height m_height;
};

struct FirstNameTag { };
struct LastNameTag { };
using FirstName = NamedType<std::string, FirstNameTag>;
using LastName = NamedType<std::string, LastNameTag>;
static const FirstName::Argument first_name;
static const LastName::Argument last_name;

void DisplayCoolNameImpl(const FirstName& first_name, const LastName& last_name)
{
    std::cout << "My name is: " << last_name.get() << ", " << first_name.get() << " " << last_name.get() << ".\n";
}

template<typename TypeToPick, typename ...Types>
TypeToPick pick(Types&& ...args)
{
    return std::get<TypeToPick>(std::make_tuple(std::forward<Types>(args)...));
}

template<typename Arg0, typename Arg1>
void DisplayCoolName(Arg0&& arg0, Arg1&& arg1)
{
    DisplayCoolNameImpl(pick<FirstName>(arg0, arg1), pick<LastName>(arg0, arg1));
}

void TestComplexImpl(Width width, Height heigh, FirstName name, LastName last_name)
{
    printf_s("Nombre: %s, Apellido: %s, Ancho: %g, Alto: %g\n",
        name.get().c_str(), last_name.get().c_str(), width.get(), heigh.get());
}

template<typename Arg0, typename Arg1, typename Arg2, typename Arg3>
void TestComplex(Arg0 arg0, Arg1 arg1, Arg2 arg2, Arg3 arg3)
{
    TestComplexImpl(
        pick<Width>(arg0, arg1, arg2, arg3),
        pick<Height>(arg0, arg1, arg2, arg3),
        pick<FirstName>(arg0, arg1, arg2, arg3),
        pick<LastName>(arg0, arg1, arg2, arg3)
    );
}

int main()
{
    Rectangle R(width = 5, height = 10);
    std::cout << R << std::endl;
    DisplayCoolName(first_name = "Denis", last_name = "West");
    DisplayCoolName(last_name = "West", first_name = "Denis");
    TestComplex(last_name = "West", width = 150, first_name = "Denis", height = 70);
    TestComplex(height = 70, last_name = "West", width = 150, first_name = "Denis");
    TestComplex(width = 150, first_name = "Denis", height = 70, last_name = "West");
    TestComplex(first_name = "Denis", width = 150, last_name = "West", height = 70);
    return 0;
}

