package com.example.fabien.lo53_project;

import android.content.Context;
import android.widget.Toast;

/**
 * Created by Fabien on 27/05/2016.
 */
public class DisplayToasts implements Runnable
{
    private final Context mContext;
    String mText;
    public DisplayToasts(Context mContext, String text)
    {
        this.mContext = mContext; mText = text;
    }

    @Override public void run()
    {
        Toast.makeText(mContext, mText, Toast.LENGTH_SHORT).show();
    }
}