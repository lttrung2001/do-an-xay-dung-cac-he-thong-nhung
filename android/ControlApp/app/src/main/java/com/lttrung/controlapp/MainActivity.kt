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

    private val client: Mqtt5AsyncClient by lazy {
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
        client.connect().whenComplete { connAck, t ->
            if (t != null) {
                t.printStackTrace()
            } else {
                Log.i("INFO", "CONNECTED")
                viewBinding.buttonLeft.setOnClickListener {
                    val value = 1
                    client.publishWith().topic("control").payload(ByteArray(value)).send()
                }
                viewBinding.buttonRight.setOnClickListener {
                    val value = 3
                    client.publishWith().topic("control").payload(ByteArray(value)).send()
                }
                viewBinding.buttonForward.setOnClickListener {
                    val value = 2
                    client.publishWith().topic("control").payload(ByteArray(value)).send()
                }
                viewBinding.buttonBackward.setOnClickListener {
                    val value = 4
                    client.publishWith().topic("control").payload(ByteArray(value)).send()
                }
            }
        }
    }

    companion object {
        private const val BROKER = "4a05b8e1d6604c8c8c84238e32f702e8.s2.eu.hivemq.cloud"
        private const val PORT = 8883
        private const val USERNAME = "n19dccn214"
        private const val PASSWORD = "ejURUQaYZWn2@t"
    }
}