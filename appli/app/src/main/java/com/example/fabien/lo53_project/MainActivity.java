package com.example.fabien.lo53_project;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.pm.ActivityInfo;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Point;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.nfc.Tag;
import android.os.CountDownTimer;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.text.InputType;
import android.util.Log;
import android.view.Display;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.view.ViewTreeObserver;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.widget.Toast;
import com.android.volley.Request;
import com.android.volley.Response;
import com.android.volley.VolleyError;
import com.android.volley.toolbox.JsonObjectRequest;
import org.json.JSONException;
import org.json.JSONObject;

public class MainActivity extends AppCompatActivity {

    /****************************************************************
     *********************ATTRIBUTS DE LA CLASSE*********************
     * **************************************************************
     */
    /**Coordonées instant t en pixel**/
    private int X_coord;
    private int Y_coord;
    /**variables relatives à la calibrations en pixel**/
    private int pixYDistanceMax;
    private int pixXDistanceMax;
    private int pixMeter = 0;
    private int pixStepCalibration = 0;
    private int coordOrigine[] = {0,0};
    /**variables relatives à la calibrations en metre**/
    private float meterDistanceRepere = (float)6.9;
    private int meterStepCalibration = 0;
    /**Boolean modes**/
    private boolean calibrateStepMode = false;
    private boolean calibrateManualMode = false;
    /**Chaines de caracteres**/
    private static final String TAG = "MyActivity";
    private String mUrlRefresh = "http://192.168.1.42/api/findme/";
    private String mUrlCalibrate = "";
    private String mUrlReset = "http://192.168.1.42/api/reset";
    private String UrlCalibrateBase = "http://192.168.1.42/api/calibrate/";
    /**Requetes HTTP JSON**/
    private JsonObjectRequest calibrateRequest;
    private JsonObjectRequest resfreshRequest;
    /**Timer pour l'envoie des trames HTTP**/
    private CountDownTimer timer = null;
    /**Elements graphiques**/
    private ImageView cursor;
    private Button sendCoord ;
    private Button validPosition;
    private Button endCalibration;
    private AlertDialog.Builder alertDialog;
    private TextView timeOUTresponse = null;
    /**Context de l'application**/
    private Context mAppContext;



    /****************************************************************
     ***************METHODES SURCHARGEES DE LA CLASSE****************
     * **************************************************************
     */

    /**Methode qui se déclanche automatiquement au lancement de l'application
     * Elle représente la création de notre activité. = Lancement de toute les
     * initialisations nécessaires au programme et activation du timer qui cadence
     * les envoies URL (mode de démarage = Find Me)*/
    @Override
    protected void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        mAppContext = this.getApplicationContext();
        setRequestedOrientation (ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
        timeOUTresponse = (TextView) findViewById(R.id.textInfoConnexion);
        initMacAdresse();
        initCursor();
        initButton();
        initCounter();
        MySingleton.getInstance(mAppContext).getRequestQueue();
        initRefreshRequete();
    }

    /**Methode qui se déclanche automatiquement lorsque notre activité prend le
     * focus de l'écran. Se déclanche après le onCreate lors du démarage de l'application.
     * Cette méthode nous sert à récupere la dimenssion actuelle de l'activité (suivant
     * les telephone) et de lancer le calcul du ratio suivant ces informations
     * Le code implémenté ici ne fonctionne pas dans le onCreate raison pour laquelle nous
     * devons surcharger cette méthode*/
    @Override
    protected void onStart() {
        super.onStart();

        final RelativeLayout mainLayout = (RelativeLayout) findViewById(R.id.mlayout);

        ViewTreeObserver observer = mainLayout.getViewTreeObserver();
        observer.addOnGlobalLayoutListener(new ViewTreeObserver.OnGlobalLayoutListener() {
            @Override
            public void onGlobalLayout() {
                int screenWidth  = mainLayout.getMeasuredWidth();
                int screenHeight = mainLayout.getMeasuredHeight();
                mainLayout.getViewTreeObserver().removeGlobalOnLayoutListener(this);
                initRatioPixelAdapt(screenWidth, screenHeight);
            }
        });

        timer.start();
    }

    /**Methode permettant d'initialiser le menu option (avec l'option nouvelle
     * calibration decrit en XML)*/
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        MenuInflater inflater = getMenuInflater();
        inflater.inflate(R.menu.menu_test, menu);
        return true;
    }

    /**Methode appelé lors de la perte du focus par l'application*/
    @Override
    protected void onStop () {
        super.onStop();
        timer.cancel();
        timeOUTresponse.setVisibility(((View)timeOUTresponse).GONE);
        if ((MySingleton.getInstance(mAppContext).getRequestQueue()) != null) {
            MySingleton.getInstance(mAppContext).getRequestQueue().cancelAll(TAG);
        }
    }

    /**Méthode s'activement automatique lors de la selection d'une option dans le menu
     * Dans notre cas la seule option disponible est "nouvelle calibration"*/
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle item selection
        switch (item.getItemId()) {
            case R.id.resetRequete:
                sendReset();
                return true;

            case R.id.newCal:
                timer.cancel();
                timeOUTresponse.setVisibility(((View)timeOUTresponse).GONE);
                calibrateStepMode = true;
                calibrateManualMode = false;
                sendCoord.setVisibility(((View)sendCoord).GONE);
                endCalibration.setVisibility(((View)endCalibration).GONE);
                validPosition.setVisibility(((View)validPosition).GONE);
                initPixelCalibration();
                return true;

            case R.id.newCalManuelle:
                timer.cancel();
                timeOUTresponse.setVisibility(((View)timeOUTresponse).GONE);
                calibrateManualMode = true;
                calibrateStepMode = false;
                sendCoord.setVisibility(((View)sendCoord).GONE);
                endCalibration.setVisibility(((View)endCalibration).GONE);
                validPosition.setVisibility(((View)validPosition).GONE);
                initPixelCalibration();
                return true;

            default:
                return super.onOptionsItemSelected(item);
        }
    }

    /**Methode qui se déclanche automatiquement lorsque l'utilisateur intéragie
     * avec l'écran. Mais elle n'est prise en compte que lorsque l'utilisateur
     * doit initialiser la position d'origine via l'écran (au début de chaque calibration)*/
    @Override
    public boolean onTouchEvent(MotionEvent event){

        if (calibrateManualMode)
        {
            X_coord = (int)event.getRawX();
            Y_coord = (int)event.getRawY();
            moveCursor(X_coord, Y_coord);
        }
        return false;
    }

    /****************************************************************
     **********************METHODES DE LA CLASSE*********************
     * **************************************************************
     */

    /**Methode permettant l'envoie d'une requête HTTP Reset*/
    public void sendReset()
    {
        JsonObjectRequest resetRequete = new JsonObjectRequest (Request.Method.GET, mUrlReset, (String)null,
                new Response.Listener<JSONObject>()
                {
                    @Override
                    public void onResponse(JSONObject response)
                    {
                        String code = null;
                        try {
                            code = response.get("code").toString();
                            //Log.d(TAG, ("Code pass: " + code));
                        } catch (JSONException e) {
                            Log.d(TAG, ("Code Error : " + e.toString()));
                        }

                        if (code == "ACK")
                            Toast.makeText(MainActivity.this, "Reset ACK recu", Toast.LENGTH_LONG).show();
                    }
                },
                new Response.ErrorListener()
                {
                    @Override
                    public void onErrorResponse(VolleyError error)
                    {
                        Log.d(TAG, ("ERROR OCCURED : " + error.toString()));
                        timeOUTresponse.setVisibility(((View)timeOUTresponse).VISIBLE);
                    }
                }
        );
        MySingleton.getInstance(mAppContext).addToRequestQueue(resetRequete);
    }

    /**Methode permettant de deplacer le curseur suivant ses coordonnées*/
    public void moveCursor(int x, int y)
    {
        ViewGroup.MarginLayoutParams lp = (ViewGroup.MarginLayoutParams)cursor.getLayoutParams();
        lp.leftMargin = x;
        lp.topMargin = y;
        cursor.requestLayout();
    }

    /**Methode permettant de récuperer l'adresse MAC propre au mobile et
     * d'implementer cette derniere dans les URL a envoyer*/
    public void initMacAdresse()
    {
        WifiManager manager = (WifiManager) getSystemService(Context.WIFI_SERVICE);
        WifiInfo info = manager.getConnectionInfo();
        String addressMAC = info.getMacAddress();

        mUrlRefresh = mUrlRefresh + addressMAC;
        UrlCalibrateBase = UrlCalibrateBase + addressMAC + "/";
    }

    /**Methode permettant d'initialiser le dialogue concernant la demande
     * à l'utilisateur du pas de calibration en metre*/
    public void initDialogStepCalibration()
    {
        alertDialog = new AlertDialog.Builder(MainActivity.this);
        alertDialog.setTitle("PAS CALIBRATION");
        alertDialog.setMessage("Entrer un pas de calibration");

        final EditText input = new EditText(MainActivity.this);
        input.setInputType(InputType.TYPE_CLASS_NUMBER);
        LinearLayout.LayoutParams lp = new LinearLayout.LayoutParams(
                LinearLayout.LayoutParams.MATCH_PARENT,
                LinearLayout.LayoutParams.MATCH_PARENT);
        input.setLayoutParams(lp);
        alertDialog.setView(input);

        // Setting Positive "Yes" Button
        alertDialog.setPositiveButton("YES", new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog,int which) {
                        if (input.getText().toString().isEmpty())
                        {
                            Toast.makeText(MainActivity.this, "Veuillez saisir une valeur", Toast.LENGTH_LONG).show();
                            onEmptyDialogResponse();
                        }
                        else
                        {
                            meterStepCalibration = Integer.valueOf(input.getText().toString());
                            pixStepCalibration =  meterStepCalibration * pixMeter;
                        }
                    }
                }
        );
        alertDialog.setCancelable(false);
    }

    /**Methode permettant de relancer le dialogue ci-dessus en cas d'un saisie invalide
     * (Pour eviter de faire planter l'application)*/
    public void onEmptyDialogResponse()
    {
        initDialogStepCalibration();
        alertDialog.show();
    }

    /**Methode qui crée et initialise les requetes JSON de calibration
     * 1___le code a executer lors d'une réponse
     * 2___le code a executer lors d'un TIMEOUT*/
    public void initCalibrateRequete()
    {
        calibrateRequest = new JsonObjectRequest (Request.Method.GET, mUrlCalibrate, (String)null,
                new Response.Listener<JSONObject>()
                {
                    @Override
                    public void onResponse(JSONObject response)
                    {
                        //MANAGE RESPONSE
                        String code = null;
                        timeOUTresponse.setVisibility(((View)timeOUTresponse).GONE);
                        try {
                            code = response.get("code").toString();
                            //Log.d(TAG, ("Code pass: " + code));
                        } catch (JSONException e) {
                            Log.d(TAG, ("Code Error : " + e.toString()));
                        }

                        switch (code)
                        {
                            case "WAIT" :
                                Log.d(TAG, ("Code WAIT"));
                                break;

                            case "ACK" :
                                Log.d(TAG, ("Code ACK"));
                                break;

                            case "DONE" :
                                timer.cancel();
                                if ((MySingleton.getInstance(mAppContext).getRequestQueue()) != null) {
                                    MySingleton.getInstance(mAppContext).getRequestQueue().cancelAll(TAG);
                                }
                                if (calibrateStepMode)
                                {
                                    Log.d(TAG, ("Code DONE"));
                                    timeOUTresponse.setVisibility(((View)timeOUTresponse).GONE);
                                    X_coord = X_coord + pixStepCalibration;

                                    if ((X_coord -coordOrigine[0])>= pixXDistanceMax)
                                    {
                                        Y_coord = Y_coord - pixStepCalibration;
                                        X_coord = coordOrigine[0];
                                        if ((coordOrigine[1] - Y_coord)>= pixYDistanceMax)
                                        {
                                            X_coord = coordOrigine[0];
                                            Y_coord = coordOrigine[1];
                                            Toast.makeText(MainActivity.this, "Calibration end", Toast.LENGTH_LONG).show();
                                            sendCoord.setVisibility(((View)sendCoord).GONE);
                                            endCalibration.setVisibility(((View)endCalibration).VISIBLE);
                                        }
                                        else
                                        {
                                            Toast.makeText(MainActivity.this, "Avancer le telephone de " + String.valueOf(meterStepCalibration) + " mètres, et revenir au X origine", Toast.LENGTH_LONG).show();
                                        }
                                    }
                                    else
                                    {
                                        Toast.makeText(MainActivity.this, "Deplacer le telephone sur la droite de " + String.valueOf(meterStepCalibration) + " mètres", Toast.LENGTH_LONG).show();
                                    }
                                    Log.d(TAG, String.valueOf("Xcoord send   " + X_coord));
                                    Log.d(TAG, String.valueOf("Ycoord send   " + Y_coord));
                                    moveCursor(X_coord, Y_coord);
                                    sendCoord.setEnabled(true);
                                }

                                else
                                {
                                    Toast.makeText(MainActivity.this, "deplacer le curseur (ecran) et le telephone (physique) sur un nouveau point pour envoyer", Toast.LENGTH_LONG).show();
                                    sendCoord.setEnabled(true);
                                }

                                break;

                            default:
                                Log.d(TAG, ("Code UNKONW"));
                                break;
                        }
                    }
                },
                new Response.ErrorListener()
                {
                    @Override
                    public void onErrorResponse(VolleyError error)
                    {
                        // MANAGE ERROR
                        Log.d(TAG, ("ERROR OCCURED : " + error.toString()));
                        timeOUTresponse.setVisibility(((View)timeOUTresponse).VISIBLE);
                    }
                }
        );
    }

    /**Methode qui crée et initialise les requetes JSON de FIND ME
     * 1___le code a executer lors d'une réponse
     * 2___le code a executer lors d'un TIMEOUT*/
    public void initRefreshRequete()
    {
        resfreshRequest = new JsonObjectRequest (Request.Method.GET, mUrlRefresh, (String)null,
                new Response.Listener<JSONObject>()
                {
                    @Override
                    public void onResponse(JSONObject response)
                    {
                        //MANAGE RESPONSE
                        String code = null;
                        timeOUTresponse.setVisibility(((View)timeOUTresponse).GONE);

                        try {
                            code = response.get("code").toString();
                            //Log.d(TAG, ("Code pass: " + code));
                        } catch (JSONException e) {
                            Log.d(TAG, ("Code Error : " + e.toString()));
                        }

                        switch (code)
                        {
                            case "WAIT" :
                                Log.d(TAG, ("Code WAIT"));
                                break;

                            case "ACK" :
                                Log.d(TAG, ("Code ACK"));
                                break;

                            case "MAP" :
                                Log.d(TAG, ("Code MAP"));
                                try {
                                    X_coord = response.getInt("x");
                                } catch (JSONException e) {
                                    Log.d(TAG, ("Code Error : " + e.toString()));
                                }
                                try {
                                    Y_coord = response.getInt("y");
                                } catch (JSONException e) {
                                    Log.d(TAG, ("Code Error : " + e.toString()));
                                }
                                Log.d(TAG, "Move Cursor at new coordonates : \t X = " + String.valueOf(X_coord) + " ; Y = " +String.valueOf(Y_coord));
                                //Toast.makeText(MainActivity.this, String.valueOf(X_coord), Toast.LENGTH_LONG).show();
                                //Toast.makeText(MainActivity.this, String.valueOf(Y_coord), Toast.LENGTH_LONG).show();
                                moveCursor(X_coord, Y_coord);
                                break;

                            default:
                                Log.d(TAG, ("Code UNKONW"));
                                break;
                        }
                    }
                },
                new Response.ErrorListener()
                {
                    @Override
                    public void onErrorResponse(VolleyError error)
                    {
                        // MANAGE ERROR
                        Log.d(TAG, ("ERROR OCCURED : " + error.toString()));
                        timeOUTresponse.setVisibility(((View)timeOUTresponse).VISIBLE);
                    }
                }
        );
    }

    /**Methode permettant d'initialiser les différents bouton intégrés à l'application
     *  1___validPosition       utile lors de la calibration pour valider l'origine
     *  2___endCalibreation     utile pour valider la fin de la calibration
     *  3___sendCoord           utile pour envoyer les coord de chaque point lors de la calibration
     * */
    public void initButton()
    {
        sendCoord = (Button) findViewById(R.id.send_coord);
        endCalibration = (Button)findViewById(R.id.endCalibration);
        validPosition = (Button) findViewById(R.id.validPosition);

        validPosition.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                validPosition.setVisibility(((View)validPosition).GONE);
                if (calibrateStepMode)
                {
                    calibrateManualMode = false;
                    initDialogStepCalibration();
                    alertDialog.show();
                    sendCoord.setVisibility(((View)sendCoord).VISIBLE);
                }
                else
                {
                    Toast.makeText(MainActivity.this, "deplacer le curseur (ecran) et le telephone (physique) sur un nouveau point pour envoyer", Toast.LENGTH_LONG).show();
                    sendCoord.setVisibility(((View)sendCoord).VISIBLE);
                    endCalibration.setVisibility(((View)endCalibration).VISIBLE);
                }
                coordOrigine[0] = X_coord;
                coordOrigine[1] = Y_coord;
                moveCursor(X_coord, Y_coord);
            }
        });

        endCalibration.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                endCalibration.setVisibility(((View)endCalibration).GONE);
                validPosition.setVisibility(((View)validPosition).GONE);
                sendCoord.setVisibility(((View)sendCoord).GONE);
                calibrateStepMode = false;
                calibrateManualMode = false;
                timer.start();
            }
        });

        sendCoord.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mUrlCalibrate =  UrlCalibrateBase   + String.valueOf(X_coord) + "/"
                        + String.valueOf(Y_coord) + "/1";
                Log.d(TAG, (mUrlCalibrate));
                initCalibrateRequete();
                timer.start();
                sendCoord.setEnabled(false);
                //sendCoord.setVisibility(((View)sendCoord).GONE);
            }
        });

        sendCoord.setVisibility(((View)sendCoord).GONE);
        endCalibration.setVisibility(((View)endCalibration).GONE);
        validPosition.setVisibility(((View)validPosition).GONE);
    }

    /**Methode permettant d'initialiser le ratio en pixel entre l'ecran actuel
     * et l'image d'origine. Cette methode permet le calcul necessaire entre
     * les différents point de repère pour permettre une bonne calibration.
     * Elle prend en paramètre les dimensions de l'ecran actuel*/
    void initRatioPixelAdapt(int screenWidth, int screenHeight)
    {

        int originalWidth = 393;
        int originalHeight = 629;

        float ratioWidth = (float)screenWidth / (float)originalWidth;
        float ratioHeight = (float)screenHeight / (float)originalHeight;

        int repereP1Y = (int)((float)125 * ratioHeight);
        int repereP2Y = (int)((float)266 * ratioHeight);

        pixMeter = (int)((float)(repereP2Y - repereP1Y) / meterDistanceRepere);

        coordOrigine[0] = (int)((float)40 * ratioWidth);
        coordOrigine[1] = (int)((float)558 * ratioHeight);

        int repereP4X = (int)((float)337 * ratioWidth);
        int repereP3Y = (int)((float)40 * ratioHeight);

        pixXDistanceMax = repereP4X - coordOrigine[0];
        pixYDistanceMax = coordOrigine[1] - repereP3Y;

        moveCursor(coordOrigine[0],coordOrigine[1]);
        //Log.d(TAG, "XP4_____________" + repereP4X);
        //Log.d(TAG, "YP3_____________" + repereP3Y);

        //Log.d(TAG, "XMAX_____________" + pixXDistanceMax);
        //Log.d(TAG, "YMAX_____________" + pixYDistanceMax);
    }

    /**Methode permettant d'initialiser l'image du curseur */
    public void initCursor()
    {
        Display display = getWindowManager().getDefaultDisplay();
        Point size = new Point();
        display.getSize(size);

        Bitmap bmp = Bitmap.createScaledBitmap(BitmapFactory.decodeResource(getResources(),
                R.drawable.curser), ((size.x*5)/100), ((size.x*5)/100), false);
        cursor = (ImageView) findViewById(R.id.iv_cursor);
        cursor.setImageBitmap(bmp);
    }

    /**Methode ayant pour but d'autoriser la calibration de la position d'origine
     * par l'utilisateur via la position du doigt du l'écran (methode onTouch surchargée)*/
    public void initPixelCalibration()
    {
        calibrateManualMode = true;
        Toast.makeText(MainActivity.this, ("Placer le curseur sur le point d'origine"), Toast.LENGTH_LONG).show();

        validPosition.setVisibility(((View)validPosition).VISIBLE);
    }

    /**Methode permettant d'initialiser le timer du programme
     * Ce timer permet l'envoie tous les 500 ms des requetes URL
     * suivant le mode actif (calibration ou Find me ).
     * Ce timer est lancer et areter dans le reste du code suivant l'execution*/
    public void initCounter()
    {
        timer = new CountDownTimer(1000, 500)
        {
            @Override
            public void onTick(long millisUntilFinished)
            {
                if(calibrateStepMode || calibrateManualMode)
                {
                    MySingleton.getInstance(mAppContext).addToRequestQueue(calibrateRequest);
                    initCalibrateRequete();
                }
                else
                {
                    MySingleton.getInstance(mAppContext).addToRequestQueue(resfreshRequest);
                    initRefreshRequete();
                }
            }
            @Override
            public void onFinish()
            {
                timer.cancel();
                timer.start();
            }
        };
    }
}




/**##################################_____PIECES OF CODE______####################################
 * ###############################################################################################
 * ###############################################################################################
 * _______________________________________________________________________________________________
 * _______________________________________________________________________________________________
 * **/

/**_____________________________________NE PAS PRENDRE EN COMPTE__________________________________
 * _______________________________________________________________________________________________
 * **/

/**     BOUTON DONE RECEIVE POUR SIMULER LE DONE RECU EN CALIBRATION
 *
 * */
//private Button doneReceive;

//doneReceive = (Button) findViewById(R.id.doneReceive);//A ENLEVER POUR FINAL

//doneReceive.setVisibility(((View)doneReceive).GONE);
/*
doneReceive.setOnClickListener(new View.OnClickListener() {
@Override
public void onClick(View v) {

        timer.cancel();
        Y_coord = Y_coord - pixStepCalibration;

        if ((coordOrigine[1] - Y_coord)>= pixYDistanceMax)
        {
        X_coord = X_coord + pixStepCalibration;
        Y_coord = coordOrigine[1];
        if ((X_coord -coordOrigine[0])>= pixXDistanceMax)
        {
        X_coord = coordOrigine[0];
        Y_coord = coordOrigine[1];
        Toast.makeText(MainActivity.this, "Calibration end", Toast.LENGTH_LONG).show();
        sendCoord.setVisibility(((View)sendCoord).GONE);
        doneReceive.setVisibility(((View)doneReceive).GONE);
        endCalibration.setVisibility(((View)endCalibration).VISIBLE);
        }
        else
        {
        Toast.makeText(MainActivity.this, "Deplacer le telephone sur la droite de " + String.valueOf(meterStepCalibration) + " mètres, et revenir au Y origine", Toast.LENGTH_LONG).show();
        }
        }
        else
        {
        Toast.makeText(MainActivity.this, "Avancer le telephone de " + String.valueOf(meterStepCalibration) + " mètres", Toast.LENGTH_LONG).show();
        }

        Log.d(TAG, String.valueOf("XXXXXXXX   " + X_coord));
        Log.d(TAG, String.valueOf("YYYYYYYY   " + Y_coord));

        moveCursor(X_coord, Y_coord);

        sendCoord.setEnabled(true);

        //doneReceive.setVisibility(((View)doneReceive).GONE);
        //sendCoord.setVisibility(((View)sendCoord).VISIBLE);
        }
        });*/
/**     BOUTON DONE RECEIVE POUR SIMULER LE DONE RECU EN CALIBRATION
 *
 * */
/*

/*
                switch(step[0])
                {
                    case "step1":
                        tabrepere[0] = Y_coord;
                        Toast.makeText(MainActivity.this, ("Placer le curseur sur le point 2"), Toast.LENGTH_LONG).show();
                        step[0] = "step2";
                        break;

                    case "step2":
                        tabrepere[1] = Y_coord;
                        Toast.makeText(MainActivity.this, ("Placer le curseur sur le point d'origine"), Toast.LENGTH_LONG).show();
                        step[0] = "step3";
                        break;

                    case "step3":
                        tabrepere[2] = X_coord;
                        tabrepere[3] = Y_coord;
                        //pixMeter = (tabrepere[1]- tabrepere[0]) / meterDistanceRepere;
                        Log.d(TAG,"PIXMETER : " + pixMeter);
                        coordOrigine[0] = tabrepere[2];
                        coordOrigine[1] = tabrepere[3];
                        for (int i = 0;i<4;i++)
                        {
                            Log.d(TAG,String.valueOf(tabrepere[i]));
                            tabrepere[i] = 0;
                        }
                        step[0] = "step1";

                        break;

                    default:
                        break;
                }*/


/*
            switch (event.getAction()) {
                case MotionEvent.ACTION_DOWN:
                    //Log.d(TAG, "touch me>>>");
                    break;
                case MotionEvent.ACTION_MOVE:

                    break;
                case MotionEvent.ACTION_UP:

                    break;
            }*/