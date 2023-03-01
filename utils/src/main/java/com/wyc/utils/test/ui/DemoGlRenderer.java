package com.wyc.utils.test.ui;


import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.util.Log;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 *  https://cloud.tencent.com/developer/article/1720275
 * */
public class DemoGlRenderer implements GLSurfaceView.Renderer {
    private static final String TAG = "AndCore.View";
    private Triangle mTriangle;
    private Square mSquare;

    public void onSurfaceCreated(GL10 unused, EGLConfig config) {
 //        Set the background frame color
        GLES20.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        mTriangle = new Triangle();

        Log.d(TAG, "onSurfaceCreated");
    }

    public void onDrawFrame(GL10 unused) {
        // Redraw background color
        // GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT);

        mTriangle.draw();
        Log.d(TAG, "onDrawFrame");
    }

    public void onSurfaceChanged(GL10 unused, int width, int height) {
        GLES20.glViewport(0, 0, width, height);
        Log.d(TAG, "onSurfaceChanged");
    }

    public static int loadShader(int type, String shaderCode){
        // 创造顶点着色器类型(GLES20.GL_VERTEX_SHADER)
        // 或者是片段着色器类型 (GLES20.GL_FRAGMENT_SHADER)
        int shader = GLES20.glCreateShader(type);
        // 添加上面编写的着色器代码并编译它
        GLES20.glShaderSource(shader, shaderCode);
        GLES20.glCompileShader(shader);
        return shader;
    }
}
