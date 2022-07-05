#!/bin/sh

ver=10.2.0
os=`uname | sed 's\CYGWIN.*\CYGWIN\g'`

function prebuild_exec(){
	#echo Current param 0:$0 1:$1 2:$2 3:$3
	if [ -L "$1/../../../component/os/freertos/freertos" ]; then
		echo freertos linked
	else
		echo link freertos_v$ver to freertos on $os
		if [ $os == "CYGWIN" ]; then
			$1/../../../component/os/freertos/junction.exe `cygpath -w $1/../../../component/os/freertos/freertos` `cygpath -w $1/../../../component/os/freertos/freertos_v$ver`
		else
			ln -s $1/../../../component/os/freertos/freertos_v$ver $1/../../../component/os/freertos/freertos
		fi
	fi
}

function local_exec(){
	if [ -L freertos ]; then
		echo freertos linked
	else
		echo link freertos_v$ver to freertos
		ln -s freertos_v$ver freertos
	fi
}

#echo Current param 0:$0 1:$1 2:$2 3:$3
if [ -z $1 ]; then
	echo PROJ_DIR empty
	local_exec;
else
	echo PROJ_DIR not empty 
	prebuild_exec $1;
fi

