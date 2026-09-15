package com.KITE;

import android.R;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.graphics.PixelFormat;
import android.graphics.Point;
import android.os.Build;
import android.os.Handler;
import android.os.Looper;
import android.os.RemoteException;
import android.os.SystemClock;
import android.view.Gravity;
import android.view.MotionEvent;
import android.view.SurfaceView;
import android.view.View;
import android.view.WindowManager;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import com.KITE.IPCCond.IPCService;

public class StartGame {
    private static WindowManager manager;
    private static View Touch_View;
    private static WindowManager.LayoutParams Touch_Params;

	public static void showFloatWindow(final Activity Activity) {
        manager = (WindowManager) Activity.getSystemService(Context.WINDOW_SERVICE);

        final WindowManager.LayoutParams MyTV_Params = new WindowManager.LayoutParams();
        MyTV_Params.systemUiVisibility = View.SYSTEM_UI_FLAG_HIDE_NAVIGATION |
			View.SYSTEM_UI_FLAG_FULLSCREEN |
			View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION |
			View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY |
			View.SYSTEM_UI_FLAG_LAYOUT_STABLE |
			View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN;
        MyTV_Params.type = Build.VERSION.SDK_INT >= Build.VERSION_CODES.O ? WindowManager.LayoutParams.TYPE_APPLICATION_OVERLAY : WindowManager.LayoutParams.TYPE_SYSTEM_ALERT;
        MyTV_Params.gravity = Gravity.TOP | Gravity.LEFT;
        MyTV_Params.format = PixelFormat.TRANSPARENT;
        MyTV_Params.width = WindowManager.LayoutParams.MATCH_PARENT;
        MyTV_Params.height = WindowManager.LayoutParams.MATCH_PARENT;
        MyTV_Params.flags = //WindowManager.LayoutParams.FLAG_SECURE |//防截屏
			//WindowManager.LayoutParams.FLAG_DITHER | //抖动(过录屏)
			WindowManager.LayoutParams.FLAG_NOT_TOUCHABLE |//不接受触控
			WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE |
			WindowManager.LayoutParams.FLAG_NOT_TOUCH_MODAL |
            //WindowManager.LayoutParams.FLAG_SPLIT_TOUCH |
			WindowManager.LayoutParams.FLAG_HARDWARE_ACCELERATED |//硬件加速
			WindowManager.LayoutParams.FLAG_FULLSCREEN |//隐藏状态栏导航栏以全屏(貌似没什么用)
			WindowManager.LayoutParams.FLAG_LAYOUT_NO_LIMITS |//忽略屏幕边界
			WindowManager.LayoutParams.FLAG_LAYOUT_ATTACHED_IN_DECOR |//显示在状态栏上方(貌似高版本无效
			WindowManager.LayoutParams.FLAG_LAYOUT_IN_SCREEN;//布局充满整个屏幕 忽略应用窗口限制

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
            MyTV_Params.layoutInDisplayCutoutMode = WindowManager.LayoutParams.LAYOUT_IN_DISPLAY_CUTOUT_MODE_SHORT_EDGES;//覆盖刘海
        }

        Touch_View = new View(Activity);
        //Touch_View.setZOrderOnTop(true);
        //Touch_View..setFormat(PixelFormat.TRANSPARENT);
        Touch_Params = new WindowManager.LayoutParams();
        Touch_Params.type = Build.VERSION.SDK_INT >= Build.VERSION_CODES.O ? WindowManager.LayoutParams.TYPE_APPLICATION_OVERLAY : WindowManager.LayoutParams.TYPE_SYSTEM_ALERT;
        Touch_Params.gravity = Gravity.TOP | Gravity.LEFT;
        Touch_Params.format = PixelFormat.TRANSPARENT;
        Touch_Params.width = 770;
        Touch_Params.height = 780;
        Touch_Params.x = 60;
        Touch_Params.y = 60;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
            Touch_Params.layoutInDisplayCutoutMode = WindowManager.LayoutParams.LAYOUT_IN_DISPLAY_CUTOUT_MODE_SHORT_EDGES;//覆盖刘海
        }

        Touch_Params.flags = 1024 | 8 | 256 |
			//WindowManager.LayoutParams.FLAG_SECURE |//防截屏
			//WindowManager.LayoutParams.FLAG_DITHER | //抖动(过录屏)
            //WindowManager.LayoutParams.FLAG_SPLIT_TOUCH |
			WindowManager.LayoutParams.FLAG_NOT_TOUCH_MODAL |
			WindowManager.LayoutParams.FLAG_HARDWARE_ACCELERATED |//硬件加速
			WindowManager.LayoutParams.FLAG_LAYOUT_NO_LIMITS |//忽略屏幕边界
			WindowManager.LayoutParams.FLAG_LAYOUT_ATTACHED_IN_DECOR;//显示在状态栏上方(貌似高版本无效

        Touch_View.setOnTouchListener(new View.OnTouchListener() {
                @Override
                public boolean onTouch(View v, MotionEvent event) {
					try {
                        if (MainActivity.app_Operation_mode_root) {
                            IPCService.GetIPC().MotionEventClick(event.getAction(), event.getRawX(), event.getRawY());
                        } else {
                            SuperJNI.MotionEventClick(event.getAction(), event.getRawX(), event.getRawY());                        
                        }
                    } catch (RemoteException e) {
                        e.printStackTrace();
					}
					return true;
				}
		});



	    final TV MyTV = new TV(Activity);

        MyTV_Params.token = MyTV.getApplicationWindowToken();
        Touch_Params.token = Touch_View.getApplicationWindowToken();

		manager.addView(MyTV, MyTV_Params);
	    manager.addView(Touch_View, Touch_Params);
        if (MainActivity.app_Operation_mode_root) {
    	    updateTouchWinSize();
    	} else {
    	    updateTouchWinSize_not_root();
    	}
    }


    public static void updateTouchWinSize() {
        final Handler handler = new Handler();
        handler.postDelayed(new Runnable() {
				@Override
				public void run() {
					try {
                        float[] rect = IPCService.GetIPC().GetImGuiwinsize();
                        Touch_Params.x = (int) rect[0];
                        Touch_Params.y = (int) rect[1];
                        Touch_Params.width = (int) rect[2];
                        Touch_Params.height = (int) rect[3];
                        manager.updateViewLayout(Touch_View, Touch_Params);
                    } catch (RemoteException e) {
                        e.printStackTrace();
					}
					handler.postDelayed(this, 17);
				}
        }, 17);
    }


    public static void updateTouchWinSize_not_root() {
        final Handler handler = new Handler();
        handler.postDelayed(new Runnable() {
				@Override
				public void run() {
                        float[] rect = SuperJNI.GetImGuiwinsize();
                        Touch_Params.x = (int) rect[0];
                        Touch_Params.y = (int) rect[1];
                        Touch_Params.width = (int) rect[2];
                        Touch_Params.height = (int) rect[3];
                        manager.updateViewLayout(Touch_View, Touch_Params);
					handler.postDelayed(this, 17);
				}
        }, 17);
    }

}
