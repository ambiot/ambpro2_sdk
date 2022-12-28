# AmebaPro2 SDK

AIoT wireless network camera 
<a href="https://www.amebaiot.com/en/amebapro2/">
  <img src="https://img.shields.io/badge/Realtek%20IoT-AmebaPro2-blue" valign="middle" alt="test image size" height="15%" width="15%"/>
</a>  

## Clone Project

To check out this repository:  

```
git clone https://github.com/ambiot/ambpro2_sdk.git
```

## Prepare toolchain

GCC build on Linux. Prepare linux toolchain: 
```
cd <AmebaPro2_SDK>/tools
cat asdk-10.3.0-linux-newlib-build-3633-x86_64.tar.bz2.* | tar jxvf -
export PATH=$PATH:<AmebaPro2_SDK>/tools/asdk-10.3.0/linux/newlib/bin
```
If using windows, you can build the project by WSL(Windows Subsystem for Linux)

## Demo quick start

- :bulb: **Edge AI - object detection on AmebaPro2 (YOLOv4-tiny)**
  <a href="https://github.com/ambiot/ambpro2_sdk/blob/main/doc/NN_example_README.md">
    <img src="https://img.shields.io/badge/-Getting%20Started-green" valign="middle" height=25px width=120px/>
  </a>

- :bulb: **AWS-IoT - Amazon web service with FreeRTOS-LTS Libraries**
  <a href="https://github.com/ambiot/ambpro2_sdk/blob/main/doc/AmebaPro2_Amazon_FreeRTOS_Getting_Started_Guide.pdf">
    <img src="https://img.shields.io/badge/-Getting%20Started-green" valign="middle" height=25px width=120px/>
  </a>  
  - AmebaPro2 can connect to AWS IoT with the long term support libraries maintained by Amazon.  
  - [FreeRTOS demos](https://docs.aws.amazon.com/freertos/latest/userguide/freertos-next-steps.html): coreHTTP, coreMQTT, Over-the-air updates(OTA), AWS IoT Device Shadow...  

- :bulb: **Amazon Kinesis Video Streams - video cloud storage and analytics**
  <a href="https://github.com/ambiot/ambpro2_sdk/blob/main/doc/KVS_producer_example_README.md">
    <img src="https://img.shields.io/badge/-Getting%20Started-green" valign="middle" height=25px width=120px/>
  </a>  
  - [What Is Amazon Kinesis Video Streams?](https://aws.amazon.com/kinesis/video-streams)  

## Scenario integration

- :bulb: **Edge AI (YOLOv4-tiny) + KVS cloud storage**
  <a href="https://github.com/ambiot/ambpro2_sdk/blob/main/doc/KVS_producer_with_object_detection_example_README.md">
    <img src="https://img.shields.io/badge/-Getting%20Started-green" valign="middle" height=25px width=120px/>
  </a>  
  - stream a 30s video to KVS cloud if a person or car is detected by edge AI
