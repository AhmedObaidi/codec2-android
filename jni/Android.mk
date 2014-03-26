LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := codec2
LOCAL_LDLIBS := -llog
LOCAL_CFLAGS := -O3 -ffast-math -DNDEBUG
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/codec2
LOCAL_SRC_FILES := codec2/codebook.c codec2/codebookd.c codec2/codebookdt.c \
    codec2/codebookge.c codec2/codebookjnd.c codec2/codebookjvm.c \
    codec2/codebookvqanssi.c codec2/codebookvq.c codec2/codec2.c \
    codec2/comp.c codec2/fdmdv.c codec2/interp.c codec2/kiss_fft.c \
    codec2/lpc.c codec2/lsp.c codec2/nlp.c codec2/pack.c codec2/phase.c \
    codec2/postfilter.c codec2/quantise.c codec2/sine.c codec2/varicode.c \
    Codec2JNI.cpp
include $(BUILD_SHARED_LIBRARY)
