//
//  DDLogHub.m
//  data-trans-hub
//
//  Created by 北 on 2020/4/14.
//

#import "DataTransHub.h"
#include "data_trans_hub.h"
#include <vector>

@interface DataTransHub() {
    future::DataTransHub *_dataHub;
}
@property (atomic, assign) BOOL isRelease;
@end

@implementation DataTransHub
#pragma mark - logHub Constructor & destruct
+ (instancetype)data_MakeDataTransHubCacheDir:(NSString *) cacheDir
                                dataDir:(NSString *) dataDir
                             encryptKey:(NSString *) encryptKey {
    DataTransHub *dataHub = [DataTransHub new];
    dataHub->_dataHub = future::DataTransHub::MakeDataTransHub(cacheDir.UTF8String, dataDir.UTF8String, encryptKey.UTF8String);
    return dataHub;
}

- (void)data_Destroy {
    self.isRelease = YES;
    if (nullptr == _dataHub) {
        return;
    }
    future::DataTransHub::Destroy(_dataHub);
}

#pragma mark - logHub callBacks
- (void)data_SetUploadBlock:(void (^)(NSString *filePath)) callBack {
    
    if (self.isRelease || !callBack || nullptr == _dataHub) {
        return;
    }
    _dataHub->SetUploadImp([callBack](const std::string &file) {
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
- (void)data_SetDataFilePrefix:(NSString *) filePrefix {
    if (self.isRelease || nullptr == _dataHub) {
        return;
    }
    _dataHub->SetDataFilePrefix(filePrefix.UTF8String);
}

- (void)data_SetFileMaxSize:(NSUInteger) fileSize {
    if (self.isRelease || nullptr == _dataHub) {
        return;
    }
    _dataHub->SetFileMaxSize(fileSize);
}

- (void)data_SetBufferSize:(NSUInteger) bufferSize {
    if (self.isRelease || nullptr == _dataHub) {
        return;
    }
    _dataHub->SetBufferSize(bufferSize);
}

- (void)data_SetExpiredTime:(NSTimeInterval) expiredTime {
    if (self.isRelease || nullptr == _dataHub) {
        return;
    }
    _dataHub->SetExpiredTime(expiredTime);
}

- (void)data_SetReportingInterval:(NSTimeInterval) reportingInterval {
    if (self.isRelease || nullptr == _dataHub) {
        return;
    }
    _dataHub->SetReportingInterval(reportingInterval);
}

- (void)data_SetUploadTriggerWayWay:(HubUploadTriggerWay) uploadTriggerWay {
    if (self.isRelease || nullptr == _dataHub) {
        return;
    }
    switch (uploadTriggerWay) {
        case HubUploadTriggerWayAuto:
            _dataHub->SetUploadTriggerWayWay(future::DataTransHub::UploadTriggerWay::Auto);
            break;
        default:
            _dataHub->SetUploadTriggerWayWay(future::DataTransHub::UploadTriggerWay::Manual);
            break;
    }
}

- (void)data_SetWriteDiskPeriod:(NSUInteger) period {
    if (self.isRelease || nullptr == _dataHub) {
        return;
    }
    _dataHub->SetWriteDiskPeriod(period);
}

- (void)data_SetRetryInterval:(NSTimeInterval) retryInterval {
    if (self.isRelease || nullptr == _dataHub) {
        return;
    }
    _dataHub->SetRetryInterval(retryInterval);
}

#pragma mark - logHub operat
- (void)data_Start {
    if (self.isRelease || nullptr == _dataHub) {
        return;
    }
    _dataHub->Start();
}

- (void)data_ReaWaken {
    if (self.isRelease || nullptr == _dataHub) {
        return;
    }
    _dataHub->ReaWaken();
}


- (void)data_ManualTriggerUploadCompletionHandler:(dispatch_block_t) completionHandler {
    if (!completionHandler || self.isRelease || nullptr == _dataHub) {
        return;
    }
    _dataHub->ManualTriggerUpload([completionHandler](void) {
        dispatch_async(dispatch_get_main_queue(), ^{
            completionHandler();
        });
    });
}

- (void)data_NotifyUploadSuccess:(NSString *) filePath {
    if (self.isRelease || nullptr == _dataHub) {
        return;
    }
    _dataHub->NotifyUploadSuccess(filePath.UTF8String);
}

- (void)data_NotifyUploadFailed:(NSString *) filePath {
    if (self.isRelease || nullptr == _dataHub) {
        return;
    }
    _dataHub->NotifyUploadFailed(filePath.UTF8String);
}

#pragma mark - DataTransHub write data to DataTransHub
- (void)data_PushData:(NSData *) data {
    if (self.isRelease || nullptr == _dataHub || data.length == 0) {
        return;
    }
    std::vector<unsigned char> vectorData((unsigned char*)[data bytes], (unsigned char*)[data bytes] + [data length]);
    _dataHub->Push(vectorData);
}
@end

