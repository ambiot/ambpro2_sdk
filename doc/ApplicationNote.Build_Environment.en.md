# Build Environment  

## Prepare toolchain  

```
cd <AmebaPro2_SDK>/tools
cat asdk-10.3.0-linux-newlib-build-3633-x86_64.tar.bz2.* | tar jxvf -
export PATH=$PATH:<AmebaPro2_SDK>/tools/asdk-10.3.0/linux/newlib/bin
```

If using windows, you can build the project by WSL(Windows Subsystem for Linux)

## Building project

1. Open linux terminal and enter the project location: project/realtek_amebapro2_v0_example/GCC-RELEASE/.
2. Create folder “build” and enter “build” folder.
3. Run “cmake .. -G"Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=../toolchain.cmake” to create the makefile.
4. Run “cmake --build . --target flash” to build and generate flash binary.

Note:  
- If building successfully, you can see flash_ntz.bin in the build folder  
- If the ‘build’ folder has been used by others, you can remove ‘build’ folder first to have clean build
- If there’s some permission issues, you can do “chmod -R 777 <PATH_TO_YOUR_SDK>”  

## Log UART Settings  

For the log UART setting:
1. Connect AmebaPro2 to the computer via Micro USB as shown in following figure
2. After connecting to PC, create a console session by console tools (like tera term, MoBaxterm) with baud rate 115200
2. Press the reset button, then you can see the log from EVB

![serial](https://github.com/ambiot/ambpro2_sdk/assets/56305789/b4ad3a18-2385-4688-adc5-facc3deae931)

The default image boot up console log

![image](https://github.com/ambiot/ambpro2_sdk/assets/56305789/0444912b-d48f-4715-9877-0cb0f146ad50)