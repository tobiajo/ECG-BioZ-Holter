package xyz.johansson.ecgbiozholter;

import android.media.MediaScannerConnection;
import android.os.Environment;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Calendar;

/**
 * Logs data on request on a file named after the date and time it was created.
 *
 * (Min Sdk Version = 16)
 * @author Tobias Johansson
 * @version 1.3, 23 Apr 2015
 */
public class Logger {

    private boolean active;
    private BufferedWriter adsBufferedWriter;
    private BufferedWriter mpuBufferedWriter;
    private File adsFile;
    private File mpuFile;
    private long logTime;
    private MainActivity mainActivity;
    private String filename;

    // Constructor.
    public Logger(MainActivity mainActivity) {
        this.active = false;
        this.adsBufferedWriter = null;
        this.mpuBufferedWriter = null;
        this.mainActivity = mainActivity;
    }

    // Start logging to file.
    public void start() {
        try {
            // Path to file /ECG-BioZ-Holter/yyMMdd-HHmmss.txt
            DateFormat df = new SimpleDateFormat("yyMMdd-HHmmss");
            // If statement avoids filename collision
            if (filename != df.format(Calendar.getInstance().getTime())) {
                filename = df.format(Calendar.getInstance().getTime());
                String adsFilename = filename + "-ADS.txt";
                String mpuFilename = filename + "-MPU.txt";
                File folder = new File(Environment.getExternalStorageDirectory()
                        + File.separator + "ECG-BioZ-Holter");
                folder.mkdir();
                adsFile = new File(folder + File.separator + adsFilename);
                adsBufferedWriter = new BufferedWriter(new FileWriter(adsFile));
                mpuFile = new File(folder + File.separator + mpuFilename);
                mpuBufferedWriter = new BufferedWriter(new FileWriter(mpuFile));
                active = true;
                mainActivity.viewInfo("Log " + filename + " created");
                mainActivity.toastMessage("Logging started");
                mainActivity.buttonSetText("Stop");
                logTime = System.nanoTime();
            }
        } catch (Exception e) {
            mainActivity.errorHandling("Creating file");
        }
    }

    // Terminate logging to file and update for MTP.
    public void stop() {
        if (adsBufferedWriter != null && mpuBufferedWriter != null) {
            try {
                adsBufferedWriter.write(Long.toString(
                        System.nanoTime() - logTime) + "\r\n");
                mpuBufferedWriter.write(Long.toString(
                        System.nanoTime() - logTime) + "\r\n");
                active = false;
                adsBufferedWriter.close();
                mpuBufferedWriter.close();
                MediaScannerConnection.scanFile(mainActivity,
                        new String[]{adsFile.getAbsolutePath()}, null, null);
                MediaScannerConnection.scanFile(mainActivity,
                        new String[]{mpuFile.getAbsolutePath()}, null, null);
                mainActivity.viewInfo("Log " + filename + " closed");
                mainActivity.toastMessage("Logging stopped");
                mainActivity.buttonSetText("Start");
            } catch (Exception e) {
                e.printStackTrace();
                mainActivity.errorHandling("Closing file");
            }
        }
    }

    // Write data to file.
    public void write(long[][] adsData, long[] mpuData) {
        try {
            for (int i = 0; i < 10; i++) {
                adsBufferedWriter.write(arrayToString(adsData[i]) + "\r\n");
            }
            mpuBufferedWriter.write(arrayToString(mpuData) + "\r\n");
        } catch (Exception e) {
            e.printStackTrace();
            mainActivity.errorHandling("Writing data");
        }
    }

    // Return true if logger is active, otherwise false.
    public boolean active() {
        return active;
    }

    // Convert long array to a String.
    private String arrayToString(long[] data) {
        StringBuilder sb = new StringBuilder();
        for (int i = 0; i < data.length - 1; i++) {
            sb.append(Long.toString(data[i]));
            sb.append(";");
        }
        return sb.append(Long.toString(data[data.length - 1])).toString();
    }
}
