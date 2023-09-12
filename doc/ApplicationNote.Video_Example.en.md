# Video Example

## Choose video demo

Go to `project/realtek_amebapro2_v0_example/src/mmfv2_video_example/video_example_media_framework.c`

uncomment `mmf2_video_example_v1_init()` only

```
static void example_mmf2_video_surport(void)
{
	// CH1 Video -> H264/HEVC -> RTSP
	mmf2_video_example_v1_init();

	// H264 -> RTSP (V1)
	// RGB  -> NN object detect (V4)
	//mmf2_video_example_vipnn_rtsp_init();
}
```

## Configure video parameter

Go to `project/realtek_amebapro2_v0_example/src/mmfv2_video_example/mmf2_video_example_v1_init.c` and set the video resolution, FPS, GOP, BPS...

```
...
#define V1_RESOLUTION VIDEO_HD
#define V1_FPS 30
#define V1_GOP 30
#define V1_BPS 1024*1024
```

## Build video example

- Run following commands to build the image with option `-DVIDEO_EXAMPLE=ON`
  ```
  cd <AmebaPro2_SDK>/project/realtek_amebapro2_v0_example/GCC-RELEASE
  mkdir build
  cd build
  cmake .. -G"Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=../toolchain.cmake -DVIDEO_EXAMPLE=ON
  cmake --build . --target flash -j4
  ```
  The image is located in `<AmebaPro2_SDK>/project/realtek_amebapro2_v0_example/GCC-RELEASE/build/flash_ntz.bin`

## Download and Run

- Download the image to device

- Configure WiFi Connection  
  While runnung the example, you may need to configure WiFi connection by using these commands in uart terminal.  
  ```
  ATW0=<WiFi_SSID> : Set the WiFi AP to be connected
  ATW1=<WiFi_Password> : Set the WiFi AP password
  ATWC : Initiate the connection
  ```

- Use VLC to validate the result  
  Open VLC and create a network stream with URL: rtsp://192.168.x.xx:554  
  If everything works fine, you should see the video streaming via VLC player.
