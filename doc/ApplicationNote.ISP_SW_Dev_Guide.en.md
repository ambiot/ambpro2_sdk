# 1. Overview

## 1.1 Module Description

The ISP module analyses and processes data inputted from video source,
sets up associated video parameters, and conducts camera tuning to
realize various functions such as black level correction, lens
correction, 2A and 2D/3D noise reduction, CCM, and Gamma.

Amebapro2 support customized image quality tuning throught desgined API.
So user can adjust image quality based on specific IQ configuration
after streaming. When user is designed customized level, they can also
set as initial configuration when steaming reopen. In order to reduce
trial time for this tuning flow, we also designed command line method
and will be introduced in following chapter.
## 1.2 Flow Chart

## 1.3 Keyword Description

📌 **AT command**

Runtime commands (Could be applied after streaming), user can use
commands through uart to adjust image quality based on current image
quality parameter.

✍️***For Example:***

```
ATIC=set_flag,Control-ID,value
```
<table>
	<tbody>
	<tr class="odd">
	<th>Read</td>
	<th>Write</td>
	</tr>
	<tr class="even">
	<td>Set flag=0</td>
	<td>Set flag=0</td>
	</tr>
	</tbody>
</table>

👉***Read*** the value of `0xF002` parameter.

```diff
# ATIC=0,0xF002
[isp ctrl] set_flag:0 id:61442.
[VOE]isp_ctrl 0x0098f902 id 61442
result 0x00000000 0 
	
[MEM] After do cmd, available heap 82537408
````

👉***Write*** the value of `0xF002` parameter.

```diff
# ATIC=0,0xF002
[isp ctrl] set_flag:0 id:61442.
[VOE]isp_ctrl 0x0098f902 id 61442
result 0x00000000 0 

[MEM] After do cmd, available heap 82537408
```
📌 **ISP**

Acronym for Image Signal Processing. The ISP unit is responsible for
brightness adjustment, color rendering, and noise reduction…etc.

📌 **NR**

Acronym for Noise Reduction. NR include 2DNR and 3DNR.

2DNR: Average an image pixel to the surrounding image pixels to reduce
noise. This, however, comes with the disadvantage that the picture will
be blurred.

3DNR: 3DNR provides time-domain processing. In contrast to 2DNR, which
focuses on single frame only, 3DNR takes into consideration the
time-domain relationship between frames and averages image pixels on a
time-domain basis.

📌 **LDC**

Acronym for Lens Distortion Correction.

📌 **WDR**

Acronym for Wide Dynamic Range. High dynamic range impact by single
frame processing.

# 2. Ameba Pro2 ISP Control API

## 2.1 isp_set_brightness

- **Object**

This function is used to set the value of brightness parameter.

- **Syntax**

```
isp_set_brightness(int val);
```
- **Description**

Call this function to set the value of brightness parameter.

- **Parameter**

<table>
<tbody>
<tr class="odd">
<th>Parameter Name</td>
<th>Description</td>
</tr>
<tr class="even">
<td>val</td>
<td><p>Value = -64~64 </p>
<p>(Default=0)</p></td>
</tr>
</tbody>
</table>

- **AT command**

```
ATIC=1,0x0000,value
``` 

- **Requirement**
  - Header File: isp_ctrl_api.h

## 2.2 isp_get_brightness

- **Object**

This function is used to get the value of brightness parameters.

- **Syntax**

```
isp_get_brightness(int *pval); 
```
 
- **Description**

Call this function to get the value of brightness parameter.

- **Parameter**s

<table>
<tbody>
<tr class="odd">
<th>Parameter Name</td>
<th>Description</td>
</tr>
<tr class="even">
<td>*pval</td>
<td><p>Value = -64~64 </p>
<p>(Default=0)</p></td>
</tr>
</tbody>
</table>

- **AT command**

```
ATIC=0,0x0000
```
 
- **Requirement**
  - Header File: isp_ctrl_api.h

## 2.3 isp_set_contrast

- **Object**

This function is used to set the value of contrast parameter.

- **Syntax**

```
isp_set_contrast(int val);
``` 

- **Description**

Call this function to set the value of contrast parameter.

- **Parameter**

<table>
<tbody>
<tr class="odd">
<th>Parameter Name</td>
<th>Description</td>
</tr>
<tr class="even">
<td>val</td>
<td><p>Value =0~100 </p>
<p>(Default=50)</p></td>
</tr>
</tbody>
</table>

- **AT command**

```
ATIC=1,0x0001,value 
```

- **Requirement**
  - Header File: isp_ctrl_api.h

## 2.4 isp_get_contrast

- **Object**

This function is used to get the value of brightness parameters.

- **Syntax**

 ```
 isp_get_contrast(int *pval);
 ```   

- **Description**

Call this function to get the value of contrast parameter.

- **Parameter**s

<table>
<tbody>
<tr class="odd">
<th>Parameter Name</td>
<th>Description</td>
</tr>
<tr class="even">
<td>*pval</td>
<td><p>Value = 0~100 </p>
<p>(Default=50)</p></td>
</tr>
</tbody>
</table>

- **AT command**

```
ATIC=0,0x0001 
```

- **Requirement**
  - Header File: isp_ctrl_api.h

## 2.5 isp_set_saturation

- **Object**

This function is used to set the value of saturation parameter.

- **Syntax**

```
isp_set_saturation(int val);
```

- **Description**

Call this function to set the value of saturation parameter.

- **Parameter**

<table>
<tbody>
<tr class="odd">
<th>Parameter Name</td>
<th>Description</td>
</tr>
<tr class="even">
<td>val</td>
<td><p>Value =0~100 </p>
<p>(Default=50)</p></td>
</tr>
</tbody>
</table>

- **AT command**

```
ATIC=1,0x0002,value 
```

- **Requirement**
  - Header File: isp_ctrl_api.h

## 2.6 isp_get_saturation

- **Object**

This function is used to get the value of saturation parameter.

- **Syntax**

```
isp_set_saturation(int val);
```  

- **Description**

Call this function to get the value of saturation parameter.

- **Parameter**

<table>
<tbody>
<tr class="odd">
<th>Parameter Name</td>
<th>Description</td>
</tr>
<tr class="even">
<td>*pval</td>
<td><p>Value =0~100 </p>
<p>(Default=50)</p></td>
</tr>
</tbody>
</table>

- **AT command**

```
ATIC=0,0x0002
``` 

- **Requirement**
  - Header File: isp_ctrl_api.h
    
## 2.7 isp_set_awb_ctrl

- **Object**

This function is used to enable AWB.

- **Syntax**

```
isp_set_awb_ctrl(int val);
```
	
- **Description**

Call this function to enable AWB.

- **Parameter**

<table>
<tbody>
<tr class="odd">
<th>Parameter Name</td>
<th>Description</td>
</tr>
<tr class="even">
<td>val</td>
<td><p>Value =1 (Auto)</p>
<p>Value =0 (Manual)</p></td>
</tr>
</tbody>
</table>

- **AT command**

```
ATIC=1,0x000C,value
```

- **Requirement**
  - Header File: isp_ctrl_api.h

## 2.8 isp_get_awb_ctrl

- **Object**

This function is used to get AWB status.

- **Syntax**

```
isp_get_awb_ctrl(int *pval);
```

- **Description**

Call this function to enable AWB.

- **Parameter**

<table>
<tbody>
<tr class="odd">
<th>Parameter Name</td>
<th>Description</td>
</tr>
<tr class="even">
<td>*pval</td>
<td><p>Value =1 (Auto)</p>
<p>Value =0 (Manual)</p></td>
</tr>
</tbody>
</table>

- **AT command**

```
ATIC=0,0x000C
```

- **Requirement**
  - Header File: isp_ctrl_api.h

## 2.9 isp_set_red_balance

- **Object**

This function is used to set the R gain of AWB parameter.

- **Syntax**

```
isp_set_red_balance(int val);
``` 

- **Description**

Call this function to set the R gain of AWB parameter.

- **Parameter**

<table>
<tbody>
<tr class="odd">
<th>Parameter Name</td>
<th>Description</td>
</tr>
<tr class="even">
<td>val</td>
<td><p>Value = 256~2047 </p>
<p>Unit = 256(1x)</p></td>
</tr>
</tbody>
</table>

- **AT command**

```
ATIC=1,0x000E,value
```

- **Requirement**
  - Header File: isp_ctrl_api.h

## 2.10 isp_get_red_balance

- **Object**

This function is used to get the R gain of AWB parameter.

- **Syntax**

```
isp_get_red_balance(int *pval);
```

- **Description**

Call this function to get the R gain of AWB parameter.

- **Parameter**

<table>
<tbody>
<tr class="odd">
<th>Parameter Name</td>
<th>Description</td>
</tr>
<tr class="even">
<td>*pval</td>
<td><p>Value = 256~2047 </p>
<p>Unit = 256(1x)</p></td>
</tr>
</tbody>
</table>

- **AT command**

```
ATIC=0,0x000E
```

- **Requirement**
  - Header File: isp_ctrl_api.h

## 2.11 isp_set_blue_balance

- **Object**

This function is used to set the B gain of AWB parameter.

- **Syntax**

```
isp_set_blue_balance(int val);
```

- **Description**

Call this function to set the B gain of AWB parameter.

- **Parameter**

<table>
<tbody>
<tr class="odd">
<th>Parameter Name</td>
<th>Description</td>
</tr>
<tr class="even">
<td>val</td>
<td><p>Value = 256~2047 </p>
<p>Unit = 256(1x)</p></td>
</tr>
</tbody>
</table>

- **AT command**

```
ATIC=1,0x000F,value
```

- **Requirement**
  - Header File: isp_ctrl_api.h

## 2.12 isp_get_blue_balance

- **Object**

This function is used to get the B gain of AWB parameter.

- **Syntax**

```
isp_get_blue_balance(int *pval);
```
	
- **Description**

Call this function to get the B gain of AWB parameter.

- **Parameter**

<table>
<tbody>
<tr class="odd">
<th>Parameter Name</td>
<th>Description</td>
</tr>
<tr class="even">
<td>*pval</td>
<td><p>Value = 256~2047 </p>
<p>Unit = 256(1x)</p></td>
</tr>
</tbody>
</table>

- **AT command**

```
ATIC=0,0x000F
```

- **Requirement**
  - Header File: isp_ctrl_api.h

## 2.13 isp_set_gamma

- **Object**

This function is used to set the parameter value of RGB Gamma curve.

- **Syntax**

```
isp_set_gamma(int val);
```

- **Description**

Call this function to set the parameter value of RGB Gamma curve.

- **Parameter**

<table>
<tbody>
<tr class="odd">
<th>Parameter Name</td>
<th>Description</td>
</tr>
<tr class="even">
<td>val</td>
<td><p>Value = 100~500</p>
<p>(Default=300) </p></td>
</tr>
</tbody>
</table>

- **AT command**

```
ATIC=1,0x0010,value
```

- **Requirement**
  - Header File: isp_ctrl_api.h

## 2.14 isp_get_gamma

- **Object**

This function is used to get the parameter value of RGB Gamma curve.

- **Syntax**

```
isp_get_gamma(int *pval);
```

- **Description**

Call this function to get the parameter value of RGB Gamma curve.

- **Parameter**

<table>
<tbody>
<tr class="odd">
<th>Parameter Name</td>
<th>Description</td>
</tr>
<tr class="even">
<td>*pval</td>
<td><p>Value = 100~500</p>
<p>(Default=300)</p></td>
</tr>
</tbody>
</table>

- **AT command**

```
ATIC=0,0x0010
```

- **Requirement**
  - Header File: isp_ctrl_api.h


## 2.15 isp_set_exposure_time

- **Object**

This function is used to set the parameter value of exposure time.

- **Syntax**

```
isp_set_exposure_time(int val);
```

- **Description**

Call this function to set the parameter value of exposure time.

- **Parameter**

<table>
<tbody>
<tr class="odd">
<th>Parameter Name</td>
<th>Description</td>
</tr>
<tr class="even">
<td>val</td>
<td><p>Value = 30~1000000</p>
<p>Unit = us</p>
<p>Max = base on reciprocal of fps </p></td>
</tr>
</tbody>
</table>

- **AT command**

```
ATIC=1,0x0011,value
```

- **Requirement**
  - Header File: isp_ctrl_api.h


## 2.16 isp_get_exposure_time

- **Object**

This function is used to get the parameter value of exposure time.

- **Syntax**

 ```
isp_get_exposure_time(int *pval);
```

- **Description**

Call this function to get the parameter value of exposure time.

- **Parameter**

<table>
<tbody>
<tr class="odd">
<th>Parameter Name</td>
<th>Description</td>
</tr>
<tr class="even">
<td>*pval</td>
<td><p>Value = 30~1000000</p>
<p>Unit = us</p>
<p>Max = base on reciprocal of fps </p></td>
</tr>
</tbody>
</table>

- **AT command**

```
ATIC=0,0x0011
```

- **Requirement**
  - Header File: isp_ctrl_api.h


## 2.17 isp_set_exposure_mode

- **Object**

This function is used to set the parameter value of exposure modes.

- **Syntax**

```
isp_set_exposure_mode(int val);
```

- **Description**

Call this function to set the parameter value of exposure modes.

- **Parameter**

<table>
<tbody>
<tr class="odd">
<th>Parameter Name</td>
<th>Description</td>
</tr>
<tr class="even">
<td>val</td>
<td><p>Value = 1 (Auto)</p>
<p>Value = 0 (Manual)</p></td>
</tr>
</tbody>
</table>

- **AT command**

```
ATIC=1,0x0012,value
```

- **Requirement**
  - Header File: isp_ctrl_api.h

## 2.18 isp_get_exposure_mode

- **Object**

This function is used to get the parameter value of exposure modes.

- **Syntax**

```
isp_get_exposure_mode(int *pval);
```

- **Description**

Call this function to get the parameter value of exposure modes.

- **Parameter**

<table>
<tbody>
<tr class="odd">
<th>Parameter Name</td>
<th>Description</td>
</tr>
<tr class="even">
<td>*pval</td>
<td><p>Value = 1 (Auto)</p>
<p>Value = 0 (Manual)</p></td>
</tr>
</tbody>
</table>

- **AT command**

```
ATIC=0,0x0012
```

- **Requirement**
  - Header File: isp_ctrl_api.h

## 2.19 isp_set_ae_gain

- **Object**

This function is used to set the parameter value of AE gain.

- **Syntax**

```
isp_set_ae_gain(int val);
```

- **Description**

Call this function to set the parameter value of AE gain.

- **Parameter**

<table>
<tbody>
<tr class="odd">
<th>Parameter Name</td>
<th>Description</td>
</tr>
<tr class="even">
<td>val</td>
<td><p>Value = 256~32768</p>
<p>Unit = 256 (1x)</p></td>
</tr>
</tbody>
</table>

- **AT command**

```
ATIC=1,0x0013,value
```

- **Requirement**
  - Header File: isp_ctrl_api.h

## 2.20 isp_get_ae_gain

- **Object**

This function is used to get the parameter value of AE gain.

- **Syntax**

```
isp_get_ae_gain(int *pval);
```

- **Description**

Call this function to set the parameter value of AE gain.

- **Parameter**

<table>
<tbody>
<tr class="odd">
<th>Parameter Name</td>
<th>Description</td>
</tr>
<tr class="even">
<td>*pval</td>
<td><p>Value = 256 ~ 32768</p>
<p>Unit = 256 (1x)</p></td>
</tr>
</tbody>
</table>

- **AT command**

```
ATIC=1,0x0013,value
```

- **Requirement**
  - Header File: isp_ctrl_api.h

## 2.21 isp_set_power_line_freq

- **Object**

This function is used to set the value of AE power frequency detection.

- **Syntax**

```
isp_set_power_line_freq(int val);
```

- **Description**

Call this function to set the value of AE power frequency detection,
50Hz and 60Hz are supported.

- **Parameter**

<table>
<tbody>
<tr class="odd">
<th>Parameter Name</td>
<th>Description</td>
</tr>
<tr class="even">
<td>val</td>
<td><p>Value = 0 ( Disable)</p>
<p>Value = 1 ( 50Hz)</p>
<p>Value = 2 ( 60Hz)</p>
<p>Value = 3 ( Auto) </p></td>
</tr>
</tbody>
</table>

- **AT command**

```
ATIC=1,0x0018,value
```

- **Requirement**
  - Header File: isp_ctrl_api.h

## 2.22 isp_get_power_line_freq

- **Object**

This function is used to get the value of AE power frequency detection.

- **Syntax**

```
isp_get_power_line_freq(int *pval);
```

- **Description**

Call this function to get the value of AE power frequency detection,
50Hz and 60Hz are supported.

- **Parameter**

<table>
<tbody>
<tr class="odd">
<th>Parameter Name</td>
<th>Description</td>
</tr>
<tr class="even">
<td>val</td>
<td><p>Value = 0 ( Disable)</p>
<p>Value = 1 ( 50Hz)</p>
<p>Value = 2 ( 60Hz)</p>
<p>Value = 3 ( Auto) </p></td>
</tr>
</tbody>
</table>

- **AT command**

```
ATIC=0,0x0018
```

- **Requirement**
  - Header File: isp_ctrl_api.h

## 2.23 isp_set_wb_temperature

- **Object**

This function is used to set the parameter value of color temperature.

- **Syntax**

```
isp_set_wb_temperature(int val);
```

- **Description**

Call this function to set the parameter value of color temperature while
AWB disable.

- **Parameter**

<table>
<tbody>
<tr class="odd">
<th>Parameter Name</td>
<th>Description</td>
</tr>
<tr class="even">
<td>val</td>
<td><p>Value = 1000~10000</td>
</tr>
</tbody>
</table>

- **AT command**

```
ATIC=1,0x001A,value
```

- **Requirement**
  - Header File: isp_ctrl_api.h

## 2.24 isp_get_wb_temperature

- **Object**

This function is used to get the parameter value of color temperature.

- **Syntax**

```
isp_get_wb_temperature(int *pval);
```

- **Description**

Call this function to get the parameter value of color temperature.

- **Parameter**

<table>
<tbody>
<tr class="odd">
<th>Parameter Name</td>
<th>Description</td>
</tr>
<tr class="even">
<td>val</td>
<td><p>Value = 1000~10000</td>
</tr>
</tbody>
</table>

- **AT command**

```
ATIC=0,0x001A
```

- **Requirement**
  - Header File: isp_ctrl_api.h

## 2.25 isp_set_sharpness

- **Object**

This function is used to set the value of sharpness parameter.

- **Syntax**

```
isp_set_sharpness(int val);
```

- **Description**

Call this function to set the value of sharpness parameter.

- **Parameter**

<table>
<tbody>
<tr class="odd">
<th>Parameter Name</td>
<th>Description</td>
</tr>
<tr class="even">
<td>val</td>
<td><p>Value = 0~100</p>
<p>(Default = 50)</p></td>
</tr>
</tbody>
</table>

- **AT command**

```
ATIC=1,0x001B,value
```

- **Requirement**
  - Header File: isp_ctrl_api.h

## 2.26 isp_get_sharpness

- **Object**

This function is used to get the value of sharpness parameter.

- **Syntax**

```
isp_get_sharpness(int *pval);
```

- **Description**

Call this function to get the value of sharpness parameter.

- **Parameter**

<table>
<tbody>
<tr class="odd">
<th>Parameter Name</td>
<th>Description</td>
</tr>
<tr class="even">
<td>val</td>
<td><p>Value = 0~100</p>
<p>(Default = 50)</p></td>
</tr>
</tbody>
</table>

- **AT command**

```
ATIC=0,0x001B
```

- **Requirement**
  - Header File: isp_ctrl_api.h

## 2.27 isp_set_green_balance

- **Object**

This function is used to set the G gain value of AWB parameter.

- **Syntax**

```
isp_set_green_balance(int val);
```

- **Description**

Call this function to set the G gain value of AWB parameter.

- **Parameter**

<table>
<tbody>
<tr class="odd">
<th>Parameter Name</td>
<th>Description</td>
</tr>
<tr class="even">
<td>val</td>
<td><p>Value = 256~2047</p>
<p>Unit = 256 (1x)</p></td>
</tr>
</tbody>
</table>

- **AT command**

```
ATIC=1,0xF000,value
```

- **Requirement**
  - Header File: isp_ctrl_api.h

## 2.28 isp_get_green_balance

- **Object**

This function is used to get the G gain value of AWB parameter.

- **Syntax**

```
isp_get_green_balance(int *pval);
```

- **Description**

Call this function to get the G gain value of AWB parameter.

- **Parameter**

<table>
<tbody>
<tr class="odd">
<th>Parameter Name</td>
<th>Description</td>
</tr>
<tr class="even">
<td>*pval</td>
<td><p>Value = 256~2047</p>
<p>Unit = 256 (1x)</p></td>
</tr>
</tbody>
</table>

- **AT command**

```
ATIC=0,0xF000
```
- **Requirement**
  - Header File: isp_ctrl_api.h

## 2.29 isp_set_day_night

- **Object**

This function is used to set the parameter value of Day/Night detection
result.

- **Syntax**

```
isp_set_day_night(int val);
```

- **Description**

Call this function to set the parameter value of Day/Night detection
result.

- **Parameter**

<table>
<tbody>
<tr class="odd">
<th>Parameter Name</td>
<th>Description</td>
</tr>
<tr class="even">
<td>val</td>
<td><p>Value = 0(Day)</p>
<p>Value = 1(Night)</p>
<p>Value = 2(Others)</p></td>
</tr>
</tbody>
</table>

- **AT command**

```
ATIC=1,0xF002,value
```
	
- **Requirement**
  - Header File: isp_ctrl_api.h

## 2.30 isp_get_day_night

- **Object**

This function is used to get the parameter value of Day/Night detection
result.

- **Syntax**

```
isp_get_day_night(int *pval);
```
	
- **Description**

Call this function to get the parameter value of Day/Night detection
result.

- **Parameter**

<table>
<tbody>
<tr class="odd">
<th>Parameter Name</td>
<th>Description</td>
</tr>
<tr class="even">
<td>*pval</td>
<td><p>Value = 0(Day)</p>
<p>Value = 1(Night)</p>
<p>Value = 2(Others)</p></td>
</tr>
</tbody>
</table>

- **AT command**

```
ATIC=0,0xF002
```
	
- **Requirement**
  - Header File: isp_ctrl_api.h

## 2.31 isp_set_dynamic_iq

- **Object**

This function is used to set the value of dynamic IQ enable.

- **Syntax**

```
isp_set_dynamic_iq(int val);
```

- **Description**

Call this function to set the value of dynamic IQ enable.

- **Parameter**

<table>
<tbody>
<tr class="odd">
<th>Parameter Name</td>
<th>Description</td>
</tr>
<tr class="even">
<td>val</td>
<td><p>Value = 0(Disable)</p>
<p>Value = 1(Enable)</p></td>
</tr>
</tbody>
</table>

- **AT command**

```
ATIC=1,0xF004,value
```

- **Requirement**
  - Header File: isp_ctrl_api.h

## 2.32 isp_get_dynamic_iq

- **Object**

This function is used to set the value of dynamic IQ enable.

- **Syntax**

```
isp_get_dynamic_iq(int *pval);
```

- **Description**

Call this function to set the value of dynamic IQ enable.

- **Parameter**

<table>
<tbody>
<tr class="odd">
<th>Parameter Name</td>
<th>Description</td>
</tr>
<tr class="even">
<td>*pval</td>
<td><p>Value = 0(Disable)</p>
<p>Value = 1(Enable)</p></td>
</tr>
</tbody>
</table>

- **AT command**

```
ATIC=0,0xF004
```

- **Requirement**
  - Header File: isp_ctrl_api.h

## 2.33 isp_set_tnr

- **Object**

This function is used to enable 3DNR.

- **Syntax**

```
isp_set_tnr(int val);
```

- **Description**

Call this function to set the value of 3DNR status.

- **Parameter**

<table>
<tbody>
<tr class="odd">
<th>Parameter Name</td>
<th>Description</td>
</tr>
<tr class="even">
<td>val</td>
<td><p>Value = 0(Disable)</p>
<p>Value = 1(Enable)</p></td>
</tr>
</tbody>
</table>

- **AT command**

```
ATIC=1,0xF005,value
```

- **Requirement**
  - Header File: isp_ctrl_api.h

## 2.34 isp_get_tnr

- **Object**

This function is used to get the value of 3DNR status.

- **Syntax**

```
isp_get_tnr(int *pval);
```

- **Description**

Call this function to get the value of 3DNR status.

- **Parameter**

<table>
<tbody>
<tr class="odd">
<th>Parameter Name</td>
<th>Description</td>
</tr>
<tr class="even">
<td>*pval</td>
<td><p>Value = 0(Disable)</p>
<p>Value = 1(Enable)</p></td>
</tr>
</tbody>
</table>

- **AT command**

```
ATIC=0,0xF005
```

- **Requirement**
  - Header File: isp_ctrl_api.h

## 2.35 isp_set_ldc

- **Object**

This function is used to enable LDC.

- **Syntax**

```
isp_set_ldc(int val);
```

- **Description**

Call this function to set the value of LDC enable.

- **Parameter**

<table>
<tbody>
<tr class="odd">
<th>Parameter Name</td>
<th>Description</td>
</tr>
<tr class="even">
<td>val</td>
<td><p>Value = 0(Disable)</p>
<p>Value = 1(Enable)</p></td>
</tr>
</tbody>
</table>

- **AT command**

```
ATIC=1,0xF008,value
```

- **Requirement**
  - Header File: isp_ctrl_api.h

## 2.36 isp_get_ldc

- **Object**

This function is used to get the value of LDC status.

- **Syntax**

```
isp_get_ldc(int *pval);
```

- **Description**

Call this function to get the value of LDC status.

- **Parameter**

<table>
<tbody>
<tr class="odd">
<th>Parameter Name</td>
<th>Description</td>
</tr>
<tr class="even">
<td>*pval</td>
<td><p>Value = 0(Disable)</p>
<p>Value = 1(Enable)</p></td>
</tr>
</tbody>
</table>

- **AT command**

```
ATIC=0,0xF008
```

- **Requirement**
  - Header File: isp_ctrl_api.h

## 2.37 isp_set_gray_mode

- **Object**

This function is used to set the value of gray mode enable.

- **Syntax**

```
isp_set_gray_mode(int val);
```

- **Description**

Call this function to set the value of gray mode enable.

- **Parameter**

<table>
<tbody>
<tr class="odd">
<th>Parameter Name</td>
<th>Description</td>
</tr>
<tr class="even">
<td>val</td>
<td><p>Value = 0(RGB)</p>
<p>Value = 1(Gray)</p></td>
</tr>
</tbody>
</table>

- **AT command**

```
ATIC=1,0xF009,value
```

- **Requirement**
  - Header File: isp_ctrl_api.h

## 2.38 isp_get_gray_mode

- **Object**

This function is used to get the value of gray mode status.

- **Syntax**

```
isp_set_gray_mode(int *pval);
```

- **Description**

Call this function to set the value of gray mode status.

- **Parameter**

<table>
<tbody>
<tr class="odd">
<th>Parameter Name</td>
<th>Description</td>
</tr>
<tr class="even">
<td>*pval</td>
<td><p>Value = 0(RGB)</p>
<p>Value = 1(Gray)</p></td>
</tr>
</tbody>
</table>

- **AT command**

```
ATIC=0,0xF009
```

- **Requirement**
  - Header File: isp_ctrl_api.h

## 2.39 isp_set_wdr_mode

- **Object**

This function is used to set the value of wdr mode switch.

- **Syntax**

```
isp_set_wdr_mode(int val);
```

- **Description**

Call this function to set the value of wdr mode switch.

- **Parameter**

<table>
<tbody>
<tr class="odd">
<th>Parameter Name</td>
<th>Description</td>
</tr>
<tr class="even">
<td>val</td>
<td><p>Value = 0 (Disable)</p>
<p>Value = 1 (Manual)</p>
<p>Value = 2 (Auto)</p></td>
</tr>
</tbody>
</table>

- **AT command**

```
ATIC=1,0xF00C,value
```

- **Requirement**
  - Header File: isp_ctrl_api.h

## 2.40 isp_get_wdr_mode

- **Object**

This function is used to get the value of wdr mode status

- **Syntax**

```
isp_set_wdr_mode(int *pval);
```

- **Description**

Call this function to get the value of wdr mode status.

- **Parameter**

<table>
<tbody>
<tr class="odd">
<th>Parameter Name</td>
<th>Description</td>
</tr>
<tr class="even">
<td>*pval</td>
<td><p>Value = 0 (Disable)</p>
<p>Value = 1 (Manual)</p>
<p>Value = 2 (Auto)</p></td>
</tr>
</tbody>
</table>

- **AT command**

```
ATIC=0,0xF00C
```

- **Requirement**
  - Header File: isp_ctrl_api.h

## 2.41 isp_set_wdr_level

- **Object**

This function is used to set the parameter value of WDR level.

- **Syntax**

```
isp_set_wdr_level(int val);
```

- **Description**

Call this function to set the parameter value of WDR level.

- **Parameter**

<table>
<tbody>
<tr class="odd">
<th>Parameter Name</td>
<th>Description</td>
</tr>
<tr class="even">
<td>val</td>
<td><p>Value =0~100</td>
</tr>
</tbody>
</table>

- **AT command**

```
ATIC=1,0xF00D,value
```

- **Requirement**
  - Header File: isp_ctrl_api.h

## 2.42 isp_get_wdr_level

- **Object**

This function is used to get the parameter value of WDR level.

- **Syntax**

```
isp_get_wdr_level(int *pval);
```

- **Description**

Call this function to get the parameter value of WDR level.

- **Parameter**

<table>
<tbody>
<tr class="odd">
<th>Parameter Name</td>
<th>Description</td>
</tr>
<tr class="even">
<td>*pval</td>
<td><p>Value =0~100</td>
</tr>
</tbody>
</table>

- **AT command**

```
ATIC=0,0xF00D
```

- **Requirement**
  - Header File: isp_ctrl_api.h

## 2.43 isp_set_orientation

- **Object**

This function is used to set the parameter value of Mirror-Flip.

- **Syntax**

```
isp_set_orientation(int val);
```

- **Description**

Call this function to set the parameter value of Mirror-Flip.

- **Parameter**

<table>
<tbody>
<tr class="odd">
<th>Parameter Name</td>
<th>Description</td>
</tr>
<tr class="even">
<td>val</td>
<td><p>Value = 0 ~ 3</td>
</tr>
</tbody>
</table>

- **AT command**

```
ATIC=1,0xF020,value
```

- **Requirement**
  - Header File: isp_ctrl_api.h

## 2.44 isp_get_orientation

- **Object**

This function is used to get the parameter value of Mirror-Flip.

- **Syntax**

```
isp_get_orientation(int *pval);
```

- **Description**

Call this function to get the parameter value of Mirror-Flip.

- **Parameter**

<table>
<tbody>
<tr class="odd">
<th>Parameter Name</td>
<th>Description</td>
</tr>
<tr class="even">
<td>*pval</td>
<td><p>Value = 0 ~ 3</td>
</tr>
</tbody>
</table>

- **AT command**

```
ATIC=0,0xF020
```

- **Requirement**
  - Header File: isp_ctrl_api.h

## 2.45 isp_set_isp_max_fps

- **Object**

This function is used to set the parameter value of maximum frame rate.

- **Syntax**

```
isp_set_isp_max_fps(int val);
```

- **Description**

Call this function to set the parameter value of maximum frame rate.

- **Parameter**

<table>
<tbody>
<tr class="odd">
<th>Parameter Name</td>
<th>Description</td>
</tr>
<tr class="even">
<td>val</td>
<td><p>Value = 1 ~ 30</td>
</tr>
</tbody>
</table>

- **AT command**

```
ATIC=1,0xF021,value
```

- **Requirement**
  - Header File: isp_ctrl_api.h

## 2.46 isp_get_isp_max_fps

- **Object**

This function is used to get the parameter value of maximum frame rate.

- **Syntax**

```
isp_get_isp_max_fps(int *pval);
```

- **Description**

Call this function to get the parameter value of maximum frame rate.

- **Parameter**

<table>
<tbody>
<tr class="odd">
<th>Parameter Name</td>
<th>Description</td>
</tr>
<tr class="even">
<td>*pval</td>
<td><p>Value = 1 ~ 30</td>
</tr>
</tbody>
</table>

- **AT command**

```
ATIC=0,0xF021
```

- **Requirement**
  - Header File: isp_ctrl_api.h

## 2.47 isp_set_isp_min_fps

- **Object**

This function is used to set the parameter value of minimum frame rate.

- **Syntax**

```
isp_set_isp_min_fps(int val);
```

- **Description**

Call this function to set the parameter value of minimum frame rate.

- **Parameter**

<table>
<tbody>
<tr class="odd">
<th>Parameter Name</td>
<th>Description</td>
</tr>
<tr class="even">
<td>val</td>
<td><p>Value = 1 ~ 30</td>
</tr>
</tbody>
</table>

- **AT command**

```
ATIC=1,0xF022,value
```

- **Requirement**
  - Header File: isp_ctrl_api.h

## 2.48 isp_get_isp_min_fps

- **Object**

This function is used to get the parameter value of minimum frame rate.

- **Syntax**

```
isp_get_isp_max_fps(int *pval);
```

- **Description**

Call this function to get the parameter value of minimum frame rate.

- **Parameter**

<table>
<tbody>
<tr class="odd">
<th>Parameter Name</td>
<th>Description</td>
</tr>
<tr class="even">
<td>*pval</td>
<td><p>Value = 1 ~ 30</td>
</tr>
</tbody>
</table>

- **AT command**

```
ATIC=0,0xF022
```

- **Requirement**
  - Header File: isp_ctrl_api.h
