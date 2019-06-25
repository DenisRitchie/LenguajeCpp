// https://codereview.stackexchange.com/questions/158341/an-interval-map-data-structure

#include <map>
#include <limits>
#include <iostream>

template<typename K, typename V>
class interval_map
{
    std::map<K, V> m_map;

public:
    // constructor associates whole range of K with val by inserting (K_min, val)
    // into the map
    interval_map(V const& val)
    {
        m_map.insert(m_map.end(), std::make_pair(std::numeric_limits<K>::lowest(), val));
    }

    // Assign value val to interval [keyBegin, keyEnd).
    // Overwrite previous values in this interval.
    // Conforming to the C++ Standard Library conventions, the interval
    // includes keyBegin, but excludes keyEnd.
    // If !( keyBegin < keyEnd ), this designates an empty interval,
    // and assign must do nothing.
    void assign(K const& keyBegin, K const& keyEnd, V const& val)
    {
        if (keyBegin < keyEnd)
        {
            if (auto first = std::prev(m_map.lower_bound(keyBegin)); first->second != val)
            {
                auto last = m_map.upper_bound(keyEnd);
                auto prev = std::prev(last);
                last = m_map.insert(prev, std::make_pair(keyEnd, prev->second));
                first = m_map.lower_bound(keyBegin);
                m_map.erase(first, last);
                m_map.insert(last, std::make_pair(keyBegin, val));
            }
        }
    }

    // look-up of the value associated with key
    V const& operator[](K const& key) const
    {
        return (--m_map.upper_bound(key))->second;
    }

    // a print function for debugging
    void show()
    {
        std::cout << "\n\nshow" << std::endl;
        for (auto entry : m_map) 
        {
            std::cout
                << "[" << entry.first << "]: "
                << entry.second
                << std::endl;
        }
    }
};

void test_equal_range()
{
    const std::map<int, char const*> m
    {
        { 0, "Zero" },
        { 1, "One" },
        { 2, "Two" }
    };

    {
        auto p = m.equal_range(1);

        for (auto& q = p.first; q != p.second; q++)
        {
            std::cout << "m[" << q->first << "] = " << q->second << '\n';
        }

        if (p.second == m.find(2))
            std::cout << "end equal_range (p.second) is one-past p.first\n";
        else
            std::cout << "unexpected; p.second to be one-past p.first\n";
    }

    {
        auto pp = m.equal_range(-1);

        if (pp.first == m.begin())
            std::cout << "pp.first is iterator to first not-less than - 1\n";
        else
            std::cout << "unexpected pp.first\n";

        if (pp.second == m.begin())
            std::cout << "pp.second is iterator to first element greater-than -1\n";
        else
            std::cout << "unexpected pp.second.\n";
    }

    {
        auto ppp = m.equal_range(3);
        if (ppp.first == m.end())
            std::cout << "ppp.first is iterator to first not-less than 3\n";
        else
            std::cout << "unexpected ppp.first\n";

        if (ppp.second == m.end())
            std::cout << "ppp.second is iterator to first element greater-than 3\n";
        else
            std::cout << "unexpected ppp.second\n";
    }
}

void test_bound()
{
    std::map<int, char> m;
    for (int i = 1, c = 'A'; i <= 27; ++i, ++c)
        m[i] = c;

    for (auto& [key, value] : m)
    {
        std::cout << "[" << key << "]: " << value << std::endl;
    }

    puts("\n\n");

    std::cout << "buscar el 14: ";
    auto p = m.lower_bound(14);
    std::cout << p->first << ", " << p->second << std::endl;
    --p;
    std::cout << "antes del 14 esta: " << p->first << ", " << p->second << std::endl;

    auto p2 = m.upper_bound(20);
    std::cout << "despues del 20 esta: " << p2->first << ", " << p2->second << std::endl;

    std::cout << "iterando desde el 14 hasta el 20:\n";
    while (++p != p2)
        std::cout << "\t[" << p->first << "]: " << p->second << std::endl;
}

void test_insert()
{
    std::map<int, char> m;

    auto print_map([&m]
    {
        for (auto& [key, value] : m)
        {
            std::cout << "[" << key << "]: " << value << "\n";
        }
    });

    m.insert(m.end(), std::make_pair(std::numeric_limits<int>::lowest(), 'D'));
    m.insert(m.end(), std::make_pair(15, 'Z'));
    m.insert(m.end(), std::make_pair(1, 'F'));
    m.insert(m.end(), std::make_pair(46, 'G'));
    m.insert(m.end(), std::make_pair(7, 'Y'));

    print_map();
}

int main()
{
    interval_map<int, char> m{ 'A' };
    m.show();
    std::cout << "\n\n";
    m.assign(0, 100, 'C');
    m.assign(2, 30, 'C');
    m.assign(5, 12, 'B');
    m.assign(20, 40, 'D');
    m.assign(41, 80, 'F');
    m.assign(15, 25, 'H');
    m.assign(70, 95, 'Z');
    m.assign(96, 99, 'K');
    m.show();

    for (int i = -15; i <= 115; ++i)
        printf_s("\n[%d]: %c", i, m[i]);

    //test_equal_range();
    //test_bound();
    //test_insert();

    return 0;
}

