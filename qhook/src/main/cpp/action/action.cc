#include "action.h"
#include "base/utils.h"
#include "base/dl_sym.h"
#include "hook/dl.h"

void do_action() {
    ALOG_ALWAYS("kSetIdealFootprint addr: %p", FUNC_DL(kSetIdealFootprint));
//    if (FUNC_DL(kSetIdealFootprint) != NULL) {
//        ((FUNC_TYPE_DL(kSetIdealFootprint)) FUNC_DL(kSetIdealFootprint))(NULL, 0);
//    }
}