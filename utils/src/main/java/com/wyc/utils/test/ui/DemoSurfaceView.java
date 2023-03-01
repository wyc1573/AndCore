package com.wyc.utils.test.ui;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.util.AttributeSet;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import androidx.annotation.Keep;

@Keep
public class DemoSurfaceView extends SurfaceView implements SurfaceHolder.Callback {
    /*
        SurfaceView功能简述:
        1.Provide a dedicated drawing surface embedded inside of a view hierarchy.
        2.Provide a surface in which a secondary thread can render in to the screen.

        SurfaceView注意事项:
        1.All SurfaceView and SurfaceHolder.Callback methods will be called from the UI thread.
        2.Drawing thread only touches the underlying Surface between SurfaceHolder.Callback.surfaceCreated()
        and SurfaceHolder.Callback.surfaceDestroyed().
    */
    private static final String TAG = "AndCore.View";
    private SurfaceHolder holder;
    private RenderThread renderThread; // 渲染绘制线程
    private boolean isRender;          // 控制线程

    private int screenWidth;
    private int screenHeight;

    public DemoSurfaceView(Context context) {
        super(context);
        holder = this.getHolder();
        holder.addCallback(this);
        renderThread = new RenderThread();
    }

    public DemoSurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
        holder = this.getHolder();
        holder.addCallback(this);
        renderThread = new RenderThread();
        screenWidth = dp2px(context, context.getResources().getConfiguration().screenWidthDp);
        screenHeight = dp2px(context, context.getResources().getConfiguration().screenHeightDp);
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        Log.d(TAG, "surfaceCreated");
        isRender = true;
        renderThread.start();
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        Log.d(TAG, "surfaceChanged");
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        isRender = false;
        Log.d(TAG, "surfaceDestroyed");
    }

    private class RenderThread extends Thread {
        @Override
        public void run() {
            // 死循环绘制线程
            while (isRender) {
                Canvas canvas = null;
                try {
                    // 1.锁定Canvas
                    canvas = holder.lockCanvas();
                    // 2.通过Canvas绘制图形
                    Paint paint = new Paint();
                    paint.setColor(Color.BLUE);
                    canvas.drawCircle(0, 0, screenWidth, paint);
                    paint.setColor(Color.RED);
                    canvas.drawCircle(screenWidth, screenHeight, screenWidth, paint);
                } catch (Exception e) {
                    // 3.捕获异常,防止异常导致Canvas没有解锁
                    Log.d(TAG, "", e);
                } finally {
                    // 4.解锁Canvas,把图形更新到屏幕
                    if (canvas != null)
                        holder.unlockCanvasAndPost(canvas);
                }
            }
        }
    }

    public static int dp2px(Context ctx, float dp) {
        float scale = ctx.getResources().getDisplayMetrics().density;
        return (int) (dp * scale + 0.5f);
    }
}
