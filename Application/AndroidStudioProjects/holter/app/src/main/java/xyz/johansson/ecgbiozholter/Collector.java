package xyz.johansson.ecgbiozholter;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;

import java.io.InputStream;
import java.text.DecimalFormat;
import java.text.DecimalFormatSymbols;
import java.util.Locale;
import java.util.Set;
import java.util.UUID;

/**
 * Collects data via Bluetooth, displays and logs the data.
 *
 * (Min Sdk Version = 16)
 * @author Tobias Johansson
 * @version 1.3, 23 Apr 2015
 */
public class Collector implements Runnable {

    private static final double ADS_WEIGHT = 2.42 / (0x800000 - 1) / 4;
    private static final double BIOZ_WEIGHT = ADS_WEIGHT / (30 * 0.000001);
    private static final double ECG_WEIGHT = ADS_WEIGHT * 1000;
    private static final double MPU_WEIGHT = 1.0 / (0x8000 - 1);
    private static final double ACC_WEIGHT = MPU_WEIGHT * 4;
    private static final double GYR_WEIGHT = MPU_WEIGHT * 500;

    private BluetoothAdapter bluetoothAdapter;
    private BluetoothDevice bluetoothDevice;
    private BluetoothSocket bluetoothSocket;
    private InputStream inputStream;
    private Logger logger;
    private MainActivity mainActivity;
    private Thread thread;

    // Constructor.
    public Collector(MainActivity mainActivity, Logger logger) {
        this.mainActivity = mainActivity;
        this.logger = logger;
    }

    // Create thread.
    public void start() {
        thread = new Thread(this);
        thread.start();
    }

    // Interrupt thread.
    public void interrupt() {
        thread.interrupt();
    }

    // Implemented run method in Runnable.
    // Determines how the thread is running.
    @Override
    public void run() {
        viewData(null, null);
        initBluetooth();
        if (connectToHolter()) readDataStream(); // loops until interrupted
        disconnectFromHolter();
    }

    // Init Bluetooth.
    private void initBluetooth() {
        bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        if (bluetoothAdapter != null && bluetoothAdapter.isEnabled()) {
            bluetoothDevice = null;
            Set<BluetoothDevice> pairedBTDevices =
                    bluetoothAdapter.getBondedDevices();
            if (pairedBTDevices.size() > 0) {
                for (BluetoothDevice device : pairedBTDevices) {
                    String name = device.getName();
                    if (name.contains("ECG/BioZ Holter")) {
                        bluetoothDevice = device;
                        mainActivity.toastMessage("Paired device: " + name);
                        return;
                    }
                }
            }
            if (bluetoothDevice == null) {
                mainActivity.toastMessage("No paired ECG/BioZ Holter found!");
            }
        } else mainActivity.toastMessage("Bluetooth not available or disabled!");
    }

    // Try to connect Holter device to input stream.
    // Return true if succeeded, otherwise false.
    private boolean connectToHolter() {
        try {
            bluetoothSocket = bluetoothDevice
                    .createInsecureRfcommSocketToServiceRecord(UUID.fromString(
                            "00001101-0000-1000-8000-00805F9B34FB")); // SPP UUID
            bluetoothSocket.connect();
            inputStream = bluetoothSocket.getInputStream();
            mainActivity.viewInfo("Holter connected");
            return true;
        } catch (Exception e) {
            mainActivity.errorHandling("Bluetooth connection");
            return false;
        }
    }

    // Disconnect from Holter; closing stream and socket.
    private void disconnectFromHolter() {
        if (inputStream != null) {
            try {
                inputStream.close();
            } catch (Exception e) {
                e.printStackTrace();
            }
            inputStream = null;
        }
        if (bluetoothSocket != null) {
            try {
                bluetoothSocket.close();
            } catch (Exception e) {
                e.printStackTrace();
            }
            bluetoothSocket = null;
        }
    }

    // Read input stream, display and log data until an exception.
    private void readDataStream() {
        long[][] adsData = new long[10][2];
        long[] mpuData = new long[6];

        try {
            inputStream.read();
            // activates Start/Stop button after incoming data
            mainActivity.buttonSetActive(true);

            while (!thread.isInterrupted()) {
                if (inputStream.read() == 0x33) { // start field
                    for (int i = 0; i < 10; i++) {
                        adsData[i][0] = inputStream.read() << 16;
                        adsData[i][0] += inputStream.read() << 8;
                        adsData[i][0] += inputStream.read();
                        // if x >= 2^23 then x = x - 2^24
                        if(adsData[i][0] >= 0x800000) adsData[i][0] -= 0x1000000;

                        adsData[i][1] = inputStream.read() << 16;
                        adsData[i][1] += inputStream.read() << 8;
                        adsData[i][1] += inputStream.read();
                        // if x >= 2^23 then x = x - 2^24
                        if(adsData[i][1] >= 0x800000) adsData[i][1] -= 0x1000000;
                    }
                    for (int i = 0; i < 6; i++) {
                        mpuData[i] = inputStream.read() << 8;
                        mpuData[i] += inputStream.read();
                        // if x >= 2^15 then x = x - 2^16
                        if (mpuData[i] >= 0x8000) mpuData[i] -= 0x10000;
                    }

                    if (inputStream.read() == 0x55) { // "checksum"
                        viewData(adsData[0], mpuData);
                        if (logger.active()) logger.write(adsData, mpuData);
                    }
                }
            }
        } catch (Exception e) {
            mainActivity.errorHandling("Reading data");
        }
    }

    // View Holter data.
    private void viewData(long[] adsData, long[] mpuData) {
        if (adsData == null || mpuData == null) {
            mainActivity.viewData("BioZ\u0009\u0009\u0009[Ω]\u0009:\u0009" +
                    "\nECG\u0009\u0009[mV]\u0009:\u0009" +
                    "\nAcc_x\u0009\u0009[g]\u0009:\u0009" +
                    "\nAcc_y\u0009\u0009[g]\u0009:\u0009" +
                    "\nAcc_z\u0009\u0009[g]\u0009:\u0009" +
                    "\nGyr_x\u0009[°/s]\u0009:\u0009" +
                    "\nGyr_y\u0009[°/s]\u0009:\u0009" +
                    "\nGyr_z\u0009[°/s]\u0009:\u0009");
        } else {
            DecimalFormat df = new DecimalFormat(
                    "#.###", new DecimalFormatSymbols(Locale.US));
            mainActivity.viewData(
                    "BioZ\u0009\u0009\u0009[Ω]\u0009:\u0009"
                            + df.format(adsData[0] * BIOZ_WEIGHT) +
                            "\nECG\u0009\u0009[mV]\u0009:\u0009"
                            + df.format(adsData[1] * ECG_WEIGHT) +
                            "\nAcc_x\u0009\u0009[g]\u0009:\u0009"
                            + df.format(mpuData[0] * ACC_WEIGHT) +
                            "\nAcc_y\u0009\u0009[g]\u0009:\u0009"
                            + df.format(mpuData[1] * ACC_WEIGHT) +
                            "\nAcc_z\u0009\u0009[g]\u0009:\u0009"
                            + df.format(mpuData[2] * ACC_WEIGHT) +
                            "\nGyr_x\u0009[°/s]\u0009:\u0009"
                            + df.format(mpuData[3] * GYR_WEIGHT) +
                            "\nGyr_y\u0009[°/s]\u0009:\u0009"
                            + df.format(mpuData[4] * GYR_WEIGHT) +
                            "\nGyr_z\u0009[°/s]\u0009:\u0009"
                            + df.format(mpuData[5] * GYR_WEIGHT));
        }
    }
}
