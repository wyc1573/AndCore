package com.wyc.utils.base;

import java.lang.reflect.Field;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

public class UnSafeHelper {

    public static Object getUnSafe() throws ClassNotFoundException, NoSuchFieldException, IllegalAccessException {
        Class<?> unSafeCls = Class.forName("sun.misc.Unsafe");
        Field field = ReflectionUtils.findField(unSafeCls, "theUnsafe");
        field.setAccessible(true);
        return field.get(null);
    }

    /**
     * Allocates an instance of the given class without running the constructor.
     * The class' <clinit> will be run, if necessary.
     */
    public static Object allocateInstance(Object unsafe, Class cls) throws ClassNotFoundException, NoSuchMethodException, InvocationTargetException, IllegalAccessException {
        Class<?> unSafeCls = Class.forName("sun.misc.Unsafe");
        Method method = ReflectionUtils.findMethod(unSafeCls, "allocateInstance", Class.class);
        return method.invoke(unsafe, cls);
    }

    /**
     * Gets the raw byte offset from the start of an object's memory to
     * the memory used to store the indicated instance field.
     *
     * @param field non-null; the field in question, which must be an
     * instance field
     * @return the offset to the field
     */
    public static long objectFieldOffset(Object unsafe, Field field) throws ClassNotFoundException, NoSuchMethodException, InvocationTargetException, IllegalAccessException {
        Class<?> unSafeCls = Class.forName("sun.misc.Unsafe");
        Method method = ReflectionUtils.findMethod(unSafeCls, "objectFieldOffset", Field.class);
        return (long) method.invoke(unsafe, field);
    }

    /**
     * Gets the offset from the start of an array object's memory to
     * the memory used to store its initial (zeroeth) element.
     *
     * @param clazz non-null; class in question; must be an array class
     * @return the offset to the initial element
     */
    public static int arrayBaseOffset(Object unsafe, Class clazz) throws ClassNotFoundException, NoSuchMethodException, InvocationTargetException, IllegalAccessException {
        Class<?> unSafeCls = Class.forName("sun.misc.Unsafe");
        Method method = ReflectionUtils.findMethod(unSafeCls, "arrayBaseOffset", Class.class);
        return (int) method.invoke(unsafe, clazz);
    }

    /**
     * Gets the size of each element of the given array class.
     *
     * @param clazz non-null; class in question; must be an array class
     * @return &gt; 0; the size of each element of the array
     */
    public static int arrayIndexScale(Object unsafe,Class clazz) throws ClassNotFoundException, NoSuchMethodException, InvocationTargetException, IllegalAccessException {
        Class<?> unSafeCls = Class.forName("sun.misc.Unsafe");
        Method method = ReflectionUtils.findMethod(unSafeCls, "arrayIndexScale", Class.class);
        return (int) method.invoke(unsafe, clazz);
    }

    /**
     * Performs a compare-and-set operation on an <code>int</code>
     * field within the given object.
     *
     * @param obj non-null; object containing the field
     * @param offset offset to the field within <code>obj</code>
     * @param expectedValue expected value of the field
     * @param newValue new value to store in the field if the contents are
     * as expected
     * @return <code>true</code> if the new value was in fact stored, and
     * <code>false</code> if not
     */
    public static boolean compareAndSwapInt(Object unsafe, Object obj, long offset,
              int expectedValue, int newValue) throws ClassNotFoundException, NoSuchMethodException, InvocationTargetException, IllegalAccessException {
        Class<?> unSafeCls = Class.forName("sun.misc.Unsafe");
        Method method = ReflectionUtils.findMethod(unSafeCls, "compareAndSwapInt", Object.class, long.class, int.class, int.class);
        return (boolean) method.invoke(unsafe, obj, offset, expectedValue, newValue);
    }
    public static boolean compareAndSwapLong(Object unsafe, Object obj, long offset,
                                            long expectedValue, long newValue) throws ClassNotFoundException, NoSuchMethodException, InvocationTargetException, IllegalAccessException {
        Class<?> unSafeCls = Class.forName("sun.misc.Unsafe");
        Method method = ReflectionUtils.findMethod(unSafeCls, "compareAndSwapLong", Object.class, long.class, long.class, long.class);
        return (boolean) method.invoke(unsafe, obj, offset, expectedValue, newValue);
    }
    public static boolean compareAndSwapObject(Object unsafe, Object obj, long offset,
                                             Object expectedValue, Object newValue) throws ClassNotFoundException, NoSuchMethodException, InvocationTargetException, IllegalAccessException {
        Class<?> unSafeCls = Class.forName("sun.misc.Unsafe");
        Method method = ReflectionUtils.findMethod(unSafeCls, "compareAndSwapLong", Object.class, long.class, Object.class, Object.class);
        return (boolean) method.invoke(unsafe, obj, offset, expectedValue, newValue);
    }
    /**
     * Parks the calling thread for the specified amount of time,
     * unless the "permit" for the thread is already available (due to
     * a previous call to {@link #unpark}. This method may also return
     * spuriously (that is, without the thread being told to unpark
     * and without the indicated amount of time elapsing).
     *
     * <p>See {@link java.util.concurrent.locks.LockSupport} for more
     * in-depth information of the behavior of this method.</p>
     *
     * @param absolute whether the given time value is absolute
     * milliseconds-since-the-epoch (<code>true</code>) or relative
     * nanoseconds-from-now (<code>false</code>)
     * @param time the (absolute millis or relative nanos) time value
     */
    public static void park(Object unsafe, boolean absolute, long time) throws ClassNotFoundException, NoSuchMethodException, InvocationTargetException, IllegalAccessException {
        Class<?> unSafeCls = Class.forName("sun.misc.Unsafe");
        Method method = ReflectionUtils.findMethod(unSafeCls, "park", boolean.class, long.class);
        method.invoke(unsafe, absolute, time);
    }

    /**
     * Unparks the given object, which must be a {@link Thread}.
     *
     * <p>See {@link java.util.concurrent.locks.LockSupport} for more
     * in-depth information of the behavior of this method.</p>
     *
     * @param thread non-null; the object to unpark
     */
    public static void park(Object unsafe, Object thread) throws ClassNotFoundException, NoSuchMethodException, InvocationTargetException, IllegalAccessException {
        Class<?> unSafeCls = Class.forName("sun.misc.Unsafe");
        Method method = ReflectionUtils.findMethod(unSafeCls, "unpark", Object.class);
        method.invoke(unsafe, thread);
    }

    public static long allocateMemory(Object unsafe, long bytes) throws ClassNotFoundException, NoSuchMethodException, InvocationTargetException, IllegalAccessException {
        Class<?> unSafeCls = Class.forName("sun.misc.Unsafe");
        Method method = ReflectionUtils.findMethod(unSafeCls, "allocateMemory", long.class);
        return (long) method.invoke(unsafe, bytes);
    }
    public static void freeMemory(Object unsafe, long address) throws ClassNotFoundException, NoSuchMethodException, InvocationTargetException, IllegalAccessException {
        Class<?> unSafeCls = Class.forName("sun.misc.Unsafe");
        Method method = ReflectionUtils.findMethod(unSafeCls, "freeMemory", long.class);
        method.invoke(unsafe, address);
    }

    public static void setMemory(Object unsafe, long address, long bytes, byte value) throws ClassNotFoundException, NoSuchMethodException, InvocationTargetException, IllegalAccessException {
        Class<?> unSafeCls = Class.forName("sun.misc.Unsafe");
        Method method = ReflectionUtils.findMethod(unSafeCls, "setMemory", long.class, long.class, byte.class);
        method.invoke(unsafe, address, bytes, value);
    }
    /**
     * Ensures that loads before the fence will not be reordered with loads and
     * stores after the fence; a "LoadLoad plus LoadStore barrier".
     *
     * Corresponds to C11 atomic_thread_fence(memory_order_acquire)
     * (an "acquire fence").
     *
     * A pure LoadLoad fence is not provided, since the addition of LoadStore
     * is almost always desired, and most current hardware instructions that
     * provide a LoadLoad barrier also provide a LoadStore barrier for free.
     * @since 1.8
     */
    public static void loadFence(Object unsafe) throws ClassNotFoundException, NoSuchMethodException, InvocationTargetException, IllegalAccessException {
        Class<?> unSafeCls = Class.forName("sun.misc.Unsafe");
        Method method = ReflectionUtils.findMethod(unSafeCls, "loadFence");
        method.invoke(unsafe);
    }

    /**
     * Ensures that loads and stores before the fence will not be reordered with
     * stores after the fence; a "StoreStore plus LoadStore barrier".
     *
     * Corresponds to C11 atomic_thread_fence(memory_order_release)
     * (a "release fence").
     *
     * A pure StoreStore fence is not provided, since the addition of LoadStore
     * is almost always desired, and most current hardware instructions that
     * provide a StoreStore barrier also provide a LoadStore barrier for free.
     * @since 1.8
     * */
    public static void storeFence(Object unsafe) throws ClassNotFoundException, NoSuchMethodException, InvocationTargetException, IllegalAccessException {
        Class<?> unSafeCls = Class.forName("sun.misc.Unsafe");
        Method method = ReflectionUtils.findMethod(unSafeCls, "storeFence");
        method.invoke(unsafe);
    }

    /**
     * Ensures that loads and stores before the fence will not be reordered
     * with loads and stores after the fence.  Implies the effects of both
     * loadFence() and storeFence(), and in addition, the effect of a StoreLoad
     * barrier.
     *
     * Corresponds to C11 atomic_thread_fence(memory_order_seq_cst).
     * @since 1.8
     */
    public static void fullFence(Object unsafe) throws ClassNotFoundException, NoSuchMethodException, InvocationTargetException, IllegalAccessException {
        Class<?> unSafeCls = Class.forName("sun.misc.Unsafe");
        Method method = ReflectionUtils.findMethod(unSafeCls, "fullFence");
        method.invoke(unsafe);
    }
}
