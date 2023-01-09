package com.wyc.home;

import android.util.Log;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.LinkedList;

public class LockUtils {
    public static final String TAG = "AndCore";

    public static String formatLock(Object lock) {
        if (lock == null) {
            return "<0x0>";
        }
        return String.format("<0x%08x>", System.identityHashCode(lock));
    }

    private static ThreadLockInfo findWhoHeldBlockLock(Object lockBlockedOn, ArrayList<ThreadLockInfo> threadLockInfos) {
        for (ThreadLockInfo threadLockInfo : threadLockInfos) {
            if (threadLockInfo != null) {
                for (Object lockHeld : threadLockInfo.lockHelds()) {
                    if (lockHeld != null && System.identityHashCode(lockHeld) == System.identityHashCode(lockBlockedOn)) {
                        return threadLockInfo;
                    }
                }
            }
        }
        return null;
    }

    public static void detectDeadLock(ArrayList<ThreadLockInfo> threadLockInfos) {
        HashSet<ThreadLockInfo> used = new HashSet();
        LinkedList<ThreadLockInfo> chain = new LinkedList();
        for (ThreadLockInfo threadLockInfo : threadLockInfos) {
            if (threadLockInfo != null && !used.contains(threadLockInfo)) {
                used.add(threadLockInfo);
                chain.addLast(threadLockInfo);
                LinkedList<ThreadLockInfo> res = findCircle(threadLockInfos, used,
                        chain, threadLockInfo);
                if (res != null) {
                    // find
                    Log.d(TAG, "find!!!!! ");
                    for (ThreadLockInfo ti : res) {
                        Log.d(TAG, " " + ti.toString());
                    }
                }
            }
        }
    }

    /**
     * T1的block lock被T2持有，T2的block lock被T3持有.. T1 T2 T3形成链表
     * @param threadLockInfos 各个线程的等锁信息 包括持有的锁 与 等待的锁
     * @param used 出现在一个链中的元素 不会出现在其他链表中 用used标记已经被链接的元素 提高搜索效率
     * @param chain T1的block lock被T2持有，T2的block lock被T3持有... set中保存T1 T2 T3的信息；如果一个元素出现两次说明存在环，即dead lock.
     * @param threadLockInfo 待构建链表的当前元素
     * @author wyc
     * */
    public static LinkedList<ThreadLockInfo> findCircle(ArrayList<ThreadLockInfo> threadLockInfos, HashSet<ThreadLockInfo> used,
                                                        LinkedList<ThreadLockInfo> chain, ThreadLockInfo threadLockInfo) {
        if (threadLockInfos == null || used == null || chain == null || threadLockInfo == null) {
            return null;
        }
        Object blockedOn = threadLockInfo.lockBlockedOn();
        ThreadLockInfo whoHeldBlockLock = findWhoHeldBlockLock(blockedOn, threadLockInfos);
        if (whoHeldBlockLock == null) {
            return null;
        }
        chain.addLast(whoHeldBlockLock);
        used.add(whoHeldBlockLock);
        ThreadLockInfo who = findWhoHeldBlockLock(whoHeldBlockLock.lockBlockedOn(), threadLockInfos);
        if (who == null) {
            return null;
        }

        // 检查链表中是否有环
        LinkedList<ThreadLockInfo> res = new LinkedList<>();
        boolean find = false;
        for (ThreadLockInfo ti : chain) {
            if (ti != null) {
                if (ti.tid() == who.tid()) {
                    find = true;
                }
                if (find) {
                    res.addLast(ti);
                }
            }
        }
        if (find) {
            return res;
        }
        chain.addLast(who);
        used.add(who);
        return findCircle(threadLockInfos, used, chain, who);
    }

    /**
     * Print the annotated stack for the given thread.
     */
    public static ArrayList<ThreadLockInfo> getThreadLockInfos() {
        ArrayList<ThreadLockInfo> threadLockInfos = new ArrayList();
        Thread[] threads = getThreadList();
        if (threads == null || threads.length == 0) {
            Log.d(TAG, "getThreadList return null or empty, return!");
            return threadLockInfos;
        }

        Log.d(TAG, "threads.length = " + threads.length);
        for (Thread thd : threads) {
            if(thd == null) continue;
            Object[] annotatedThreadStackTrace = AnnotatedStackTrace.instance().getAnnotatedThreadStackTrace(thd);
            if (annotatedThreadStackTrace == null) {
                Log.d(TAG, "annotatedThreadStackTrace == null, continue!");
                continue;
            }
            ThreadLockInfo threadLockInfo = new ThreadLockInfo(thd);
            for(Object ast : annotatedThreadStackTrace) {
                Object[] heldLocks = AnnotatedStackTrace.instance().getHeldLocks(ast);
                if (heldLocks != null) {
                    for (Object lockHeld : heldLocks) {
                        threadLockInfo.addHeldLock(lockHeld);
                    }
                }
                Object blockedOn = AnnotatedStackTrace.instance().getBlockedOn(ast);
                threadLockInfo.setLockBlockedOn(blockedOn);
                Object stackTraceElement = AnnotatedStackTrace.instance().getStackTraceElement(ast);
                threadLockInfo.addStackTraceElement(stackTraceElement);
            }
            threadLockInfos.add(threadLockInfo);
        }
        return threadLockInfos;
    }

    public static void dumpThreadLockInfo(ArrayList<ThreadLockInfo> threadLockInfos) {
        Log.d(TAG, "dumpThreadLockInfo:");
        StringBuilder sb = new StringBuilder();
        for (ThreadLockInfo threadLockInfo : threadLockInfos) {
            sb.append(threadLockInfo.toString()).append("\n");
        }
        Log.d(TAG, sb.toString());
    }

    public static Thread[] getThreadList() {
        ThreadGroup rootGroup = Thread.currentThread().getThreadGroup();
        ThreadGroup parentGroup;
        while ((parentGroup = rootGroup.getParent()) != null) {
            rootGroup = parentGroup;
        }

        Thread[] threads = new Thread[rootGroup.activeCount()];
        while (rootGroup.enumerate(threads, true ) == threads.length) {
            threads = new Thread[threads.length * 2];
        }
        return threads;
    }

    private static String getBlockedOnString(Object blockedOn) {
        if (blockedOn == null) {
            return "- waiting to lock null";
        }
        return String.format("- waiting to lock <0x%08x> (a %s)",
                System.identityHashCode(blockedOn), blockedOn.getClass().getName());
    }

    private static String getLockedString(Object heldLock) {
        if (heldLock == null) {
            return "- locked null";
        }
        return String.format("- locked <0x%08x> (a %s)", System.identityHashCode(heldLock),
                heldLock.getClass().getName());
    }
}
