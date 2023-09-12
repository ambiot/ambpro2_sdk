# Image Download

## Enter Download Mode

Press these two buttons in following figure simultaneously to enter download mode

![download](https://github.com/ambiot/ambpro2_sdk/assets/56305789/1be15d37-3d50-480b-9816-d2d7118b568e)

## Download Image to Flash

- Copy the image `build/flash_ntz.bin (or flash_ntz.nn.bin)` to download tool folder `tools/Pro2_PG_tool_linux_v1.3.0`.
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
