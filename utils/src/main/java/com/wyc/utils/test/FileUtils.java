package com.wyc.utils.test;

import android.os.Build;
import android.os.CancellationSignal;
import android.util.Log;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import java.io.FileDescriptor;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.concurrent.Executor;

public class FileUtils {
    private static final String TAG = "AndCore.FileUtils";

    /**
     * Copy the contents of one FD to another.
     * <p>
     * Attempts to use several optimization strategies to copy the data in the
     * kernel before falling back to a userspace copy as a last resort.
     *
     * @return number of bytes copied.
     */
    public static long copy(@NonNull FileDescriptor in, @NonNull FileDescriptor out)
            throws IOException {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {
            return android.os.FileUtils.copy(in, out);
        }
        Log.d(TAG, "OS VERSION < Q, Failed!");
        return -1;
    }

    /**
     * Copy the contents of one stream to another.
     * <p>
     * Attempts to use several optimization strategies to copy the data in the
     * kernel before falling back to a userspace copy as a last resort.
     *
     * @return number of bytes copied.
     */
    public static long copy(@NonNull InputStream in, @NonNull OutputStream out) throws IOException {
        return FileUtils.copy(in, out);
    }

    /**
     * Copy the contents of one stream to another.
     * <p>
     * Attempts to use several optimization strategies to copy the data in the
     * kernel before falling back to a userspace copy as a last resort.
     *
     * @param signal to signal if the copy should be cancelled early.
     * @param executor that listener events should be delivered via.
     * @param listener to be periodically notified as the copy progresses.
     * @return number of bytes copied.
     */
    public static long copy(@NonNull InputStream in, @NonNull OutputStream out,
                            @Nullable CancellationSignal signal, @Nullable Executor executor,
                            @Nullable android.os.FileUtils.ProgressListener listener) throws IOException {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {
            return android.os.FileUtils.copy(in, out, signal, executor, listener);
        }
        Log.d(TAG, "OS VERSION < Q, Failed!");
        return -1;
    }

}
