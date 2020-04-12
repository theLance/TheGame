package com.example.testapp;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.EditText;

public class MainActivity extends AppCompatActivity {
    public static final String EXTRA_MSG = "com.example.testapp.MSG";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
    }

    public void sendMessage(View view) {
        Intent msgInt = new Intent(this, DisplayMessageActivity.class);
        EditText txt = (EditText) findViewById(R.id.editText);
        String msg = txt.getText().toString();
        msgInt.putExtra(EXTRA_MSG, msg);
        startActivity(msgInt);
    }
}
