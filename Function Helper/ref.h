#pragma once
#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>
#include <ostream>
#include <string>
#include <vector>
#include <experimental/string>

using namespace std;
using namespace std::placeholders;

namespace test::_ref
{
    bool shorter_than(const string& str1, const string& str2)
    {
        return str1.length() < str2.length();
    }

    void run()
    {
        vector<string> v_original{ "tiger", "cat", "lion", "cougar" };

        copy(v_original.begin(), v_original.end(), ostream_iterator<string>(cout, " "));
        cout << endl;

        string s{ "meow" };

        function<bool(const string&)> f = bind(shorter_than, _1, s);
        function<bool(const string&)> f_ref = bind(shorter_than, _1, ref(s));

        vector<string> v;

        // remove elements that are shorter than a ("meow")

        v = v_original;
        v.erase(remove_if(v.begin(), v.end(), f), v.end());

        copy(v.begin(), v.end(), ostream_iterator<string>(cout, " "));
        cout << endl;

        // Now change the value of s.
        // f_ref, which is bound to ref(s), will use the
        // new value, while f is still bound to the old value.

        s = "kitty";

        // Remove elements that are shorter that "meow" (f is bound to old value of s)

        v = v_original;
        v.erase(remove_if(v.begin(), v.end(), f), v.end());

        copy(v.begin(), v.end(), ostream_iterator<string>(cout, " "));
        cout << endl;

        // Remove elements that are shorter that "kitty" (f is bound to ref(s))
        v = v_original;
        v.erase(remove_if(v.begin(), v.end(), f_ref), v.end());

        copy(v.begin(), v.end(), ostream_iterator<string>(cout, " "));
        cout << endl;
    }
}

