cmake_minimum_required(VERSION 3.6)

project(kvs_producer)

set(kvs_producer kvs_producer)

list(
	APPEND kvs_producer_sources

##c-utility
#pal
	${prj_root}/src/amazon_kvs/lib_amazon/producer/libraries/3rdparty/c-utility/pal/freertos/lock.c
#src
	${prj_root}/src/amazon_kvs/lib_amazon/producer/libraries/3rdparty/c-utility/src/buffer.c
	${prj_root}/src/amazon_kvs/lib_amazon/producer/libraries/3rdparty/c-utility/src/consolelogger.c
	${prj_root}/src/amazon_kvs/lib_amazon/producer/libraries/3rdparty/c-utility/src/crt_abstractions.c
	${prj_root}/src/amazon_kvs/lib_amazon/producer/libraries/3rdparty/c-utility/src/doublylinkedlist.c
	${prj_root}/src/amazon_kvs/lib_amazon/producer/libraries/3rdparty/c-utility/src/httpheaders.c
	${prj_root}/src/amazon_kvs/lib_amazon/producer/libraries/3rdparty/c-utility/src/map.c
	${prj_root}/src/amazon_kvs/lib_amazon/producer/libraries/3rdparty/c-utility/src/strings.c
	${prj_root}/src/amazon_kvs/lib_amazon/producer/libraries/3rdparty/c-utility/src/xlogging.c
##llhttp
	${prj_root}/src/amazon_kvs/lib_amazon/producer/libraries/3rdparty/llhttp/src/api.c
	${prj_root}/src/amazon_kvs/lib_amazon/producer/libraries/3rdparty/llhttp/src/http.c
	${prj_root}/src/amazon_kvs/lib_amazon/producer/libraries/3rdparty/llhttp/src/llhttp.c
##parson
	${prj_root}/src/amazon_kvs/lib_amazon/producer/libraries/3rdparty/parson/parson.c
##producer
	${prj_root}/src/amazon_kvs/lib_amazon/producer/src/source/codec/nalu.c
	${prj_root}/src/amazon_kvs/lib_amazon/producer/src/source/codec/sps_decode.c
	${prj_root}/src/amazon_kvs/lib_amazon/producer/src/source/misc/json_helper.c
	${prj_root}/src/amazon_kvs/lib_amazon/producer/src/source/mkv/mkv_generator.c
	${prj_root}/src/amazon_kvs/lib_amazon/producer/src/source/net/http_helper.c
	${prj_root}/src/amazon_kvs/lib_amazon/producer/src/source/net/http_parser_adapter_llhttp.c
	${prj_root}/src/amazon_kvs/lib_amazon/producer/src/source/net/netio.c
	${prj_root}/src/amazon_kvs/lib_amazon/producer/src/source/os/allocator.c
	${prj_root}/src/amazon_kvs/lib_amazon/producer/src/source/restful/iot/iot_credential_provider.c
	${prj_root}/src/amazon_kvs/lib_amazon/producer/src/source/restful/kvs/restapi_kvs.c
	${prj_root}/src/amazon_kvs/lib_amazon/producer/src/source/restful/aws_signer_v4.c
	${prj_root}/src/amazon_kvs/lib_amazon/producer/src/source/stream/stream.c
##port
	${prj_root}/src/amazon_kvs/lib_amazon/producer/src/port/port_amebapro.c
)


add_library(
	${kvs_producer} STATIC
	${kvs_producer_sources}
)

list(
	APPEND producer_netio_func_rename_flag
	NetIo_create=producer_NetIo_create
	NetIo_terminate=producer_NetIo_terminate
	NetIo_connect=producer_NetIo_connect
	NetIo_connectWithX509=producer_NetIo_connectWithX509
	NetIo_disconnect=producer_NetIo_disconnect
	NetIo_send=producer_NetIo_send
	NetIo_recv=producer_NetIo_recv
	NetIo_isDataAvailable=producer_NetIo_isDataAvailable
	NetIo_setRecvTimeout=producer_NetIo_setRecvTimeout
	NetIo_setSendTimeout=producer_NetIo_setSendTimeout
)

list(
	APPEND kvs_producer_flags
	CONFIG_BUILD_RAM=1 
	CONFIG_BUILD_LIB=1 
	CONFIG_PLATFORM_8735B
	CONFIG_RTL8735B_PLATFORM=1
	CONFIG_SYSTEM_TIME64=1

	${producer_netio_func_rename_flag}
)

target_compile_definitions(${kvs_producer} PRIVATE ${kvs_producer_flags} )
target_compile_options(${kvs_producer} PRIVATE ${LIBS_WARN_ERR_FLAGS} )

target_include_directories(
	${kvs_producer}
	PUBLIC

	${inc_path}
	${sdk_root}/component/os/freertos/${freertos}/Source/portable/GCC/ARM_CM33_NTZ/non_secure

	${prj_root}/src/amazon_kvs/lib_amazon/gcc_include
	${prj_root}/src/amazon_kvs/lib_amazon/producer/src/include
	${prj_root}/src/amazon_kvs/lib_amazon/producer/src/source
	${prj_root}/src/amazon_kvs/lib_amazon/producer/libraries/3rdparty/parson
	${prj_root}/src/amazon_kvs/lib_amazon/producer/libraries/3rdparty/llhttp/include
	${prj_root}/src/amazon_kvs/lib_amazon/producer/libraries/3rdparty/c-utility/inc
	${prj_root}/src/amazon_kvs/lib_amazon/producer/libraries/3rdparty/c-utility/deps/umock-c/inc
	${prj_root}/src/amazon_kvs/lib_amazon/producer/libraries/3rdparty/c-utility/deps/azure-macro-utils-c/inc
)