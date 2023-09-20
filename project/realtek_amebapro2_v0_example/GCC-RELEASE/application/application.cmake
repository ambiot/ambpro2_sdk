cmake_minimum_required(VERSION 3.6)

project(app)

enable_language(C CXX ASM)

if(BUILD_TZ)
set(app application.ns)
else()
set(app application.ntz)
endif()

include(../includepath.cmake)

include(./libbsp.cmake OPTIONAL)

if(BUILD_TZ)
	include(./libsoc_ns.cmake OPTIONAL)
else()
	include(./libsoc_ntz.cmake OPTIONAL)
endif()
include(./libwlan.cmake OPTIONAL)
include(./libwps.cmake OPTIONAL)
if(BUILD_TZ)
	include(./libvideo_ns.cmake OPTIONAL)
else()
	include(./libvideo_ntz.cmake OPTIONAL)
endif()

if(BUILD_LIB)
	message(STATUS "build libraries")
else()
	message(STATUS "use released libraries")
	link_directories(${prj_root}/GCC-RELEASE/application/output)
endif()

if(NOT BUILD_TZ)
ADD_LIBRARY (hal_pmc_lib STATIC IMPORTED )
SET_PROPERTY ( TARGET hal_pmc_lib PROPERTY IMPORTED_LOCATION ${sdk_root}/component/soc/8735b/fwlib/rtl8735b/lib/lib/hal_pmc.a )
endif()

#MBED
list(
    APPEND out_sources
	${sdk_root}/component/mbed/targets/hal/rtl8735b/audio_api.c
	${sdk_root}/component/mbed/targets/hal/rtl8735b/crypto_api.c
	${sdk_root}/component/mbed/targets/hal/rtl8735b/dma_api.c
	${sdk_root}/component/mbed/targets/hal/rtl8735b/flash_api.c
	${sdk_root}/component/soc/8735b/misc/driver/flash_api_ext.c
	${sdk_root}/component/mbed/targets/hal/rtl8735b/i2c_api.c
	${sdk_root}/component/mbed/targets/hal/rtl8735b/i2s_api.c
	${sdk_root}/component/mbed/targets/hal/rtl8735b/pwmout_api.c
	${sdk_root}/component/mbed/targets/hal/rtl8735b/sgpio_api.c
	${sdk_root}/component/mbed/targets/hal/rtl8735b/spi_api.c
	${sdk_root}/component/mbed/targets/hal/rtl8735b/timer_api.c
	${sdk_root}/component/soc/8735b/mbed-drivers/source/wait_api.c
	${sdk_root}/component/soc/8735b/mbed-drivers/source/us_ticker_api.c
	${sdk_root}/component/mbed/targets/hal/rtl8735b/us_ticker.c
	${sdk_root}/component/mbed/targets/hal/rtl8735b/gpio_api.c
	${sdk_root}/component/mbed/targets/hal/rtl8735b/gpio_irq_api.c
	${sdk_root}/component/mbed/targets/hal/rtl8735b/serial_api.c
	${sdk_root}/component/mbed/targets/hal/rtl8735b/wdt_api.c
	${sdk_root}/component/mbed/targets/hal/rtl8735b/rtc_api.c
	${sdk_root}/component/mbed/targets/hal/rtl8735b/analogin_api.c
	${sdk_root}/component/mbed/targets/hal/rtl8735b/pinmap_common.c
	${sdk_root}/component/mbed/targets/hal/rtl8735b/pinmap.c
	${sdk_root}/component/mbed/targets/hal/rtl8735b/ethernet_api.c
	${sdk_root}/component/mbed/targets/hal/rtl8735b/trng_api.c
	${sdk_root}/component/mbed/targets/hal/rtl8735b/power_mode_api.c
	${sdk_root}/component/mbed/targets/hal/rtl8735b/snand_api.c
	${sdk_root}/component/mbed/targets/hal/rtl8735b/sys_api.c
	${sdk_root}/component/mbed/targets/hal/rtl8735b/ethernet_api.c
)

#RTOS
list(
    APPEND out_sources
	${sdk_root}/component/os/freertos/${freertos}/Source/croutine.c
	${sdk_root}/component/os/freertos/${freertos}/Source/event_groups.c
	${sdk_root}/component/os/freertos/${freertos}/Source/list.c
	${sdk_root}/component/os/freertos/${freertos}/Source/queue.c
	${sdk_root}/component/os/freertos/${freertos}/Source/stream_buffer.c
	${sdk_root}/component/os/freertos/${freertos}/Source/tasks.c
	${sdk_root}/component/os/freertos/${freertos}/Source/timers.c
	${sdk_root}/component/os/freertos/${freertos}/Source/portable/MemMang/heap_4_2.c
	
	${sdk_root}/component/os/freertos/freertos_cb.c
	${sdk_root}/component/os/freertos/freertos_service.c
	${sdk_root}/component/os/freertos/cmsis_os.c
	
	${sdk_root}/component/os/os_dep/osdep_service.c
	${sdk_root}/component/os/os_dep/device_lock.c
	${sdk_root}/component/os/os_dep/timer_service.c
	#posix
	${sdk_root}/component/os/freertos/freertos_posix/lib/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_clock.c
	${sdk_root}/component/os/freertos/freertos_posix/lib/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_mqueue.c
	${sdk_root}/component/os/freertos/freertos_posix/lib/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_pthread.c
	${sdk_root}/component/os/freertos/freertos_posix/lib/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_pthread_barrier.c
	${sdk_root}/component/os/freertos/freertos_posix/lib/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_pthread_cond.c
	${sdk_root}/component/os/freertos/freertos_posix/lib/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_pthread_mutex.c
	${sdk_root}/component/os/freertos/freertos_posix/lib/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_sched.c
	${sdk_root}/component/os/freertos/freertos_posix/lib/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_semaphore.c
	${sdk_root}/component/os/freertos/freertos_posix/lib/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_timer.c
	${sdk_root}/component/os/freertos/freertos_posix/lib/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_unistd.c
	${sdk_root}/component/os/freertos/freertos_posix/lib/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_utils.c
)

list(
    APPEND out_sources
	#FREERTOS
	${sdk_root}/component/os/freertos/${freertos}/Source/portable/GCC/ARM_CM33_NTZ/non_secure/port.c
	${sdk_root}/component/os/freertos/${freertos}/Source/portable/GCC/ARM_CM33_NTZ/non_secure/portasm.c
)


#lwip
list(
	APPEND out_sources
	#api
	${sdk_root}/component/lwip/api/lwip_netconf.c
	#lwip - api
	${sdk_root}/component/lwip/${lwip}/src/api/api_lib.c
	${sdk_root}/component/lwip/${lwip}/src/api/api_msg.c
	${sdk_root}/component/lwip/${lwip}/src/api/err.c
	${sdk_root}/component/lwip/${lwip}/src/api/netbuf.c
	${sdk_root}/component/lwip/${lwip}/src/api/netdb.c
	${sdk_root}/component/lwip/${lwip}/src/api/netifapi.c
	${sdk_root}/component/lwip/${lwip}/src/api/sockets.c
	${sdk_root}/component/lwip/${lwip}/src/api/tcpip.c
	#lwip - core
	${sdk_root}/component/lwip/${lwip}/src/core/def.c
	${sdk_root}/component/lwip/${lwip}/src/core/dns.c
	${sdk_root}/component/lwip/${lwip}/src/core/inet_chksum.c
	${sdk_root}/component/lwip/${lwip}/src/core/init.c
	${sdk_root}/component/lwip/${lwip}/src/core/ip.c
	${sdk_root}/component/lwip/${lwip}/src/core/mem.c
	${sdk_root}/component/lwip/${lwip}/src/core/memp.c
	${sdk_root}/component/lwip/${lwip}/src/core/netif.c
	${sdk_root}/component/lwip/${lwip}/src/core/pbuf.c
	${sdk_root}/component/lwip/${lwip}/src/core/raw.c
	${sdk_root}/component/lwip/${lwip}/src/core/stats.c
	${sdk_root}/component/lwip/${lwip}/src/core/sys.c
	${sdk_root}/component/lwip/${lwip}/src/core/tcp.c
	${sdk_root}/component/lwip/${lwip}/src/core/tcp_in.c
	${sdk_root}/component/lwip/${lwip}/src/core/tcp_out.c
	${sdk_root}/component/lwip/${lwip}/src/core/timeouts.c
	${sdk_root}/component/lwip/${lwip}/src/core/udp.c
	#lwip - core - ipv4
	${sdk_root}/component/lwip/${lwip}/src/core/ipv4/autoip.c
	${sdk_root}/component/lwip/${lwip}/src/core/ipv4/dhcp.c
	${sdk_root}/component/lwip/${lwip}/src/core/ipv4/etharp.c
	${sdk_root}/component/lwip/${lwip}/src/core/ipv4/icmp.c
	${sdk_root}/component/lwip/${lwip}/src/core/ipv4/igmp.c
	${sdk_root}/component/lwip/${lwip}/src/core/ipv4/ip4.c
	${sdk_root}/component/lwip/${lwip}/src/core/ipv4/ip4_addr.c
	${sdk_root}/component/lwip/${lwip}/src/core/ipv4/ip4_frag.c
	#lwip - core - ipv6
	${sdk_root}/component/lwip/${lwip}/src/core/ipv6/dhcp6.c
	${sdk_root}/component/lwip/${lwip}/src/core/ipv6/ethip6.c
	${sdk_root}/component/lwip/${lwip}/src/core/ipv6/icmp6.c
	${sdk_root}/component/lwip/${lwip}/src/core/ipv6/inet6.c
	${sdk_root}/component/lwip/${lwip}/src/core/ipv6/ip6.c
	${sdk_root}/component/lwip/${lwip}/src/core/ipv6/ip6_addr.c
	${sdk_root}/component/lwip/${lwip}/src/core/ipv6/ip6_frag.c
	${sdk_root}/component/lwip/${lwip}/src/core/ipv6/mld6.c
	${sdk_root}/component/lwip/${lwip}/src/core/ipv6/nd6.c
	#lwip - netif
	${sdk_root}/component/lwip/${lwip}/src/netif/ethernet.c
	#lwip - port
	${sdk_root}/component/lwip/${lwip}/port/realtek/freertos/ethernetif.c
	${sdk_root}/component/wifi/driver/src/osdep/lwip_intf.c
	${sdk_root}/component/lwip/${lwip}/port/realtek/freertos/sys_arch.c
)

#ssl
if(${mbedtls} STREQUAL "mbedtls-2.16.6")
file(GLOB MBEDTLS_SRC CONFIGURE_DEPENDS ${sdk_root}/component/ssl/mbedtls-2.16.6/library/*.c)
list(
	APPEND out_sources
	${MBEDTLS_SRC}
	#ssl_ram_map
	${sdk_root}/component/ssl/ssl_ram_map/rom/rom_ssl_ram_map.c
	${sdk_root}/component/ssl/ssl_func_stubs/ssl_func_stubs.c
)
endif()

#USER
list(
    APPEND app_sources
	${prj_root}/src/main.c
)

#MISC
list(
	APPEND app_sources
	
	${sdk_root}/component/soc/8735b/misc/utilities/source/ram/libc_wrap.c
	${sdk_root}/component/soc/8735b/misc/driver/low_level_io.c
)

#VIDEO
list(
	APPEND app_sources		
	${sdk_root}/component/video/driver/RTL8735B/video_api.c
    ${sdk_root}/component/video/osd2/isp_osd_lite.c
)

#MMF_MODULE
list(
	APPEND app_sources	
	${sdk_root}/component/media/mmfv2/module_video.c
	${sdk_root}/component/media/mmfv2/module_rtsp2.c
    ${sdk_root}/component/media/mmfv2/module_vipnn.c
)

#NN MODEL
list(
	APPEND app_sources
	${prj_root}/src/test_model/model_yolo.c
)

#NN utils
list(
	APPEND app_sources
	${prj_root}/src/test_model/nn_utils/sigmoid.c
	${prj_root}/src/test_model/nn_utils/quantize.c
	${prj_root}/src/test_model/nn_utils/iou.c
	${prj_root}/src/test_model/nn_utils/nms.c
	${prj_root}/src/test_model/nn_utils/tensor.c
	${prj_root}/src/test_model/nn_utils/class_name.c
)


if(DEFINED EXAMPLE AND EXAMPLE)
    message(STATUS "EXAMPLE = ${EXAMPLE}")
    if(EXISTS ${sdk_root}/component/example/${EXAMPLE})
		if(EXISTS ${sdk_root}/component/example/${EXAMPLE}/${EXAMPLE}.cmake)
			message(STATUS "Found ${EXAMPLE} include project")
			include(${sdk_root}/component/example/${EXAMPLE}/${EXAMPLE}.cmake)
		else()
			message(WARNING "Found ${EXAMPLE} include project but ${EXAMPLE}.cmake not exist")
		endif()
    else()
        message(WARNING "${EXAMPLE} Not Found")
    endif()
    if(NOT DEBUG)
        set(EXAMPLE OFF CACHE STRING INTERNAL FORCE)
    endif()
elseif(DEFINED VIDEO_EXAMPLE AND VIDEO_EXAMPLE)
    message(STATUS "Build VIDEO_EXAMPLE project")
    include(${prj_root}/src/mmfv2_video_example/video_example_media_framework.cmake)
    if(NOT DEBUG)
        set(VIDEO_EXAMPLE OFF CACHE STRING INTERNAL FORCE)
    endif()
else()
endif()

add_library(outsrc ${out_sources})


add_executable(
	${app}
	${app_sources}
	${app_example_sources}
	$<TARGET_OBJECTS:outsrc>
)

set( bsplib bsp_ntz)
set( soclib soc_ntz)
set( videolib video_ntz)

set( ld_script ${CMAKE_CURRENT_SOURCE_DIR}/rtl8735b_ram.ld )

list(
	APPEND app_flags
	${app_example_flags}
	CONFIG_BUILD_RAM=1 
	CONFIG_PLATFORM_8735B
	CONFIG_RTL8735B_PLATFORM=1
	CONFIG_SYSTEM_TIME64=1
)

target_compile_options(${app} PRIVATE $<$<COMPILE_LANGUAGE:C>:${WARN_ERR_FLAGS}>)
target_compile_options(outsrc PRIVATE $<$<COMPILE_LANGUAGE:C>:${OUTSRC_WARN_ERR_FLAGS}>)

target_compile_definitions(${app} PRIVATE ${app_flags})
target_compile_definitions(outsrc PRIVATE ${app_flags})

# HEADER FILE PATH
list(
	APPEND app_inc_path

	"${sdk_root}/component/soc/8735b/fwlib/rtl8735b/lib/include/pub"
    "${sdk_root}/component/soc/8735b/cmsis/rtl8735b/lib/include/pub"
	
	${inc_path}
	${app_example_inc_path}
	${sdk_root}/component/os/freertos/${freertos}/Source/portable/GCC/ARM_CM33/non_secure
	${sdk_root}/component/os/freertos/${freertos}/Source/portable/GCC/ARM_CM33/secure
	${sdk_root}/component/soc/8735b/fwlib/rtl8735b/lib/source/ram/video/voe_bin
    ${sdk_root}/component/soc/8735b/fwlib/rtl8735b/lib/source/ram/video/osd
    ${sdk_root}/component/soc/8735b/misc/platform
	${sdk_root}/component/video/driver/RTL8735B
    ${sdk_root}/component/video/osd2

	${sdk_root}/component/media/mmfv2
	${sdk_root}/component/media/rtp_codec
	${sdk_root}/component/mbed/hal_ext
	${sdk_root}/component/file_system/ftl
	${sdk_root}/component/file_system/system_data
	${sdk_root}/component/file_system/fwfs

	${sdk_root}/component/wifi/wpa_supplicant/src
	${sdk_root}/component/wifi/wpa_supplicant/src
	${sdk_root}/component/wifi/driver/src/core/option
    ${sdk_root}/component/wifi/wifi_config
	
	${sdk_root}/component/ssl/ssl_ram_map/rom
	
	${sdk_root}/component/media/framework
    
    ${prj_root}/src/test_model
	${prj_root}/src
    
    ${prj_root}/src/${viplite}/sdk/inc
)

target_include_directories( ${app} PUBLIC ${app_inc_path} )
target_include_directories( outsrc PUBLIC ${app_inc_path} )

set( wlanlib wlan)

if(NOT BUILD_TZ)
target_link_libraries(
	${app}
	hal_pmc_lib
)
endif()

target_link_libraries(
	${app}
	-Wl,--start-group
	${bsplib}
	${wlanlib}
	${app_example_lib}
	wps
	${videolib}
	mmf
	nn
	lightsensor
	ispfeature
	
	${soclib}
	-Wl,--end-group
    stdc++
	m
	c
	gcc
)



if(NOT PICOLIBC)
target_link_libraries(
	${app} 
	nosys
)
endif()

target_link_options(
	${app} 
	PUBLIC
	"LINKER:SHELL:-L ${CMAKE_CURRENT_SOURCE_DIR}/../ROM/GCC"
	"LINKER:SHELL:-L ${CMAKE_CURRENT_BINARY_DIR}"
	"LINKER:SHELL:-T ${ld_script}"
	"LINKER:SHELL:-Map=${CMAKE_CURRENT_BINARY_DIR}/${app}.map"
	#"SHELL:${CMAKE_CURRENT_SOURCE_DIR}/build/import.lib"
)

if(BUILD_TZ)
target_link_options(
	${app} 
	PUBLIC
	"LINKER:SHELL:-wrap,hal_crypto_engine_init_platform"
	"LINKER:SHELL:-wrap,hal_pinmux_register"
	"LINKER:SHELL:-wrap,hal_pinmux_unregister"
	"LINKER:SHELL:-wrap,hal_otp_byte_rd_syss"
	"LINKER:SHELL:-wrap,hal_otp_byte_wr_syss"
	"LINKER:SHELL:-wrap,hal_sys_get_video_info"
	"LINKER:SHELL:-wrap,hal_sys_peripheral_en"
	"LINKER:SHELL:-wrap,hal_sys_set_clk"
	"LINKER:SHELL:-wrap,hal_sys_get_clk"
	"LINKER:SHELL:-wrap,hal_sys_lxbus_shared_en"
	"LINKER:SHELL:-wrap,bt_power_on"
	"LINKER:SHELL:-wrap,hal_pll_98p304_ctrl"
	"LINKER:SHELL:-wrap,hal_pll_45p158_ctrl"
	"LINKER:SHELL:-wrap,hal_osc4m_cal"
	"LINKER:SHELL:-wrap,hal_sdm_32k_enable"
	"LINKER:SHELL:-wrap,hal_sys_get_rom_ver"
	"LINKER:SHELL:-wrap,hal_otp_init"
	"LINKER:SHELL:-wrap,hal_otp_sb_key_get"
	"LINKER:SHELL:-wrap,hal_otp_sb_key_write"
	"LINKER:SHELL:-wrap,hal_otp_ssz_lock"
	"LINKER:SHELL:-wrap,hal_sys_spic_boot_finish"
	"LINKER:SHELL:-wrap,hal_sys_spic_ddr_ctrl"
	"LINKER:SHELL:-wrap,hal_sys_spic_phy_en"
	"LINKER:SHELL:-wrap,hal_sys_spic_set_phy_delay"
	"LINKER:SHELL:-wrap,hal_sys_spic_read_phy_delay"
	"LINKER:SHELL:-wrap,hal_sys_bt_uart_mux"
	"LINKER:SHELL:-wrap,hal_pwm_clock_init"
	"LINKER:SHELL:-wrap,hal_pwm_clk_sel"
	"LINKER:SHELL:-wrap,hal_timer_clock_init"
	"LINKER:SHELL:-wrap,hal_timer_group_sclk_sel"
	"LINKER:SHELL:-wrap,hal_sys_get_ld_fw_idx"
	"LINKER:SHELL:-wrap,hal_sys_get_boot_select"
	"LINKER:SHELL:-wrap,hal_sys_dbg_port_cfg"
	"LINKER:SHELL:-wrap,hal_otp_byte_rd_sys"
	"LINKER:SHELL:-wrap,hal_crypto_engine_init_s4ns"
	"LINKER:SHELL:-wrap,hal_sys_get_chip_id"
)
endif()

set_target_properties(${app} PROPERTIES LINK_DEPENDS ${ld_script})


add_custom_command(TARGET ${app} POST_BUILD 
	COMMAND ${CMAKE_NM} $<TARGET_FILE:${app}> | sort > ${app}.nm.map
	COMMAND ${CMAKE_OBJEDUMP} -d $<TARGET_FILE:${app}> > ${app}.asm
	COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${app}> ${app}.axf
	COMMAND ${CMAKE_OBJCOPY} -j .bluetooth_trace.text -Obinary ${app}.axf APP.trace
	COMMAND ${CMAKE_OBJCOPY} -R .bluetooth_trace.text ${app}.axf 
	COMMAND ${CMAKE_READELF} -s -W $<TARGET_FILE:${app}>  > ${app}.symbols
	
	#COMMAND [ -d output ] || mkdir output
	COMMAND ${CMAKE_COMMAND} -E remove_directory output && ${CMAKE_COMMAND} -E make_directory  output
	COMMAND ${CMAKE_COMMAND} -E copy ${app}.nm.map output
	COMMAND ${CMAKE_COMMAND} -E copy ${app}.asm output
	COMMAND ${CMAKE_COMMAND} -E copy ${app}.map output 
	COMMAND ${CMAKE_COMMAND} -E copy ${app}.axf output
	COMMAND ${CMAKE_COMMAND} -E copy APP.trace output 
	
	COMMAND ${PLAT_COPY} *.a output
)
