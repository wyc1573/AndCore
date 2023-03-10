//
// Created by wyc on 2023/3/5.
//
#include "cpp_sample.h"
#include "android-base/logging.h"

class A {
public:
    void virtual funA() {}
};

class B {
public:
    void virtual funB() {}
};

class C {
public:
    void virtual funC() {}
};

class VirA : public virtual A {};

class VirAB : public virtual  A, public virtual  B {};

class VirABVir : public virtual  A, public virtual  B {
public:
    virtual void funVABV() {}
};

class VirABC : public virtual VirAB, public virtual C {};

class D : public A {};
class E : public A {};
class Diamond : public virtual D, public virtual E {};

void cpp_obj_model() {
    LOG(INFO) << "A(expect 8): " << sizeof(A) << ", VirA(expect: 8): " << sizeof(VirA) << ", VirAB(expect: 16): " << sizeof(VirAB)
    << ", VirABVir(expect: 16): " << sizeof(VirABVir) << ", VirABC(expect: 24): " << sizeof(VirABC)
              << ", Diamond(expect: 16)" << sizeof(Diamond);
}