package xyz.johansson.ecgbiozholter;

import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.TaskStackBuilder;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Vibrator;
import android.support.v4.app.NotificationCompat;
import android.support.v7.app.ActionBarActivity;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

/**
 * Main activity.
 *
 * (Min Sdk Version = 16)
 * @author Tobias Johansson
 * @version 1.3, 23 Apr 2015
 */
public class MainActivity extends ActionBarActivity {

    private boolean backgroundMode;
    private boolean error;
    private Collector collector;
    private Handler handler;
    private Logger logger;

    //---------------------------------------------------------------------------
    // Activity methods

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        // Runs when the application starts.
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        backgroundMode = false;
        error = false;

        handler = new Handler(); // creates a Handler

        logger = new Logger(this); // creates a Logger
        buttonSetActive(false); // button inactive before incoming data

        collector = new Collector(this, logger); // creates a Collector
        collector.start(); // starts collection of data
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        // Exits application if exit option is selected
        if (id == R.id.action_exit) {
            collector.interrupt();
            System.exit(0);
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    @Override
    public void onBackPressed() {
        moveTaskToBack(true);
    }

    @Override
    protected void onPause() {
        super.onPause();
        backgroundMode = true;
    }

    @Override
    protected void onResume() {
        super.onResume();
        backgroundMode = false;
    }

    //---------------------------------------------------------------------------
    // Button methods

    // On button clicked logger start or stop.
    public void onButtonClicked(View v) {
        switch (v.getId()) {
            case R.id.button_start_stop:
                if (error) {
                    System.exit(0);
                } else {
                    if (!logger.active()) logger.start();
                    else logger.stop();
                }
                break;
        }
    }

    // Set button active or inactive.
    public void buttonSetActive(final boolean bool) {
        handler.post(new Runnable() {
            public void run() {
                Button button = (Button) findViewById(R.id.button_start_stop);
                button.setEnabled(bool);
            }
        });
    }

    // Set button text after if logger activity.
    public void buttonSetText(final String msg) {
        handler.post(new Runnable() {
            public void run() {
                Button button = (Button) findViewById(R.id.button_start_stop);
                button.setText(msg);
            }
        });
    }

    //---------------------------------------------------------------------------
    // Display methods

    // View information about the execution.
    public void viewData(final String msg) {
        handler.post(new Runnable() {
            public void run() {
                TextView infoText = (TextView) findViewById(R.id.text_data);
                infoText.setText(msg);
            }
        });
    }

    // View data from the Holter device.
    public void viewInfo(final String msg) {
        handler.post(new Runnable() {
            public void run() {
                TextView infoText = (TextView) findViewById(R.id.text_info);
                infoText.setText(msg);
            }
        });
    }

    // Popup message.
    public void toastMessage(final String msg) {
        handler.post(new Runnable() {
            public void run() {
                Toast.makeText(
                        getApplicationContext(), msg, Toast.LENGTH_LONG).show();
            }
        });
    }

    //---------------------------------------------------------------------------
    // Error handling

    // If an error occur, if in background show notification, in any case make
    // mobile device vibrate for a half second, stop eventual logging for prevent
    // data loss, close Bluetooth and ask the user to restart the application.
    // Also gives an error message of what went wrong.
    public void errorHandling(String msg) {
        if (backgroundMode) {
            NotificationCompat.Builder builder =
                    new NotificationCompat.Builder(this)
                            .setSmallIcon(R.drawable.ic_launcher)
                            .setContentTitle("Error: " + msg)
                            .setContentText("\nPlease restart the application.");
            Intent resultIntent = new Intent(this, MainActivity.class);
            TaskStackBuilder stackBuilder = TaskStackBuilder.create(this);
            stackBuilder.addParentStack(MainActivity.class);
            stackBuilder.addNextIntent(resultIntent);
            PendingIntent resultPendingIntent =
                    stackBuilder.getPendingIntent(
                            0, PendingIntent.FLAG_UPDATE_CURRENT);
            builder.setContentIntent(resultPendingIntent);
            NotificationManager notificationManager =
                    (NotificationManager) getSystemService(
                            Context.NOTIFICATION_SERVICE);
            notificationManager.notify(-1, builder.build());
        }

        Vibrator v = (Vibrator) getSystemService(Context.VIBRATOR_SERVICE);
        v.vibrate(500);

        if (logger.active()) logger.stop();
        collector.interrupt();
        error = true;
        buttonSetText("Exit");
        buttonSetActive(true);
        viewError("Error: " + msg + "\nPlease restart the application.");
    }

    // View error message.
    private void viewError(final String msg) {
        handler.post(new Runnable() {
            public void run() {
                TextView errorText = (TextView) findViewById(R.id.text_error);
                errorText.setText(msg);
            }
        });
    }
}
