
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean


#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = cmsis_os.o \
       freertos_service.o \
       ../osdep/osdep_service.o \
       ../osdep/device_lock.o

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

# Define the Rules to build the core targets
all: CORE_TARGETS COPY_RAM_OBJS
ifeq ($(CONFIG_KERNEL),y)	
	make -C freertos_v8.1.2/Source all
endif

#*****************************************************************************#
#              GENERATE OBJECT FILE 
#*****************************************************************************#
CORE_TARGETS: $(OBJS)

	


#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean: CLEAN_OBJS 
ifeq ($(CONFIG_KERNEL),y)	
	make -C freertos_v8.1.2/Source clean
endif
	$(REMOVE) *.o
	$(REMOVE) *.i
	$(REMOVE) *.s
	$(REMOVE) *.d
	



