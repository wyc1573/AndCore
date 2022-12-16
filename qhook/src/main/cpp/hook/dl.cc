//
// Created by wyc on 2022/12/8.
//

#include "dl.h"
#include <xdl.h>
#include "base/dl_sym.h"
#include "base/utils.h"
#include "action/action.h"

#define HANDLE(SYM) handle_##SYM

// 定义
#define D(TYPE, SYM, VAL)
#define L(TYPE, LIB, SYM, VAL, RET, PARAM) void* FUNC_DL(SYM) = NULL;
#include "dl_sym"
#undef D
#undef L

bool init_DL() {
    ALOGD("init_DL called ...");
    bool res = true;

    // xdl open
#define D(TYPE, LIB, VAL) void* HANDLE(LIB) = xdl_open(DL_SYM(LIB), XDL_DEFAULT); \
    if(HANDLE(LIB) == NULL) {                                              \
        ALOG_ALWAYS("xdl_open " #LIB "failed!");                           \
        res  &= false;                                                     \
    }
#define L(TYPE, LIB, SYM, VAL, RET, PARAM)
    #include "dl_sym"
#undef D
#undef L

    // xdl sym
#define D(TYPE, SYM, VAL)
#define L(TYPE, LIB, SYM, VAL, RET, PARAM) if (HANDLE(LIB) != NULL) { \
    ALOGD("xdl sym");                                                                  \
    FUNC_DL(SYM) = xdl_sym(HANDLE(LIB), DL_SYM(SYM), NULL);           \
    ALOGD("%p", FUNC_DL(SYM)) ;                                                                     \
}
    #include "dl_sym"
#undef D
#undef L

    // xdl close
#define D(TYPE, LIB, VAL) if (HANDLE(LIB) != NULL) { xdl_close(HANDLE(LIB)); }
#define L(TYPE, LIB, SYM, VAL, RET, PARAM)
    #include "dl_sym"
#undef D
#undef L
    return res;
}