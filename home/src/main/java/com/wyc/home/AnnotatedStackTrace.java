package com.wyc.home;

import android.annotation.SuppressLint;
import android.util.Log;

import java.lang.reflect.Field;
import java.lang.reflect.Method;

public class AnnotatedStackTrace {
    public static final String TAG = "AndCore";

    private Class<?> vmStackCls;
    private Method getAnnotatedThreadStackTraceMethod;
    private Field stackTraceElementField;
    private Field heldLocksField;
    private Field blockedOnField;

    private static volatile AnnotatedStackTrace instance;

    public static AnnotatedStackTrace instance() {
        if (instance == null) {
            synchronized (AnnotatedStackTrace.class) {
                if (instance == null) {
                    instance = new AnnotatedStackTrace();
                }
            }
        }
        return instance;
    }

    @SuppressLint("SoonBlockedPrivateApi")
    private AnnotatedStackTrace() {
        try {
            vmStackCls = Class.forName("dalvik.system.VMStack");
            getAnnotatedThreadStackTraceMethod =
                    vmStackCls.getDeclaredMethod("getAnnotatedThreadStackTrace", Thread.class);
            Class<?> annotatedStackTraceElementCls = Class.forName("dalvik.system.AnnotatedStackTraceElement");

            stackTraceElementField = annotatedStackTraceElementCls.getDeclaredField("stackTraceElement");
            heldLocksField = annotatedStackTraceElementCls.getDeclaredField("heldLocks");
            blockedOnField = annotatedStackTraceElementCls.getDeclaredField("blockedOn");
            stackTraceElementField.setAccessible(true);
            heldLocksField.setAccessible(true);
            blockedOnField.setAccessible(true);
        } catch (Throwable t) {
            Log.e(TAG, "AnnotatedStackTrace Reflect", t);
        }
    }

    public Object getStackTraceElement(Object annotatedStackTrace) {
        if (annotatedStackTrace == null) {
            Log.i(TAG, "annotatedStackTrace == null, return null!");
            return null;
        }
        if (stackTraceElementField == null) {
            Log.i(TAG, "stackTraceElementField param is null!");
            return null;
        }
        Object stackTraceElement = null;
        try {
            stackTraceElement = stackTraceElementField.get(annotatedStackTrace);
        } catch (Throwable t) {
            Log.e(TAG, "getStackTraceElement", t);
            stackTraceElement = null;
        } finally {
            return stackTraceElement;
        }
    }

    public Object getBlockedOn(Object annotatedStackTrace) {
        if (blockedOnField == null) {
            Log.i(TAG, "blockedOnField == null, return null!");
            return null;
        }
        if (annotatedStackTrace == null) {
            Log.i(TAG, "getBlockedOn param is null!");
            return null;
        }
        Object blockedOn = null;
        try {
            blockedOn = blockedOnField.get(annotatedStackTrace);
        } catch (Throwable t) {
            Log.e(TAG, "getBlockedOn", t);
            blockedOn = null;
        } finally {
            return blockedOn;
        }
    }

    public Object[] getHeldLocks(Object annotatedStackTrace) {
        if (heldLocksField == null) {
            Log.i(TAG, "heldLocksField == null, return null!");
            return null;
        }
        if (annotatedStackTrace == null) {
            Log.i(TAG, "getHeldLocks param is null, return null!");
            return null;
        }
        Object[] heldLocks = null;
        try {
            heldLocks = (Object[]) heldLocksField.get(annotatedStackTrace);
        } catch (Throwable t) {
            Log.i(TAG, "getHeldLocks", t);
            heldLocks = null;
        } finally {
            return heldLocks;
        }
    }

    public Object[] getAnnotatedThreadStackTrace(Thread thd) {
        if (getAnnotatedThreadStackTraceMethod == null) {
            Log.i(TAG, "getAnnotatedThreadStackTraceMethod is null, return null!");
            return null;
        }
        Object[] annotatedStackTrace = null;
        try {
            annotatedStackTrace = (Object[]) getAnnotatedThreadStackTraceMethod.invoke(null, thd);
        } catch(Throwable t) {
            Log.e(TAG, "getAnnotatedThreadStackTrace", t);
            annotatedStackTrace = null;
        } finally {
            return annotatedStackTrace;
        }
    }
}
