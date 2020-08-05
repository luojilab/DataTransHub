//
//  DDLogHub.m
//  data-trans-hub
//
//  Created by 北 on 2020/4/14.
//

#import "LogHub.h"
#include "f_log.h"
#include <vector>

@implementation LogInfo

@end


@interface LogHub() {
    future::F_Log *_log;
}
@property (atomic, assign) BOOL isRelease;
@end


@implementation LogHub
#pragma mark - logHub Constructor & destruct
+ (instancetype)log_MakeLogCacheDir:(NSString *) cacheDir
                      dataDir:(NSString *) dataDir
                   encryptKey:(NSString *) encryptKey {
    LogHub *logHub = [LogHub new];
    logHub->_log = future::F_Log::MakeLog(cacheDir.UTF8String, dataDir.UTF8String, encryptKey.UTF8String);
    return logHub;
}

- (void)log_Destroy {
    self.isRelease = YES;
    if (nullptr == _log) {
        return;
    }
    future::F_Log::Destroy(_log);
}

#pragma mark - logHub callBacks
- (void)log_SetUploadBlock:(void (^)(NSString *filePath)) callBack {
    if (!callBack || self.isRelease || nullptr == _log) {
        return;
    }
    _log->SetUploadImp([callBack](const std::string &file) {
        if (file.empty()) {
            callBack(@"");
            return;
        }
        NSString *filePath = [NSString stringWithUTF8String:file.c_str()];
        dispatch_async(dispatch_get_main_queue(), ^{
            callBack(filePath);
        });
    });
}

#pragma mark - logHub config
- (void)log_SetDataFilePrefix:(NSString *) filePrefix {
    if (self.isRelease || nullptr == _log) {
        return;
    }
    _log->SetDataFilePrefix(filePrefix.UTF8String);
}

- (void)log_SetFileMaxSize:(NSUInteger) fileSize {
    if (self.isRelease || nullptr == _log) {
        return;
    }
    _log->SetFileMaxSize(fileSize);
}

- (void)log_SetBufferSize:(NSUInteger) bufferSize {
    if (self.isRelease || nullptr == _log) {
        return;
    }
    _log->SetBufferSize(bufferSize);
}

- (void)log_SetExpiredTime:(NSTimeInterval) expiredTime {
    if (self.isRelease || nullptr == _log) {
        return;
    }
    _log->SetExpiredTime(expiredTime);
}

- (void)log_SetReportingInterval:(NSTimeInterval) reportingInterval {
    if (self.isRelease || nullptr == _log) {
        return;
    }
    _log->SetReportingInterval(reportingInterval);
}

- (void)log_SetUploadTriggerWayWay:(HubUploadTriggerWay) uploadTriggerWay {
    if (self.isRelease || nullptr == _log) {
        return;
    }
    switch (uploadTriggerWay) {
        case HubUploadTriggerWayAuto:
            _log->SetUploadTriggerWayWay(future::DataTransHub::UploadTriggerWay::Auto);
            break;
        default:
            _log->SetUploadTriggerWayWay(future::DataTransHub::UploadTriggerWay::Manual);
            break;
    }
}

- (void)log_SetRetryInterval:(NSTimeInterval) retryInterval {
    if (self.isRelease || nullptr == _log) {
        return;
    }
    _log->SetRetryInterval(retryInterval);
}

- (void)data_SetWriteDiskPeriod:(NSUInteger) period {
    if (self.isRelease || nullptr == _log) {
        return;
    }
    _log->SetWriteDiskPeriod(period);
}

#pragma mark - logHub operat
- (void)log_Start {
    if (self.isRelease || nullptr == _log) {
        return;
    }
    _log->Start();
}

- (void)log_ReaWaken {
    if (self.isRelease || nullptr == _log) {
        return;
    }
    _log->ReaWaken();
}


- (void)log_ManualTriggerUploadCompletionHandler:(dispatch_block_t) completionHandler {
    if (!completionHandler || self.isRelease || nullptr == _log) {
        return;
    }
    _log->ManualTriggerUpload([completionHandler](void) {
        dispatch_async(dispatch_get_main_queue(), ^{
            completionHandler();
        });
    });
}

- (void)log_NotifyUploadSuccess:(NSString *) filePath {
    if (self.isRelease || nullptr == _log) {
        return;
    }
    _log->NotifyUploadSuccess(filePath.UTF8String);
}

- (void)log_NotifyUploadFailed:(NSString *) filePath {
    if (self.isRelease || nullptr == _log) {
        return;
    }
    _log->NotifyUploadFailed(filePath.UTF8String);
}

#pragma mark - logHub write data to f_log
- (void)log_WriteLogData:(NSString *) data {
    if (self.isRelease || nullptr == _log) {
        return;
    }
    _log->WriteData([LogHub cStr2Vector:data]);
}

- (void)log_WriteLogData:(LogInfo *) info logMsg:(NSString *) msg {
    if (self.isRelease || msg.length == 0 || nullptr == _log) {
        return;
    }
    future::F_Log::Info f_logInfo = {};
    switch (info.lever) {
        case LogHubLeverVerBose:
            f_logInfo.level = future::F_Log::Priority::VERBOSE;
            break;
        case LogHubLeverDebug:
            f_logInfo.level = future::F_Log::Priority::DEBUG;
            break;
        case LogHubLeverInfo:
            f_logInfo.level = future::F_Log::Priority::INFO;
            break;
        case LogHubLeverWarning:
            f_logInfo.level = future::F_Log::Priority::WARNING;
            break;
        case LogHubLeverError:
            f_logInfo.level = future::F_Log::Priority::ERROR;
            break;
        case LogHubLeverFatal:
            f_logInfo.level = future::F_Log::Priority::FATAL;
            break;
        default:
            f_logInfo.level = future::F_Log::Priority::NONE;
            break;
    }
    f_logInfo.tag = info.tag.UTF8String;
    f_logInfo.filename = info.filename.UTF8String;
    f_logInfo.funcname = info.funcname.UTF8String;
    f_logInfo.line = info.line;
    f_logInfo.pid = info.pid;
    f_logInfo.tid = info.tid;
    f_logInfo.maintid = info.maintid;

    _log->Write(f_logInfo, [LogHub cStr2Vector:msg]);
}

+ (std::vector<unsigned char>)cStr2Vector:(NSString *) data {
    std::string msg;
    msg.append(data.UTF8String);
    std::vector<unsigned char> vectorData;
    vectorData.assign(msg.begin(), msg.end());
    return vectorData;
}
@end
