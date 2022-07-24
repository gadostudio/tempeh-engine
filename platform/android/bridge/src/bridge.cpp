#include <tempeh/common.hpp>

#include <android/native_window_jni.h>
#include <android/asset_manager_jni.h>
#include <tempeh/android-bridge/bridge.hpp>
#include <tempeh/application.hpp>

extern "C" {

    JNIEXPORT void JNICALL
    Java_id_shaderboi_dawnwebgpu_TempehAppBridge_onSurfaceCreated(
        JNIEnv * env ,
        jobject tempeh_app_bridge_obj,
        jobject surface_obj,
        jobject asset_manager_obj) {
        auto window = ANativeWindow_fromSurface(env, surface_obj);
        auto asset_manager = AAssetManager_fromJava(env, asset_manager_obj);

        application = new Tempeh::Application{window, asset_manager};
    }

    JNIEXPORT void JNICALL Java_id_shaderboi_dawnwebgpu_TempehAppBridge_onSurfaceDestroyed(
        JNIEnv *env,
        jobject tempeh_app_bridge_obj) {
        delete application;
    }

    JNIEXPORT void JNICALL Java_id_shaderboi_dawnwebgpu_TempehAppBridge_draw(
        JNIEnv *env,
        jobject tempeh_app_bridge_obj) {
        application->frame();
    }

    JNIEXPORT void JNICALL Java_id_shaderboi_dawnwebgpu_TempehAppBridge_onSurfaceChanged(
        JNIEnv *env,
        jobject tempeh_app_bridge_obj,
        i32 width,
        i32 height) {
    }

}