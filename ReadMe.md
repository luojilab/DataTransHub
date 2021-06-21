## DataTransHub

[![name](https://img.shields.io/badge/release-1.2.5-green?style=flat)]()
[![license](https://img.shields.io/badge/license-MIT-brightgreen?style=flat)]()
[![platform](https://img.shields.io/badge/Platform-Android%20%7C%20iOS-red?style=flat)]()

DataTransHub 是基于mmap的客户端海量数据上报组件，其中包括两个组件DataTransHub和Flog
DataTransHub主要用于数据的上报，Flog主要用于log的上报。
实现客户端海量数据的高性能上报，上报过程不会block任何操作，实现高性能上报，基于腾讯Xlog的数据压缩和加密算法，实现数据的高压缩比压缩和加密存储。全新设计上报调度模块，策略调度完全由内部控制。

## 特点：
1. 跨平台，DataTransHub是跨平台实现，适用于Android和iOS，多端代码统一，逻辑统一，避免逻辑不一致性。
2. 海量数据存储和上报，采用实时压缩和加密，实现海量数据高效上报服务端。
3. 多实例实现，不同业务采用不同实例，不同上报接口，完全解决腾讯Xlog单实例的弊端。
4. 不丢数据，采用mmap实现缓冲区。即使发生应用crash或者进程被杀，数据不会丢失，下次启动会再次上报。
5. 低功耗，采用c++实现，整个上报共用一个线程，多实例多业务场景共用同一线程，当数据全部上报完成后，线程休眠。
6. 上报策略合理封装，通过参数设置，可以实现完全自主策略设置，满足不同业务需求
7. 合理设置缓冲区大小和上报文件大小，可以提高网络利用率，减少上报次数以及网络链接次数。

## 原理
数据通过调用接口进入DataTransHub，先缓存到数据缓冲区，缓冲区实现是mmap，即使发生crash数据不丢失。当缓冲区数据量达到阈值，异步线程进行数据落盘，或者设置定期数据落盘。当时间达到上报时间点后，内部调度线程会调用外部实现接口，进行数据上传，上层上传成功或失败后通知底层，底层进行相应策略调整，根据策略决定下次触发文件上传时间，到达时间点后再次触发上传。

demo 的使用请参考[这里](https://github.com/luojilab/DataTransHub/tree/master/sample)。

## 开始接入

接入 [Android](#android_cn) 或者 [iOS](#apple_cn)

### <a name="android_cn">[Android]()</a>
## aar接入
1. 在项目跟目录build.gradle中加入
    repositories {
        mavenCentral()
    }
2. 在项目build.gradle中加入
implementation 'com.luojilab.component:datatranshub:1.2.5'
3. 按照demo的调用方式接入

## 源码编译
准备条件：
1. AndroidStudio3.1及以上版本
2. android-ndk-r14b及以上版本

步骤：
1. AndroidStuido打开源码路径project/android AndroidStuido项目文件
2. 打开AndroidStudio中File菜单，Project Structure菜单
3. 在Android NDK location中配置NDK位置
4. 在AndroidStuido中的Gradle菜单中找到位于Android菜单下的datareporter，在Tasks中找到build，选择assembleRelease进行编译。
5. 生成datatranshub-release.aar 导入到项目中进行使用

## 调用方式

    /**
     * 创建DataTransHub实例
     *
     * @param cacheDir 缓存路径，必须全局唯一，不同业务不同路径
     * @param dataDir 数据存储路径，必须全局唯一，不同业务不同路径
     * @param encryptKey 缓存和数据加密的key，需要通过脚本生成
     * @return
     */
    public synchronized static DataTransHub makeDataTransHub(String cacheDir, String dataDir, String encryptKey);


​	
    /**
     * 释放上报实例
     *
     * @param dataTransHub 需要释放的实例
     * @param releaseDataTransHub 
     */
    public synchronized static void releaseDataTransHub(DataTransHub dataTransHub);
    
    /**
     * 设置数据文件上传实现方法
     *
     * @param uploadImp 文件上传的实现
     */
    public synchronized void setUploadImp(IUploadImp uploadImp);
    
    /**
     * 设置上传文件最大的尺寸，单位字节
     *
     * @param fileMaxSize    最大尺寸
     */
    public synchronized void setFileMaxSize(long fileMaxSize);
    
     /**
     * 设置缓存buffer的大小，该大小不应该大于单文件最大尺寸，单位字节
     *
     * @param bufferSize   buffer的尺寸
     */
    public synchronized void setBufferSize(long bufferSize);
    
    /**
     * 设置文件上报的有效期，单位是秒，如果设置为0则所有数据都上报，不存在过期逻辑
     *
     * @param expiredTime 有效期
     */
    public synchronized void setExpiredTime(long expiredTime);
    
    /**
     * 设置上报间隔，防止上报过度频繁，影响网络。可设置间隔上报
     *
     * @param reportingInterval 上报间隔 单位毫秒
     */
    public synchronized void setReportingInterval(long reportingInterval);


    /**
     * 设置重试间隔，一次上报失败后，重试阶梯通过该方法设置，防止上报失败后马上进行重试，通过该参数设置重试阶梯
     *
     * @param retryInterval 单位毫秒
     */
    public synchronized void setRetryInterval(long retryInterval);
    
    /**
     * 设置数据文件前缀，用于方便过滤数据文件，可不传
     *
     * @param prefix         文件前缀
     */
    public synchronized void setDataFilePrefix(String prefix);
    
    /**
     * 设置上报策略，如果设置为UploadTriggerWayWayManual，则不会自动上报，需要手动触发上报
     *
     * @param uploadTriggerWay UploadTriggerWayWayAuto 或者 UploadTriggerWayWayManual
     */
    public synchronized void setUploadTriggerWayWay(int uploadTriggerWay);
    
    /**
     * 手动触发上传方法，如果策略设置为手动，则通过这个方法进行触发
     *
     * @param callback 上传成功后的回调
     */
    public synchronized void manualTriggerUpload(IUploadCallback callback)
    
    /**
     * 设置写入周期，多久数据落盘一次，0表示不进行周期落盘，完全依赖缓冲区阈值控制
     *
     * @param period 单位毫秒
     */
    public synchronized void setWriteDiskPeriod(long period);
    
    /**
     * 实例上报开始，设置完参数后调用该方法
     *
     */
    public synchronized void start()
    
    /**
     * 唤醒方法，当持续上报失败，会触发延长重试策略，当网络转好后，可以通过该方法马上触发再次上报
     *
     */
    public synchronized void reaWaken()
    
    /**
     * 真正的上报数据方法，通过该方法传入数据，然后大量数据存储磁盘成单文件
     *
     * @param data 字节数据
     */
    public synchronized void push(byte[] data)
    
    /**
     * 通知上传成功，当上传成功后调用该方法
     *
     * @param filePath 需要上传的文件路径
     */
    public synchronized void notifyUploadSuccess(String filePath)
    
    /**
     * 通知上传失败，当上传失败后调用该方法
     *
     * @param filePath 需要上传的文件路径
     */
    public synchronized void notifyUploadFailed(String filePath)
    
    /**
     * 上传文件的真正实现接口，需要上层实现该接口
     *
     * @param filePath 需要上传的文件路径
     */
    public interface IUploadImp {
        void upload(String filePath);
    }

## 混淆配置
-keep class com.iget.datatranshub.\*\*{\*;}
-keep class com.iget.flog.\*\*{\*;}


### <a name="apple_cn">[iOS]()</a>

## 库接入
1. 拷贝release\iOS下对应架构的静态库文件导入到应用项目
2. 参照下面的调用方式接入

## 源码编译
准备条件：
1. xcode最新版本
2. cmake最新版本

步骤：
1. 使用终端进入项目中build/iOS目录
2. 编译真机版本库执行 build_all.sh 执行后生成的Framwork在libs/all目录下
3. 编译模拟器版本库执行 build_x86_64.sh 执行后生成的Framwork在libs/x86_64目录下
4. 生成调试工程执行 generate_project.sh 执行后会生成调试的xcode工程，工程文件在build文件夹下

## 调用方式

    /**
     * 创建DataTransHub实例
     *
     * @param cacheDir 缓存路径，必须全局唯一，不同业务不同路径
     * @param dataDir 数据存储路径，必须全局唯一，不同业务不同路径
     * @param encryptKey 缓存和数据加密的key，需要通过脚本生成
     * @return
     */
    (instancetype)log_MakeLogCacheDir:(NSString *) cacheDir dataDir:(NSString *) dataDir encryptKey:(NSString *) encryptKey

​	
    /**
     * 释放上报实例
     *
     */
    (void)log_Destroy
    
    /**
     * 设置数据文件上传实现方法
     *
     * @param callBack 文件上传的实现
     */
    (void)log_SetUploadBlock:(void (^)(NSString *filePath)) callBack;
    
    /**
     * 设置上传文件最大的尺寸，单位字节
     *
     * @param fileMaxSize    最大尺寸
     */
    (void)log_SetFileMaxSize:(NSUInteger) fileSize
    
     /**
     * 设置缓存buffer的大小，该大小不应该大于单文件最大尺寸，单位字节
     *
     * @param bufferSize   buffer的尺寸
     */
    (void)log_SetBufferSize:(NSUInteger) bufferSize
    
    /**
     * 设置文件上报的有效期，单位是秒，如果设置为0则所有数据都上报，不存在过期逻辑
     *
     * @param expiredTime 有效期
     */
    (void)log_SetExpiredTime:(NSTimeInterval) expiredTime
    
    /**
     * 设置上报间隔，防止上报过度频繁，影响网络。可设置间隔上报
     *
     * @param reportingInterval 上报间隔 单位毫秒
     */
    (void)log_SetReportingInterval:(NSTimeInterval) reportingInterval


    /**
     * 设置重试间隔，一次上报失败后，重试阶梯通过该方法设置，防止上报失败后马上进行重试，通过该参数设置重试阶梯
     *
     * @param retryInterval 单位毫秒
     */
    (void)log_SetRetryInterval:(NSTimeInterval) retryInterval
    
    /**
     * 设置数据文件前缀，用于方便过滤数据文件，可不传
     *
     * @param prefix         文件前缀
     */
    (void)log_SetDataFilePrefix:(NSString *) filePrefix
    
    /**
     * 设置上报策略，如果设置为UploadTriggerWayWayManual，则不会自动上报，需要手动触发上报
     *
     * @param uploadTriggerWay UploadTriggerWayWayAuto 或者 UploadTriggerWayWayManual
     */
    (void)log_SetUploadTriggerWayWay:(HubUploadTriggerWay) uploadTriggerWay
    
    /**
     * 手动触发上传方法，如果策略设置为手动，则通过这个方法进行触发
     *
     * @param completionHandler 上传成功后的回调
     */
    (void)log_ManualTriggerUploadCompletionHandler:(dispatch_block_t) completionHandler
    
    /**
     * 设置写入周期，多久数据落盘一次，0表示不进行周期落盘，完全依赖缓冲区阈值控制
     *
     * @param period 单位毫秒
     */
    (void)data_SetWriteDiskPeriod:(NSUInteger) period
    
    /**
     * 实例上报开始，设置完参数后调用该方法
     *
     */
    (void)log_Start
    
    /**
     * 唤醒方法，当持续上报失败，会触发延长重试策略，当网络转好后，可以通过该方法马上触发再次上报
     *
     */
    (void)log_ReaWaken
    
    /**
     * 真正的上报数据方法，通过该方法传入数据，然后大量数据存储磁盘成单文件
     *
     * @param data 字节数据
     */
    (void)data_PushData:(NSData *) data
    
    /**
     * 通知上传成功，当上传成功后调用该方法
     *
     * @param filePath 需要上传的文件路径
     */
    (void)log_NotifyUploadSuccess:(NSString *) filePath
    
    /**
     * 通知上传失败，当上传失败后调用该方法
     *
     * @param filePath 需要上传的文件路径
     */
    (void)log_NotifyUploadFailed:(NSString *) filePath

## Support

还有其他问题？

1. 参看 [DataTranshHub/sample](https://github.com/luojilab/DataTransHub/tree/master/sample)；
2. 阅读 [源码](https://github.com/luojilab/DataTransHub/tree/master/src)；
3. 阅读 [wiki](https://github.com/luojilab/DataTransHub/wiki)；
4. 联系我们。得到电子书技术组 wx:hlj-lixiaoyu
5. 开发成员：
        跨平台、android：李晓宇
		      iOS对接层：由振北
	     go语言解密算法：刘思远

## 致谢：
感谢腾讯团队开源的MMKV项目。本项目借鉴了部分源码。
感谢开源。

## License
DataTransHub 使用的 MIT 协议，详细请参考 [LICENSE](https://github.com/luojilab/DataTransHub/blob/master/LICENSE.md)。