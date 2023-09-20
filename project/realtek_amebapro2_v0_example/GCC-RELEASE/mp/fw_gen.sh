#!/bin/bash
echo "====== ELF2Bin Script ======"

set -e

# Env vars:
#   $PARTITION_MGR

# Configurabel options
secure_img_en=0
isp_iq_en=1
xip_en=0
quite=1
partition_mgr_en=1

certificate_json="certificate.json"
partition_json="amebapro2_partitiontable.json"
bootloader_json="amebapro2_bootloader.json"
key_cfg_json="key_cfg.json"
key_private_json="key_private.json"
key_public_json="key_public.json"
enc_bl_json="encrypt_bl.json"
isp_iq_sensor_json="amebapro2_sensor_set.json"
isp_iq_json="amebapro2_isp_iq.json"

if [ ! -z "$PARTITION_MGR" ]; then
    partition_mgr_en=$PARTITION_MGR
fi

if [ "$xip_en" == "1" ]; then
    ntz_firmware_json="amebapro2_firmware_ntz_xip.json"
    tz_firmware_json="amebapro2_firmware_tz_xip.json"
    enc_fw_json="encrypt_fw_xip.json"
    enc_fw_tz_json="encrypt_fw_tz_xip.json"
else
    ntz_firmware_json="amebapro2_firmware_ntz.json"
    tz_firmware_json="amebapro2_firmware_tz.json"
    enc_fw_json="encrypt_fw.json"
    enc_fw_tz_json="encrypt_fw_tz.json"
fi

sec_op_hash="hash+dbg"
sec_op_sign="sign+dbg"
sec_op_hash_sign="hash+sign+dbg"
sec_op_enc_hash="hash+enc+dbg"
sec_op_enc_hash_sign="hash+enc+sign+dbg"

certificate_tbl_img_file="cert_table.bin"
certificate_img_file="certificate.bin"
partition_img_file="partition.bin"
boot_img_file="boot.bin"
boot_enc_img_file="boot_enc.bin"
firmware_img_file="firmware.bin"
isp_iq_sensor_file="isp_iq.bin"
isp_iq_img_file="isp_iq_img.bin"
flash_img_file="flash.bin"
isp_fcs_data_file="fcs_data.bin"
isp_fcs_dummy_file="fcs_data_dummy.bin"

boot_axf_file="target_boot.axf"
hs_axf_file="target_ram.axf"
hs_s_axf_file="target_ram_s.axf"
hs_ns_axf_file="target_ram_ns.axf"

bin_root_dir="build/output/"
elf2bin_new_format_root_dir="post_build/elf2bin_new_format/"
img_format_str="img_format_bin_parser"
utility_img_format_str="utility/img_format_bin_parser"
parse_path="build"

OS=`uname -s`
if [ $OS == "Linux" ]; then
    ELF2BIN=./elf2bin.linux
else
    ELF2BIN=./elf2bin.exe
fi

quiet_elf2bin() {
    echo "    $_ELF2BIN_ORIG $@"
    echo ""
    $_ELF2BIN_ORIG "$@" > /dev/null
}

if [ "$quite" == "1" ]; then
    _ELF2BIN_ORIG=$ELF2BIN
    ELF2BIN="quiet_elf2bin"
fi

function bootloader_gen {
    # Generate boot img
    if [ "$bootloader_json" -nt "$boot_img_file" ] || [ "$boot_axf_file" -nt "$boot_img_file" ]; then

        if [ ! -f $boot_axf_file ]; then
            echo "'$boot_img_file' not found"
        return 0
    fi

        if [ -f "$boot_img_file" ]; then
            echo "==== Update $boot_img_file... ===="
        else
            echo "==== Generate $boot_img_file... ===="
        fi
	$ELF2BIN convert $bootloader_json BOOTLOADER $boot_img_file
    else
        echo "'$boot_img_file' is up to date"
    fi
}

function firmware_gen {
    use_axf=""

    # Check axf exist
    if [ ! -f "$hs_s_axf_file" ] || [ ! -f "$hs_ns_axf_file" ]; then
        no_tz_axf=1
    fi
    if [ ! -f "$hs_axf_file" ]; then
        no_ntz_axf=1
    fi

    if [ "$no_ntz_axf" == "1" ] && [ "$no_tz_axf" == "1" ]; then
        echo "'$hs_axf_file' not found"
        return 0
    elif [ "$no_ntz_axf" == "1" ]; then
        use_axf="tz"
    elif [ "$no_tz_axf" == "1" ]; then
        use_axf="ntz"
    else
        # Both exist -> use newer one
        if [ "$hs_s_axf_file" -nt "$hs_axf_file" ] || [ "$hs_ns_axf_file" -nt "$hs_axf_file" ]; then
            use_axf="tz"
        else
            use_axf="ntz"
        fi
    fi

    if [ "$use_axf" == "ntz" ]; then
        echo "==== Generate $firmware_img_file with Non-TrustZone(NTZ) mode... ===="
		$ELF2BIN convert ${ntz_firmware_json} FIRMWARE $firmware_img_file
    elif [ "$use_axf" == "tz" ]; then
        echo "==== Generate $firmware_img_file with TrustZone(S/NS) mode... ===="
		$ELF2BIN convert ${tz_firmware_json} FIRMWARE $firmware_img_file
    fi
}

function rm_all_bin {
    rm -f $certificate_tbl_img_file $certificate_img_file $partition_img_file $boot_img_file
    rm -f $firmware_img_file $isp_iq_img_file $isp_iq_sensor_file $flash_img_file
}

function chk_gen_partition_bin {
    # Generate partition table img
    if [ "$partition_json" -nt "$partition_img_file" ]
	then
        if [ -e "$partition_img_file" ]; then
            echo "==== Update $partition_img_file... ===="
        else
            echo "==== Generate $partition_img_file... ===="
	fi
        $ELF2BIN convert $partition_json PARTITIONTABLE $partition_img_file
    else
        echo "'$partition_img_file' is up to date"
	fi
}

function gen_isp_iq {
    # Generate isp_iq_sensor file
    if [ "$isp_iq_sensor_json" -nt "$isp_iq_sensor_file" ] || [ "$isp_fcs_dummy_file" -nt "$isp_iq_sensor_file" ] || [ "$isp_fcs_data_file" -nt "$isp_iq_sensor_file" ]; then
        if [ -e "$isp_iq_sensor_file" ]; then
            echo "==== Update $isp_iq_sensor_file... ===="
        else
            echo "==== Generate $isp_iq_sensor_file... ===="
	fi
        $ELF2BIN convert $isp_iq_sensor_json ISP_SENSOR_SETS $isp_iq_sensor_file
    else
        echo "'$isp_iq_sensor_file' is up to date"
	fi
	
    # Generate isp_iq img
    if [ "$isp_iq_sensor_file" -nt "$isp_iq_img_file" ] || [ "$isp_iq_json" -nt "$isp_iq_img_file" ]; then
        if [ -e "$isp_iq_img_file" ]; then
            echo "==== Update $isp_iq_img_file... ===="
        else
            echo "==== Generate $isp_iq_img_file... ===="
	fi
        $ELF2BIN convert $isp_iq_json FIRMWARE $isp_iq_img_file
    else
        echo "'$isp_iq_img_file' is up to date"
	fi
}

SCRIPT_DIR="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
cd $SCRIPT_DIR

if [ -e "$key_private_json" ] || [ -e "$key_public_json" ]
then
	echo "key info found."
else
	echo "==== Generate $key_private_json and $key_public_json... ===="
	$ELF2BIN keygen $key_cfg_json key
	echo "==== Remove all binfiles... ===="
	rm_all_bin
fi

# Generate certificate table img
if [ -e "$certificate_tbl_img_file" ]
then
	echo "$certificate_tbl_img_file found."
else
	echo "==== Generate $certificate_tbl_img_file... ===="
	$ELF2BIN convert $certificate_json CERT_TABLE $certificate_tbl_img_file
fi

# Update certificate table img(if modified, then update)
if [ "$certificate_json" -nt "$certificate_tbl_img_file" ]
then
	echo "==== Update $certificate_tbl_img_file... ===="
	$ELF2BIN convert $certificate_json CERT_TABLE $certificate_tbl_img_file
fi

# Generate certificate img
if [ "$certificate_json" -nt "$certificate_img_file" ]; then
    if [ -e "$certificate_img_file" ]; then
	echo "==== Update $certificate_img_file... ===="
    else
        echo "==== Generate $certificate_img_file... ===="
    fi
	$ELF2BIN convert $certificate_json CERTIFICATE $certificate_img_file
else
    ls $certificate_img_file >/dev/null
    echo "'$certificate_img_file' is up to date"
fi

# Generate partition table img
chk_gen_partition_bin


if [ "$isp_iq_en" == "1" ]; then
    gen_isp_iq
fi

# Generate boot img
bootloader_gen

# Generate firmware img
if [ -e "$firmware_img_file" ]; then
	echo "$firmware_img_file found."

    # Update firmware img if axf is new
    if [ "$hs_s_axf_file" -nt "$firmware_img_file" ] || [ "$hs_ns_axf_file" -nt "$firmware_img_file" ] || [ "$hs_axf_file" -nt "$firmware_img_file" ]
    then
        firmware_gen
    # Update firmware img if json is new
    elif [ "$ntz_firmware_json" -nt "$firmware_img_file" ] || [ "$tz_firmware_json" -nt "$firmware_img_file" ]; then
        firmware_gen
    fi
else
    firmware_gen
fi

function key_certificate_sign {
    if [ -e "$certificate_img_file" ]
    then
	    echo "==== Sign $certificate_img_file... ===="
	    $ELF2BIN secure sign+dbg=cert $key_private_json $key_public_json $certificate_img_file
    else
	    echo "$certificate_img_file not found."
    fi
}

function img_sign {
    sec_img=$1
    sec_op=$2
    echo "    image=$sec_img"
    echo "    op=$sec_op"

    if [ "$partition_img_file" = "$sec_img" ]
    then
        if [ -e "$partition_img_file" ]
        then
    	    echo "==== Sign $partition_img_file... ===="
	        $ELF2BIN secure $sec_op=ptab $key_private_json $key_public_json $partition_img_file
        else
	        echo "$partition_img_file not found."
        fi
    elif [ "$boot_img_file" = "$sec_img" ]
    then
        if [ -e "$boot_img_file" ]
        then
    	    echo "==== Sign $boot_img_file... ===="
	        $ELF2BIN secure $sec_op=boot $key_private_json $key_public_json $boot_img_file
        else
	        echo "$boot_img_file not found."
        fi
    elif [ "$firmware_img_file" = "$sec_img" ]
    then
        if [ -e "$firmware_img_file" ]
        then
    	    echo "==== Sign $firmware_img_file... ===="
	        $ELF2BIN secure $sec_op=fw $key_private_json $key_public_json $firmware_img_file
        else
	        echo "$firmware_img_file not found."
        fi
    else
        echo "==== $sec_img is not match... ===="
    fi
}

function img_sec_op {
    # FIXME Avoid repeat encrypt same file
    set +e
    sec_img=$1
    sec_op=$2
    echo "    image=$sec_img"
    echo "    op=$sec_op"

    if [ "$partition_img_file" = "$sec_img" ]
    then
        if [ -e "$partition_img_file" ]
        then
			if [ "$sec_op_hash_sign" = "$sec_op" ]
			then
    	        echo "==== Sign $partition_img_file... ===="
	            $ELF2BIN secure $sec_op=ptab $key_private_json $key_public_json $partition_img_file
            elif [ "$sec_op_hash" = "$sec_op" ]
            then
    	        echo "==== Plain Hash $partition_img_file... ===="
	            $ELF2BIN secure $sec_op=ptab $key_private_json $key_public_json $partition_img_file
            fi
        else
	        echo "$partition_img_file not found."
        fi
    elif [ "$boot_img_file" = "$sec_img" ]
    then
        if [ -e "$boot_img_file" ]
        then
			if [ "$sec_op_hash" = "$sec_op" ]
			then
    	        echo "==== Plain Hash $boot_img_file... ===="
	            $ELF2BIN secure $sec_op=boot $key_private_json $key_public_json $boot_img_file
			elif [ "$sec_op_enc_hash" = "$sec_op" ]
			then
				echo "==== Encrypt $boot_img_file... ===="
				$ELF2BIN secure $sec_op=boot $key_private_json $key_public_json $enc_bl_json $boot_img_file $boot_img_file
            elif [ "$sec_op_enc_hash_sign" = "$sec_op" ]
            then
    	        echo "==== Encrypt & Sign $boot_img_file... ===="
	            $ELF2BIN secure $sec_op=boot $key_private_json $key_public_json $enc_bl_json $boot_img_file $boot_img_file
            fi
        else
	        echo "$boot_img_file not found."
        fi
    elif [ "$firmware_img_file" = "$sec_img" ]
    then
        if [ -e "$firmware_img_file" ]
        then
			if [ "$sec_op_hash" = "$sec_op" ]
			then
    	        echo "==== Plain Hash $firmware_img_file... ===="
	            $ELF2BIN secure $sec_op=fw $key_private_json $key_public_json $firmware_img_file
    	    elif [ "$sec_op_enc_hash" = "$sec_op" ]
			then
				if [ "$hs_s_axf_file" -nt "$hs_axf_file" ] || [ "$hs_ns_axf_file" -nt "$hs_axf_file" ]
	            then
    	            echo "==== Encrypt TZ $firmware_img_file... ===="
	                $ELF2BIN secure $sec_op=fw $key_private_json $key_public_json $enc_fw_tz_json $firmware_img_file $firmware_img_file
            	else
    	            echo "==== Encrypt NTZ $firmware_img_file... ===="
					$ELF2BIN secure $sec_op=fw $key_private_json $key_public_json $enc_fw_json $firmware_img_file $firmware_img_file
            	fi
			elif [ "$sec_op_enc_hash_sign" = "$sec_op" ]
            then
	            if [ "$hs_s_axf_file" -nt "$hs_axf_file" ] || [ "$hs_ns_axf_file" -nt "$hs_axf_file" ]
	            then
    	            echo "==== Encrypt & Sign TZ $firmware_img_file... ===="
	                $ELF2BIN secure $sec_op=fw $key_private_json $key_public_json $enc_fw_tz_json $firmware_img_file $firmware_img_file
            	else
    	            echo "==== Encrypt & Sign NTZ $firmware_img_file... ===="
	                $ELF2BIN secure $sec_op=fw $key_private_json $key_public_json $enc_fw_json $firmware_img_file $firmware_img_file
            	fi
            fi
        else
	        echo "$firmware_img_file not found."
        fi
    else
        echo "==== $sec_img is not match... ===="
    fi
    set -e
}


echo "==== Generate trust $certificate_img_file... ===="
key_certificate_sign

# Generate trust boot image
if [ $secure_img_en -eq 1 ]
then
	#echo "==== Generate secure $certificate_img_file... ===="
	#key_certificate_sign
	echo "==== Generate trust $partition_img_file... ===="
	img_sign $partition_img_file $sec_op_hash_sign
	echo "==== Generate trust $boot_img_file... ===="
	img_sign $boot_img_file $sec_op_hash_sign
	echo "==== Generate trust $firmware_img_file ===="
	img_sign $firmware_img_file $sec_op_hash_sign

# Generate secure enc image with hash check image
elif [ $secure_img_en -eq 2 ]
then
	echo "==== Generate plain check $partition_img_file... ===="
	img_sec_op $partition_img_file $sec_op_hash
	echo "==== Generate cipher check $boot_img_file... ===="
	img_sec_op $boot_img_file $sec_op_enc_hash
	echo "==== Generate cipher check $firmware_img_file ===="
	img_sec_op $firmware_img_file $sec_op_enc_hash

# Generate secure enc with trust boot image
elif [ $secure_img_en -eq 3 ]
then
	echo "==== Generate trust $partition_img_file... ===="
	img_sec_op $partition_img_file $sec_op_hash_sign
	echo "==== Generate cipher trust $boot_img_file... ===="
	img_sec_op $boot_img_file $sec_op_enc_hash_sign
	echo "==== Generate cipher trust $firmware_img_file ===="
	img_sec_op $firmware_img_file $sec_op_enc_hash_sign

# Generate plaintext with hash check image
elif [ $secure_img_en -eq 4 ]
then
	echo "==== Generate plain check $partition_img_file... ===="
	img_sec_op $partition_img_file $sec_op_hash
	echo "==== Generate plain check $boot_img_file... ===="
	img_sec_op $boot_img_file $sec_op_hash
	echo "==== Generate plain check $firmware_img_file ===="
	img_sec_op $firmware_img_file $sec_op_hash
fi

# Generate flash binary
update_flash_bin=0

# Default binary file combination
combine_arg="PT_PT=$partition_img_file,CER_TBL=$certificate_tbl_img_file,KEY_CER1=$certificate_img_file"

if  [ "$certificate_tbl_img_file" -nt "$flash_img_file" ] || \
    [ "$certificate_img_file" -nt "$flash_img_file" ] || \
    [ "$isp_iq_img_file" -nt "$flash_img_file" ]; then
    update_flash_bin=1
fi

# Add boot img if exists
if [ -e "$boot_img_file" ]; then
    combine_arg+=",PT_BL_PRI=$boot_img_file"
    if [ "$boot_img_file" -nt "$flash_img_file" ]; then
        update_flash_bin=1
    fi
fi

# Add fw img if exists
if [ -e "$firmware_img_file" ]; then
    combine_arg+=",PT_FW1=$firmware_img_file"
    if [ "$firmware_img_file" -nt "$flash_img_file" ]; then
        update_flash_bin=1
    fi
fi

# Add isp img if exists
if [ "$isp_iq_en" == "1" ]; then
    combine_arg+=",PT_ISP_IQ=$isp_iq_img_file"
    if [ "$isp_iq_img_file" -nt "$flash_img_file" ]; then
        update_flash_bin=1
    fi
fi

# Enable auto-adjust partition table
if [ "$partition_mgr_en" == "1" ]; then
    ./partition_mgr.py $partition_json $combine_arg

    # Update partition table
    chk_gen_partition_bin
    if [ "$partition_img_file" -nt "flash_img_file" ]; then
        update_flash_bin=1
    fi
fi

# Update partition table
if [ "$update_flash_bin" == "1" ]; then
    if [ -e "flash_img_file" ]; then
	echo "==== Update $flash_img_file... ===="
    else
        echo "==== Generate $flash_img_file... ===="
    fi
	$ELF2BIN combine $partition_json $flash_img_file $combine_arg

else
    echo "'$flash_img_file' is up to date"
fi


