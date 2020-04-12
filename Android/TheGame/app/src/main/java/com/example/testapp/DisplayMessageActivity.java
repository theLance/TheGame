package com.example.testapp;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.widget.TextView;

public class DisplayMessageActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_display_message);

        Intent mine = getIntent();
        String msg = mine.getStringExtra(MainActivity.EXTRA_MSG);
        TextView tv = findViewById(R.id.textView);
        tv.setText(msg);
    }
}
