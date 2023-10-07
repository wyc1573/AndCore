//
// Created by wyc on 2023/9/12.
//

#include "effect.h"
#include <set>
#include <string>
#include <chrono>
#include <android-base/logging.h>

std::multiset<std::string> names;

// 没有const不能传右值
void logAndAdd(const std::string& name) {
    auto now = std::chrono::system_clock::now();
    LOG(INFO) << "running";
    names.emplace(name);
}

void test() {
    std::string petName("Darla");

    logAndAdd(petName);

    logAndAdd(std::string("Persephone"));

    logAndAdd("Patty Dog");  //创建临时对象
}

// -----------------------------------
template<typename T>
void logAndAdd(T&& name) {
    auto now = std::chrono::system_clock::now();
    LOG(INFO) << "Running...";
    names.emplace(std::forward<T>(name));
}

void test2() {
    std::string petName("Darla");

    logAndAdd(petName);   // 一如此前

    logAndAdd(std::string("Persephone")); // 对右值实施移动而非赋值

    logAndAdd("Patty Dog");  //在multiset中直接构造一个std::string对象，而非复制一个
                                   // std::string临时对象
}

//---------------------------------------
std::string nameFromIdx(int idx) {
    return "";
}

void logAndAdd(int idx) {
    auto now = std::chrono::system_clock::now();
    LOG(INFO) << "Running";
    names.emplace(nameFromIdx(idx));
}

void test3() {
    short nameIdx = 1;
    /**
     *  按照普适重载决议规则，精确匹配优先于提升后才能匹配，所以万能引用版本
     *  才是被调用的版本
     * */
//    logAndAdd(nameIdx);  //!!!无法匹配!!!
}

//-----------------------------------
class Person {
public:
    template<typename T>
    explicit Person(T&& n) : name(std::forward<T>(n)) {}
    explicit Person(int idx) {}

    Person(const Person& rhs) {} //编译器生成
    Person(const Person&& rhs) {} //编译器生成
private:
    std::string name;
};

void test4() {
    Person p("Nancy");
//    auto cloneOfP(p); //失败
    // const精确匹配
    Person const cp("Nancy");
    auto cloneOfP(cp);
}

//----------------------------------------
class SpecialPerson : public Person {
public:
    // 调用的基类的完美转发构造方法
//    SpecialPerson(const SpecialPerson& rhs) : Person(rhs) {}
    // 调用的基类的完美转发构造函数
//    SpecialPerson(SpecialPerson&& rhs) : Person(std::move(rhs)) {}
};

