package com.obteq.android.codec2;

import java.util.Arrays;

import com.example.hellondk2.R;

import android.app.Activity;
import android.media.AudioRecord;
import android.media.MediaRecorder;
import android.os.Bundle;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnTouchListener;
import android.widget.Button;
import android.widget.EditText;

public class MainActivity extends Activity {
	private static String TAG = "AudioClient";
	// the button the user presses to send the audio stream to the server
	public Button sendAudioButton;

	// the audio recorder
	public AudioRecord recorder;

	// are we currently sending audio data
	private boolean currentlySendingAudio = false;
	private long time;
	private long sentBytes;
	private long maxBaud;
	private long readSamples;
	private long sentPockets = 0;

	private long con = Codec2.create(Config.CODEC2);
	private int bitsSize = Codec2.getBitsSize(con);
	private int samples = Codec2.getSamplesPerFrame(con) * 2;
	private char[] bits = new char[bitsSize];
	private short[] buffer = new short[Config.BUFFER_SIZE + samples + 2];
	private Speaker speaker = new Speaker();

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		android.os.Process
				.setThreadPriority(android.os.Process.THREAD_PRIORITY_URGENT_AUDIO);
		Log.i(TAG, "Creating the Audio Client with minimum buffer of "
				+ Config.BUFFER_SIZE + " bytes");

		final EditText text = (EditText) findViewById(R.id.serverAddress);
		// set up the button
		sendAudioButton = (Button) findViewById(R.id.btnStart);
		sendAudioButton.setOnTouchListener(new OnTouchListener() {

			@Override
			public boolean onTouch(View v, MotionEvent event) {

				switch (event.getAction()) {

				case MotionEvent.ACTION_DOWN:
					startStreamingAudio(text.getText().toString());
					break;

				case MotionEvent.ACTION_UP:
					stopStreamingAudio();
					break;
				}

				return false;
			}
		});
	}

	protected void onResume() {
		super.onResume();
		// if (speaker != null)
		// speaker.kill();
		// speaker = new Speaker();
		// speaker.start();
	}

	protected void onPause() {
		// speaker.kill();
		super.onPause();
	}

	private void startStreamingAudio(String server) {

		Log.i(TAG, "Starting the audio stream");
		currentlySendingAudio = true;
		startStreaming(server);
	}

	private void stopStreamingAudio() {
		Log.i(TAG, "Stopping the audio stream");
		currentlySendingAudio = false;
		recorder.release();
	}

	private void startStreaming(final String server) {
		Log.i(TAG, "Starting the background thread to stream the audio data");
		// Log.i("PNT", String.format("samples=%d bs=%d", samples, bitsSize));
		Thread streamThread = new Thread(new Runnable() {
			@Override
			public void run() {
				try {

					// private Speaker speaker = new Speaker();
					// --
					recorder = new AudioRecord(MediaRecorder.AudioSource.MIC,
							Config.RECORDING_RATE, Config.CHANNEL,
							Config.FORMAT, Config.BUFFER_SIZE * 2);
					recorder.startRecording();
					currentlySendingAudio = true;
					int offset = 0;
					restartStatistics();
					while (currentlySendingAudio) {
						try {
							// read the data into the buffer
							int read = recorder.read(buffer, offset,
									Config.BUFFER_SIZE);
							if (read > 0) {
								readSamples += read;
								int size = read + offset;
								for (int i = 0; i < (size / samples); i++) {
									sentPockets++;
									Codec2.encode(
											con,
											Arrays.copyOfRange(buffer, i
													* samples, (i + 1)
													* samples), bits);
									if (sentPockets % 10 != 0
											&& (sentPockets + 1) % 10 != 0) {
										speaker.speek(bits);
									} else {
										speaker.speek(bits);
										// Log.i("PNT", "Skip packet");
									}
									doStatistics();
								}
								System.arraycopy(buffer, samples
										* (size / samples), buffer, 0, size
										% samples);
								offset = size % samples;
							}
						} catch (Exception e) {
							Log.e("BT", e.getMessage(), e);
						}
					}
					Log.d(TAG, "AudioRecord finished recording");

				} catch (Exception e) {
					Log.e(TAG, e.getMessage(), e);
				}
			}
		});

		// start the thread
		streamThread.start();
	}

	private void doStatistics() {
		sentBytes += (bitsSize * 4 / 3) + 4;
		long dt = System.currentTimeMillis() - time;
		if (dt > 1000) {
			long baud = (sentBytes * 8000) / dt;
			if (baud > maxBaud)
				maxBaud = baud;
			Log.i("PNT", "sps=" + Config.RECORDING_RATE + " Real sps="
					+ ((readSamples * 1000) / dt) + " baud=" + baud + " max="
					+ maxBaud);
			sentBytes = 0;
			readSamples = 0;
			time = System.currentTimeMillis();
		}
	}

	private void restartStatistics() {
		time = System.currentTimeMillis();
		sentBytes = 0;
		maxBaud = 0;
		readSamples = 0;
		sentPockets = 0;
	}

}