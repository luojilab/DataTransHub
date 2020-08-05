//
//  LogHubInstance.h
//  FlogSample
//
//  Created by 北 on 2020/8/5.
//  Copyright © 2020 北. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <DataTransHub/LogHub.h>

#define DDLogDebug(...)     ([LogHubInstance dd_logLever:LogHubLeverDebug actionLine:__LINE__ actionFile:__FILE__ function:__func__ stringWithFormat:__VA_ARGS__])
#define DDLogInfo(...)      ([LogHubInstance dd_logLever:LogHubLeverInfo  actionLine:__LINE__ actionFile:__FILE__ function:__func__ stringWithFormat:__VA_ARGS__])
#define DDLogNetInfo(...)   ([LogHubInstance dd_logLever:LogHubLeverInfo  actionLine:__LINE__ actionFile:__FILE__ function:__func__ stringWithFormat:__VA_ARGS__])
#define DDLogError(...)     ([LogHubInstance dd_logLever:LogHubLeverError actionLine:__LINE__ actionFile:__FILE__ function:__func__ stringWithFormat:__VA_ARGS__])

NS_ASSUME_NONNULL_BEGIN

@interface LogHubInstance : NSObject
+ (instancetype)shareInstance;
+ (void)dd_logLever:(LogHubLever) lever actionLine:(int) line actionFile:(const char *) file
           function:(const char *) function stringWithFormat:(NSString *)format, ...;
- (void)log_reWeaken;
- (void)log_release;
@end

NS_ASSUME_NONNULL_END
