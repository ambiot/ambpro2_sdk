# AmebaPro2 SDK

### Clone Project  
To check out this repository:  

```
git clone https://github.com/ambiot/ambpro2_sdk.git
```

### Build project
- GCC build on Linux: 
  - Prepare linux toolchain
  ```
  cd <AmebaPro2_SDK>/tools
  cat asdk-10.3.0-linux-newlib-build-3633-x86_64.tar.bz2.* | tar jxvf -
  export PATH=$PATH:<AmebaPro2_SDK>/tools/asdk-10.3.0/linux/newlib/bin
  ```
  - Run following commands to build the image with option `-DVIDEO_EXAMPLE=ON`
  ```
  cd <AmebaPro2_SDK>/project/realtek_amebapro2_v0_example/GCC-RELEASE
  mkdir build
  cd build
  cmake .. -G"Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=../toolchain.cmake -DVIDEO_EXAMPLE=ON
  cmake --build . --target flash_nn -j4
  ```
  or Ninja build
  ```
  cmake .. -G"Ninja" -DCMAKE_TOOLCHAIN_FILE=../toolchain.cmake -DVIDEO_EXAMPLE=ON
  ninja flash_nn
  ```
  The image is located in `<AmebaPro2_SDK>/project/realtek_amebapro2_v0_example/GCC-RELEASE/build/flash_ntz.nn.bin`
- If using windows, you can build the project by WSL(Windows Subsystem for Linux)

### Download and Run
- Copy the image `build/flash_ntz.nn.bin` to download tool folder `tools/Pro2_PG_tool_linux_v1.3.0`.
- Use Pro2_PG_tool_linux_v1.3.0 command line tool to download image and reboot.  
  - Nor flash
    ```
    ./uartfwburn.linux -p /dev/ttyUSB? -f flash_ntz.nn.bin -b 2000000 -U
    ```
  - Nand flash
    ```
    ./uartfwburn.linux -p /dev/ttyUSB? -f flash_ntz.nn.bin -b 2000000 -n pro2
    ```
  Note: If using windows, replace `uartfwburn.linux` with `uartfwburn.exe` and replace `/dev/ttyUSB?` with `COM?`

- Configure WiFi Connection  
  While runnung the example, you may need to configure WiFi connection by using these commands in uart terminal.  
  ```
  ATW0=<WiFi_SSID> : Set the WiFi AP to be connected
  ATW1=<WiFi_Password> : Set the WiFi AP password
  ATWC : Initiate the connection
  ```

- Use VLC to validate the result  
  Open VLC and create a network stream with URL: rtsp://192.168.x.xx:554  
  If everything works fine, you should see the object detection result on VLC player.
