package com.obteq.android.codec2;

import android.media.AudioFormat;
import android.media.AudioRecord;

public class Config {

	// the audio recording options
	public static final int RECORDING_RATE = 16000;
	// AudioTrack.getNativeOutputSampleRate(AudioManager.STREAM_SYSTEM);
	public static final int CHANNEL = AudioFormat.CHANNEL_IN_MONO;
	public static final int FORMAT = AudioFormat.ENCODING_PCM_16BIT;
	public static final int CODEC2 = Codec2.CODEC2_MODE_1200;

	// the minimum buffer size needed for audio recording
	public static int BUFFER_SIZE = AudioRecord.getMinBufferSize(
			RECORDING_RATE, CHANNEL, FORMAT) / 2;

}
