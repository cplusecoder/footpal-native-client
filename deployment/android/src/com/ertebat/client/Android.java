/**
 * @author  Mohammad S. Babaei <info@babaei.net>
 */


package com.ertebat.client;

import android.app.Activity;
import android.app.Notification;
import android.app.NotificationManager;
import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;
import android.widget.Toast;
import java.lang.Exception;

public class Android extends org.qtproject.qt5.android.bindings.QtActivity
{
    private static final String TAG = "[Android Interface / com.ertebat.client.Android]";
    private static final int PICKFILE_RESULT_CODE = 101;

    private static Android s_instance;
    private static MailProfile s_mailProfileInstance;

    private static NotificationManager m_notificationManager;
    private static Notification.Builder m_notificationBuilder;

    public static native void OpenFileDialogAccepted(String path);
    public static native void OpenFileDialogRejected();

    public static native void mailProfile_onConnectCompleted(boolean succeeded);
    public static native void mailProfile_onDisconnectCompleted();
    public static native void mailProfile_onSendCompleted(boolean succeeded);
    public static native void mailProfile_onGetMessageCountCompleted(int count);
    public static native void mailProfile_onFetchMessagesCompleted(CharSequence json);

    public static boolean release()
    {
        try {
            if (s_instance != null) {
                s_instance = null;

                Log.v(TAG, "Android interface successfully released!");
            }

            if (s_mailProfileInstance != null) {
                s_mailProfileInstance = null;

                Log.v(TAG, "Android interface successfully released!");
            }
        }

        catch (Exception e) {
            e.printStackTrace();
            return false;
        }

        return true;
    }

    public static boolean isInitialized()
    {
        Log.v(TAG, "isInitialized");

        return s_instance != null ? true : false;
    }

    public static CharSequence getScreenType()
    {
        return s_instance.getResources().getString(R.string.screen_type);
    }

    public static boolean notify(final CharSequence title, final CharSequence text, final int id)
    {
        Log.v(TAG, "notify");

        try {
            if (m_notificationManager == null) {
                m_notificationManager = (NotificationManager)s_instance.getSystemService(Context.NOTIFICATION_SERVICE);
                m_notificationBuilder = new Notification.Builder(s_instance);
                m_notificationBuilder.setSmallIcon(R.drawable.icon);
            }

            m_notificationBuilder.setContentTitle(title);
            m_notificationBuilder.setContentText(text);
            m_notificationManager.notify(id, m_notificationBuilder.build());
        }

        catch (Exception e) {
            e.printStackTrace();
            return false;
        }

        return true;
    }

    public static boolean openFileDialog()
    {
        Log.v(TAG, "openFileDialog");

        try {
            Intent intent = new Intent(Intent.ACTION_GET_CONTENT);
            intent.setType("*/*");
            intent.addCategory(Intent.CATEGORY_OPENABLE);
            s_instance.startActivityForResult(intent.createChooser(intent, "Complete action using"), PICKFILE_RESULT_CODE);
        }

        catch (Exception e) {
            e.printStackTrace();
            return false;
        }

        return true;
    }

    public static boolean showToast(final CharSequence text, final int duration)
    {
        Log.v(TAG, "showToast");

        try {
            s_instance.runOnUiThread(new Runnable() {
                public void run() {
                    Toast toast = Toast.makeText(s_instance, text, duration);
                    toast.show();
                }
            });
        }

        catch (Exception e) {
            e.printStackTrace();
            return false;
        }

        return true;
    }

    public static void mailProfile_setHost(CharSequence host, CharSequence protocol)
    {
        s_mailProfileInstance.setHost(host.toString(), protocol.toString());
    }

    public static void mailProfile_init() {
        s_mailProfileInstance = new MailProfile();
    }

    public static void mailProfile_setPort(short port, CharSequence protocol) {
        s_mailProfileInstance.setPort(port, protocol.toString());
    }

    public static void mailProfile_setUsername(CharSequence username, CharSequence protocol) {
        s_mailProfileInstance.setUsername(username.toString(), protocol.toString());
    }

    public static void mailProfile_setPassword(CharSequence password, CharSequence protocol) {
        s_mailProfileInstance.setPassword(password.toString(), protocol.toString());
    }

    public static void mailProfile_setSecurity(int securityTypeIndex, CharSequence protocol) {
        s_mailProfileInstance.setSecurity(securityTypeIndex, protocol.toString());
    }

    public static boolean mailProfile_connect(CharSequence protocol) {
        return s_mailProfileInstance.connect(String.valueOf(protocol));
    }

    public static void mailProfile_disconnect(final CharSequence protocol) {
        s_mailProfileInstance.disconnect(protocol.toString());
    }

    public static void mailProfile_send(final CharSequence jsonMessage) {
        s_mailProfileInstance.send(jsonMessage.toString());
    }

    public static int mailProfile_getMessageCount() {
        return s_mailProfileInstance.getMessageCount();
    }

    public static CharSequence mailProfile_fetchMessages(final int startIndex, final int count) {
        return s_mailProfileInstance.fetchMessages(startIndex, count);
    }

    public Android()
    {
        s_instance = this;
        s_mailProfileInstance = new MailProfile();

        Log.v(TAG, "Android interface successfully initialized!");
    }

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data)
    {
        switch (requestCode) {
        case PICKFILE_RESULT_CODE:
            if (resultCode == Activity.RESULT_OK) {
                Log.v(TAG, "OpenFileDialogAccepted!");
                Uri uri = data.getData();
                showToast(uri.getPath(), Toast.LENGTH_LONG);
                OpenFileDialogAccepted(uri.getPath());

            } else {
                Log.v(TAG, "OpenFileDialogRejected!");
                OpenFileDialogRejected();
            }
            break;
        default:
            break;
        }
    }
}

