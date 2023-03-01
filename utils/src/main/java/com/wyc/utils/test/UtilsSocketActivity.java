package com.wyc.utils.test;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.view.View;
import android.widget.Button;

import com.wyc.utils.JNI;
import com.wyc.utils.R;

public class UtilsSocketActivity extends AppCompatActivity {

    Button socketNormal;
    Button socketDiagram;
    Button socketMsg;
    Button socketPair;
    Button socketPipe;
    Button eventFd;

    Button epoll;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.utils_socket);
        socketNormal = findViewById(R.id.utils_socket_normal);
        socketDiagram = findViewById(R.id.utils_socket_diagram);
        socketMsg = findViewById(R.id.utils_socket_msg);
        socketPair = findViewById(R.id.utils_socket_pair);
        socketPipe = findViewById(R.id.utils_socket_pipe);
        eventFd = findViewById(R.id.utils_socket_event_fd);
        epoll = findViewById(R.id.utils_socket_epoll);

        socketNormal.setOnClickListener(new SocketNormalClickListener());
        socketDiagram.setOnClickListener(new SocketDiagramClickListener());
        socketMsg.setOnClickListener(new SocketMsgClickListener());
        socketPair.setOnClickListener(new SocketPairClickListener());
        socketPipe.setOnClickListener(new SocketPipeClickListener());
        eventFd.setOnClickListener(new EventFdClickListener());
        epoll.setOnClickListener(new EpollClickListener());
    }

    public class SocketNormalClickListener implements View.OnClickListener {
        @Override
        public void onClick(View view) {
            JNI.socketNormal();
        }
    }

    public class SocketDiagramClickListener implements View.OnClickListener {
        @Override
        public void onClick(View view) {
            JNI.socketDiagram();
        }
    }

    public class SocketMsgClickListener implements View.OnClickListener {
        @Override
        public void onClick(View view) {
            JNI.socketMsg();;
        }
    }

    public class SocketPairClickListener implements View.OnClickListener {
        @Override
        public void onClick(View view) {
            JNI.socketPair();
        }
    }

    public class SocketPipeClickListener implements View.OnClickListener {
        @Override
        public void onClick(View view) {
            JNI.socketPipe();
        }
    }

    public class EventFdClickListener implements View.OnClickListener {
        @Override
        public void onClick(View view) {
            JNI.eventFd();
        }
    }

    public class EpollClickListener implements View.OnClickListener {
        @Override
        public void onClick(View view) {
            JNI.epoll();
        }
    }
}
