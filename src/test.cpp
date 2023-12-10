#include "test.hpp"
#include "CppUTest/TestHarness.h"

constexpr static std::size_t cSize = 3;

TEST_GROUP(TestCopyGroup) {
    std::vector<int> arr = {1,2,3};
    std::list<short> l;

    std::vector<TestNode<std::uint8_t,std::uint8_t>> vTestNode;
    std::vector<TestNodeAl<std::uint64_t, std::uint8_t>> vTestNodeAl;

    void setup() {
        for(std::size_t i = 0; i < cSize*2; ++i)
            l.push_front(0);

        std::uint8_t t = 1;
        std::uint8_t* u = &t;

        std::uint64_t tAl = 0xffff;
        std::uint8_t temp = 10;
        std::uint8_t* uAl = &temp;
        vTestNode.push_back(TestNode(t,u));
        vTestNodeAl.push_back(TestNodeAl(tAl, uAl));
    }
};

TEST(TestCopyGroup, copyWithTheSameSize) {
    copy(arr.begin(), arr.end(), l.begin(), l.end());
    CHECK_EQUAL(arr[0], l.front()); // arr<int>[0] = 1; l<short>[0] = 1, l<short>[1] = 0;
    CHECK_EQUAL(*(++l.begin()), 0);

    copy(vTestNode.begin(), vTestNode.end(), vTestNodeAl.begin(), vTestNodeAl.end());
}