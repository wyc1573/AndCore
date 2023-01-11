package com.wyc.utils.deadlock;

import android.util.Log;

import java.util.ArrayList;
import java.util.concurrent.atomic.AtomicInteger;

public class ThreadLockInfo {
    public static final String TAG = "AndCore";
    private static AtomicInteger atomicInteger = new AtomicInteger(0);
    private int tId;
    private Thread thread;
    private ArrayList lockHelds;
    private Object lockBlockedOn;
    private ArrayList stackTraceElements;

    public ThreadLockInfo() {
        thread = null;
        tId = atomicInteger.incrementAndGet();
        lockHelds = new ArrayList();
        lockBlockedOn = null;
        stackTraceElements = new ArrayList();
    }

    public ThreadLockInfo(Thread thread) {
        tId = atomicInteger.incrementAndGet();
        this.thread = thread;
        lockHelds = new ArrayList();
        lockBlockedOn = null;
        stackTraceElements = new ArrayList();
    }

    public void setLockBlockedOn(Object lockBlockedOn) {
        if (lockBlockedOn == null) {
            return;
        }
        if (this.lockBlockedOn == null) {
            this.lockBlockedOn = lockBlockedOn;
            return;
        }
        if (lockBlockedOn != this.lockBlockedOn) {
            Log.d(TAG, "Abnormal, One Thread blocked on different lock! threadName = " + thread.getName());
            return;
        }
    }

    public void addHeldLock(Object lockHeld) {
        if (lockHeld != null) {
            lockHelds.add(lockHeld);
        }
    }

    public void addStackTraceElement(Object stackTraceElement) {
        if (stackTraceElement != null) {
            stackTraceElements.add(stackTraceElement);
        }
    }

    public Thread thread() {
        return thread;
    }

    public ArrayList lockHelds() {
        return lockHelds;
    }

    public Object lockBlockedOn() {
        return lockBlockedOn;
    }

    public ArrayList stackTraceElement() {
        return stackTraceElements;
    }

    public int tid() {
        return tId;
    }

    public String toString() {
        StringBuilder sb = new StringBuilder();
        sb.append("[\n").append("threadName: ").append(thread.getName())
                .append("\nhelds: ");

        if (lockHelds.isEmpty()) {
            sb.append("Empty");
        } else {
            for(Object heldLock : lockHelds) {
                sb.append(LockUtils.formatLock(heldLock)).append(";");
            }
        }
        sb.append("\nblockedOn: ").append(LockUtils.formatLock(lockBlockedOn)).append("\n");
        sb.append("\nstackTraceElement:\n");
        if (stackTraceElements.isEmpty()) {
            sb.append("Empty");
        } else {
            for (Object stackTraceElement : stackTraceElements) {
                sb.append(stackTraceElement).append("\n");
            }
        }
        sb.append("\n]");
        return sb.toString();
    }
}
