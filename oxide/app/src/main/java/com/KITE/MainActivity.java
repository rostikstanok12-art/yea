package com.KITE;

import android.app.Activity;
import android.content.Intent;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.provider.Settings;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;
import com.KITE.ToolClass.FloatTool;
import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.FileReader;

public class MainActivity extends Activity {
    private FloatTool floatTool;
    private int gamePid = -1;
    private TextView statusText;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            if (!Settings.canDrawOverlays(this)) {
                Intent intent = new Intent(Settings.ACTION_MANAGE_OVERLAY_PERMISSION, Uri.parse("package:" + getPackageName()));
                startActivityForResult(intent, 123);
            }
        }

        statusText = findViewById(R.id.status_text);
        if (statusText == null) {
            // fallback if layout doesn't have it
            statusText = new TextView(this);
        }

        Button btn = findViewById(R.id.go_app);
        Button btnRootCheck = findViewById(R.id.btn_root_check);
        if (btnRootCheck != null) {
            btnRootCheck.setOnClickListener(new View.OnClickListener() {
                @Override public void onClick(View v) {
                    checkRootAndSelinux();
                }
            });
        }

        btn.setOnClickListener(new View.OnClickListener() {
            @Override public void onClick(View v) {
                startOxideESP();
            }
        });

        // Auto check root on start for BlueStacks
        checkRootAndSelinux();
    }

    private void checkRootAndSelinux() {
        StringBuilder sb = new StringBuilder();
        sb.append("=== BlueStacks Root Check ===\n");

        // Check id
        String id = runShell("id", true);
        sb.append("id: ").append(id).append("\n");

        // Check getenforce
        String enforce = runShell("getenforce", true);
        sb.append("getenforce: ").append(enforce).append("\n");

        // Try setenforce 0 if Enforcing (for BlueStacks)
        if (enforce.contains("Enforcing")) {
            runShell("setenforce 0", true);
            String enforce2 = runShell("getenforce", true);
            sb.append("after setenforce 0: ").append(enforce2).append("\n");
        }

        // Check oxide packages
        String pkgs = runShell("pm list packages | grep -i oxide", true);
        sb.append("oxide pkgs: ").append(pkgs).append("\n");

        // Check pid
        int pid = getPidForPackage("com.oxide.survivalisland");
        if (pid == -1) pid = getPidForPackage("com.oxide.survival");
        if (pid == -1) pid = getPidForPackage("com.catsbit.oxide");
        sb.append("oxide pid: ").append(pid).append("\n");

        if (pid != -1) {
            String maps = runShell("cat /proc/" + pid + "/maps | grep libil2cpp | head -n 3", true);
            sb.append("libil2cpp: ").append(maps).append("\n");
        }

        if (statusText != null) statusText.setText(sb.toString());
        Toast.makeText(this, sb.toString(), Toast.LENGTH_LONG).show();
    }

    private void startOxideESP() {
        // BlueStacks: try all known oxide package names
        String[] pkgs = {"com.oxide.survivalisland", "com.oxide.survival", "com.catsbit.oxide", "com.oxide.survivalisland.overseas"};
        for (String pkg : pkgs) {
            gamePid = getPidForPackage(pkg);
            if (gamePid != -1) {
                Toast.makeText(this, "Found " + pkg + " pid: " + gamePid, Toast.LENGTH_SHORT).show();
                break;
            }
        }

        if (gamePid == -1) {
            Toast.makeText(this, "Oxide not found - check adb root: adb -s 127.0.0.1:5555 root && adb shell id", Toast.LENGTH_LONG).show();
            // Still start overlay for testing
        } else {
            // For BlueStacks, we already have root via adb root, so setenforce 0
            runShell("setenforce 0", true);
            SuperJNI.setPid(gamePid);
        }

        if (floatTool == null) floatTool = new FloatTool(MainActivity.this);
        floatTool.show();
        Toast.makeText(this, "ESP overlay started - BlueStacks OpenGL", Toast.LENGTH_SHORT).show();
    }

    private int getPidForPackage(String pkg) {
        try {
            java.io.File proc = new java.io.File("/proc");
            for (java.io.File f : proc.listFiles()) {
                if (!f.isDirectory()) continue;
                try {
                    int pid = Integer.parseInt(f.getName());
                    BufferedReader br = new BufferedReader(new FileReader(f.getAbsolutePath() + "/cmdline"));
                    String cmd = br.readLine();
                    br.close();
                    if (cmd != null && cmd.contains(pkg)) return pid;
                } catch (Exception e) {}
            }
        } catch (Exception e) {}
        // Try pidof as fallback (works with root)
        try {
            String out = runShell("pidof " + pkg, true);
            if (out != null && !out.trim().isEmpty()) {
                return Integer.parseInt(out.trim().split(" ")[0]);
            }
        } catch (Exception e) {}
        return -1;
    }

    public static String runShell(String cmd, boolean isRoot) {
        try {
            Process p = Runtime.getRuntime().exec(isRoot ? "su" : "sh");
            DataOutputStream dos = new DataOutputStream(p.getOutputStream());
            dos.writeBytes(cmd + "\n");
            dos.writeBytes("exit\n");
            dos.flush();
            java.io.BufferedReader br = new java.io.BufferedReader(new java.io.InputStreamReader(p.getInputStream()));
            StringBuilder sb = new StringBuilder();
            String line;
            while ((line = br.readLine()) != null) sb.append(line).append(" ");
            p.waitFor();
            return sb.toString();
        } catch (Exception e) {
            return "err: " + e.getMessage();
        }
    }
}
