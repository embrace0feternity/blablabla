#include <cstdlib>
#include <type_traits>
#include <list>
#include <array>
#include <vector>
#include <iostream>

template <typename T, typename U>
class TestNode {
private:
    T fieldT = T();
    U* filedU = nullptr;
public:
    explicit TestNode(const T& t, U* u): fieldT(t), filedU(u) {}
    const T& getT() const {return fieldT;}
    U* getU() const {return filedU;}
};

#pragma pack(push, 2)

template <typename T, typename U>
class TestNodeAl {
private:
    T fieldT = T();
    U* fieldU = nullptr;
public:
    explicit TestNodeAl(const T& t, U* u): fieldT(t), fieldU(u) {}

    template <typename TT, typename UU>
    TestNodeAl& operator=(const TestNode<TT,UU>& other) {
        this->fieldT = static_cast<T>(other.getT());
        this->fieldU = reinterpret_cast<U*>(other.getU());
        return *this;
    }
};

#pragma pop()

template <typename InputIt, typename OutputIt>
void copy(InputIt inputHead, InputIt inputTail, OutputIt outputHead, OutputIt outputTail) {
    std::size_t alignIn = alignof(typename InputIt::value_type);
    std::size_t alignOut = alignof(typename OutputIt::value_type);
    std::size_t align = (alignof(typename InputIt::value_type) > alignof(typename OutputIt::value_type)) ?
        (alignof(*outputHead)) : (alignof(*inputHead));
    
    while(inputHead != inputTail)
    {
        *outputHead = *inputHead;
        ++inputHead;
        for (std::size_t i = 0; i < align && (outputHead != outputTail); ++i)
        {
            ++outputHead;
        }
    }
}