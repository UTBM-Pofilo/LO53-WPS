package com.example.fabien.lo53_project;

import android.annotation.TargetApi;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Point;
import android.hardware.usb.UsbEndpoint;
import android.os.AsyncTask;
import android.os.Build;
import android.os.CountDownTimer;
import android.os.Handler;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.Display;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.TextView;
import com.example.fabien.lo53_project.DisplayToasts;

public class MainActivity extends AppCompatActivity {

    private int X_coord;
    private int Y_coord;
    private Button button_test;
    private ImageView cursor;
    private EditText X_coordinate;
    private EditText Y_coordinate;
    private refreshScreenTask refresh_task = null;
    private CountDownTimer timer = null;
    private Float[] refreshTaskParams;
    private Handler mHandler = new Handler();

    private class refreshScreenTask extends AsyncTask<Float, Void, Void>
    {
        private String method;
        public refreshScreenTask(String m_method)
        {
            super();
            method = m_method;
        }

        @Override
        protected Void doInBackground(Float... params) {
            Float coord_X = params[0];
            Float coord_Y = params[1];
            //cursor.drawableHotspotChanged(coord_X, coord_Y);

            return null;
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        refreshTaskParams = new Float[2];
        refresh_task = new refreshScreenTask("test");
        timer = new CountDownTimer(5000, 1000) {

            @Override
            public void onTick(long millisUntilFinished) {
                mHandler.post(new DisplayToasts(getBaseContext(),"Tick" ));

            }
            @Override
            public void onFinish() {
                mHandler.post(new DisplayToasts(getBaseContext(),"Count" ));
                String X_string = X_coordinate.getText().toString();
                String Y_string = Y_coordinate.getText().toString();
                X_coord = Integer.valueOf(X_string);
                Y_coord = Integer.valueOf(Y_string);
                //refreshTaskParams[0] = X_coord;
                //refreshTaskParams[1] = Y_coord;
                //refresh_task.execute(refreshTaskParams);

                moveCursor(X_coord, Y_coord);

                restartTimer();
            }
        };

        Display display = getWindowManager().getDefaultDisplay();
        Point size = new Point();
        display.getSize(size);

        Bitmap bmp = Bitmap.createScaledBitmap(BitmapFactory.decodeResource(getResources(), R.drawable.curser), ((size.x*10)/100), ((size.x*10)/100), true);
        cursor = (ImageView) findViewById(R.id.iv_cursor);
        cursor.setImageBitmap(bmp);

        X_coordinate = (EditText) findViewById(R.id.text_edit_X);
        Y_coordinate = (EditText) findViewById(R.id.text_edit_Y);
        button_test = (Button) findViewById(R.id.btn);

        button_test.setOnClickListener(btn_listener);

        mHandler.post(new DisplayToasts(this,"OK" ));

        //timer.start();
    }

    private void moveCursor(int x, int y) {
        ViewGroup.MarginLayoutParams lp = (ViewGroup.MarginLayoutParams)cursor.getLayoutParams();
        lp.leftMargin = x;
        lp.topMargin = y;
        cursor.requestLayout();
    }

    public void restartTimer()
    {
        timer.cancel();
        timer.start();
    }

    private View.OnClickListener btn_listener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            mHandler.post(new DisplayToasts(getBaseContext(),"Click" ));
            String X_string = X_coordinate.getText().toString();
            String Y_string = Y_coordinate.getText().toString();
            X_coord = Integer.valueOf(X_string);
            Y_coord = Integer.valueOf(Y_string);
            moveCursor(X_coord, Y_coord);
        }
    };
}
