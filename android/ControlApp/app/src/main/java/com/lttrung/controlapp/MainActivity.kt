package com.lttrung.controlapp

import android.annotation.SuppressLint
import android.os.Bundle
import android.util.Log
import androidx.appcompat.app.AppCompatActivity
import com.hivemq.client.internal.mqtt.datatypes.MqttUtf8StringImpl
import com.hivemq.client.internal.mqtt.message.auth.MqttSimpleAuth
import com.hivemq.client.mqtt.MqttClient
import com.hivemq.client.mqtt.mqtt5.Mqtt5AsyncClient
import com.lttrung.broadcastimageapp.databinding.ActivityMainBinding
import java.nio.ByteBuffer
import java.util.*

@SuppressLint("RestrictedApi")
class MainActivity : AppCompatActivity() {
    private val viewBinding: ActivityMainBinding by lazy {
        ActivityMainBinding.inflate(layoutInflater)
    }

    // Build a client with parameters to connect mqtt broker in the future.
    private val client: Mqtt5AsyncClient by lazy {
        // Build with server address, port, username, password.
        MqttClient.builder().identifier(UUID.randomUUID().toString()).serverHost(BROKER)
            .serverPort(PORT).useMqttVersion5().sslWithDefaultConfig().simpleAuth(
                MqttSimpleAuth(
                    MqttUtf8StringImpl.of(USERNAME),
                    ByteBuffer.wrap(PASSWORD.toByteArray(Charsets.UTF_8))
                )
            ).build().toAsync()
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(viewBinding.root)
        // Connect to mqtt broker async
        client.connect().whenComplete { connAck, t ->
            // If connect failed then print stack trace.
            if (t != null) {
                t.printStackTrace()
            }
            // Connected.
            else {
                Log.i("INFO", "CONNECTED")
                viewBinding.buttonChangeStatus.setOnClickListener {
                    client.publishWith().topic("control").payload("0".toByteArray()).send()
                }
                viewBinding.buttonLeft.setOnClickListener {
                    client.publishWith().topic("control").payload("1".toByteArray()).send()
                }
                viewBinding.buttonRight.setOnClickListener {
                    client.publishWith().topic("control").payload("3".toByteArray()).send()
                }
                viewBinding.buttonForward.setOnClickListener {
                    client.publishWith().topic("control").payload("2".toByteArray()).send()
                }
                viewBinding.buttonBackward.setOnClickListener {
                    client.publishWith().topic("control").payload("4".toByteArray()).send()
                }
            }
        }
    }

    companion object {
        // Define mqtt parameters.
        private const val BROKER = "4a05b8e1d6604c8c8c84238e32f702e8.s2.eu.hivemq.cloud"
        private const val PORT = 8883
        private const val USERNAME = "n19dccn214"
        private const val PASSWORD = "ejURUQaYZWn2@t"
    }
}