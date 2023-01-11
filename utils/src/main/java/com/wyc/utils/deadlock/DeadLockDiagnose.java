package com.wyc.utils.deadlock;

import java.util.ArrayList;

/**
 * Class to give diagnostic messages for Watchdogs.
 */
class DeadLockDiagnose {
    public static final String TAG = "AndCore";

    private static volatile DeadLockDiagnose instance;

    public static DeadLockDiagnose instance() {
        if (instance == null) {
            synchronized (DeadLockDiagnose.class) {
                if (instance == null) {
                    instance = new DeadLockDiagnose();
                }
            }
        }
        return instance;
    }

    private DeadLockDiagnose() {}

    public void startDetectDeadLock() {
//        mockSimpleDeadLock();
        mockChainDeadLock();
        ArrayList<ThreadLockInfo> lockInfoArrayList = LockUtils.getThreadLockInfos();
        LockUtils.detectDeadLock(lockInfoArrayList);
        LockUtils.dumpThreadLockInfo(lockInfoArrayList);
    }

    public void mockSimpleDeadLock() {
        LockMock.mockSimpleDeadLock();
        LockMock.safeSleep(1000);
    }

    public void mockChainDeadLock() {
        LockMock.mockChainDeadLock();
        LockMock.safeSleep(1000);
    }
}

