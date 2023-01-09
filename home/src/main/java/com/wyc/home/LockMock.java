package com.wyc.home;

import android.util.Log;

public class LockMock {
    public static final String TAG = "AndCore";

    static Object lockA = new Object();
    static Object lockB = new Object();
    static Object lockC = new Object();

    static Runnable abRunnable = new Runnable() {
        @Override
        public void run() {
            synchronized (lockA) {
                // 让B先拿到lockB
                safeSleep(500);
                synchronized (lockB) {
                    while(true) {
                        Log.d(TAG, "abRun running ...");
                    }
                }
            }
        }
    };

    static Runnable baRunnable = new Runnable() {
        @Override
        public void run() {
            synchronized (lockB) {
                safeSleep(500);
                synchronized (lockA) {
                    while (true) {
                        Log.d(TAG, "baRun running ...");
                    }
                }
            }
        }
    };

    static Runnable bcRunnable = new Runnable() {
        @Override
        public void run() {
            synchronized (lockB) {
                safeSleep(500);
                synchronized (lockC) {
                    while (true) {
                        Log.d(TAG, "bcRun running ...");
                    }
                }
            }
        }
    };

    static Runnable caRunnable = new Runnable() {
        @Override
        public void run() {
            synchronized (lockC) {
                safeSleep(500);
                synchronized (lockA) {
                    while (true) {
                        Log.d(TAG, "bcRun running ...");
                    }
                }
            }
        }
    };

    public static void mockSimpleDeadLock() {
        new Thread(abRunnable, "Thd-heldA-waitB").start();
        new Thread(baRunnable, "Thd-heldB-waitA").start();
    }

    public static void mockChainDeadLock() {
        new Thread(abRunnable, "Thd-heldA-waitB").start();
        new Thread(bcRunnable, "Thd-heldB-waitC").start();
        new Thread(caRunnable, "Thd-heldC-waitA").start();
    }

    public static void safeSleep(int millis) {
        try {
            Thread.sleep(millis);
        } catch (Throwable t) {
            Log.d(TAG, "safeSleep", t);
        }
    }
}
