#include "../Easylog.hpp"

int main(int, char**)
{
    auto [t, s] = eLog::fmt::Impl::GetArgsAsTulpe(1, 2, 3, 4, 5, "hello world", 7, 8, 9, 3.14, 3.15, 3.16, 3.17, 3.18, 3.19, true, false);
    assert((s == 17) && "tuple size failed");
    std::vector<std::string> vals;
    eLog::fmt::Impl::ToString(vals, t);

    auto vals2 = eLog::fmt::Impl::ArgsToVector(1, 2, 3, 4, 5, "hello world", 7, 8, 9, 3.14, 3.15, 3.16, 3.17, 3.18, 3.19, true, false);
    assert((vals == vals2) && "vector test failed");

    auto result = eLog::fmt::Format("hello {0}, {1}, {2}, {3}, {4}, {5}, {6}, {7}, {8}, {9}!", 1, 2, 3, 4, 5, "hello world", 7, 8, 9, 3.14);
    assert ((result == "hello 1, 2, 3, 4, 5, hello world, 7, 8, 9, 3.14!") && "normal test failed");

    result = eLog::fmt::Format("hello {9}, {8}, {7}, {6}, {5}, {4}, {3}, {2}, {1}, {0}!", 1, 2, 3, 4, 5, "hello world", 7, 8, 9, 3.14);
    assert ((result == "hello 3.14, 9, 8, 7, hello world, 5, 4, 3, 2, 1!") &&  "reverse test failed");

    result = eLog::fmt::Format("hello {2}, {8}, {1}, {9}, {3}, {5}, {7}, {4}, {6}, {0}!", 1, 2, 3, 4, 5, "hello world", 7, 8, 9, 3.14);
    assert ((result == "hello 3, 9, 2, 3.14, 4, hello world, 8, 5, 7, 1!") && "random test failed");

    result = eLog::fmt::Format("hello {}, {}, {}, {}, {}, {}, {}, {}, {}, {}!", 1, 2, 3, 4, 5, "hello world", 7, 8, 9, 3.14);
    assert ((result == "hello 1, 2, 3, 4, 5, hello world, 7, 8, 9, 3.14!") && "empty bracket test failed");

    result = eLog::fmt::Format("hello { }, { }, { }, { }, { }, { }, { }, { }, { }, { }!", 1, 2, 3, 4, 5, "hello world", 7, 8, 9, 3.14);
    assert ((result == "hello 1, 2, 3, 4, 5, hello world, 7, 8, 9, 3.14!") && "empty bracket test failed");

    result = eLog::fmt::Format("hello {:d}, {:d}, {:d}, {:d}, {:d}, {:s}, {:d}, {:d}, {:d}, {:f}!", 1, 2, 3, 4, 5, "hello world", 7, 8, 9, 3.14);
    assert ((result == "hello 1, 2, 3, 4, 5, hello world, 7, 8, 9, 3.140000!") && "format test failed");

    result = eLog::fmt::Format("{:f4}!", 3.14);
    assert ((result == "3.1400!") && "format test failed");

    result = eLog::fmt::Format("{:x}!", 3);
    assert ((result == "0x3!") && "hex format test failed");

    result = eLog::fmt::Format("{:x}!", 6789);
    assert ((result == "0x1A85!") && "hex format test failed");

    result = eLog::fmt::Format("{:x1}!", 3);
    assert ((result == "0x3!") && "hex format test failed");

    result = eLog::fmt::Format("{:x4}!", 3);
    assert ((result == "0x0003!") && "hex format test failed");

    result = eLog::fmt::Format("{:x4}!", 6789);
    assert ((result == "0x1A85!") && "hex format test failed");

    return 0;
}