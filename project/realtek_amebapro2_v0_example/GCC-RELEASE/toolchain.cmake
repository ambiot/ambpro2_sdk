set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)
set(CMAKE_ASM_COMPILER_WORKS 1)

###****************NEW*********************#####
set(PICOLIBC OFF)

set(CMAKE_C_COMPILER "arm-none-eabi-gcc" )
set(CMAKE_CXX_COMPILER "arm-none-eabi-g++" )
set(CMAKE_ASM_COMPILER "arm-none-eabi-gcc" )
set(CMAKE_OBJECOPY "arm-none-eabi-objcopy" )
set(CMAKE_OBJEDUMP "arm-none-eabi-objdump" )
set(CMAKE_STRIP "arm-none-eabi-strip" )
set(CMAKE_AR "arm-none-eabi-ar" )
set(CMAKE_AS "arm-none-eabi-as" )
set(CMAKE_NM "arm-none-eabi-nm" )
#set(CMAKE_LD "arm-none-eabi-gcc" )
#set(CMAKE_LD "arm-none-eabi-ld" )

add_definitions(-D__thumb2__ -DCONFIG_PLATFORM_8735B -DARM_MATH_ARMV8MML -D__FPU_PRESENT -D__ARM_ARCH_7M__=0 -D__ARM_ARCH_7EM__=0 -D__ARM_ARCH_8M_MAIN__=1 -D__ARM_ARCH_8M_BASE__=0 -D__ARM_FEATURE_FP16_SCALAR_ARITHMETIC=1 -D__DSP_PRESENT=1 -D__ARMVFP__)

#set(CMAKE_C_FLAGS "-march=armv8-m.main+dsp+fp -mcpu=real-m500+fp -mthumb -mcmse -mfpu=fpv5-sp-d16 -mfp16-format=ieee -mfloat-abi=softfp ")
set(CMAKE_C_FLAGS "-march=armv8-m.main+dsp -mthumb -mcmse -mfpu=fpv5-sp-d16 -mfp16-format=ieee -mfloat-abi=softfp -fno-common -fsigned-char")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Os -fstack-usage -fdata-sections -ffunction-sections  -fno-optimize-sibling-calls")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -gdwarf-3 -MMD -nostartfiles -nodefaultlibs -nostdlib ")

#set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=gnu++11 -Wall -Wpointer-arith -Wundef -Wno-write-strings -Wno-maybe-uninitialized -fdiagnostics-color=always -fno-PIC")#-fPIC -fexceptions
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99 -Wall -Wpointer-arith -Wstrict-prototypes -Wundef -Wno-write-strings -Wno-maybe-uninitialized -fdiagnostics-color=always")

set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=gnu++11 -fdiagnostics-color=always -fno-PIC")#-fPIC -fexceptions
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99 -fdiagnostics-color=always")

set(CMAKE_ASM_FLAGS "-march=armv8-m.main -mthumb -mfpu=fpv5-sp-d16 -mfloat-abi=softfp -x assembler-with-cpp")

set(OUTSRC_WARN_ERR_FLAGS -Wall -Wpointer-arith -Wstrict-prototypes -Wundef -Wno-write-strings -Wno-maybe-uninitialized)
set(LIBS_WARN_ERR_FLAGS -Wall -Wpointer-arith -Wstrict-prototypes -Wundef -Wno-write-strings -Wno-maybe-uninitialized)
set(WARN_ERR_FLAGS -Wall -Wpointer-arith -Wstrict-prototypes -Wundef -Wno-write-strings -Wno-maybe-uninitialized)


if(NOT CONSOLE STREQUAL "UART")
list(
    APPEND _wrapper
#		"-Wl,-wrap,puts"
#		"-Wl,-wrap,printf"
#		"-Wl,-wrap,sprintf"
#		"-Wl,-wrap,snprintf"
#		"-Wl,-wrap,vsnprintf"
#		"-Wl,-wrap,vprintf"	
)
else()
list(
    APPEND _wrapper
#		"-Wl,-wrap,puts"
#		"-Wl,-wrap,printf"
#		"-Wl,-wrap,sprintf"
#		"-Wl,-wrap,snprintf"
#		"-Wl,-wrap,vsnprintf"
#		"-Wl,-wrap,vprintf"	
)
endif()

list(
    APPEND _wrapper
		"-Wl,-wrap,strcat"
		"-Wl,-wrap,strchr"
		"-Wl,-wrap,strcmp"
		"-Wl,-wrap,strncmp"
		"-Wl,-wrap,strnicmp"
		"-Wl,-wrap,strcpy"
		"-Wl,-wrap,strncpy"
		"-Wl,-wrap,strlcpy"
		"-Wl,-wrap,strlen"
		"-Wl,-wrap,strnlen"
		"-Wl,-wrap,strncat" 
		"-Wl,-wrap,strpbrk"
		"-Wl,-wrap,strspn" 
		"-Wl,-wrap,strstr"  
		"-Wl,-wrap,strtok"
		"-Wl,-wrap,strxfrm"
		"-Wl,-wrap,strsep"
#		"-Wl,-wrap,strdup"
		"-Wl,-wrap,strtod"
		"-Wl,-wrap,strtof"
		"-Wl,-wrap,strtold"
		"-Wl,-wrap,strtoll"
		"-Wl,-wrap,strtoul"
		"-Wl,-wrap,strtoull"
		"-Wl,-wrap,atoi"
		"-Wl,-wrap,atoui"
		"-Wl,-wrap,atol"
		"-Wl,-wrap,atoul"
		"-Wl,-wrap,atoull" 
		"-Wl,-wrap,atof"
		"-Wl,-wrap,memcmp"
		"-Wl,-wrap,memcpy"
		"-Wl,-wrap,memmove"
		"-Wl,-wrap,memset"
		"-Wl,-wrap,_malloc_r"
		"-Wl,-wrap,_free_r"
		"-Wl,-wrap,_realloc_r"
		"-Wl,-wrap,_calloc_r"
		"-Wl,-wrap,malloc"
		"-Wl,-wrap,free"
		"-Wl,-wrap,realloc"
		"-Wl,-wrap,calloc"	
		"-Wl,-wrap,abort"
		"-Wl,-wrap,fopen"
		"-Wl,-wrap,fclose"
		"-Wl,-wrap,fread"
		"-Wl,-wrap,fwrite"
		"-Wl,-wrap,fseek"
		"-Wl,-wrap,fsetpos"
		"-Wl,-wrap,fgetpos"
		"-Wl,-wrap,rewind"
		"-Wl,-wrap,fflush"
		"-Wl,-wrap,remove"
		"-Wl,-wrap,rename"
		"-Wl,-wrap,feof"
		"-Wl,-wrap,ferror"
		"-Wl,-wrap,ftell"
		"-Wl,-wrap,ftruncate"
		"-Wl,-wrap,fputc"
		"-Wl,-wrap,fputs"
		"-Wl,-wrap,fgets"
		"-Wl,-wrap,stat"
		"-Wl,-wrap,mkdir"
		"-Wl,-wrap,scandir"
		"-Wl,-wrap,readdir"
		"-Wl,-wrap,opendir"
		"-Wl,-wrap,access"
		"-Wl,-wrap,rmdir"
		"-Wl,-wrap,closedir"
)

list(JOIN _wrapper " " function_wrapper)
	
set(CMAKE_EXE_LINKER_FLAGS "${function_wrapper}" CACHE INTERNAL "")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -nostartfiles -nodefaultlibs -nostdlib --specs=nosys.specs" CACHE INTERNAL "")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections -Wl,--cref -Wl,--build-id=none -Wl,--use-blx" CACHE INTERNAL "")
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T ${CMAKE_CURRENT_SOURCE_DIR}/rtl8735b_fpga.ld -Wl,-Map=text.map " CACHE INTERNAL "")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)





#-DCONFIG_BUILD_ALL=1 -DROM_REGION=1 




