//
//  DataTransHubEbook.m
//  DataTransHubSample
//
//  Created by 北 on 2020/8/5.
//  Copyright © 2020 北. All rights reserved.
//

#import "DataTransHubEbook.h"
#import <DataTransHub/DataTransHub.h>

typedef NSString *LOGHUBLogDirPath NS_EXTENSIBLE_STRING_ENUM;

#define LogHubDocumentDirectory NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES).firstObject
#define LogPath(root, dir) [[LogHubDocumentDirectory stringByAppendingPathComponent:root] stringByAppendingPathComponent:dir]

LOGHUBLogDirPath const logHubAppLogRootDirPath     = @"DataHub";
LOGHUBLogDirPath const logHubAppLogCacheDirPath    = @"Cache/DataHub";
LOGHUBLogDirPath const logHubAppLogDataDirPath     = @"Data/DataHub";

@interface DataTransHubEbook()
@property (nonatomic, strong) DataTransHub *dataHub;
@end

@implementation DataTransHubEbook

static DataTransHubEbook *singleton;
+ (instancetype)shareInstance {
    static dispatch_once_t once;
    dispatch_once(&once, ^{
        singleton = [(DataTransHubEbook *)[super allocWithZone:NULL] init];
        [singleton defautValues];
    });
    return singleton;
}

- (void)defautValues {
    NSString *rootPath = LogHubDocumentDirectory;
    NSString *cachePath = LogPath(rootPath, logHubAppLogCacheDirPath);
    NSString *dataPath  = LogPath(rootPath, logHubAppLogDataDirPath);
    if (rootPath.length == 0 || cachePath.length == 0 || dataPath.length == 0) {
        return;
    }
    NSString *encryptKey = @"572d1e2710ae5fbca54c76a382fdd44050b3a675cb2bf39feebe85ef63d947aff0fa4943f1112e8b6af34bebebbaefa1a0aae055d9259b89a1858f7cc9af9df1";
    self.dataHub = [DataTransHub data_MakeDataTransHubCacheDir:cachePath
                                                       dataDir:dataPath
                                                    encryptKey:encryptKey];
    [self.dataHub data_SetReportingInterval:1000 * 10];
    //10s
    [self.dataHub data_SetRetryInterval:1000 * 10];
    //150k
    [self.dataHub data_SetBufferSize:1024 * 150];
    [self.dataHub data_SetDataFilePrefix:@"dm"];
    //15day
    [self.dataHub data_SetExpiredTime:3600 * 24 * 15];
    //300k
    [self.dataHub data_SetFileMaxSize:1024 * 300];
    //auto report
    [self.dataHub data_SetUploadTriggerWayWay:HubUploadTriggerWayAuto];
    //10 min
    [self.dataHub data_SetWriteDiskPeriod:1000 * 60 * 10];
    __weak typeof(self) weakSelf = self;
    [self.dataHub data_SetUploadBlock:^(NSString * _Nonnull filePath) {
        __strong __typeof(weakSelf) strongSelf = weakSelf;
        NSInteger min = 0;
        NSInteger max = 10;
        NSInteger num = arc4random() % max + min;
        if (num != 5) {
            [strongSelf.dataHub data_NotifyUploadSuccess:filePath];
        } else {
            [strongSelf.dataHub data_NotifyUploadFailed:filePath];
        }
    }];
    
    [self.dataHub data_Start];
}


- (void)data_pushData:(NSData *) data {
    [self.dataHub data_PushData:data];
}

- (void)data_reWeaken {
    [self.dataHub data_ReaWaken];
}

- (void)data_notifySucces:(NSString *) file {
    [self.dataHub data_NotifyUploadSuccess:file];
}

- (void)data_notifyFailed:(NSString *) file {
    [self.dataHub data_NotifyUploadFailed:file];
}

- (void)data_release {
    [self.dataHub data_Destroy];
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
