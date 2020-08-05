//
//  LogHubLogLever.h
//  DDXlog
//
//  Created by 北 on 2020/4/15.
//

#ifndef LogHubLogLever_h
#define LogHubLogLever_h
#import <Foundation/Foundation.h>

typedef NS_ENUM(NSInteger, LogHubLever) {
    LogHubLeverVerBose  = 0,
    LogHubLeverDebug,
    LogHubLeverInfo,
    LogHubLeverWarning,
    LogHubLeverError,
    LogHubLeverFatal,
    LogHubLeverNone,
};

typedef NS_ENUM(NSInteger, HubUploadTriggerWay) {
    HubUploadTriggerWayManual  = 0,
    HubUploadTriggerWayAuto,
};

#endif /* LogHubLogLever_h */
