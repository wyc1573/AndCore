#include "effect.h"
#include <iostream>
#include <list>
#include <vector>

// 万能引用类型推导
template<typename T>
void ref(T&& param) {}

template<typename T>
void value(T param) {}

// 编译器能够知道数组的大小
template<typename T, size_t N>
constexpr size_t arraySize(T (&) [N]) noexcept {
    return N;
}

template <typename T>
void valueI(std::initializer_list<T> param) {}

// c++ 14
template<typename Container, typename Index>
decltype(auto) authAndAccess(Container&& c, Index i) {
    return std::forward<Container>(c)[i];
}

// c++ 11
template<typename Container, typename Index>
auto authAndAccess2(Container&& c, Index i)
-> decltype(std::forward<Container>(c)[i])
{
    return std::forward<Container>(c)[i];
}

// 优先选用别名声明而非typedef
template<typename T>
class MyAlloc {};
template<typename T>
using MyAllocList = std::list<T, MyAlloc<T>>;

template<typename T>
struct MyAllocList2 {
    typedef std::list<T, MyAlloc<T>> type;
};

class Widget {
    void doWork() &;
    void doWork() &&;
};

template<typename T>
struct Widget2 {
    typename MyAllocList2<T>::type t;
};

//DIY
template <class C>
auto cbegin(const C& container) -> decltype(std::begin(container)) {
    return std::begin(container);
}

//
void noExcept() throw() { // old
}

void noExcept2() noexcept { // new
}

class Widget3 : public std::enable_shared_from_this<Widget3> {
public:
    void process() {
        processedWidgets.push_back(shared_from_this()); //寻找控制块
    }
private:
    std::vector<std::shared_ptr<Widget3>> processedWidgets;
};

void effect() {
    int x = 0;
    const int cx = x;
    const int& rx = x;
    volatile int vx = x;

    ref(x);  // T -> int&; paramType int&
    ref(cx); // T -> const int&; paramType const int&
    ref(rx); // T -> const int&; paramType const int&
    ref(3); // T -> int; paramType int&&

    value(x); // T -> int; paramType int
    value(cx); // T -> int; paramType int
    value(rx); // T -> int; paramType int
    value(vx); // T -> int; paramType int

    const char* const ptr = "Hello, World!";
    value(ptr); // const char* const

    const char name[] = "Hello, World!";
    value(name); // const char*
    ref(name);  // const char (&) [13]

    // auto x2 = {0, 1, 2, 3.0}; //错误
    // ref({1,2,3,4,5});
    valueI({1, 2, 3, 4, 5});

    //decltype
    Widget w;
    const Widget& rw = w;
    auto w1 = rw;

    decltype(auto) w2 = rw;
}
