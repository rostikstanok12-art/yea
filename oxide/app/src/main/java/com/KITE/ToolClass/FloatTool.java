package com.KITE.ToolClass;

import android.content.Context;
import android.graphics.PixelFormat;
import android.os.Build;
import android.view.Gravity;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.WindowManager;
import com.KITE.SuperJNI;

public class FloatTool {
    private WindowManager windowManager;
    private SurfaceView floatView;
    private WindowManager.LayoutParams params;
    private Context context;
    private boolean isShowing = false;

    public FloatTool(Context ctx) {
        context = ctx;
        windowManager = (WindowManager) ctx.getSystemService(Context.WINDOW_SERVICE);
    }

    public void show() {
        if (isShowing) return;
        floatView = new SurfaceView(context);
        params = new WindowManager.LayoutParams(
                WindowManager.LayoutParams.MATCH_PARENT,
                WindowManager.LayoutParams.MATCH_PARENT,
                Build.VERSION.SDK_INT >= Build.VERSION_CODES.O ?
                        WindowManager.LayoutParams.TYPE_APPLICATION_OVERLAY :
                        WindowManager.LayoutParams.TYPE_SYSTEM_ALERT,
                WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE | WindowManager.LayoutParams.FLAG_NOT_TOUCH_MODAL | WindowManager.LayoutParams.FLAG_LAYOUT_IN_SCREEN,
                PixelFormat.TRANSLUCENT);
        params.gravity = Gravity.TOP | Gravity.LEFT;

        floatView.getHolder().addCallback(new SurfaceHolder.Callback() {
            @Override public void surfaceCreated(SurfaceHolder holder) {
                SuperJNI.setSurface(holder.getSurface());
            }
            @Override public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
                SuperJNI.start(width, height);
            }
            @Override public void surfaceDestroyed(SurfaceHolder holder) {}
        });

        floatView.setOnTouchListener(new View.OnTouchListener() {
            float downX, downY;
            @Override public boolean onTouch(View v, MotionEvent event) {
                SuperJNI.MotionEventClick(event.getAction(), event.getX(), event.getY());
                return true;
            }
        });

        windowManager.addView(floatView, params);
        isShowing = true;
    }

    public void hide() {
        if (isShowing && floatView != null) {
            windowManager.removeView(floatView);
            isShowing = false;
        }
    }
}
