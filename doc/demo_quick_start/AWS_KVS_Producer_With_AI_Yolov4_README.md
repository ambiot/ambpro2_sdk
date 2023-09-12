# Edge AI (YOLOv4-tiny) + Amazon KVS Producer demo

This demo will demonstrate how to stream a 30s video to KVS cloud if a person or car is detected by edge AI

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
- mark out example_kvs_producer_with_object_detection() in `/component/example/kvs_producer_mmf/app_example.c`
    ```
    //example_kvs_producer_mmf();
	example_kvs_producer_with_object_detection();
    ```

- run following commands to build the image with option `-DEXAMPLE=kvs_producer_mmf`
    ```
    cd project/realtek_amebapro2_v0_example/GCC-RELEASE
    mkdir build
    cd build
    cmake .. -G"Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=../toolchain.cmake -DEXAMPLE=kvs_producer_mmf
    cmake --build . --target flash_nn -j4
    ```

- use image tool to download the image to AmebaPro2 and reboot

## Validation
- configure WiFi Connection  
    While runnung the example, you may need to configure WiFi connection by using these commands in uart terminal.  
    ```
    ATW0=<WiFi_SSID> : Set the WiFi AP to be connected
    ATW1=<WiFi_Password> : Set the WiFi AP password
    ATWC : Initiate the connection
    ```

- First, NN will be started to detect object
    ```
    input count 1, output count 2
    input param 0
            data_format  2
            memory_type  0
            num_of_dims  4
            quant_format 2
            quant_data  , scale=0.003922, zero_point=0
            sizes        1a0 1a0 3 1 0 0
    output param 0
            data_format  2
            memory_type  0
            num_of_dims  4
            quant_format 2
            quant_data  , scale=0.137945, zero_point=178
            sizes        d d ff 1 0 0
    output param 1
            data_format  2
            memory_type  0
            num_of_dims  4
            quant_format 2
            quant_data  , scale=0.131652, zero_point=193
            sizes        1a 1a ff 1 0 0
    ---------------------------------
    VIPNN opened
    siso_video_kvs_v1 started
    hal_voe_send2voe too long 48602 cmd 0x00000206 p1 0x00000000 p2 0x00000004
    siso_video_vipnn started
    NN OSD Draw start
    nn_rect_txt_w:16, nn_rect_txt_h:32.
    NN OSD Draw start 0
    font resize new size: 4768.
    font resize new size: 3688.
    font resize from 32 64 to 16 32.
    font resize from 64 64 to 32 32.
    font resize:21.
    YOLOv4t tick[0] = 56
    YOLOv4t tick[0] = 55
    YOLOv4t tick[0] = 55
    YOLOv4t tick[0] = 54
    ```

- Second, please use console command to register the desired object class  
  for example, if user want to detect person or car, run the command:
    ```
    ATKVS=person,car
    ```

- Third, once a person or a car be detected, device will start the video encoder and stream a 30 second video to KVS cloud storage
    ```
    Info: Fragment received, timecode:1672230101400
    Info: Fragment buffering, timecode:1672230102400
    YOLOv4t tick[0] = 63
    YOLOv4t FPS = 12.55, 7728 615706
    YOLOv4t tick[0] = 60
    YOLOv4t tick[0] = 65
    Info: Fragment persisted, timecode:1672230101400
    YOLOv4t tick[0] = 66
    Info: Fragment received, timecode:1672230102400
    Info: Fragment buffering, timecode:1672230103401
    Info: Fragment persisted, timecode:1672230102400
    YOLOv4t tick[0] = 62
    Info: Fragment received, timecode:1672230103401
    YOLOv4t tick[0] = 60
    YOLOv4t tick[0] = 59
    ```

## Check result on KVS
- we can use KVS Test Page to test the result  
https://aws-samples.github.io/amazon-kinesis-video-streams-media-viewer/  
