#ifndef _TEMPEH_ANDROID_BRIDGE_BRIDGE_HPP
#define _TEMPEH_ANDROID_BRIDGE_BRIDGE_HPP

#include <tempeh/common.hpp>

#include <android/native_window_jni.h>
#include <android/asset_manager_jni.h>
#include <tempeh/application.hpp>

static Tempeh::Application* application = nullptr;

extern "C" {
    JNIEXPORT void JNICALL Java_id_shaderboi_dawnwebgpu_TempehAppBridge_onSurfaceCreated(
        JNIEnv *env,
        jobject tempeh_app_bridge_obj,
        jobject surface_obj,
        jobject asset_manager_obj);

    JNIEXPORT void JNICALL Java_id_shaderboi_dawnwebgpu_TempehAppBridge_onSurfaceDestroyed(
        JNIEnv *env,
        jobject tempeh_app_bridge_obj);

    JNIEXPORT void JNICALL Java_id_shaderboi_dawnwebgpu_TempehAppBridge_draw(
        JNIEnv *env,
        jobject tempeh_app_bridge_obj);

    JNIEXPORT void JNICALL Java_id_shaderboi_dawnwebgpu_TempehAppBridge_onSurfaceChanged(
        JNIEnv *env,
        jobject tempeh_app_bridge_obj,
        int32_t width,
        int32_t height);
}

#endif
