//
//  DataTransHubEbook.h
//  DataTransHubSample
//
//  Created by 北 on 2020/8/5.
//  Copyright © 2020 北. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface DataTransHubEbook : NSObject
+ (instancetype)shareInstance;

- (void)data_pushData:(NSData *) data;

- (void)data_reWeaken;

- (void)data_notifySucces:(NSString *) file;

- (void)data_notifyFailed:(NSString *) file;

- (void)data_release;
@end

NS_ASSUME_NONNULL_END
