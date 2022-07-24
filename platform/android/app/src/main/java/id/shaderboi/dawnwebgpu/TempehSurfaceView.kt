package id.shaderboi.dawnwebgpu

import android.content.Context
import android.util.AttributeSet
import android.view.SurfaceHolder
import android.view.SurfaceView

class TempehSurfaceView : SurfaceView, SurfaceHolder.Callback2 {
    private val tempehAppBridge = TempehAppBridge()

    init {
        holder.addCallback(this)
    }

    constructor(context: Context) : super(context) {
    }

    constructor(context: Context, attributeSet: AttributeSet) : super(context, attributeSet) {
    }

    constructor(context: Context, attributeSet: AttributeSet, defStyleAttr: Int) : super(
        context,
        attributeSet,
        defStyleAttr
    ) {
    }

    constructor(
        context: Context,
        attributeSet: AttributeSet,
        defStyleAttr: Int,
        defStyleRes: Int
    ) : super(context, attributeSet, defStyleAttr, defStyleRes) {
    }

    override fun surfaceCreated(holder: SurfaceHolder) {
        tempehAppBridge.onSurfaceCreated(holder.surface, resources.assets)
    }

    override fun surfaceChanged(holder: SurfaceHolder, format: Int, width: Int, height: Int) {
        tempehAppBridge.onSurfaceChanged(width, height)
    }

    override fun surfaceDestroyed(holder: SurfaceHolder) {
        tempehAppBridge.onSurfaceDestroyed()
    }

    override fun surfaceRedrawNeeded(holder: SurfaceHolder) {
        tempehAppBridge.draw()
    }

}