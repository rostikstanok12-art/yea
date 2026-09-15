package com.KITE;

import android.content.ClipboardManager;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.icu.text.SimpleDateFormat;
import android.media.MediaPlayer;
import android.provider.Settings;
import android.text.TextUtils;
import android.util.Log;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.UnsupportedEncodingException;
import java.math.BigInteger;
import java.nio.charset.StandardCharsets;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.sql.Timestamp;
import java.util.Random;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import android.Manifest;
import android.content.pm.PackageManager;

public class Util {

	
    private static MediaPlayer mMediaPlayer;
	public static Context getContext() {
        return MainActivity.getContext;
    }
	
	
	/*
     *
     获取剪切板
     *
     */
    public static String 获取卡密() {
        ClipboardManager manager = (ClipboardManager)getContext().getSystemService(Context.CLIPBOARD_SERVICE);

        if (manager != null) {

            if (manager.hasPrimaryClip() && manager.getPrimaryClip().getItemCount() > 0) {

                CharSequence addedText = manager.getPrimaryClip().getItemAt(0).getText();

                String addedTextString = String.valueOf(addedText);

                if (!TextUtils.isEmpty(addedTextString)) {

                    return addedTextString;

                }

            }

        }
        return "获取卡密失败";
    }
	
	
	public static String imei() {
        return Settings.System.getString(getContext().getContentResolver(), Settings.Secure.ANDROID_ID);
    }
	
    public static String getTimeStateNew(String long_time) {
        String long_by_13 = "1000000000000";
        String long_by_10 = "1000000000";
        if (Long.valueOf(long_time) / Long.valueOf(long_by_13) < 1) {
            if (Long.valueOf(long_time) / Long.valueOf(long_by_10) >= 1) {
                long_time = long_time + "000";
            }
        }
        Timestamp time = new Timestamp(Long.valueOf(long_time));
        Timestamp now = new Timestamp(System.currentTimeMillis());
        SimpleDateFormat format = new SimpleDateFormat("yyyy-MM-dd");
        long day_conver = 1000 * 60 * 60 * 24;
        long hour_conver = 1000 * 60 * 60;
        long min_conver = 1000 * 60;
        long time_conver = now.getTime() - time.getTime();
        long temp_conver;
        if ((time_conver / day_conver) < 3) {
            temp_conver = time_conver / day_conver;
            if (temp_conver <= 2 && temp_conver >= 1) {
                return temp_conver + "天前";
            } else {
                temp_conver = (time_conver / hour_conver);
                if (temp_conver >= 1) {
                    return temp_conver + "小時前";
                } else {
                    temp_conver = (time_conver / min_conver);
                    if (temp_conver >= 1) {
                        return temp_conver + "分鐘前";
                    } else {
                        return "剛剛";
                    }
                }
            }
        } else {
            return format.format(time);
        }
    }

    public static boolean isRoot() {
        try {
            Process process = Runtime.getRuntime().exec("su");
            OutputStream os = process.getOutputStream();
            os.write("\n".getBytes());
            os.flush();
            os.write("exit".getBytes());
            os.flush();
            os.write("\n".getBytes());
            os.flush();
            int exitValue = process.waitFor();
            os.close();
            return exitValue == 0;
        } catch (Exception e) {
            return false;
        }
    }

    private static void load(Context context, File f, int a, String path) {
        for (int i = 1; i < a; i++) {
            File newFile = new File(f.getPath() + "/" + path + (i) + ".png");
            if (!newFile.exists()) {
                try {
                    InputStream in = context.getAssets().open("image/" + path + (i) + ".png");
                    FileOutputStream out = new FileOutputStream(newFile);
                    byte[] bs = new byte[1 * 1024 * 1024];
                    int count = 0;
                    while ((count = in.read(bs)) != -1) {
                        out.write(bs, 0, count);
                    }
                    out.flush();
                    in.close();
                    out.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
    }

	public static void loadImage(Context context, String assetsPath, String savePath){
		try {
			String fileNames[] = context.getAssets().list(assetsPath);// 获取assets目录下的所有文件及目录名
			if (fileNames.length > 0) {// 如果是目录
				File file = new File(savePath);
				if (!file.exists()) {
					file.mkdirs();// 如果文件夹不存在，则递归
				}
				for (String fileName : fileNames) {
					loadImage(context, assetsPath + "/" + fileName,
										savePath + "/" + fileName);
				}
			} else {// 如果是文件
				InputStream is = context.getAssets().open(assetsPath);
				FileOutputStream fos = new FileOutputStream(new File(savePath));
				byte[] buffer = new byte[1024];
				int byteCount = 0;
				while ((byteCount = is.read(buffer)) != -1) {// 循环从输入流读取
					// buffer字节
					fos.write(buffer, 0, byteCount);// 将读取的输入流写入到输出流
				}
				fos.flush();// 刷新缓冲区
				is.close();
				fos.close();
			}
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
    
    
    public static String getFileMd5(File file) {
        MessageDigest digest = null;
        FileInputStream fis = null;
        byte[] buffer = new byte[1024];
        try {
            if (!file.isFile()) {
                return "";
            }
            digest = MessageDigest.getInstance("MD5");
            fis = new FileInputStream(file);
            while (true) {
                int len;
                if ((len = fis.read(buffer, 0, 1024)) == -1) {
                    fis.close();
                    break;
                }
                digest.update(buffer, 0, len);
            }
        } catch (Exception e) {
            e.printStackTrace();
            return null;
        }
        BigInteger var5 = new BigInteger(1, digest.digest());
        return String.format("%1$032x", new Object[]{var5});
    }

    public static String getBinData() {
        StringBuilder sb = new StringBuilder();
        for (int i = 0; i < 1024; i++) {
            String temp = Integer.toHexString(new Random().nextInt(255) & 0xFF);
            sb.append("\\x");
            if (temp.length() == 1) {
                sb.append("0");
            }
            sb.append(temp);
        }
        return sb.toString();
    }

    private static String getMd5(String text) throws NoSuchAlgorithmException, UnsupportedEncodingException {
        MessageDigest md5 = MessageDigest.getInstance("MD5");
        byte[] bytes = md5.digest(text.getBytes(StandardCharsets.UTF_8));
        StringBuilder builder = new StringBuilder();
        for (byte aByte : bytes) {
            builder.append(Integer.toHexString((0x000000FF & aByte) | 0xFFFFFF00).substring(6));
        }
        return builder.toString();
    }

    public static String getAndroidID() {
        String[] paths = new String[]{
			"/data/local/tmp/.system.r350.x29",
			"/data/app/install_check",
			"/data/data/android/code_cache/pmx32.dat"
        };
        StringBuilder sb = new StringBuilder();
        for (String path : paths) {
            File f = new File(path);
            if (!f.exists()) {
                Util.runShell("echo -e -n \"" + getBinData() + "\" > " + path + "\n" + "chmod 644 " + path, true);
            }
            sb.append(getFileMd5(f)).append("&");
        }
        try {
            return getMd5(sb.toString());
        } catch (NoSuchAlgorithmException | UnsupportedEncodingException e) {
            e.printStackTrace();
        }
        return null;
    }

    public static byte[] runShell(String command, boolean isRoot) {
        try {
            Process process = Runtime.getRuntime().exec(isRoot ? "su" : "sh");
            InputStream ins = process.getInputStream();
            InputStream es = process.getErrorStream();
            OutputStream ous = process.getOutputStream();
            ous.write("\n".getBytes());
            ous.flush();
            ous.write(command.getBytes());
            ous.flush();
            ous.write("\n".getBytes());
            ous.flush();
            ous.write("exit".getBytes());
            ous.flush();
            ous.write("\n".getBytes());
            ous.flush();
            byte[] result = readInputStream(ins, false);
            byte[] error = readInputStream(es, false);
            process.waitFor();
            ins.close();
            es.close();
            ous.close();
            if (new String(error).trim().isEmpty()) {
                return result;
            } else {
                return ("Shell Result : \n" + new String(result) + "\n" + "Shell Error : \n" + new String(error) + "\n").getBytes();
            }
        } catch (Throwable th) {
            return ("Application Error : \n" + Log.getStackTraceString(th)).getBytes();
        }
    }

    public static byte[] readInputStream(InputStream ins, boolean close) {
        try {
            ByteArrayOutputStream bos = new ByteArrayOutputStream();
            int i = -1;
            byte[] buf = new byte[1024];
            while ((i = ins.read(buf)) != -1) {
                bos.write(buf, 0, i);
            }
            if (close) {
                ins.close();
                bos.close();
            }
            return bos.toByteArray();
        } catch (Throwable th) {
            return Log.getStackTraceString(th).getBytes();
        }
    }


    public static void doPlay(File audioFile) {
        //配置播放器MediaPlayer
        mMediaPlayer = new MediaPlayer();
        try {
            //设置声音文件
            mMediaPlayer.setDataSource(audioFile.getAbsolutePath());
            //设置监听回掉
            mMediaPlayer.setOnCompletionListener(new MediaPlayer.OnCompletionListener() {
					@Override
					public void onCompletion(MediaPlayer mediaPlayer) {
						//播放结束，释放播放器
						stopPlay();
					}
				});
            mMediaPlayer.setOnErrorListener(new MediaPlayer.OnErrorListener() {
					@Override
					public boolean onError(MediaPlayer mediaPlayer, int i, int i1) {
						//释放播放器
						stopPlay();
						//错误已经处理，返回true
						return true;
					}
				});
            //配置音量，是否循环
            mMediaPlayer.setVolume(1, 1);
            mMediaPlayer.setLooping(false);
            //准备，开始
            mMediaPlayer.prepare();
            mMediaPlayer.start();
        } catch (RuntimeException | IOException e) {
            //异常处理，防止闪退
            e.printStackTrace();
            //释放播放器
            stopPlay();
        }

    }


    /**
     * 停止播放逻辑
     */
    public static void stopPlay() {
        //释放播放器
        if (mMediaPlayer != null) {
            //充值监听器，防止内存泄漏
            mMediaPlayer.setOnCompletionListener(null);
            mMediaPlayer.setOnErrorListener(null);
            mMediaPlayer.stop();
            mMediaPlayer.reset();
            mMediaPlayer.release();
            mMediaPlayer = null;
        }
    }

    public static void copyFileUsingFileStreams(File source, File dest)
	throws IOException {
        InputStream input = null;
        OutputStream output = null;
        try {
            input = new FileInputStream(source);
            output = new FileOutputStream(dest);
            byte[] buf = new byte[1024];
            int bytesRead;
            while ((bytesRead = input.read(buf)) !=-1){
                output.write(buf, 0, bytesRead);
            }
        } finally {
            input.close();
            output.close();
        }
    }

    public static Bitmap compressImage(Bitmap image,int size,int options) {
        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        // 质量压缩方法，这里100表示不压缩，把压缩后的数据存放到baos中
        image.compress(Bitmap.CompressFormat.JPEG, 80, baos);
        // 循环判断如果压缩后图片是否大于100kb,大于继续压缩
        while (baos.toByteArray().length / 1024 > size) {
            options -= 10;// 每次都减少10
            baos.reset();// 重置baos即清空baos
            // 这里压缩options%，把压缩后的数据存放到baos中
            image.compress(Bitmap.CompressFormat.JPEG, options, baos);
        }
        // 把压缩后的数据baos存放到ByteArrayInputStream中
        ByteArrayInputStream isBm = new ByteArrayInputStream(baos.toByteArray());
        // 把ByteArrayInputStream数据生成图片
        Bitmap bitmap = BitmapFactory.decodeStream(isBm, null, null);
        return bitmap;
    }
	
	
	
	
}

