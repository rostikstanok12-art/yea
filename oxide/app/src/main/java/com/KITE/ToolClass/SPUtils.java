package com.KITE.ToolClass;
import android.content.Context;
import android.content.SharedPreferences;
public class SPUtils {
    public static void put(Context c, String k, String v) {
        c.getSharedPreferences("oxide", Context.MODE_PRIVATE).edit().putString(k, v).apply();
    }
    public static String get(Context c, String k, String def) {
        return c.getSharedPreferences("oxide", Context.MODE_PRIVATE).getString(k, def);
    }
}
