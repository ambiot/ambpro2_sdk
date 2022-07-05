cmake_minimum_required(VERSION 3.6)

project(flash_bin)

# root of realtek_amebapro2_v0_example
set (prj_root "${CMAKE_CURRENT_SOURCE_DIR}/..")
# root of SDK
set (sdk_root "${CMAKE_CURRENT_SOURCE_DIR}/../../..")

include(./config.cmake)

##############################################################
# COPY FILE 
##############################################################
file(GLOB NETWORK_BINARY
  "${NN_MODEL_PATH}/*.nb"
)

file(COPY ${NETWORK_BINARY}
     DESTINATION .)
	  
##############################################################


if (EXISTS "${prj_root}/GCC-RELEASE/bootloader/CMakeLists.txt")
	message(STATUS "build bootloader")
	add_subdirectory (bootloader)
	set(boot_image bootloader/boot.bin)
	set(fcs_param bootloader/boot_fcs.bin)
	set(boot_dep bootloader)
else()
	message(STATUS "use released boot.bin")
	set(boot_image ${prj_root}/GCC-RELEASE/bootloader/boot.bin)
	set(fcs_param ${prj_root}/GCC-RELEASE/bootloader/boot_fcs.bin)
	set(boot_dep ${ELF2BIN})
endif()

if(BUILD_TZ)
	if(BUILD_WLANMP)
		set(target flash_tz_mp)
	else()
		set(target flash_tz)
	endif()
	set(fw application/firmware_tz.bin)
	set(isp_iq application/firmware_isp_iq.bin)
	set(app_dep application.s application.ns)
	
	set(bttrace application/APP.trace)
	add_subdirectory (application)
else()
	if(BUILD_WLANMP)
		set(target flash_ntz_mp)
	else()
		set(target flash_ntz)
	endif()
	set(fw application/firmware_ntz.bin)
	set(isp_iq application/firmware_isp_iq.bin)
	set(app_dep application.ntz)
	
	set(bttrace application/APP.trace)
	add_subdirectory (application)
endif()


if(MPCHIP)
add_custom_command(
	OUTPUT key_public.json key_private.json
	
	COMMAND ${CMAKE_COMMAND} -E copy ${POSTBUILD_KEY_CFG} keycfg.json
	COMMAND ${ELF2BIN} keygen keycfg.json key
)

add_custom_target(
	keygen
	
	COMMAND ${CMAKE_COMMAND} -E copy ${POSTBUILD_KEY_CFG} keycfg.json
	COMMAND ${ELF2BIN} keygen keycfg.json key
	
	DEPENDS ${ELF2BIN}
)

add_custom_target(
	plain_img

	COMMAND ${CMAKE_COMMAND} -E copy ${POSTBUILD_CERT} certificate.json
	COMMAND ${ELF2BIN} convert certificate.json CERT_TABLE certable.bin	
	COMMAND ${ELF2BIN} convert certificate.json CERTIFICATE certificate.bin	
	
	#COMMAND cp ${POSTBUILD_KEY_CFG} keycfg.json
	#COMMAND ${ELF2BIN} keygen keycfg.json key

	COMMAND ${CMAKE_COMMAND} -E copy ${POSTBUILD_PART} amebapro2_partitiontable.json
	COMMAND ${ELF2BIN} convert amebapro2_partitiontable.json PARTITIONTABLE partition.bin
	
	COMMAND ${CMAKE_COMMAND} -E copy ${boot_image} .
	COMMAND ${CMAKE_COMMAND} -E copy ${fcs_param} .
	COMMAND ${CMAKE_COMMAND} -E copy ${fw} ./firmware.bin
	COMMAND ${CMAKE_COMMAND} -E copy ${isp_iq} ./firmware_isp_iq.bin

	COMMAND ${CMAKE_COMMAND} -E copy ${bttrace} ./APP.trace
	
	DEPENDS ${boot_dep}
	DEPENDS ${app_dep}
	DEPENDS ${POSTBUILD_CERT}
	DEPENDS ${POSTBUILD_KEY_CFG}
	DEPENDS ${POSTBUILD_PART}
	DEPENDS key_public.json
	DEPENDS key_private.json
	DEPENDS ${ELF2BIN}
)

add_custom_target(
	flash

	COMMAND ${ELF2BIN} combine amebapro2_partitiontable.json ${target}.bin PT_PT=partition.bin,CER_TBL=certable.bin,KEY_CER1=certificate.bin,PT_BL_PRI=boot.bin,PT_FW1=firmware.bin,PT_ISP_IQ=firmware_isp_iq.bin,PT_FCSDATA=boot_fcs.bin
	COMMAND ${CMAKE_COMMAND} -E copy ./firmware.bin ./ota.bin
	COMMAND ${CHKSUM} ./ota.bin
	
	COMMAND ${CMAKE_COMMAND} -E copy ./firmware_isp_iq.bin ./isp_iq_ota.bin
	COMMAND ${CHKSUM} ./isp_iq_ota.bin
	
	DEPENDS plain_img
	DEPENDS ${ELF2BIN}
)

add_custom_target(
	flash_nn
	#COMMAND ${PLAT_COPY} ${NN_MODEL_PATH}/*.nb .
	#COMMAND ${CMAKE_COMMAND} -P ${prj_root}/GCC-RELEASE/copy_nb.cmake
	COMMAND ${CMAKE_COMMAND} -E copy ${POSTBUILD_NNMDL} amebapro2_nn_model.json
	COMMAND ${CMAKE_COMMAND} -E copy ${POSTBUILD_FWFS_NN} amebapro2_fwfs_nn_models.json
	COMMAND ${ELF2BIN} convert amebapro2_fwfs_nn_models.json FWFS fwfs_nn_model.bin
	COMMAND ${ELF2BIN} convert amebapro2_nn_model.json FIRMWARE nn_model.bin
	COMMAND ${ELF2BIN} combine amebapro2_partitiontable.json ${target}.nn.bin PT_PT=partition.bin,CER_TBL=certable.bin,KEY_CER1=certificate.bin,PT_BL_PRI=boot.bin,PT_FW1=firmware.bin,PT_NN_MDL=nn_model.bin,PT_ISP_IQ=firmware_isp_iq.bin,PT_FCSDATA=boot_fcs.bin
	
	COMMAND ${CMAKE_COMMAND} -E copy ./firmware.bin ./ota.bin
	COMMAND ${CHKSUM} ./ota.bin
	
	COMMAND ${CMAKE_COMMAND} -E copy ./nn_model.bin ./nn_model_ota.bin
	COMMAND ${CHKSUM} ./nn_model_ota.bin
	
	COMMAND ${CMAKE_COMMAND} -E copy ./firmware_isp_iq.bin ./isp_iq_ota.bin
	COMMAND ${CHKSUM} ./isp_iq_ota.bin
	
	DEPENDS plain_img
	DEPENDS ${ELF2BIN}
)

	
add_custom_target(
	hash

	COMMAND ${ELF2BIN} secure sign+dbg=cert key_private.json key_public.json certificate.bin certificate_signed.bin
	COMMAND ${ELF2BIN} secure hash+dbg=ptab key_private.json key_public.json partition.bin partition_hashed.bin
	COMMAND ${ELF2BIN} secure hash+dbg=boot key_private.json key_public.json boot.bin boot_hashed.bin
	COMMAND ${ELF2BIN} secure hash+dbg=fw key_private.json key_public.json firmware.bin firmware_hashed.bin
	
	
	COMMAND ${ELF2BIN} combine amebapro2_partitiontable.json ${target}.bin PT_PT=partition_hashed.bin,CER_TBL=certable.bin,KEY_CER1=certificate_signed.bin,PT_BL_PRI=boot_hashed.bin,PT_FW1=firmware_hashed.bin,PT_ISP_IQ=firmware_isp_iq.bin,PT_FCSDATA=boot_fcs.bin
	COMMAND ${CMAKE_COMMAND} -E copy ./firmware_hashed.bin ./ota.bin
	COMMAND ${CHKSUM} ./ota.bin
	
	DEPENDS plain_img
	DEPENDS ${ELF2BIN}
)	

add_custom_target(
	sign

	COMMAND ${ELF2BIN} secure sign+dbg=cert key_private.json key_public.json certificate.bin certificate_signed.bin
	COMMAND ${ELF2BIN} secure sign+hash+dbg=ptab key_private.json key_public.json partition.bin partition_signed.bin
	COMMAND ${ELF2BIN} secure sign+hash+dbg=boot key_private.json key_public.json boot.bin boot_signed.bin
	COMMAND ${ELF2BIN} secure sign+hash+dbg=fw key_private.json key_public.json firmware.bin firmware_signed.bin
	
	
	COMMAND ${ELF2BIN} combine amebapro2_partitiontable.json ${target}.bin PT_PT=partition_signed.bin,CER_TBL=certable.bin,KEY_CER1=certificate_signed.bin,PT_BL_PRI=boot_signed.bin,PT_FW1=firmware_signed.bin,PT_ISP_IQ=firmware_isp_iq.bin,PT_FCSDATA=boot_fcs.bin
	COMMAND ${CMAKE_COMMAND} -E copy ./firmware_signed.bin ./ota.bin
	COMMAND ${CHKSUM} ./ota.bin
	
	DEPENDS plain_img
	DEPENDS ${ELF2BIN}
)

add_custom_target(
	sign_enc
	
	COMMAND ${ELF2BIN} secure sign+dbg=cert key_private.json key_public.json certificate.bin certificate_signed.bin
	COMMAND ${ELF2BIN} secure sign+hash+dbg=ptab key_private.json key_public.json partition.bin partition_signed.bin
	COMMAND ${ELF2BIN} secure sign+enc+hash+dbg=boot key_private.json key_public.json ${POSTBUILD_ENC_BOOT}  boot.bin boot_signed_enc.bin
	COMMAND ${ELF2BIN} secure sign+enc+hash+dbg=fw key_private.json key_public.json ${POSTBUILD_ENC_NTZ} firmware.bin firmware_signed_enc.bin
	
	
	COMMAND ${ELF2BIN} combine amebapro2_partitiontable.json ${target}.bin PT_PT=partition_signed.bin,CER_TBL=certable.bin,KEY_CER1=certificate_signed.bin,PT_BL_PRI=boot_signed_enc.bin,PT_FW1=firmware_signed_enc.bin,PT_ISP_IQ=firmware_isp_iq.bin,PT_FCSDATA=boot_fcs.bin
	COMMAND ${CMAKE_COMMAND} -E copy ./firmware_signed_enc.bin ./ota.bin
	COMMAND ${CHKSUM} ./ota.bin
	
	DEPENDS plain_img
	DEPENDS ${ELF2BIN}
)
else()
add_custom_target(
	flash
	
	COMMAND ${CMAKE_COMMAND} -E copy ${prj_root}/GCC-RELEASE/testchip/keycfg.json keycfg.json
	COMMAND ${CMAKE_COMMAND} -E copy ${prj_root}/GCC-RELEASE/testchip/partition.json partition.json
	COMMAND ${CMAKE_COMMAND} -E copy ${prj_root}/GCC-RELEASE/testchip/amebapro2_partition.json amebapro2_partition.json
	COMMAND ${ELF2BIN} keygen ./testchip/keycfg.json
	
	COMMAND ${ELF2BIN} convert ./testchip/amebapro2_partition.json PARTITIONTABLE 
	COMMAND ${CMAKE_COMMAND} -E copy ${boot_image} .
	
	COMMAND ${CMAKE_COMMAND} -E copy ${fw} ./firmware.bin
	COMMAND ${ELF2BIN} combine ${target}.bin PTAB=partition.bin,BOOT=boot.bin,FW1=firmware.bin,PT_ISP_IQ=firmware_isp_iq.bin
	
	COMMAND ${CMAKE_COMMAND} -E copy ${bttrace} ./APP.trace
	
	DEPENDS ${boot_dep}
	DEPENDS ${app_dep}
	DEPENDS ${ELF2BIN}
)
endif()
