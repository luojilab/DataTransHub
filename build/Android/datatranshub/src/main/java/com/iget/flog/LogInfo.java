package com.iget.flog;

public class LogInfo {
    public static final int LEVEL_VERBOSE = 0;
    public static final int LEVEL_DEBUG = 1;
    public static final int LEVEL_INFO = 2;
    public static final int LEVEL_WARNING = 3;
    public static final int LEVEL_ERROR = 4;
    public static final int LEVEL_FATAL = 5;
    public static final int LEVEL_NONE = 6;

    public int mLevel = 0;
    public String mTag = "";
    public String mFilename = "";
    public String mFuncname = "";
    public int mLine = 0;
    public long mPid = 0;
    public long mTid = 0;
    public long mMaintid = 0;

}
