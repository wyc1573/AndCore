//
// Created by wyc on 2023/5/3.
//
#include <cstdio>
#include <stdlib.h>

void callee() {
    printf("callee");
}

void caller() {
    printf("caller");
    callee();
}

void elf_mallloc() {
    void * p = malloc(1024 * 10);
    printf("malloc return %p", p);
}

static int static_var_ = 0;

static int* sp = &static_var_;

const int kGlobal = 1;

void hidden_func() __attribute__ ((visibility ("hidden"))){
    static_var_++;
    printf("static_func: %d %d", static_var_, kGlobal);
}
