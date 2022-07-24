package id.shaderboi.dawnwebgpu

import android.content.res.AssetManager
import android.view.Surface

class TempehAppBridge {
    init {
        System.loadLibrary("tempeh_android_bridge")
    }

    external fun onSurfaceCreated(surface: Surface, assetManager: AssetManager)
    external fun onSurfaceChanged(width: Int, height: Int)
    external fun onSurfaceDestroyed()
    external fun draw()
}