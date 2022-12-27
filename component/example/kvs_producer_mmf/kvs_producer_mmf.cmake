### include .cmake need if neeeded ###
include(${prj_root}/src/amazon_kvs/lib_amazon/libkvs_producer.cmake)

### add linked library ###
list(
    APPEND app_example_lib
    kvs_producer
)

### add flags ###
list(
	APPEND app_example_flags
)

### add header files ###
include(../includepath.cmake)
list (
    APPEND app_example_inc_path

    "${prj_root}/src/amazon_kvs/lib_amazon/gcc_include"
    "${prj_root}/src/amazon_kvs/lib_amazon/producer/src/include"
    "${prj_root}/src/mmfv2_video_example"
)

### add source file ###
list(
	APPEND app_example_sources

    ${sdk_root}/component/example/kvs_producer_mmf/app_example.c
    
    ${sdk_root}/component/example/kvs_producer_mmf/module_kvs_producer.c
    ${sdk_root}/component/example/kvs_producer_mmf/example_kvs_producer_mmf.c
)
