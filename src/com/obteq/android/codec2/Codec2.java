package com.obteq.android.codec2;

public class Codec2 {
	static {
		System.loadLibrary("codec2");
	}

	public final static int CODEC2_MODE_3200 = 0;
	public final static int CODEC2_MODE_2400 = 1;
	public final static int CODEC2_MODE_1400 = 2;
	public final static int CODEC2_MODE_1200 = 3;

	public native static long create(int mode);

	public native static int getSamplesPerFrame(long con);

	public native static int getBitsSize(long con);

	public native static int destroy(long con);

	public native static long encode(long con, short[] buf, char[] bits);

	public native static long decode(long con, short[] buf, char[] bits);
}
