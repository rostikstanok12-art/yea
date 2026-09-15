package com.KITE.ToolClass;

import java.io.File;
import java.io.FileInputStream;
import java.io.InputStream;
import java.io.FileWriter;

public class fileUtil
{
    //检查文件
    public static boolean checkFiles(String path) {
        File file = new File(path);
        if (!file.exists() && !file.isDirectory()) {
            return false;
        } else {
            return true;
        }
    }
    //读取文件
    public static String readFile(String path) {
        File file = new File(path);
        try {
            InputStream buffer = new FileInputStream(file);
            byte[] bytes = new byte[buffer.available()];
            buffer.read(bytes);
            buffer.close();      
            return new String(bytes);
        } catch (Throwable e) {
            return null;
        }
    }
    //写入文件
    public static void addFile(String path, String content) {
      if(checkFiles(path)){
        M_deleteFile(path);
      }
        try {
            FileWriter fileWriter = new FileWriter(path, true);
            fileWriter.append(content);
            fileWriter.flush();
            fileWriter.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
    //删除文件
    public static void M_deleteFile(String filePath) {
        File file = new File(filePath);
        if (file.exists()) {
        if (file.delete()) {
            //Log.d("TAG", "文件已成功删除");
        } else {
            //Log.d("TAG", "文件删除失败");
        }
    } else {
        //Log.d("TAG", "文件不存在");
    }
}
}
