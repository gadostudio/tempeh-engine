package id.shaderboi.dawnwebgpu

import android.view.Surface

class JNILib {
    companion object {
        // Used to load the 'dawnwebgpu' library on application startup.
        init {
            System.loadLibrary("dawnwebgpu")
        }
    }

    external fun init(surface: Surface): Long
    external fun enterFrame()
}