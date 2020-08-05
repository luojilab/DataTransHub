//
//  ViewController.m
//  FlogSample
//
//  Created by 北 on 2020/8/5.
//  Copyright © 2020 北. All rights reserved.
//

#import "ViewController.h"
#import "LogHubInstance.h"
@interface ViewController ()

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
}

- (IBAction)pushData {
    NSString *data = @"ev=s_paid_paid_impression&uid=12005419&scr=1080*2214&t=1547627349367082203&seid=dd86a82b76722c24427b9db1fb462a4d&net=wifi&mac=c6abbef9f4bea0a0&sid=dd86a82b76722c24427b9db1fb462a4d";
    DDLogInfo(data);
}

- (IBAction)reWeaken {
    [[LogHubInstance shareInstance] log_reWeaken];
}
@end
