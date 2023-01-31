package com.wyc.utils.dexopt;

import android.content.Context;
import android.content.ContextWrapper;
import android.content.pm.PackageManager;
import android.os.Binder;
import android.os.Handler;
import android.os.IBinder;
import android.os.IInterface;
import android.os.Parcel;
import android.os.RemoteException;
import android.os.ResultReceiver;
import android.os.SystemClock;
import android.util.Log;

import java.io.File;
import java.io.FileDescriptor;
import java.lang.reflect.Field;
import java.lang.reflect.InvocationHandler;
import java.lang.reflect.Method;
import java.lang.reflect.Proxy;

import com.wyc.utils.base.ReflectionUtils;

public class DexOptUtils {
    public static final String TAG = "AndCore.DexOptUtils";
    private static final int MAX_OPT_RETRY = 10;
    private static final String DESCRIPTOR = "android.content.pm.IPackageManager";

    public static void performDexOptSecondary(Context context, String oatPath) {
        Log.d(TAG, "performDexOptSecondary");
        File oatFile = new File(oatPath);
        String packageName = context.getPackageName();
        try {
            int transactionCode = reflectPerformDexOptSecondaryTransactionCode();
            Method getServiceMethod = ReflectionUtils.findMethod(Class.forName("android.os.ServiceManager"), "getService", (Class<?>[]) new Class[]{String.class});
            IBinder pmBinder = (IBinder) getServiceMethod.invoke(null, "package");
            if (pmBinder != null) {
                for (int i = 0; i < MAX_OPT_RETRY; i++) {
                    Log.i(TAG, "retry dexOpt: " + i);
                    try {
                        performDexOptSecondaryImpl(pmBinder, transactionCode, packageName, "speed", false);
                    } catch (Throwable throwable) {
                        Log.i(TAG, "", throwable);
                        if (i == MAX_OPT_RETRY - 1) {
                            Log.i(TAG, "", throwable);
                        }
                    }
                    SystemClock.sleep(3000);
                    if (oatFile.exists()) {
                        Log.i(TAG, "Secondary dexopt success.");
                        break;
                    }
                }
            }
        } catch (Throwable t) {
            Log.i(TAG, "performDexOptSecondary", t);
        }
    }

    private static int reflectPerformDexOptSecondaryTransactionCode() throws UnsupportedOperationException {
        try {
            Method getDeclaredFieldMethod = ReflectionUtils.findMethod((Class<?>) Class.class, "getDeclaredField", (Class<?>[]) new Class[]{String.class});
            getDeclaredFieldMethod.setAccessible(true);
            Field codeField = (Field)  getDeclaredFieldMethod.invoke(Class.forName("android.content.pm.IPackageManager$Stub"), "TRANSACTION_performDexOptSecondary");
            codeField.setAccessible(true);
            return ((Integer) codeField.get(null)).intValue();
        } catch (Throwable throwable) {
            throw new UnsupportedOperationException("Cannot reflect transaction code of performDexOptSecondary.", throwable);
        }
    }

    private static int reflectPerformDexOptTransactionCode() throws UnsupportedOperationException {
        try {
            Method getDeclaredFieldMethod = ReflectionUtils.findMethod((Class<?>) Class.class, "getDeclaredField", (Class<?>[]) new Class[]{String.class});
            getDeclaredFieldMethod.setAccessible(true);
            Field codeField = (Field)  getDeclaredFieldMethod.invoke(Class.forName("android.content.pm.IPackageManager$Stub"), "TRANSACTION_performDexOptMode");
            codeField.setAccessible(true);
            return ((Integer) codeField.get(null)).intValue();
        } catch (Throwable throwable) {
            throw new UnsupportedOperationException("Cannot reflect transaction code of performDexOptSecondary.", throwable);
        }
    }

    private static void performDexOptSecondaryImpl(IBinder pmBinder, int transactionCode, String packageName, String compileFilter, boolean force) {
        Parcel data = null;
        Parcel reply = null;
        long identity = Binder.clearCallingIdentity();
        try {
            data = Parcel.obtain();
            reply = Parcel.obtain();
            data.writeInterfaceToken(DESCRIPTOR);
            data.writeString(packageName);
            data.writeString(compileFilter);
            boolean result = true;
            data.writeInt(force ? 1 : 0);
            try {
                if (pmBinder.transact(transactionCode, data, reply, 0)) {
                    try {
                        reply.readException();
                        if (reply.readInt() == 0) {
                            result = false;
                        }
                        if (!result) {
                            Log.w(TAG, "system performDexOptSecondary return false.");
                        }
                    } catch (Throwable thr) {
                        throw new IllegalStateException(thr);
                    }
                } else {
                    throw new IllegalStateException("Binder transaction failure.");
                }
            } catch (RemoteException e) {
                throw new IllegalStateException(e);
            }
        } finally {
            Binder.restoreCallingIdentity(identity);
            if (reply != null) {
                reply.recycle();
            }
            if (data != null) {
                data.recycle();
            }
        }
    }

    private static void registerDexModule(Context context, String dexPath, String oatPath) throws IllegalStateException {
        Log.i(TAG, "registerDexModule.");
        try {
            PackageManager syncPM = getSynchronizedPackageManager(context);
            Class<?> dexModuleRegisterCallbackClazz = Class.forName("android.content.pm.PackageManager$DexModuleRegisterCallback");
            Method registerDexModuleMethod = ReflectionUtils.findMethod(syncPM.getClass(), "registerDexModule", (Class<?>[]) new Class[]{String.class, dexModuleRegisterCallbackClazz});
            File oatFile = new File(oatPath);
            for (int i = 0; i < 2; i++) {
                Throwable pendingThr = null;
                try {
                    registerDexModuleMethod.invoke(syncPM, dexPath, null);
                } catch (Throwable thr) {
                    pendingThr = thr;
                }
                SystemClock.sleep(3000);
                if (!oatFile.exists()) {
                    if (i == 1) {
                        if (pendingThr != null) {
                            throw pendingThr;
                        } else if (!oatFile.exists()) {
                            throw new IllegalStateException("Expected oat file: " + oatFile.getAbsolutePath() + " does not exist.");
                        }
                    }
                } else {
                    return;
                }
            }
        } catch (IllegalStateException e) {
            throw e;
        } catch (Throwable thr2) {
            throw new IllegalStateException("Fail to call registerDexModule.", thr2);
        }
    }

    private static PackageManager getSynchronizedPackageManager(Context context) throws Throwable {
        synchronized (DexOptUtils.class) {
            Method getServiceMethod = ReflectionUtils.findMethod(Class.forName("android.os.ServiceManager"),"getService", (Class<?>[]) new Class[]{String.class});
            final IBinder pmBinder = (IBinder) getServiceMethod.invoke(null, "package");
            Method asInterfaceMethod = ReflectionUtils.findMethod(Class.forName("android.content.pm.IPackageManager$Stub"),"asInterface", (Class<?>[]) new Class[]{IBinder.class});

            IInterface pmInterface = (IInterface) asInterfaceMethod.invoke(null, (IBinder) Proxy.newProxyInstance(context.getClassLoader(), pmBinder.getClass().getInterfaces(), new InvocationHandler() {
                @Override
                public Object invoke(Object proxy, Method method, Object[] args) throws Throwable {
                    if ("transact".equals(method.getName())) {
                        args[3] = 0;
                    }
                    return method.invoke(pmBinder, args);
                }
            }));
            Object contextImpl = context instanceof ContextWrapper ? ((ContextWrapper) context).getBaseContext() : context;
            PackageManager res = (PackageManager) Class.forName("android.app.ApplicationPackageManager").getDeclaredConstructor(contextImpl.getClass(), pmInterface.getClass().getInterfaces()[0]).newInstance(contextImpl, pmInterface);
            return res;
        }
    }

    private static IBinder getPackageManager(Context context) throws Throwable {
        if (context == null) {
            return null;
        }

        PackageManager packageManager = context.getPackageManager();

        Field mPmField = ReflectionUtils.findField(packageManager, "mPM");
        if (mPmField == null) {
            return null;
        }
        Object mPM = mPmField.get(packageManager);
        if (!(mPM instanceof IInterface)) {
            return null;
        }

        IInterface mPmInterface = (IInterface) mPM;
        IBinder binder = mPmInterface.asBinder();
        return binder;
    }

    public static  void dexOpt(Context context, String compileFilter, String oatFilePath) {
        File oatFile = new File(oatFilePath);
        for (int retry = 1; retry <= MAX_OPT_RETRY; retry++) {
            Log.d(TAG, "dexOpt retry: " + retry);
            performDexOpt(context, compileFilter, false);
            if (oatFile.exists()) {
                Log.d(TAG, "dexOpt Success!");
                break;
            }
            SystemClock.sleep(2000);
        }
    }

    private static void performDexOpt(Context context, String targetCompilerFilter, boolean force) {
        Log.d(TAG, "performDexOpt");
        android.os.Parcel data = android.os.Parcel.obtain();
        android.os.Parcel reply = android.os.Parcel.obtain();
        boolean result;
        try {
            data.writeInterfaceToken(DESCRIPTOR);
            data.writeString(context.getPackageName());
            data.writeInt(0);
            data.writeString(targetCompilerFilter);
            data.writeInt(((force)?(1):(0)));
            data.writeInt(1); //bootComplete
            data.writeString(null);
            IBinder mPmBinder = getPackageManager(context);
            int TRANSACTION_performDexOptMode = reflectPerformDexOptTransactionCode();
            mPmBinder.transact(TRANSACTION_performDexOptMode, data, reply, 0);
            reply.readException();
            result = (0 != reply.readInt());
            if (!result) {
                Log.w(TAG, "system performDexOpt return false.");
            }
        } catch (Throwable throwable) {
            Log.i(TAG, "", throwable);
        } finally {
            reply.recycle();
            data.recycle();
        }
    }

    private static String[] buildDexOptArgs(Context context, String compileFilter) {
        return buildArgs("compile", "-m", compileFilter, "-f", context == null ? "" : context.getPackageName());
    }

    private static String[] buildArgs(String... args) {
        if (args == null) {
            return null;
        }
        int len = args.length;
        String[] res = new String[len];
        for (int i = 0; i < len; i++) {
            res[i] = args[i];
        }
        return res;
    }
}
