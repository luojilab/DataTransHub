//
//  DDLogHub.h
//  data-trans-hub
//
//  Created by 北 on 2020/4/14.
//

#import <Foundation/Foundation.h>
#import "LogHubLever.h"
NS_ASSUME_NONNULL_BEGIN

@interface LogInfo : NSObject
@property (nonatomic, assign) LogHubLever lever;
@property (nonatomic, copy) NSString *tag;
@property (nonatomic, copy) NSString *filename;
@property (nonatomic, copy) NSString *funcname;
@property (nonatomic, assign) NSInteger line;
@property (nonatomic, assign) NSInteger pid;
@property (nonatomic, assign) NSInteger tid;
@property (nonatomic, assign) NSInteger maintid;
@end


@interface LogHub : NSObject
#pragma mark - logHub Constructor & destruct
+ (instancetype)log_MakeLogCacheDir:(NSString *) cacheDir
                            dataDir:(NSString *) dataDir
                         encryptKey:(NSString *) encryptKey;

- (void)log_Destroy;

#pragma mark - logHub callBacks
- (void)log_SetUploadBlock:(void (^)(NSString *filePath)) callBack;

#pragma mark - logHub config
- (void)log_SetDataFilePrefix:(NSString *) filePrefix;

- (void)log_SetFileMaxSize:(int64_t) fileSize;

- (void)log_SetBufferSize:(int64_t) bufferSize;

- (void)log_SetExpiredTime:(NSTimeInterval) expiredTime;

- (void)log_SetReportingInterval:(NSTimeInterval) reportingInterval;

- (void)log_SetUploadTriggerWayWay:(HubUploadTriggerWay) uploadTriggerWay;

- (void)log_SetRetryInterval:(NSTimeInterval) retryInterval;

#pragma mark - logHub operat
- (void)log_Start;

- (void)log_ReaWaken;

- (void)log_ManualTriggerUploadCompletionHandler:(dispatch_block_t) completionHandler;

- (void)log_NotifyUploadSuccess:(NSString *) filePath;

- (void)log_NotifyUploadFailed:(NSString *) filePath;

#pragma mark - logHub write data to f_log
- (void)log_WriteLogData:(NSString *) data;

- (void)log_WriteLogData:(LogInfo *) info logMsg:(NSString *) msg;
@end

NS_ASSUME_NONNULL_END
