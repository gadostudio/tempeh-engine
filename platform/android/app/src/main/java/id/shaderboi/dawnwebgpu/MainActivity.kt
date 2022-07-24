package id.shaderboi.dawnwebgpu

import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import id.shaderboi.dawnwebgpu.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {

    private var binding: ActivityMainBinding? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater).apply {
            setContentView(root)
        }
    }

}