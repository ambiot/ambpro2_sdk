# Amazon KVS Producer demo on AmebaPro2 #

## Download the necessary source code from Github
- Go to `project/realtek_amebapro2_v0_example/src/amazon_kvs/lib_amazon`
    ```
    cd project/realtek_amebapro2_v0_example/src/amazon_kvs/lib_amazon
    ```
- Clone the following repository for KVS producer
	- amazon-kinesis-video-streams-producer-embedded-c
    ```
    git clone -b v1.0.1 --recursive https://github.com/aws-samples/amazon-kinesis-video-streams-producer-embedded-c.git producer
    ```

## Configure the example
- configure AWS key, channel name and AWS region in `component/example/kvs_producer_mmf/sample_config.h`
    ```
    /* KVS general configuration */
    #define AWS_ACCESS_KEY                  "xxxxxxxxxx"
    #define AWS_SECRET_KEY                  "xxxxxxxxxx"

    /* KVS stream configuration */
    #define KVS_STREAM_NAME                 "xxxxxxxxxx"
    #define AWS_KVS_REGION                  "us-east-1"
    ```

## Build the project
- run following commands to build the image with option `-DEXAMPLE=kvs_producer_mmf`
    ```
    cd project/realtek_amebapro2_v0_example/GCC-RELEASE
    mkdir build
    cd build
    cmake .. -G"Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=../toolchain.cmake -DEXAMPLE=kvs_producer_mmf
    cmake --build . --target flash -j4
    ```

- use image tool to download the image to AmebaPro2 and reboot

- configure WiFi Connection  
    While runnung the example, you may need to configure WiFi connection by using these commands in uart terminal.  
    ```
    ATW0=<WiFi_SSID> : Set the WiFi AP to be connected
    ATW1=<WiFi_Password> : Set the WiFi AP password
    ATWC : Initiate the connection
    ```

- if everything works fine, you should see the following log
    ```
    ...
    Interface 0 IP address : xxx.xxx.xxx.xxx
    WIFI initialized
    ...
    [H264] init encoder
    [ISP] init ISP
    ...
    PUT MEDIA endpoint: s-xxxxxxxx.kinesisvideo.us-east-1.amazonaws.com
    Try to put media
    Info: 100-continue
    Info: Fragment buffering, timecode:1620367399995
    Info: Fragment received, timecode:1620367399995
    Info: Fragment buffering, timecode:1620367401795
    Info: Fragment persisted, timecode:1620367399995
    Info: Fragment received, timecode:1620367401795
    Info: Fragment buffering, timecode:1620367403595
    ...
    ```

## Validate result
- we can use KVS Test Page to test the result  
https://aws-samples.github.io/amazon-kinesis-video-streams-media-viewer/  
