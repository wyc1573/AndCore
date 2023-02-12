package com.wyc.utils.ui;

import android.content.Context;
import android.opengl.GLSurfaceView;

/**
 *  https://cloud.tencent.com/developer/article/1720275
 * */
public class DemoGlSurfaceView extends GLSurfaceView {
    private final DemoGlRenderer mRenderer;
    public DemoGlSurfaceView(Context context) {
        super(context);
        // Create an OpenGL ES 2.0 context
        setEGLContextClientVersion(2);
        mRenderer = new DemoGlRenderer();
        // Set the Renderer for drawing on the GLSurfaceView
        setRenderer(mRenderer);
    }
}