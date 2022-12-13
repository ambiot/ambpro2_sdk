### include .cmake need if neeeded ###
include(${sdk_root}/component/example/amazon_freertos/libaws_freertos_lts.cmake)

### add linked library ###
list(
    APPEND app_example_lib
    aws_freertos_lts
)

### add flags ###
list(
	APPEND app_example_flags
    ENABLE_AMAZON_COMMON
)

### add header files ###
list (
    APPEND app_example_inc_path

    "${prj_root}/src/aws_iot_freertos_lts/libraries/3rdparty/jsmn"
    "${prj_root}/src/aws_iot_freertos_lts/libraries/3rdparty/mbedtls_utils"
    "${prj_root}/src/aws_iot_freertos_lts/libraries/3rdparty/tinycbor/src"
    "${prj_root}/src/aws_iot_freertos_lts/libraries/abstractions/wifi/include"
    "${prj_root}/src/aws_iot_freertos_lts/libraries/abstractions/platform/freertos/include"
    "${prj_root}/src/aws_iot_freertos_lts/libraries/abstractions/platform/include"
    "${prj_root}/src/aws_iot_freertos_lts/libraries/abstractions/platform/include/platform"
    "${prj_root}/src/aws_iot_freertos_lts/libraries/abstractions/secure_sockets/include"
    "${prj_root}/src/aws_iot_freertos_lts/libraries/abstractions/pkcs11/corePKCS11/source/include"
    "${prj_root}/src/aws_iot_freertos_lts/libraries/abstractions/pkcs11/corePKCS11/source/dependency/3rdparty/pkcs11"
    "${prj_root}/src/aws_iot_freertos_lts/libraries/abstractions/backoff_algorithm/source/include"
    "${prj_root}/src/aws_iot_freertos_lts/libraries/abstractions/transport/secure_sockets"
    "${prj_root}/src/aws_iot_freertos_lts/libraries/abstractions/mqtt_agent/include"
    "${prj_root}/src/aws_iot_freertos_lts/libraries/freertos_plus/standard/utils/include"
    "${prj_root}/src/aws_iot_freertos_lts/libraries/freertos_plus/standard/crypto/include"
    "${prj_root}/src/aws_iot_freertos_lts/libraries/freertos_plus/standard/tls/include"
    "${prj_root}/src/aws_iot_freertos_lts/libraries/c_sdk/standard/common/include"
    "${prj_root}/src/aws_iot_freertos_lts/libraries/c_sdk/standard/common/include/private"
    "${prj_root}/src/aws_iot_freertos_lts/libraries/logging/include"
    "${prj_root}/src/aws_iot_freertos_lts/libraries/coreMQTT/source/include"
    "${prj_root}/src/aws_iot_freertos_lts/libraries/coreMQTT/source/interface"
    "${prj_root}/src/aws_iot_freertos_lts/libraries/coreMQTT-Agent/source/include"
    "${prj_root}/src/aws_iot_freertos_lts/libraries/coreHTTP/source/include"
    "${prj_root}/src/aws_iot_freertos_lts/libraries/coreHTTP/source/dependency/3rdparty/http_parser"
    "${prj_root}/src/aws_iot_freertos_lts/libraries/coreJSON/source/include"
    "${prj_root}/src/aws_iot_freertos_lts/libraries/device_shadow_for_aws/source/include"
    "${prj_root}/src/aws_iot_freertos_lts/libraries/jobs_for_aws/source/include"
    "${prj_root}/src/aws_iot_freertos_lts/libraries/device_defender_for_aws/source/include"
    "${prj_root}/src/aws_iot_freertos_lts/libraries/ota_for_aws/source/include"
    "${prj_root}/src/aws_iot_freertos_lts/libraries/ota_for_aws/source"
    "${prj_root}/src/aws_iot_freertos_lts/libraries/ota_for_aws/source/portable"
    "${prj_root}/src/aws_iot_freertos_lts/libraries/ota_for_aws/source/portable/os"
    "${prj_root}/src/aws_iot_freertos_lts/vendors/realtek/boards/amebaPro2/ports/ota"
    
    "${prj_root}/src/aws_iot_freertos_lts/vendors/realtek/boards/amebaPro2/aws_demos/config_files"
    "${prj_root}/src/aws_iot_freertos_lts/demos/include"
    "${prj_root}/src/aws_iot_freertos_lts/demos/common/pkcs11_helpers"
    "${prj_root}/src/aws_iot_freertos_lts/demos/network_manager"
    "${prj_root}/src/aws_iot_freertos_lts/demos/dev_mode_key_provisioning/include"
    "${prj_root}/src/aws_iot_freertos_lts/demos/common/http_demo_helpers"
    "${prj_root}/src/aws_iot_freertos_lts/demos/common/mqtt_demo_helpers"
    "${prj_root}/src/aws_iot_freertos_lts/demos/common/mqtt_subscription_manager"
)

### add source file ###
list(
	APPEND app_example_sources

    ${sdk_root}/component/example/amazon_freertos/app_example.c

    ${sdk_root}/component/example/amazon_freertos/example_amazon_freertos.c
    ${prj_root}/src/aws_iot_freertos_lts/vendors/realtek/boards/amebaPro2/aws_demos/application_code/aws_main.c

##amazon_freertos_LTS - demos
    #common
    ${prj_root}/src/aws_iot_freertos_lts/demos/common/http_demo_helpers/http_demo_utils.c
    ${prj_root}/src/aws_iot_freertos_lts/demos/common/mqtt_demo_helpers/mqtt_demo_helpers.c
    ${prj_root}/src/aws_iot_freertos_lts/demos/common/ota_demo_helpers/ota_application_version.c
    ${prj_root}/src/aws_iot_freertos_lts/demos/common/mqtt_subscription_manager/mqtt_subscription_manager.c
    ${prj_root}/src/aws_iot_freertos_lts/demos/common/pkcs11_helpers/pkcs11_helpers.c
    #coreHTTP
    ${prj_root}/src/aws_iot_freertos_lts/demos/coreHTTP/http_demo_mutual_auth.c
    #coreMQTT
    ${prj_root}/src/aws_iot_freertos_lts/demos/coreMQTT/mqtt_demo_mutual_auth.c
    #coreMQTT_Agent
    ${prj_root}/src/aws_iot_freertos_lts/demos/coreMQTT_Agent/mqtt_agent_task.c
    ${prj_root}/src/aws_iot_freertos_lts/demos/coreMQTT_Agent/simple_sub_pub_demo.c
    ${prj_root}/src/aws_iot_freertos_lts/demos/coreMQTT_Agent/subscription_manager.c
    #demo_runner
    # ${prj_root}/src/aws_iot_freertos_lts/demos/demo_runner/aws_demo.c
    ${prj_root}/src/aws_iot_freertos_lts/demos/demo_runner/iot_demo_freertos.c
    ${prj_root}/src/aws_iot_freertos_lts/demos/demo_runner/iot_demo_runner.c
    #dev_mode_key_provisioning
    ${prj_root}/src/aws_iot_freertos_lts/demos/dev_mode_key_provisioning/src/aws_dev_mode_key_provisioning.c
    #device_shadow_for_aws
    ${prj_root}/src/aws_iot_freertos_lts/demos/device_shadow_for_aws/shadow_demo_main.c
    #jobs_for_aws
    ${prj_root}/src/aws_iot_freertos_lts/demos/jobs_for_aws/jobs_demo.c
    #network_manager
    ${prj_root}/src/aws_iot_freertos_lts/demos/network_manager/aws_iot_network_manager.c
    #ota
    ${prj_root}/src/aws_iot_freertos_lts/demos/ota/ota_demo_core_mqtt/ota_demo_core_mqtt.c
    ${prj_root}/src/aws_iot_freertos_lts/demos/ota/ota_demo_core_http/ota_demo_core_http.c
)