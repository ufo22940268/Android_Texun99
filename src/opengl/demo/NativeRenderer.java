package opengl.demo;

import android.util.*;
import android.widget.*;
import android.view.*;
import android.content.*;
import android.content.res.*;
import android.app.*;
import android.os.*;
import android.database.*;
import android.net.*;
import android.opengl.*;
import android.graphics.*;
import android.graphics.drawable.*;

import java.util.*;
import java.nio.*;
import javax.microedition.khronos.opengles.*;
import javax.microedition.khronos.egl.*;

public class NativeRenderer implements GLSurfaceView.Renderer {
    private Context mContext;

    public NativeRenderer(Context context) {
        mContext = context;
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        step();
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        change(width, height, mContext.getAssets());
        loadTexture();
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        init();

        //For test.
        test();
    }

    private float time; 
    private void loadTexture() {
        BitmapFactory.Options options = new BitmapFactory.Options();
        options.inScaled = false;
        Bitmap solidBitmap = BitmapFactory.decodeResource(
                    mContext.getResources(), R.drawable.all_in_one, options);

        Bitmap  bitmap = solidBitmap.copy(solidBitmap.getConfig(), true);

        Canvas canvas = new Canvas(bitmap);

        Paint paint = new Paint();
        paint.setTextSize(32);
        paint.setColor(0xff66ccff);
        canvas.drawText("Touch screen to restart", 0, 32*5, paint);
        canvas.drawText(String.format("Lasting time %f", time), 0, 32*6, paint);

        GLUtils.texImage2D(GLES20.GL_TEXTURE_2D, 0, bitmap, 0);
    }

    public void loadScoreTexture(float time) {
        this.time = time;
        loadTexture();
    }

    private native void init();
    private native void change(int width, int height, AssetManager manager);
    private native void step();

    private native void test();

    static {
        System.loadLibrary("render");
    }
}
