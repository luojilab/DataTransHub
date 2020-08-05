//
//  DDLogHub.h
//  data-trans-hub
//
//  Created by 北 on 2020/4/14.
//

#import <Foundation/Foundation.h>
#import "LogHubLever.h"
NS_ASSUME_NONNULL_BEGIN

@interface DataTransHub : NSObject
#pragma mark - logHub Constructor & destruct
+ (instancetype)data_MakeDataTransHubCacheDir:(NSString *) cacheDir
                                      dataDir:(NSString *) dataDir
                                   encryptKey:(NSString *) encryptKey;

- (void)data_Destroy;

#pragma mark - logHub callBacks
- (void)data_SetUploadBlock:(void (^)(NSString *filePath)) callBack;

#pragma mark - logHub config
- (void)data_SetDataFilePrefix:(NSString *) filePrefix;

- (void)data_SetFileMaxSize:(int64_t) fileSize;

- (void)data_SetBufferSize:(int64_t) bufferSize;

- (void)data_SetExpiredTime:(NSTimeInterval) expiredTime;

- (void)data_SetReportingInterval:(NSTimeInterval) reportingInterval;

- (void)data_SetUploadTriggerWayWay:(HubUploadTriggerWay) uploadTriggerWay;

- (void)data_SetRetryInterval:(NSTimeInterval) retryInterval;

#pragma mark - logHub operat
- (void)data_Start:(void *) f_data;

- (void)data_ReaWaken:(void *) f_data;

- (void)data_ManualTriggerUploadCompletionHandler:(dispatch_block_t) completionHandler;

- (void)data_NotifyUploadSuccess:(NSString *) filePath;

- (void)data_NotifyUploadFailed:(NSString *) filePath;

#pragma mark - DataTransHub write data to DataTransHub
- (void)data_PushData:(NSString *) data;
@end

NS_ASSUME_NONNULL_END
