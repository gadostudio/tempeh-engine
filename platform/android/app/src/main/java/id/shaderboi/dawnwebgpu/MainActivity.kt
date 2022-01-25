package id.shaderboi.dawnwebgpu

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import id.shaderboi.dawnwebgpu.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {

    private lateinit var view: WGPUView

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        view = WGPUView(application)
        setContentView(view)
    }
}