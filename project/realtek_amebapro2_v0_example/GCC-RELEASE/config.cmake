cmake_minimum_required(VERSION 3.6)

if(NOT DEFINED CONFIG_DONE)
	execute_process(COMMAND uname OUTPUT_VARIABLE uname)

	if(${CMAKE_HOST_SYSTEM_NAME} MATCHES "Linux")
		message(STATUS "Build on Linux")
		set(LINUX TRUE)
	else()
		set(LINUX FALSE)
	endif()

	if(${CMAKE_HOST_SYSTEM_NAME} MATCHES "Windows")
		set(WINDOWS TRUE)
		if (uname MATCHES "^MSYS" OR uname MATCHES "^MINGW")
			set(WIN_MSYS TRUE)
			message(STATUS "Build on Mingw")
		else()
			set(WIN_MSYS FALSE)
			message(STATUS "Build on Windows")
		endif()	
	else()
		set(WINDOWS FALSE)
		set(WIN_MSYS FALSE)
	endif()

	set(CONFIG_DONE ON)
	
	set(freertos "freertos_v202012.00")
	set(lwip "lwip_v2.1.2")
	set(mbedtls "mbedtls-2.16.6")
	set(viplite "VIPLiteDrv_1.12.0")
	
	message(STATUS "FreeRTOS = ${freertos}")
	message(STATUS "LWIP     = ${lwip}")
	message(STATUS "mbedTLS  = ${mbedtls}")
	message(STATUS "VIPLite  = ${viplite}")
	
	if(NOT DEFINED CUTVER)
		set(CUTVER "B")
	endif()
	
	if(NOT DEFINED DDR)
		set(DDR "128M")
	endif()

	if(CUTVER STREQUAL "TEST")
		set(MPCHIP OFF)
		message(FATAL_ERROR "Test chip is not supported now")
	else()
		set(MPCHIP ON)
	endif()

	if(NOT DEFINED BUILD_TZ)
		set(BUILD_TZ OFF)
	endif()

	message(STATUS "MPCHIP ${MPCHIP} : ${CUTVER}-CUT")
	message(STATUS "Build TZ ${BUILD_TZ}")

	# for simulation, not use now
	if(NOT DEFINED BUILD_PXP)
		set(BUILD_PXP OFF)
	endif()
	
	if(BUILD_PXP)
		message(FATAL_ERROR "PXP is not supported now")
	endif()	

	# for simulation, not use now
	if(NOT DEFINED BUILD_FPGA)
		set(BUILD_FPGA OFF)
	endif()
	
	if(BUILD_FPGA)
		message(FATAL_ERROR "FPGA is not supported now")
	endif()		

	if(NOT DEFINED BUILD_LIB)
		set(BUILD_LIB OFF)
	endif()
	
	message(STATUS "Build libraries ${BUILD_LIB}")
	message(STATUS "Build FPGA ${BUILD_FPGA}")
	message(STATUS "Build PXP ${BUILD_PXP}")	

	if(NOT DEFINED BUILD_KVS_DEMO)
		set(BUILD_KVS_DEMO OFF)
	endif()

	if(NOT DEFINED DEBUG)
		set(DEBUG OFF)
	endif()
    
	#AUDIO AEC LIB
	if (NOT DEFINED BUILD_NEWAEC)
		set(BUILD_NEWAEC ON)
	endif()
	
	if(NOT DEFINED UNITEST)
		set(UNITEST OFF)
	endif()
	message(STATUS "UNITEST ${UNITEST}")
	
	#elf2bin
	if(NOT DEFINED ELF2BIN)
	if(MPCHIP)
		if (LINUX)
		set(ELF2BIN ${prj_root}/GCC-RELEASE/mp/elf2bin.linux)
		else()
		set(ELF2BIN ${prj_root}/GCC-RELEASE/mp/elf2bin.exe)
		endif()
	else()
		if (LINUX)
		set(ELF2BIN ${prj_root}/GCC-RELEASE/testchip/elf2bin.linux)
		else()
		set(ELF2BIN ${prj_root}/GCC-RELEASE/testchip/elf2bin.exe)
		endif()
	endif()
	endif()		
	
	#chksum
	if(NOT DEFINED CHKSUM)
	if(MPCHIP)
		if (LINUX)
		set(CHKSUM ${prj_root}/GCC-RELEASE/mp/checksum.linux)
		else()		
		set(CHKSUM ${prj_root}/GCC-RELEASE/mp/checksum.exe)
		endif()
	endif()
	endif()		
	
	if(NOT DEFINED NNMDLCFG)
	if(MPCHIP)
		if (LINUX)
		set(NNMDLCFG ${prj_root}/GCC-RELEASE/mp/nn_model_cfg.linux)
		else()		
		set(NNMDLCFG ${prj_root}/GCC-RELEASE/mp/nn_model_cfg.exe)
		endif()
	endif()
	endif()		
	
	if(NOT DEFINED GENSNRLST)
	if(MPCHIP)
		if (LINUX)
		set(GENSNRLST ${prj_root}/GCC-RELEASE/mp/gen_snrlst.linux)
		else()		
		set(GENSNRLST ${prj_root}/GCC-RELEASE/mp/gen_snrlst.exe)
		endif()
	endif()
	endif()		
	
	#platform console command, for wildcard
	if (LINUX OR WIN_MSYS)
		set(PLAT_COPY cp)
		set(PLAT_FINDSTR grep)
	else()		
		set(PLAT_COPY copy)
		set(PLAT_FINDSTR findstr)
	endif()
	
	#default postbuild script
	if (MPCHIP)
		set(POSTBUILD_BOOT		${prj_root}/GCC-RELEASE/mp/amebapro2_bootloader.json)
		set(POSTBUILD_FW_NTZ 	${prj_root}/GCC-RELEASE/mp/amebapro2_firmware_ntz.json)
		set(POSTBUILD_FW_NTZXIP	${prj_root}/GCC-RELEASE/mp/amebapro2_firmware_ntz_xip.json)
		set(POSTBUILD_FW_TZ		${prj_root}/GCC-RELEASE/mp/amebapro2_firmware_tz.json)
		set(POSTBUILD_KEY_CFG	${prj_root}/GCC-RELEASE/mp/key_cfg.json)
		set(POSTBUILD_CERT		${prj_root}/GCC-RELEASE/mp/certificate.json)
		set(POSTBUILD_PART		${prj_root}/GCC-RELEASE/mp/amebapro2_partitiontable.json)
		set(POSTBUILD_NNMDL		${prj_root}/GCC-RELEASE/mp/amebapro2_nn_model.json)
		set(POSTBUILD_FWFS_NN	${prj_root}/GCC-RELEASE/mp/amebapro2_fwfs_nn_models.json)
		
		set(POSTBUILD_ENC_BOOT	${prj_root}/GCC-RELEASE/mp/encrypt_bl.json)
		set(POSTBUILD_ENC_NTZ	${prj_root}/GCC-RELEASE/mp/encrypt_fw.json)
		set(POSTBUILD_ENC_TZ	${prj_root}/GCC-RELEASE/mp/encrypt_fw_tz.json)	
		
		set(POSTBUILD_ISP_IQ   	    ${prj_root}/GCC-RELEASE/mp/amebapro2_isp_iq.json)
		set(POSTBUILD_SENSOR_SET    ${prj_root}/GCC-RELEASE/mp/amebapro2_sensor_set.json)
		
		set(VOE_BIN_PATH       ${sdk_root}/component/soc/8735b/fwlib/rtl8735b/lib/source/ram/video/voe_bin)
		
		set(NN_MODEL_PATH		${prj_root}/src/test_model/model_nb)
	endif()	

	execute_process(
		COMMAND
			whoami
		TIMEOUT
			1
		OUTPUT_VARIABLE
			_user_name
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)
	cmake_host_system_information(RESULT _host_name QUERY HOSTNAME)
	cmake_host_system_information(RESULT _fqdn QUERY FQDN)

	string(TIMESTAMP _configuration_time "%Y-%m-%d %H:%M:%S [UTC]" UTC)
	string(TIMESTAMP _configuration_date "%Y-%m-%d" UTC)

	get_filename_component(_compiler_name ${CMAKE_C_COMPILER} NAME)

	configure_file(${prj_root}/inc/build_info.h.in ${prj_root}/inc/build_info.h @ONLY)

	if(BUILD_PXP)
		message(STATUS "Setup for PXP")
		execute_process(COMMAND bash "-c" "sed -i 's/CONFIG_PXP.*0/CONFIG_PXP\t\t\t\t\t\t\t1/' ./component/soc/8735b/cmsis/rtl8735b/include/platform_conf.h" WORKING_DIRECTORY ${sdk_root} )
		execute_process(COMMAND bash "-c" "sed -i 's/CONFIG_FPGA.*1/CONFIG_FPGA\t\t\t\t\t\t\t0/' ./component/soc/8735b/cmsis/rtl8735b/include/platform_conf.h" WORKING_DIRECTORY ${sdk_root} )
		execute_process(COMMAND bash "-c" "sed -i 's/CONFIG_ASIC.*1/CONFIG_ASIC\t\t\t\t\t\t\t0/' ./component/soc/8735b/cmsis/rtl8735b/include/platform_conf.h" WORKING_DIRECTORY ${sdk_root} )
	elseif(BUILD_FPGA)
		message(STATUS "Setup for FPGA")
		execute_process(COMMAND bash "-c" "sed -i 's/CONFIG_PXP.*1/CONFIG_PXP\t\t\t\t\t\t\t0/' ./component/soc/8735b/cmsis/rtl8735b/include/platform_conf.h" WORKING_DIRECTORY ${sdk_root} )
		execute_process(COMMAND bash "-c" "sed -i 's/CONFIG_FPGA.*0/CONFIG_FPGA\t\t\t\t\t\t\t1/' ./component/soc/8735b/cmsis/rtl8735b/include/platform_conf.h" WORKING_DIRECTORY ${sdk_root} )
		execute_process(COMMAND bash "-c" "sed -i 's/CONFIG_ASIC.*1/CONFIG_ASIC\t\t\t\t\t\t\t0/' ./component/soc/8735b/cmsis/rtl8735b/include/platform_conf.h" WORKING_DIRECTORY ${sdk_root} )
	else()
		message(STATUS "Setup for ASIC")
		execute_process(COMMAND bash "-c" "sed -i 's/CONFIG_PXP.*1/CONFIG_PXP\t\t\t\t\t\t\t0/' ./component/soc/8735b/cmsis/rtl8735b/include/platform_conf.h" WORKING_DIRECTORY ${sdk_root} )
		execute_process(COMMAND bash "-c" "sed -i 's/CONFIG_FPGA.*1/CONFIG_FPGA\t\t\t\t\t\t\t0/' ./component/soc/8735b/cmsis/rtl8735b/include/platform_conf.h" WORKING_DIRECTORY ${sdk_root}  )
		execute_process(COMMAND bash "-c" "sed -i 's/CONFIG_ASIC.*0/CONFIG_ASIC\t\t\t\t\t\t\t1/' ./component/soc/8735b/cmsis/rtl8735b/include/platform_conf.h" WORKING_DIRECTORY ${sdk_root} )
	endif()

	if(CUTVER STREQUAL "TEST" AND MPCHIP)
		message(FATAL_ERROR "MPCHIP cannot be TEST CUT, please check setting")
	endif()

	if(CUTVER STREQUAL "A")
		message(STATUS "Setup for ${CUTVER}-CUT")
		execute_process(COMMAND bash "-c" "sed -i 's/define.*CONFIG_CHIP_VER.*_CUT/define CONFIG_CHIP_VER\t\t\t\t\t\t\tCHIP_A_CUT/' ./component/soc/8735b/cmsis/rtl8735b/include/platform_conf.h" WORKING_DIRECTORY ${sdk_root} )
	elseif(CUTVER STREQUAL "B")
		message(STATUS "Setup for ${CUTVER}-CUT")
		execute_process(COMMAND bash "-c" "sed -i 's/define.*CONFIG_CHIP_VER.*_CUT/define CONFIG_CHIP_VER\t\t\t\t\t\t\tCHIP_B_CUT/' ./component/soc/8735b/cmsis/rtl8735b/include/platform_conf.h" WORKING_DIRECTORY ${sdk_root} )
	elseif(CUTVER STREQUAL "C")
		message(STATUS "Setup for ${CUTVER}-CUT")
		execute_process(COMMAND bash "-c" "sed -i 's/define.*CONFIG_CHIP_VER.*_CUT/define CONFIG_CHIP_VER\t\t\t\t\t\t\tCHIP_C_CUT/' ./component/soc/8735b/cmsis/rtl8735b/include/platform_conf.h" WORKING_DIRECTORY ${sdk_root} )
	elseif (CUTVER STREQUAL "TEST")
		message(STATUS "Setup for ${CUTVER}-CUT")
		execute_process(COMMAND bash "-c" "sed -i 's/define.*CONFIG_CHIP_VER.*_CUT/define CONFIG_CHIP_VER\t\t\t\t\t\t\tCHIP_TEST_CUT/' ./component/soc/8735b/cmsis/rtl8735b/include/platform_conf.h" WORKING_DIRECTORY ${sdk_root} )
		execute_process(COMMAND bash "-c" "sed -i 's/define.*CONFIG_CHIP_VER.*_CUT/define CONFIG_CHIP_VER\t\t\t\t\t\t\tCHIP_TEST_CUT/' ./component/soc/8735b/cmsis/rtl8735b/include/platform_conf.h" WORKING_DIRECTORY ${sdk_root} )
	endif()
	
	if(DDR STREQUAL "128M")
		message(STATUS "Setup for DDR ${DDR}")
		execute_process(COMMAND bash "-c" "sed -i 's/define.*SAU_INIT_END5.*/define SAU_INIT_END5 0x77DFFFFF/' ./component/soc/8735b/cmsis/rtl8735b/include/partition_rtl8735b.h" WORKING_DIRECTORY ${sdk_root} )
		execute_process(COMMAND bash "-c" "sed -i 's/DDR_SIZE = .*/DDR_SIZE = 128;/' ./GCC-RELEASE/application/rtl8735b_ram.ld" WORKING_DIRECTORY ${prj_root} )
		execute_process(COMMAND bash "-c" "sed -i 's/DDR_SIZE = .*/DDR_SIZE = 128;/' ./GCC-RELEASE/application/rtl8735b_ram_ns.ld" WORKING_DIRECTORY ${prj_root} )
		execute_process(COMMAND bash "-c" "sed -i 's/DDR_SIZE = .*/DDR_SIZE = 128;/' ./GCC-RELEASE/application/rtl8735b_ram_s.ld" WORKING_DIRECTORY ${prj_root} )
		execute_process(COMMAND bash "-c" "sed -i 's/DDR_SIZE = .*/DDR_SIZE = 128;/' ./GCC-RELEASE/bootloader/rtl8735b_boot_mp.ld" WORKING_DIRECTORY ${prj_root} )
	elseif(DDR STREQUAL "64M")
		message(STATUS "Setup for DDR ${DDR}")
		execute_process(COMMAND bash "-c" "sed -i 's/define.*SAU_INIT_END5.*/define SAU_INIT_END5 0x73DFFFFF/' ./component/soc/8735b/cmsis/rtl8735b/include/partition_rtl8735b.h" WORKING_DIRECTORY ${sdk_root} )
		execute_process(COMMAND bash "-c" "sed -i 's/DDR_SIZE = .*/DDR_SIZE = 64;/' ./GCC-RELEASE/application/rtl8735b_ram.ld" WORKING_DIRECTORY ${prj_root} )
		execute_process(COMMAND bash "-c" "sed -i 's/DDR_SIZE = .*/DDR_SIZE = 64;/' ./GCC-RELEASE/application/rtl8735b_ram_ns.ld" WORKING_DIRECTORY ${prj_root} )
		execute_process(COMMAND bash "-c" "sed -i 's/DDR_SIZE = .*/DDR_SIZE = 64;/' ./GCC-RELEASE/application/rtl8735b_ram_s.ld" WORKING_DIRECTORY ${prj_root} )
		execute_process(COMMAND bash "-c" "sed -i 's/DDR_SIZE = .*/DDR_SIZE = 64;/' ./GCC-RELEASE/bootloader/rtl8735b_boot_mp.ld" WORKING_DIRECTORY ${prj_root} )
	endif()
	
endif() #CONFIG_DONE

