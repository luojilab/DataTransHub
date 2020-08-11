//
//  LogHubInstance.m
//  FlogSample
//
//  Created by 北 on 2020/8/5.
//  Copyright © 2020 北. All rights reserved.
//

#import "LogHubInstance.h"
extern "C" //xlog xlogger_threadinfo.mm
{
    
    intmax_t xlogger_pid() {
        static intmax_t pid = getpid();
        return pid;
    }
    
    intmax_t xlogger_tid() {
        return (intmax_t)[NSThread currentThread];
    }
    
    intmax_t xlogger_maintid() {
        return (intmax_t)[NSThread mainThread];
    }
    
}

typedef NSString *LOGHUBLogDirPath NS_EXTENSIBLE_STRING_ENUM;

#define LogHubDocumentDirectory NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES).firstObject
#define LogPath(root, dir) [[LogHubDocumentDirectory stringByAppendingPathComponent:root] stringByAppendingPathComponent:dir]

LOGHUBLogDirPath const logHubAppLogRootDirPath     = @"LogHub";
LOGHUBLogDirPath const logHubAppLogCacheDirPath    = @"Cache/LogHub";
LOGHUBLogDirPath const logHubAppLogDataDirPath     = @"Data/LogHub";



@interface LogHubInstance()
@property (nonatomic, strong) LogHub *logHub;

@end

@implementation LogHubInstance
static LogHubInstance *singleton;
+ (instancetype)shareInstance {
    static dispatch_once_t once;
    dispatch_once(&once, ^{
        singleton = [(LogHubInstance *)[super allocWithZone:NULL] init];
        [singleton defautValues];
    });
    return singleton;
}

- (void)defautValues {
    NSString *rootPath = LogHubDocumentDirectory;
    NSString *cachePath = LogPath(logHubAppLogRootDirPath, logHubAppLogCacheDirPath);
    NSString *dataPath  = LogPath(logHubAppLogRootDirPath, logHubAppLogDataDirPath);
    if (rootPath.length == 0 || cachePath.length == 0 || dataPath.length == 0) {
        return;
    }
    NSLog(@"%@", rootPath);
    NSString *encryptKey = @"572d1e2710ae5fbca54c76a382fdd44050b3a675cb2bf39feebe85ef63d947aff0fa4943f1112e8b6af34bebebbaefa1a0aae055d9259b89a1858f7cc9af9df1";
    self.logHub = [LogHub log_MakeLogCacheDir:cachePath
                                      dataDir:dataPath
                                   encryptKey:encryptKey];
    [self.logHub log_SetReportingInterval:1000 * 10];
    //10s
    [self.logHub log_SetRetryInterval:1000 * 10];
    //150k
    [self.logHub log_SetBufferSize:1024 * 150];
    [self.logHub log_SetDataFilePrefix:@"dm"];
    //15day
    [self.logHub log_SetExpiredTime:3600 * 24 * 15];
    //300k
    [self.logHub log_SetFileMaxSize:1024 * 300];
    //auto report
    [self.logHub log_SetUploadTriggerWayWay:HubUploadTriggerWayAuto];
    //10 min
    [self.logHub data_SetWriteDiskPeriod:1000 * 60 * 10];
    __weak typeof(self) weakSelf = self;
    [self.logHub log_SetUploadBlock:^(NSString * _Nonnull filePath) {
        __strong __typeof(weakSelf) strongSelf = weakSelf;
        NSInteger min = 0;
        NSInteger max = 10;
        NSInteger num = arc4random() % max + min;
        if (num != 5) {
            [strongSelf.logHub log_NotifyUploadSuccess:filePath];
        } else {
            [strongSelf.logHub log_NotifyUploadFailed:filePath];
        }
    }];
    
    [self.logHub log_Start];
}

+ (void)dd_logLever:(LogHubLever) lever actionLine:(int) line actionFile:(const char *) file
           function:(const char *) function stringWithFormat:(NSString *)format, ... {
    LogHubInstance *singleton = [LogHubInstance shareInstance];
    va_list argList;
    va_start(argList, format);
    NSString *log = [[NSMutableString alloc] initWithFormat:format arguments:argList];
    va_end(argList);
    NSString *json = log;
    LogInfo *info = [LogInfo new];
    info.tag = @"log";
    info.pid = xlogger_pid();
    info.maintid = xlogger_maintid();
    info.tid = xlogger_tid();
    info.line = line;
    info.funcname = [NSString stringWithUTF8String:function];
    info.filename = [NSString stringWithUTF8String:file];
    [singleton dd_writeLogDataInfo:info msg:json];
}


- (void)dd_writeLogDataInfo:(LogInfo *) info msg:(NSString *) msg {
    [self.logHub log_WriteLogData:info logMsg:msg];
}

- (void)log_reWeaken {
    [self.logHub log_ReaWaken];
}

- (void)log_notifySucces:(NSString *) file {
    [self.logHub log_NotifyUploadSuccess:file];
}

- (void)log_notifyFailed:(NSString *) file {
    [self.logHub log_NotifyUploadFailed:file];
}

- (void)log_release {
    [self.logHub log_Destroy];
}


+ (instancetype)allocWithZone:(NSZone *)zone{
    return [self shareInstance];
}
-(instancetype)copyWithZone:(NSZone *)zone{
    return [self.class shareInstance];
}
-(instancetype)mutableCopyWithZone:(NSZone *)zone{
    return [self.class shareInstance];
}
@end
