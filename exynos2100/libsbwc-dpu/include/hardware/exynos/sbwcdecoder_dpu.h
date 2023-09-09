#ifndef __DPU_SBWC_DECODER_H__
#define __DPU_SBWC_DECODER_H__

class DpuSbwcDecoder {
private:
    int mInitErr = 0;

    int mfbFd;
    int mIonClient = 0;

    unsigned int mFmtSrcSBWC = 0;
    unsigned int mNumSrcFd = 0;
    unsigned int mFmtDstSBWC = 0;
    unsigned int mNumDstFd = 0;

    unsigned int mStride = 0;
    unsigned int mDstStride = 0;
    unsigned int mSrcWidth = 0;
    unsigned int mSrcHeight = 0;
    unsigned int mDstWidth = 0;
    unsigned int mDstHeight = 0;
    unsigned int mColorSpace = 0;
    unsigned int mColorSpaceOut = 0;
    bool mIsProtected = false;

    unsigned int mTempNumSrcFd = 0;
    unsigned int mTempSrcWidth = 0;
    unsigned int mTempSrcHeight = 0;
    unsigned int mTempDstWidth = 0;
    unsigned int mTempDstHeight = 0;
    unsigned int mTempDstStride = 0;
    unsigned int mTempSrcStride = 0;
    int mTempBuf[2][2] = {{-1, -1}, {-1, -1}};
    bool mTempBufAlloc = false;
    unsigned int mTempBufAllocCnt = 0;
    bool mTempSrcProtected = 0;
    bool mTempDstProtected = 0;
    int mTempRetireFence = 0;
    int mTempReleaseFence[3] = {-1, -1, -1};

    bool mFrameUpdateAgain = false;
    unsigned int mFrameUpdateAgainCount = 0;
    unsigned int mFrameUpdateAgainCountMax = 0;
    bool mNeedTwoChannel = false;

    unsigned int log_level = 0;

    bool setPowerOn(void);
    bool setPowerOff(void);

    bool checkVarRestriction(unsigned int src_format, unsigned int dst_format, unsigned int colorspace,
		unsigned int src_width, unsigned int src_height,
		unsigned int dst_width, unsigned int dst_height,
		unsigned int stride, unsigned int dst_stride, unsigned int attr);

    bool setInputVariables(unsigned int src_format, unsigned int dst_format, unsigned int colorspace,
		unsigned int src_width, unsigned int src_height,
                unsigned int dst_width, unsigned int dst_height,
                unsigned int stride, unsigned int dst_stride, unsigned int attr);

    bool checkInputVar(int inBuf[], int outBuf[]);
    bool initData(struct decon_win_config_data *win_data);
    bool initWinState(struct decon_win_config_data *win_data);
    void printDeconFrame(struct decon_frame *frame);
    void setSize_WinData(struct decon_win_config_data *win_data);
    bool setSize(struct decon_win_config_data *win_data);
    bool dmabufFree(int *buffer, int buf_cnt);
    bool dmabufAlloc(int *buffer, int buf_cnt, int buf_size);
    void setBuf_WinData(struct decon_win_config_data *win_data,
		int inTmpBuf[], int outTmpBuf[]);
    bool setBuf(struct decon_win_config_data *win_data,
		int inBuf[], int outBuf[]);
    bool setFmt_Windata(struct decon_win_config_data *win_data);
    bool setFmt(struct decon_win_config_data *win_data);
    bool setDecode(struct decon_win_config_data *win_data);
    void releaseResources(void);

    bool decode(int inBuf[], int outBuf[]);

public:
    DpuSbwcDecoder();
    ~DpuSbwcDecoder();

    bool decodeSBWC (unsigned int src_format, unsigned int dst_format, unsigned int colorspace,
		unsigned int src_width, unsigned int src_height,
                unsigned int dst_width, unsigned int dst_height,
                unsigned int stride, unsigned int dst_stride, unsigned int attr,
		int inBuf[], int ouBuf[]);
    bool decodeSBWC (unsigned int src_format, unsigned int dst_format,
		unsigned int src_width, unsigned int src_height,
		unsigned int dst_width, unsigned int dst_height,
		unsigned int stride, unsigned int dst_stride, unsigned int attr,
		int inBuf[], int outBuf[]);
    void setLogLevel(unsigned int __log_level);
};

#endif
