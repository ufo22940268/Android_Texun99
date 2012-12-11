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

public class NativeRenderer implements GLSurfaceView.Renderer  {
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
        change(width, height);
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        init(mContext.getAssets());
        loadTexture();

        //For test.
        test();
    }

    private void loadTexture() {
        BitmapFactory.Options options = new BitmapFactory.Options();
        options.inScaled = false;
        Bitmap bitmap = 
            BitmapFactory.decodeResource(
                    mContext.getResources(), R.drawable.cute_shit, options);

        GLUtils.texImage2D(GLES20.GL_TEXTURE_2D, 0, bitmap, 0);
    }

    private native void init(AssetManager manager);
    private native void change(int width, int height);
    private native void step();

    private native void test();

    static {
        System.loadLibrary("render");
    }
}
