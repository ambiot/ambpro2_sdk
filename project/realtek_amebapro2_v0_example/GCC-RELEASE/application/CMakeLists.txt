cmake_minimum_required(VERSION 3.6)

project(app_tz)

# root of realtek_amebapro2_v0_example
set (prj_root "${CMAKE_CURRENT_SOURCE_DIR}/../..")
# root of SDK
set (sdk_root "${CMAKE_CURRENT_SOURCE_DIR}/../../../..")
set(app_ntz application.ntz)

message(STATUS "Build libraries ${BUILD_LIB}")
message(STATUS "Build FPGA ${BUILD_FPGA}")
message(STATUS "Build PXP ${BUILD_PXP}")

include(../config.cmake)

include(../sensor_iq.cmake)

if(BUILD_TZ)
include(./application_s.cmake)
endif()
include(./application.cmake)

if (MPCHIP)

if(NOT DEFINED POSTBUILD_FW_NTZ)
	set(POSTBUILD_FW_NTZ 	${prj_root}/GCC-RELEASE/mp/amebapro2_firmware_ntz.json)
endif()
if(NOT DEFINED POSTBUILD_FW_TZ)
	set(POSTBUILD_FW_TZ		${prj_root}/GCC-RELEASE/mp/amebapro2_firmware_tz.json)
endif()

# set elf2bin
if(NOT DEFINED ELF2BIN)
if(MPCHIP)
	if (LINUX)
	set(ELF2BIN ${prj_root}/GCC-RELEASE/mp/elf2bin.linux)
	else()
	set(ELF2BIN ${prj_root}/GCC-RELEASE/mp/elf2bin.exe)
	endif()
else()
	if (LINUX)
	set(ELF2BIN ${prj_root}/GCC-RELEASE/elf2bin.linux)
	else()
	set(ELF2BIN ${prj_root}/GCC-RELEASE/elf2bin.exe)
	endif()
endif()
endif()

if(BUILD_TZ)
	set_target_properties(${app} PROPERTIES LINK_DEPENDS ${POSTBUILD_FW_TZ})
	set(postbuild_cfg ${POSTBUILD_FW_TZ})
	set(firmware_name firmware_tz.bin)
else()
	set_target_properties(${app} PROPERTIES LINK_DEPENDS ${POSTBUILD_FW_NTZ})
	set(postbuild_cfg ${POSTBUILD_FW_NTZ})
	set(firmware_name firmware_ntz.bin)
endif()

add_dependencies(${app} fcs_isp_iq)
add_custom_command(TARGET ${app} POST_BUILD 
	COMMAND ${CMAKE_COMMAND} -E copy ${sdk_root}/component/soc/8735b/fwlib/rtl8735b/lib/source/ram/video/voe_bin/voe.bin voe.bin

	COMMAND ${CMAKE_COMMAND} -E copy ${postbuild_cfg} amebapro2_firmware.json
	COMMAND ${ELF2BIN} convert amebapro2_firmware.json FIRMWARE ${firmware_name}

	COMMAND ${CMAKE_COMMAND} -E make_directory  output
	
	COMMAND ${CMAKE_COMMAND} -E copy ${firmware_name} output
)

else()	
# NOT MP
# set elf2bin
if(NOT DEFINED ELF2BIN)
if (LINUX)
set(ELF2BIN ${prj_root}/GCC-RELEASE/elf2bin.linux)
else()
set(ELF2BIN ${prj_root}/GCC-RELEASE/elf2bin.exe)
endif()
endif()

if(BUILD_TZ)
	set(postbuild_cfg ${prj_root}/GCC-RELEASE/amebapro2_firmware.json)
	set(firmware_name firmware_tz.bin)
else()
	set(postbuild_cfg ${prj_root}/GCC-RELEASE/amebapro2_firmware_sns.json)
	set(firmware_name firmware_ntz.bin)
endif()

set_target_properties(${app} PROPERTIES LINK_DEPENDS ${ELF2BIN})
set_target_properties(${app} PROPERTIES LINK_DEPENDS ${postbuild_cfg})
set_target_properties(${app} PROPERTIES LINK_DEPENDS ${prj_root}/GCC-RELEASE/keycfg.json )
set_target_properties(${app} PROPERTIES LINK_DEPENDS ${prj_root}/GCC-RELEASE/partition.json)
set_target_properties(${app} PROPERTIES LINK_DEPENDS ${prj_root}/GCC-RELEASE/amebapro2_partition.json)

add_custom_command(TARGET ${app} POST_BUILD 

	COMMAND ${CMAKE_COMMAND} -E copy ${sdk_root}/component/soc/8735b/fwlib/rtl8735b/lib/source/ram/video/voe_bin/voe.bin voe.bin
	COMMAND ${CMAKE_COMMAND} -E copy ${prj_root}/GCC-RELEASE/keycfg.json keycfg.json
	COMMAND ${CMAKE_COMMAND} -E copy ${prj_root}/GCC-RELEASE/partition.json partition.json
	COMMAND ${CMAKE_COMMAND} -E copy ${prj_root}/GCC-RELEASE/amebapro2_partition.json amebapro2_partition.json
	COMMAND ${CMAKE_COMMAND} -E copy ${postbuild_cfg} amebapro2_firmware.json

	COMMAND ${ELF2BIN} convert amebapro2_partition.json PARTITIONTABLE
	COMMAND ${ELF2BIN} keygen keycfg.json
	COMMAND ${ELF2BIN} convert amebapro2_firmware.json FIRMWARE

	COMMAND [ -d output ] || ${CMAKE_COMMAND} -E make_directory  output
	
	COMMAND ${CMAKE_COMMAND} -E copy ${firmware_name} output
)
endif()	
