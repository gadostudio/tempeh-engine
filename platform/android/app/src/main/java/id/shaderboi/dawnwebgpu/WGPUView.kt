package id.shaderboi.dawnwebgpu

import android.content.Context
import android.graphics.Canvas
import android.graphics.PixelFormat
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.egl.EGLContext
import android.opengl.GLSurfaceView
import android.opengl.GLSurfaceView.EGLConfigChooser
import android.opengl.GLSurfaceView.EGLContextFactory
import android.util.Log
import android.view.Surface
import android.view.SurfaceHolder
import javax.microedition.khronos.opengles.GL10

import javax.microedition.khronos.egl.EGL10
import javax.microedition.khronos.egl.EGLDisplay

const val TAG = "WGPUView"
const val DEBUG = true

fun checkEglError(prompt: String, egl: EGL10) {
    var error: Int
    while (egl.eglGetError().also { error = it } != EGL10.EGL_SUCCESS) {
        Log.e(TAG, String.format("%s: EGL error: 0x%x", prompt, error))
    }
}

class WGPUView: GLSurfaceView, SurfaceHolder.Callback2 {
    val lib = JNILib()
    val app_view_ptr = Long.MAX_VALUE

    constructor(context: Context) : super(context) {
    }

    constructor(context: Context, translucent: Boolean, depth: Int, stencil: Int): this(context) {
    }

    override fun surfaceCreated(holder: SurfaceHolder) {
        //super.surfaceCreated(holder)

        lib.init(holder.surface)
    }

    override fun draw(canvas: Canvas?) {
        super.draw(canvas)

        lib.enterFrame()

        invalidate()
    }
}