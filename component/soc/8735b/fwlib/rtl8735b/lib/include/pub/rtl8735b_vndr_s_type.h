#ifdef CONFIG_BUILD_LIB
#error ERROR, only for application
#endif

#ifndef _RTL8735B_VNDR_S_TYPE_H_
#define _RTL8735B_VNDR_S_TYPE_H_


/**************************************************************************//**
 * @defgroup VNDR_S_REG_SECURE_WATCH_DOG_TIMER
 * @brief                                            (@ 0X000)
 * @{
 *****************************************************************************/
#define VNDR_S_SHIFT_WDT_TO                          31
#define VNDR_S_BIT_WDT_TO                            ((u32)0x00000001 << 31)                             /*!<R/W1C 0  Watch dog timer timeout. 1 cycle pulse */
#define VNDR_S_SHIFT_WDT_MODE                        30
#define VNDR_S_BIT_WDT_MODE                          ((u32)0x00000001 << 30)                             /*!<R/W 0  1: Reset system, 0: Interrupt CPU */
#define VNDR_S_SHIFT_RSVD1                           29
#define VNDR_S_BIT_RSVD1                             ((u32)0x00000001 << 29)                             /*!<R 0   */
#define VNDR_S_SHIFT_CNT_LIMIT                       25
#define VNDR_S_MASK_CNT_LIMIT                        ((u32)0x0000000F << 25)                             /*!<R/W 0  0: 0x001 1: 0x003 2: 0x007 3: 0x00F 4: 0x01F 5: 0x03F 6: 0x07F 7: 0x0FF 8: 0x1FF 9: 0x3FF 10: 0x7FF 11~15: 0xFFF */
#define VNDR_S_SHIFT_WDT_CLEAR                       24
#define VNDR_S_BIT_WDT_CLEAR                         ((u32)0x00000001 << 24)                             /*!<R/W1P 0  Write 1 to clear timer */
#define VNDR_S_SHIFT_RSVD2                           17
#define VNDR_S_MASK_RSVD2                            ((u32)0x0000007F << 17)                             /*!<R 0   */
#define VNDR_S_SHIFT_WDT_EN_BYTE                     16
#define VNDR_S_BIT_WDT_EN_BYTE                       ((u32)0x00000001 << 16)                             /*!<R/W 0  Set 1 to enable watch dog timer */
#define VNDR_S_SHIFT_VNDR_DIVFACTOR                  0
#define VNDR_S_MASK_VNDR_DIVFACTOR                   ((u32)0x0000FFFF << 0)                              /*!<R/W 1  Dividing factor. Watch dog timer is count with LP 32KHz/(divfactor+1). Minimum dividing factor is 1. */
#define VNDR_S_SHIFT_RSVD3                           0
#define VNDR_S_MASK_RSVD3                            ((u32)0xFFFFFFFF << 0)                              /*!<R 0   */
/** @} */


/**************************************************************************//**
 * @defgroup rtl8735b_VNDR_S
 * @{
 * @brief rtl8735b_VNDR_S Register Declaration
 *****************************************************************************/
typedef struct {
	__IO uint32_t VNDR_S_REG_SECURE_WATCH_DOG_TIMER ;      /*!<   register,  Address offset: 0x000 */
	__IO uint32_t RESV0[28] ;
} VNDR_S_TypeDef;
/** @} */

#endif
