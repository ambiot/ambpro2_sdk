list (
    APPEND inc_path_re

    "${prj_root}/inc"
    "${sdk_root}/component/mbed/hal"
    "${sdk_root}/component/mbed/hal_ext"
    "${sdk_root}/component/mbed/targets/hal/rtl8735b"
	"${sdk_root}/component/mbed/api"
    "${sdk_root}/component/stdlib"
    "${sdk_root}/component/at_cmd"
    "${sdk_root}/component/network"
    "${sdk_root}/component/network/cJSON"
    "${sdk_root}/component/soc/8735b/cmsis/cmsis-core/include"
    "${sdk_root}/component/soc/8735b/cmsis/rtl8735b/lib/include"
    "${sdk_root}/component/soc/8735b/cmsis/rtl8735b/include"
    
    "${sdk_root}/component/soc/8735b/fwlib/rtl8735b/include"
    "${sdk_root}/component/soc/8735b/fwlib/rtl8735b/source/ram_ns/halmac/halmac_88xx"
    "${sdk_root}/component/soc/8735b/fwlib/rtl8735b/lib/source/ram/usb_otg/host/storage/inc/quirks"
    "${sdk_root}/component/soc/8735b/fwlib/rtl8735b/lib/source/ram/usb_otg"
    "${sdk_root}/component/soc/8735b/fwlib/rtl8735b/lib/source/ram/usb_otg/device/class/ethernet/inc"
    "${sdk_root}/component/soc/8735b/fwlib/rtl8735b/source/ram_ns/halmac/halmac_88xx/halmac_8822b"
    "${sdk_root}/component/soc/8735b/fwlib/rtl8735b/lib/include"
    "${sdk_root}/component/soc/8735b/fwlib/rtl8735b/lib/source/ram/usb_otg/device/class/ethernet/src"
    "${sdk_root}/component/soc/8735b/fwlib/rtl8735b/lib/source/ram/usb_otg/device/core/inc"
    "${sdk_root}/component/soc/8735b/fwlib/rtl8735b/source/ram_ns/halmac/halmac_88xx/halmac_8735b"
    "${sdk_root}/component/soc/8735b/fwlib/rtl8735b/source/ram_ns/halmac/halmac_88xx_v1"
    "${sdk_root}/component/soc/8735b/fwlib/rtl8735b/lib/source/ram/usb_otg/device/class/vendor/inc"
    "${sdk_root}/component/soc/8735b/fwlib/rtl8735b/lib/source/ram/usb_otg/host/storage/inc"
    "${sdk_root}/component/soc/8735b/fwlib/rtl8735b/lib/source/ram/usb_otg/host/storage/inc/scatterlist"
    "${sdk_root}/component/soc/8735b/fwlib/rtl8735b/source/ram_ns/halmac/halmac_88xx/halmac_8821c"
    "${sdk_root}/component/soc/8735b/fwlib/rtl8735b/lib/source/ram/usb_otg/host/vendor_spec"
    "${sdk_root}/component/soc/8735b/fwlib/rtl8735b/source/ram_ns/halmac"
    "${sdk_root}/component/soc/8735b/fwlib/rtl8735b/source/ram_ns/halmac/halmac_88xx_v1/halmac_8814b"
    "${sdk_root}/component/soc/8735b/fwlib/rtl8735b/lib/source/ram/usb_otg/host/storage/inc/scsi"
    "${sdk_root}/component/soc/8735b/fwlib/rtl8735b/lib/source/ram/usb_otg/inc"
    "${sdk_root}/component/soc/8735b/fwlib/rtl8735b/source/ram_ns/halmac/halmac_88xx/halmac_8195b"
	"${sdk_root}/component/soc/8735b/fwlib/rtl8735b/lib/source/ram/usb_otg/device"
    
    "${sdk_root}/component/soc/8735b/misc/utilities/include"
    
    "${sdk_root}/component/soc/8735b/app/stdio_port"
    "${sdk_root}/component/soc/8735b/app/xmodem/rom"
    "${sdk_root}/component/soc/8735b/app/shell"
    "${sdk_root}/component/soc/8735b/app/shell/rom_ns"
    "${sdk_root}/component/soc/8735b/app/rtl_printf/include"
    
    "${sdk_root}/component/os/os_dep/include"
    "${sdk_root}/component/os/freertos"
    "${sdk_root}/component/os/freertos/${freertos}/Source/include"
    
    "${sdk_root}/component/wifi/driver/include"
    "${sdk_root}/component/wifi/driver/src/osdep"
	"${sdk_root}/component/wifi/driver/src/phl"
    "${sdk_root}/component/wifi/driver/src/hal"
    "${sdk_root}/component/wifi/driver/src/hal/halmac"
    "${sdk_root}/component/wifi/driver/src/hci"
    "${sdk_root}/component/wifi/driver/src/hal/phydm/rtl8735b"
    "${sdk_root}/component/wifi/driver/src/hal/phydm"
    "${sdk_root}/component/wifi/wpa_supplicant/wpa_supplicant"

    "${sdk_root}/component/os/freertos/freertos_posix/lib/include/FreeRTOS_POSIX"
    "${sdk_root}/component/os/freertos/freertos_posix/lib/include"
    "${sdk_root}/component/os/freertos/freertos_posix/lib/FreeRTOS-Plus-POSIX/include/portable/realtek/rtl8735b"
    "${sdk_root}/component/os/freertos/freertos_posix/lib/FreeRTOS-Plus-POSIX/include"
    "${sdk_root}/component/os/freertos/freertos_posix/lib/include/private"
    
	"${sdk_root}/component/lwip/api"
    "${sdk_root}/component/lwip/${lwip}/src/include"
    "${sdk_root}/component/lwip/${lwip}/src/include/lwip"
    "${sdk_root}/component/lwip/${lwip}/src/include/compat/posix"
    "${sdk_root}/component/lwip/${lwip}/port/realtek"
    "${sdk_root}/component/lwip/${lwip}/port/realtek/freertos"

	"${sdk_root}/component/ssl/${mbedtls}/include"
	"${sdk_root}/component/ssl/ssl_ram_map/rom"
    

	"${sdk_root}/component/usb/usb_class/device/class"
	"${sdk_root}/component/usb/usb_class/device"
	"${sdk_root}/component/usb/usb_class/host/uvc/inc"
	"${sdk_root}/component/video/driver/common"
	"${sdk_root}/component/video/driver/RTL8735B"
	"${sdk_root}/component/media/rtp_codec"
	"${sdk_root}/component/media/samples"
	
	"${sdk_root}/component/media/mmfv2"
    
    "${sdk_root}/component/wifi/api"
    "${sdk_root}/component/wifi/wifi_config"
	"${sdk_root}/component/wifi/wifi_fast_connect"
	
	"${sdk_root}/component/sdio/sd_host/inc"
	"${sdk_root}/component/file_system/fatfs"
	"${sdk_root}/component/file_system/fatfs/r0.14"
	"${sdk_root}/component/file_system/ftl_common"
	"${sdk_root}/component/file_system/vfs"
	
	"${sdk_root}/component/file_system/littlefs"
	"${sdk_root}/component/file_system/littlefs/r2.41"
	
	"${sdk_root}/component/audio/3rdparty/faac/libfaac"
	"${sdk_root}/component/audio/3rdparty/faac/include"
	
	"${sdk_root}/component/audio/3rdparty/haac"
	"${sdk_root}/component/media/muxer"
	
	"${sdk_root}/component/media/3rdparty/fmp4/libmov/include"
	"${sdk_root}/component/media/3rdparty/fmp4/libflv/include"
	
	"${sdk_root}/component/soc/8735b/cmsis/cmsis-dsp/include"
	
	"${sdk_root}/component/application/qr_code_scanner/inc"
	"${sdk_root}/component/audio/3rdparty/speex/speex"
    "${sdk_root}/component/audio/3rdparty/AEC/AEC"
    "${sdk_root}/component/audio/3rdparty/opus-1.3.1/include"
    "${sdk_root}/component/audio/3rdparty/libopusenc-0.2.1/include"
	
	"${sdk_root}/component/soc/8735b/fwlib/rtl8735b/lib/source/ram/video"
	"${sdk_root}/component/soc/8735b/fwlib/rtl8735b/lib/source/ram/video/semihost"
	"${sdk_root}/component/soc/8735b/cmsis/voe/rom"
	   
)

get_filename_component(inc_path "${inc_path_re}" ABSOLUTE)

#[[
include_directories (${prj_root}/inc )
include_directories (${sdk_root}/component/mbed/hal )
include_directories (${sdk_root}/component/mbed/hal_ext )
include_directories (${sdk_root}/component/mbed/targets/hal/rtl8735b )
include_directories (${sdk_root}/component/stdlib )
include_directories (${sdk_root}/component/soc/8735b/cmsis/cmsis-core/include )
include_directories (${sdk_root}/component/soc/8735b/cmsis/rtl8735b/lib/include )
include_directories (${sdk_root}/component/soc/8735b/cmsis/rtl8735b/include )

include_directories (${sdk_root}/component/usb/usb_class/device/class )
include_directories (${sdk_root}/component/usb/usb_class/device )
include_directories (${sdk_root}/component/usb/usb_class/host/uvc/inc )
include_directories (${sdk_root}/component/video/v4l2/inc )
include_directories (${sdk_root}/component/video/driver/common )
include_directories (${sdk_root}/component/media/rtp_codec )
include_directories (${sdk_root}/component/media/samples )

include_directories (${sdk_root}/component/soc/8735b/fwlib/rtl8735b/include )
include_directories (${sdk_root}/component/soc/8735b/fwlib/rtl8735b/source/ram_ns/halmac/halmac_88xx )
include_directories (${sdk_root}/component/soc/8735b/fwlib/rtl8735b/lib/source/ram/usb_otg/host/storage/inc/quirks )
include_directories (${sdk_root}/component/soc/8735b/fwlib/rtl8735b/lib/source/ram/usb_otg )
include_directories (${sdk_root}/component/soc/8735b/fwlib/rtl8735b/lib/source/ram/usb_otg/device/class/ethernet/inc )
include_directories (${sdk_root}/component/soc/8735b/fwlib/rtl8735b/source/ram_ns/halmac/halmac_88xx/halmac_8822b )
include_directories (${sdk_root}/component/soc/8735b/fwlib/rtl8735b/lib/include )
include_directories (${sdk_root}/component/soc/8735b/fwlib/rtl8735b/lib/source/ram/usb_otg/device/class/ethernet/src )
include_directories (${sdk_root}/component/soc/8735b/fwlib/rtl8735b/lib/source/ram/usb_otg/device/core/inc )
include_directories (${sdk_root}/component/soc/8735b/fwlib/rtl8735b/source/ram_ns/halmac/halmac_88xx/halmac_8735b )
include_directories (${sdk_root}/component/soc/8735b/fwlib/rtl8735b/source/ram_ns/halmac/halmac_88xx_v1 )
include_directories (${sdk_root}/component/soc/8735b/fwlib/rtl8735b/lib/source/ram/usb_otg/device/class/vendor/inc )
include_directories (${sdk_root}/component/soc/8735b/fwlib/rtl8735b/lib/source/ram/usb_otg/host/storage/inc )
include_directories (${sdk_root}/component/soc/8735b/fwlib/rtl8735b/lib/source/ram/usb_otg/host/storage/inc/scatterlist )
include_directories (${sdk_root}/component/soc/8735b/fwlib/rtl8735b/source/ram_ns/halmac/halmac_88xx/halmac_8821c )
include_directories (${sdk_root}/component/soc/8735b/fwlib/rtl8735b/lib/source/ram/usb_otg/host/vendor_spec )
include_directories (${sdk_root}/component/soc/8735b/fwlib/rtl8735b/source/ram_ns/halmac )
include_directories (${sdk_root}/component/soc/8735b/fwlib/rtl8735b/source/ram_ns/halmac/halmac_88xx_v1/halmac_8814b )
include_directories (${sdk_root}/component/soc/8735b/fwlib/rtl8735b/lib/source/ram/usb_otg/host/storage/inc/scsi )
include_directories (${sdk_root}/component/soc/8735b/fwlib/rtl8735b/lib/source/ram/usb_otg/inc )
include_directories (${sdk_root}/component/soc/8735b/fwlib/rtl8735b/source/ram_ns/halmac/halmac_88xx/halmac_8195b)
include_directories (${sdk_root}/component/soc/8735b/fwlib/rtl8735b/lib/source/ram/usb_otg/device )

include_directories (${sdk_root}/component/soc/8735b/misc/utilities/include )

include_directories (${sdk_root}/component/soc/8735b/app/stdio_port )
include_directories (${sdk_root}/component/soc/8735b/app/xmodem/rom )
include_directories (${sdk_root}/component/soc/8735b/app/shell )
include_directories (${sdk_root}/component/soc/8735b/app/shell/rom_ns )
include_directories (${sdk_root}/component/soc/8735b/app/rtl_printf/include )

include_directories (${sdk_root}/component/os/os_dep/include)
include_directories (${sdk_root}/component/os/freertos )
include_directories (${sdk_root}/component/os/freertos/${freertos}/Source/include )


include_directories (${sdk_root}/component/wifi/driver/include )
include_directories (${sdk_root}/component/wifi/driver/src/osdep )
include_directories (${sdk_root}/component/wifi/driver/src/phl )
include_directories (${sdk_root}/component/wifi/driver/src/hal )
include_directories (${sdk_root}/component/wifi/driver/src/hal/halmac )
include_directories (${sdk_root}/component/wifi/driver/src/hci )
include_directories (${sdk_root}/component/wifi/driver/src/hal/phydm/rtl8735b )
include_directories (${sdk_root}/component/wifi/driver/src/hal/phydm )

include_directories (${sdk_root}/component/lwip/${lwip}/src/include )
include_directories (${sdk_root}/component/lwip/${lwip}/src/include/lwip )
include_directories (${sdk_root}/component/lwip/${lwip}/port/realtek )
include_directories (${sdk_root}/component/lwip/${lwip}/port/realtek/freertos )

include_directories (${sdk_root}/component/wifi/api )
include_directories (${sdk_root}/component/wifi/wpa_supplicant/src )

include_directories (${sdk_root}/component/audio/3rdparty/speex)
include_directories (${sdk_root}/component/audio/3rdparty/speex/speex)
include_directories (${sdk_root}/component/audio/3rdparty/AEC/AEC)
#include_directories (${sdk_root}/component/audio/3rdparty/AEC/WebrtcAEC)
#include_directories (${sdk_root}/component/audio/3rdparty/AEC/WebrtcAEC/utility)
#include_directories (${sdk_root}/component/audio/3rdparty/AEC/WebrtcAEC/include)
include_directories (${sdk_root}/component/audio/3rdparty/opus-1.3.1/include)
include_directories (${sdk_root}/component/audio/3rdparty/libopusenc-0.2.1/include)

include_directories (${sdk_root}/component/soc/8735b/fwlib/rtl8735b/lib/source/ram/video )
include_directories (${sdk_root}/component/soc/8735b/fwlib/rtl8735b/lib/source/ram/video/semihost )
include_directories (${sdk_root}/component/soc/8735b/cmsis/voe/rom )

]]