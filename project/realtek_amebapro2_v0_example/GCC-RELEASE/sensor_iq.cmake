cmake_minimum_required(VERSION 3.6)

file(GLOB IQ_SENSOR_BINARY
  "${VOE_BIN_PATH}/*.bin"
)

file(COPY ${IQ_SENSOR_BINARY}
     DESTINATION .)	 

add_custom_target(
	fcs_isp_iq
	ALL
	DEPENDS firmware_isp_iq.bin
)

# import config.cmake before use this
add_custom_command(
	OUTPUT firmware_isp_iq.bin isp_iq.bin
	
	#set POSTBUILD_SENSOR_SET
	#set POSTBUILD_ISP_IQ
	#COMMAND ${CMAKE_COMMAND} -E copy ${POSTBUILD_SENSOR_SET} amebapro2_sensor_set.json
	#COMMAND ${CMAKE_COMMAND} -E copy ${POSTBUILD_ISP_IQ} amebapro2_isp_iq.json
	COMMAND ${GENSNRLST} ${prj_root}/inc/sensor.h
	#COMMAND cp fsc
	#COMMAND cp iq
	#COMMAND cp sensor
	#COMMAND ${CMAKE_COMMAND} -E copy ${VOE_BIN_PATH}/*.bin .
	#POSTBUILD_VOE_ISP
	#COMMAND cp POSTBUILD_VOE
	COMMAND ${ELF2BIN} convert amebapro2_sensor_set.json ISP_SENSOR_SETS isp_iq.bin
	COMMAND ${ELF2BIN} convert amebapro2_isp_iq.json FIRMWARE firmware_isp_iq.bin
	
	DEPENDS ${POSTBUILD_SENSOR_SET}
	DEPENDS ${POSTBUILD_ISP_IQ}
)