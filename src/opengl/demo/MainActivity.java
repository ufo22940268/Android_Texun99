package opengl.demo;

import android.app.Activity;
import android.os.Bundle;
import android.opengl.*;
import android.hardware.*;
import android.util.Log;

public class MainActivity extends Activity
{
    static public final String TAG = "sensor";

    private GLSurfaceView mGLSurfaceView;
    private SensorManager mSensorManager;

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);

        mSensorManager = (SensorManager)getSystemService(SENSOR_SERVICE);
        mSensorManager.registerListener(new MySensorListener(),
                mSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER),
                SensorManager.SENSOR_DELAY_NORMAL);
    }

    private class MySensorListener implements SensorEventListener {
        @Override
        public void onSensorChanged(SensorEvent sensorEvent) {
             if(sensorEvent.sensor.getType() == Sensor.TYPE_ACCELEROMETER){
                 //Log.i(TAG,"onSensorChanged");
                 
                 //图解中已经解释三个值的含义
                 float x = sensorEvent.values[0];
                 float y = sensorEvent.values[1];
                 float z = sensorEvent.values[2];
                 float threshold = 2;
                 float thresholdDouble = 3;
                 if (Math.abs(x) <= threshold && Math.abs(y) <= threshold) {
                     //Log.i(TAG,"None");
                 } else {
                     if (Math.abs(x) > thresholdDouble && Math.abs(y) > thresholdDouble) {
                         if (x < 0 && y < 0) {
                             //Log.i(TAG,"right up");
                             pressUp();
                             pressRight();
                         } else if (x < 0 && y > 0) {
                             //Log.i(TAG,"right down");
                             pressRight();
                             pressDown();
                         } else if (x > 0 && y < 0) {
                             //Log.i(TAG,"left up");
                             pressLeft();
                             pressUp();
                         } else if (x > 0 && y > 0) {
                             //Log.i(TAG,"left down");
                             pressLeft();
                             pressDown();
                         }
                     } else {
                         if (Math.abs(x) > Math.abs(y)) {
                             //Log.i(TAG, x > 0 ? "left" : "right");
                             if (x > 0) {
                                 pressLeft();
                             } else {
                                 pressRight();
                             }
                         } else {
                             //Log.i(TAG, y > 0 ? "down" : "up");
                             if (y > 0) {
                                 pressDown();
                             } else {
                                 pressUp();
                             }
                         }
                     }
                 }
             }
        }

        @Override
        public void onAccuracyChanged(Sensor sensor, int i) {
        }

    }

    @Override
    protected void onStop() {
        super.onStop();
        quitGame();
    }

    public native void quitGame();
    public native void pressUp();
    public native void pressDown();
    public native void pressLeft();
    public native void pressRight();
}
