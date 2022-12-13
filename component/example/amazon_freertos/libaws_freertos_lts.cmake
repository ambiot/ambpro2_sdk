cmake_minimum_required(VERSION 3.6)

project(aws_freertos_lts)

set(aws_freertos_lts aws_freertos_lts)

list(
    APPEND aws_freertos_lts_sources

##3rdparty
    #jsmn
    ${prj_root}/src/aws_iot_freertos_lts/libraries/3rdparty/jsmn/jsmn.c
    #mbedtls_utils
    ${prj_root}/src/aws_iot_freertos_lts/libraries/3rdparty/mbedtls_utils/mbedtls_error.c
    ${prj_root}/src/aws_iot_freertos_lts/libraries/3rdparty/mbedtls_utils/mbedtls_utils.c
    #tinycbor
    ${prj_root}/src/aws_iot_freertos_lts/libraries/3rdparty/tinycbor/src/cborencoder.c
    ${prj_root}/src/aws_iot_freertos_lts/libraries/3rdparty/tinycbor/src/cborencoder_close_container_checked.c
    ${prj_root}/src/aws_iot_freertos_lts/libraries/3rdparty/tinycbor/src/cborerrorstrings.c
    ${prj_root}/src/aws_iot_freertos_lts/libraries/3rdparty/tinycbor/src/cborparser.c
    ${prj_root}/src/aws_iot_freertos_lts/libraries/3rdparty/tinycbor/src/cborparser_dup_string.c
    ${prj_root}/src/aws_iot_freertos_lts/libraries/3rdparty/tinycbor/src/cborpretty.c
    ${prj_root}/src/aws_iot_freertos_lts/libraries/3rdparty/tinycbor/src/cborpretty_stdio.c
    ${prj_root}/src/aws_iot_freertos_lts/libraries/3rdparty/tinycbor/src/cborvalidation.c
##abstractions
    #backoff_algorithm
    ${prj_root}/src/aws_iot_freertos_lts/libraries/abstractions/backoff_algorithm/source/backoff_algorithm.c
    #mqtt_agent
    ${prj_root}/src/aws_iot_freertos_lts/libraries/abstractions/mqtt_agent/freertos_agent_message.c
    ${prj_root}/src/aws_iot_freertos_lts/libraries/abstractions/mqtt_agent/freertos_command_pool.c
    #pkcs11
    ${prj_root}/src/aws_iot_freertos_lts/libraries/abstractions/pkcs11/corePKCS11/source/portable/mbedtls/core_pkcs11_mbedtls.c
    ${prj_root}/src/aws_iot_freertos_lts/libraries/abstractions/pkcs11/corePKCS11/source/core_pkcs11.c
    ${prj_root}/src/aws_iot_freertos_lts/libraries/abstractions/pkcs11/corePKCS11/source/core_pki_utils.c
    #platform - freertos
    ${prj_root}/src/aws_iot_freertos_lts/libraries/abstractions/platform/freertos/iot_clock_freertos.c
    ${prj_root}/src/aws_iot_freertos_lts/libraries/abstractions/platform/freertos/iot_metrics.c
    ${prj_root}/src/aws_iot_freertos_lts/libraries/abstractions/platform/freertos/iot_network_freertos.c
    ${prj_root}/src/aws_iot_freertos_lts/libraries/abstractions/platform/freertos/iot_threads_freertos.c
    #transport - secure_sockets
    ${prj_root}/src/aws_iot_freertos_lts/libraries/abstractions/transport/secure_sockets/transport_secure_sockets.c
    ${prj_root}/src/aws_iot_freertos_lts/libraries/abstractions/secure_sockets/lwip/iot_secure_sockets.c
##c_sdk
    #standard
    #common - taskpool
    ${prj_root}/src/aws_iot_freertos_lts/libraries/c_sdk/standard/common/taskpool/iot_taskpool.c
    ${prj_root}/src/aws_iot_freertos_lts/libraries/c_sdk/standard/common/taskpool/iot_taskpool_static_memory.c
    ${prj_root}/src/aws_iot_freertos_lts/libraries/c_sdk/standard/common/iot_device_metrics.c
    ${prj_root}/src/aws_iot_freertos_lts/libraries/c_sdk/standard/common/iot_init.c
    ${prj_root}/src/aws_iot_freertos_lts/libraries/c_sdk/standard/common/iot_static_memory_common.c
##coreHTTP
    #http_parser
    ${prj_root}/src/aws_iot_freertos_lts/libraries/coreHTTP/source/dependency/3rdparty/http_parser/http_parser.c
    ${prj_root}/src/aws_iot_freertos_lts/libraries/coreHTTP/source/core_http_client.c
##coreJSON
    ${prj_root}/src/aws_iot_freertos_lts/libraries/coreJSON/source/core_json.c
##coreMQTT
    ${prj_root}/src/aws_iot_freertos_lts/libraries/coreMQTT/source/core_mqtt.c
    ${prj_root}/src/aws_iot_freertos_lts/libraries/coreMQTT/source/core_mqtt_serializer.c
    ${prj_root}/src/aws_iot_freertos_lts/libraries/coreMQTT/source/core_mqtt_state.c
##coreMQTT-Agent
    ${prj_root}/src/aws_iot_freertos_lts/libraries/coreMQTT-Agent/source/core_mqtt_agent.c
    ${prj_root}/src/aws_iot_freertos_lts/libraries/coreMQTT-Agent/source/core_mqtt_agent_command_functions.c
##device_shadow_for_aws
    ${prj_root}/src/aws_iot_freertos_lts/libraries/device_shadow_for_aws/source/shadow.c
##device_defender_for_aws
    ${prj_root}/src/aws_iot_freertos_lts/libraries/device_defender_for_aws/source/defender.c
##freertos_plus
    #standard
    #crypto
    ${prj_root}/src/aws_iot_freertos_lts/libraries/freertos_plus/standard/crypto/src/iot_crypto.c
    #tls
    ${prj_root}/src/aws_iot_freertos_lts/libraries/freertos_plus/standard/tls/src/iot_tls.c
    #utils
    ${prj_root}/src/aws_iot_freertos_lts/libraries/freertos_plus/standard/utils/src/iot_system_init.c
##jobs_for_aws
    ${prj_root}/src/aws_iot_freertos_lts/libraries/jobs_for_aws/source/jobs.c
##logging
    ${prj_root}/src/aws_iot_freertos_lts/libraries/logging/iot_logging.c
    ${prj_root}/src/aws_iot_freertos_lts/libraries/logging/iot_logging_task_dynamic_buffers.c
##ota_for_aws
    ${prj_root}/src/aws_iot_freertos_lts/libraries/ota_for_aws/source/portable/os/ota_os_freertos.c
    ${prj_root}/src/aws_iot_freertos_lts/libraries/ota_for_aws/source/ota.c
    ${prj_root}/src/aws_iot_freertos_lts/libraries/ota_for_aws/source/ota_base64.c
    ${prj_root}/src/aws_iot_freertos_lts/libraries/ota_for_aws/source/ota_cbor.c
    ${prj_root}/src/aws_iot_freertos_lts/libraries/ota_for_aws/source/ota_http.c
    ${prj_root}/src/aws_iot_freertos_lts/libraries/ota_for_aws/source/ota_interface.c
    ${prj_root}/src/aws_iot_freertos_lts/libraries/ota_for_aws/source/ota_mqtt.c
    
###vendor
    ##port
    # ${prj_root}/src/aws_iot_freertos_lts/vendors/realtek/boards/amebaPro2/ports/pkcs11/core_pkcs11_pal.c
    ${prj_root}/src/aws_iot_freertos_lts/vendors/realtek/boards/amebaPro2/ports/pkcs11/core_pkcs11_pal_ftl.c
    ${prj_root}/src/aws_iot_freertos_lts/vendors/realtek/boards/amebaPro2/ports/wifi/iot_wifi.c
    ${prj_root}/src/aws_iot_freertos_lts/vendors/realtek/boards/amebaPro2/ports/ota/ota_pal.c
    ${prj_root}/src/aws_iot_freertos_lts/vendors/realtek/boards/amebaPro2/ports/ota/aws_ota_amebapro2.c

)

add_library(
    ${aws_freertos_lts} STATIC
    ${aws_freertos_lts_sources}
)

list(
	APPEND aws_freertos_lts_flags
	CONFIG_BUILD_RAM=1 
	# CONFIG_BUILD_LIB=1 
	CONFIG_PLATFORM_8735B
	CONFIG_RTL8735B_PLATFORM=1
	CONFIG_SYSTEM_TIME64=1
	ARM_MATH_ARMV8MML

	ENABLE_AMAZON_COMMON
	AWS_OTA_STATUS_FTL=1
)

if(BUILD_TZ)
list(
	APPEND aws_freertos_lts_flags
	CONFIG_BUILD_NONSECURE=1
	ENABLE_SECCALL_PATCH
)
endif()

target_compile_definitions(${aws_freertos_lts} PRIVATE ${aws_freertos_lts_flags} )

include(../includepath.cmake)
target_include_directories(
	${aws_freertos_lts}
	PUBLIC
    
    "${sdk_root}/component/soc/8735b/fwlib/rtl8735b/lib/include/pub"
	"${sdk_root}/component/soc/8735b/cmsis/rtl8735b/lib/include/pub"

	${inc_path}
	${sdk_root}/component/os/freertos/${freertos}/Source/portable/GCC/ARM_CM33_NTZ/non_secure
    ${sdk_root}/component/soc/8735b/misc/platform  # for OTA
    ${sdk_root}/component/file_system/fwfs

    ${prj_root}/src/aws_iot_freertos_lts/libraries/3rdparty/jsmn
    ${prj_root}/src/aws_iot_freertos_lts/libraries/3rdparty/mbedtls_utils
    ${prj_root}/src/aws_iot_freertos_lts/libraries/3rdparty/tinycbor/src
    ${prj_root}/src/aws_iot_freertos_lts/libraries/abstractions/wifi/include
    ${prj_root}/src/aws_iot_freertos_lts/libraries/abstractions/platform/freertos/include
    ${prj_root}/src/aws_iot_freertos_lts/libraries/abstractions/platform/include
    ${prj_root}/src/aws_iot_freertos_lts/libraries/abstractions/secure_sockets/include
    ${prj_root}/src/aws_iot_freertos_lts/libraries/abstractions/pkcs11/corePKCS11/source/include
    ${prj_root}/src/aws_iot_freertos_lts/libraries/abstractions/pkcs11/corePKCS11/source/dependency/3rdparty/pkcs11
    ${prj_root}/src/aws_iot_freertos_lts/libraries/abstractions/backoff_algorithm/source/include
    ${prj_root}/src/aws_iot_freertos_lts/libraries/abstractions/transport/secure_sockets
    ${prj_root}/src/aws_iot_freertos_lts/libraries/abstractions/mqtt_agent/include
    ${prj_root}/src/aws_iot_freertos_lts/libraries/freertos_plus/standard/utils/include
    ${prj_root}/src/aws_iot_freertos_lts/libraries/freertos_plus/standard/crypto/include
    ${prj_root}/src/aws_iot_freertos_lts/libraries/freertos_plus/standard/tls/include
    ${prj_root}/src/aws_iot_freertos_lts/libraries/c_sdk/standard/common/include
    ${prj_root}/src/aws_iot_freertos_lts/libraries/c_sdk/standard/common/include/private
    ${prj_root}/src/aws_iot_freertos_lts/libraries/logging/include
    ${prj_root}/src/aws_iot_freertos_lts/libraries/coreMQTT/source/include
    ${prj_root}/src/aws_iot_freertos_lts/libraries/coreMQTT/source/interface
    ${prj_root}/src/aws_iot_freertos_lts/libraries/coreMQTT-Agent/source/include
    ${prj_root}/src/aws_iot_freertos_lts/libraries/coreHTTP/source/include
    ${prj_root}/src/aws_iot_freertos_lts/libraries/coreHTTP/source/dependency/3rdparty/http_parser
    ${prj_root}/src/aws_iot_freertos_lts/libraries/coreJSON/source/include
    ${prj_root}/src/aws_iot_freertos_lts/libraries/device_shadow_for_aws/source/include
    ${prj_root}/src/aws_iot_freertos_lts/libraries/jobs_for_aws/source/include
    ${prj_root}/src/aws_iot_freertos_lts/libraries/device_defender_for_aws/source/include
    ${prj_root}/src/aws_iot_freertos_lts/libraries/ota_for_aws/source/include
    ${prj_root}/src/aws_iot_freertos_lts/libraries/ota_for_aws/source
    ${prj_root}/src/aws_iot_freertos_lts/libraries/ota_for_aws/source/portable
    ${prj_root}/src/aws_iot_freertos_lts/libraries/ota_for_aws/source/portable/os

    ${prj_root}/src/aws_iot_freertos_lts/vendors/realtek/boards/amebaPro2/aws_demos/config_files
    ${prj_root}/src/aws_iot_freertos_lts/demos/include
    ${prj_root}/src/aws_iot_freertos_lts/demos/common/pkcs11_helpers
    ${prj_root}/src/aws_iot_freertos_lts/demos/network_manager
    ${prj_root}/src/aws_iot_freertos_lts/demos/dev_mode_key_provisioning/include
    ${prj_root}/src/aws_iot_freertos_lts/demos/common/http_demo_helpers
    ${prj_root}/src/aws_iot_freertos_lts/demos/common/mqtt_demo_helpers
)

