package opengl.demo;

import android.util.*;
import android.widget.*;
import android.view.*;
import android.content.*;
import android.app.*;
import android.os.*;
import android.database.*;
import android.net.*;
import android.opengl.*;
import android.opengl.GLSurfaceView.Renderer;

import java.util.*;

public class MyGLSurfaceView extends GLSurfaceView implements View.OnTouchListener  {

    NativeRenderer mRenderer;

    public MyGLSurfaceView(Context context, AttributeSet attr) {
        super(context, attr);
        setEGLContextClientVersion(2);
        mRenderer = new NativeRenderer(context);
        setRenderer(mRenderer);

        setOnTouchListener(this);
    }

    @Override
    public boolean onKeyDown(int keycode, KeyEvent event) {
        System.out.println("++++++++++++++++++++" + keycode + "++++++++++++++++++++");
        return true;
    }

    @Override
    public boolean onTouch(View view, MotionEvent event) {
        touch();
        return true;
    }

    private void requestReloadTexture() {
        mRenderer.reloadTexture();
    }
    
    private native void touch();
}
