package com.obteq.android.codec2;

import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.util.Log;

public class Speaker {
	private AudioTrack inSpeaker = null;
	private long con = Codec2.create(Config.CODEC2);
	private int samples = Codec2.getSamplesPerFrame(con);
	private short[] buffer = new short[samples*2];

	public Speaker() {
		try {

			inSpeaker = new AudioTrack(AudioManager.STREAM_MUSIC, 16000,
					AudioFormat.CHANNEL_OUT_MONO,
					AudioFormat.ENCODING_PCM_16BIT, Config.BUFFER_SIZE,
					AudioTrack.MODE_STREAM);
			inSpeaker.setPlaybackRate(Config.RECORDING_RATE);
			inSpeaker.play();

		} catch (Exception e) {
			Log.e("BT", e.getMessage(), e);
		}
	}	

	public void speek(char bits[]) {
		try {
			Codec2.decode(con, buffer, bits);
			inSpeaker.write(buffer, 0, buffer.length);
		} catch (Exception e) {
			Log.e("BT", e.getMessage(), e);
		}
	}

}
