#ifdef CONFIG_BUILD_LIB
#error ERROR, only for application
#endif

#ifndef _RTL8735B_AON_TYPE_H_
#define _RTL8735B_AON_TYPE_H_

/**************************************************************************//**
 * @defgroup AON_REG_AON_ISO_CTRL
 * @brief                                            (@ 0X004)
 * @{
 *****************************************************************************/
#define AON_SHIFT_FEPHY_ISO                          2
#define AON_BIT_FEPHY_ISO                            ((u32)0x00000001 << 2)                              /*!<R/W 1  1: Isolation FEPHY digital to analog. */
#define AON_SHIFT_FEPHY_POW_STATE                    1
#define AON_BIT_FEPHY_POW_STATE                      ((u32)0x00000001 << 1)                              /*!<R/W 0  1: Enable Power state */
#define AON_SHIFT_FEPHY_RSTB_L                       0
#define AON_BIT_FEPHY_RSTB_L                         ((u32)0x00000001 << 0)                              /*!<R/W 0  0: Reset */
/** @} */

/**************************************************************************//**
 * @defgroup AON_REG_AON_FUNC_CTRL
 * @brief                                            (@ 0X008)
 * @{
 *****************************************************************************/
#define AON_SHIFT_GPIO_FEN                           6
#define AON_BIT_GPIO_FEN                             ((u32)0x00000001 << 6)                              /*!<R/W 0  1: Enable AON GPIO */
#define AON_SHIFT_COMP_FEN                           5
#define AON_BIT_COMP_FEN                             ((u32)0x00000001 << 5)                              /*!<R/W 0  1: Enable AON Comparator */
/** @} */

/**************************************************************************//**
 * @defgroup AON_REG_AON_CLK_CTRL
 * @brief                                            (@ 0X00C)
 * @{
 *****************************************************************************/
#define AON_SHIFT_INT_CLK_SEL                        30
#define AON_BIT_INT_CLK_SEL                          ((u32)0x00000001 << 30)                             /*!<R/W 0  Comparator interrupt clock source selection, 0:pclk, 1:sclk */
#define AON_SHIFT_GPIO_INTR_CLK                      7
#define AON_BIT_GPIO_INTR_CLK                        ((u32)0x00000001 << 7)                              /*!<R/W 0  0:pclk, 1:sclk */
/** @} */

/**************************************************************************//**
 * @defgroup AON_REG_AON_SRC_CLK_CTRL
 * @brief                                            (@ 0X07C)
 * @{
 *****************************************************************************/
#define AON_SHIFT_GPIO_SCLK_SEL                      2
#define AON_BIT_GPIO_SCLK_SEL                        ((u32)0x00000001 << 2)                              /*!<R/W 0  0: AON LP 32K clock source 1: OSC 128K/4(without SDM?) */
/** @} */

/**************************************************************************//**
 * @defgroup AON_REG_AON_PLL_CTRL0
 * @brief                                            (@ 0X088)
 * @{
 *****************************************************************************/
#define AON_SHIFT_PLL_R2_TUNE_2_0                    8
#define AON_MASK_PLL_R2_TUNE_2_0                     ((u32)0x00000007 << 8)                              /*!<R/W 100  Add 20k per step fine tune BG */

#define AON_SHIFT_PLL_R1_TUNE_2_0                    4
#define AON_MASK_PLL_R1_TUNE_2_0                     ((u32)0x00000007 << 4)                              /*!<R/W 100  Add 200k per step fine tune BG */
/** @} */

/**************************************************************************//**
 * @defgroup AON_REG_AON_SD_CTRL0
 * @brief                                            (@ 0X08C)
 * @{
 *****************************************************************************/
#define AON_SHIFT_POW_SD                             0
#define AON_BIT_POW_SD                               ((u32)0x00000001 << 0)                              /*!<R/W 0  Power on signal detector */
/** @} */

/**************************************************************************//**
 * @defgroup AON_REG_AON_LSFIF_CMD
 * @brief                                            (@ 0X0A8)
 * @{
 *****************************************************************************/
#define AON_SHIFT_AON_LSFIF_POLL                     31
#define AON_BIT_AON_LSFIF_POLL                       ((u32)0x00000001 << 31)                             /*!<R/W 0  LSF (low speed function) register access polling bit. Set this bit to do LSF register read or write transfer depend on BIT_AON_LSFIF_WR. When transfer done, this bit will be clear by HW */
#define AON_SHIFT_LSF_SEL                            28
#define AON_MASK_LSF_SEL                             ((u32)0x00000007 << 28)                             /*!<R/W 0  000: RTC 001: OSC32K SDM 010: XTAL (high speed) with divider 011: AON Comparator 100: RSVD */
#define AON_SHIFT_AON_LSFIF_WR                       27
#define AON_BIT_AON_LSFIF_WR                         ((u32)0x00000001 << 27)                             /*!<R/W 0  LSF register write transfer indicator.1: LSF register write transfer, 0: LSF register read transfer */
#define AON_SHIFT_AON_LSFIF_WE                       23
#define AON_MASK_AON_LSFIF_WE                        ((u32)0x0000000F << 23)                             /*!<R/W 0  LSF register byte write enable. This field is valid during LSF register write transfer, and is ignored during LSF register read transfer */
#define AON_SHIFT_LSFIF_CMD_DUMMY_0                  16
#define AON_MASK_LSFIF_CMD_DUMMY_0                   ((u32)0x0000007F << 16)                             /*!<R/W 0  Dummy */
#define AON_SHIFT_AON_LSFIF_AD                       0
#define AON_MASK_AON_LSFIF_AD                        ((u32)0x0000FFFF << 0)                              /*!<R/W 0  LSF register access address. BIT_AON_LSFIF_AD[15:8]=8'h00: Indirect register base address */
/** @} */


/**************************************************************************//**
 * @defgroup AON_REG_AON_WDT_TIMER
 * @brief                                            (@ 0X0B4)
 * @{
 *****************************************************************************/
#define AON_SHIFT_WDT_EN_BYTE                        0
#define AON_BIT_WDT_EN_BYTE                          ((u32)0x00000001 << 0)                              /*!<R/W 1  Set 1 to enable watch dog timer */
/** @} */

/**************************************************************************//**
 * @defgroup rtl8735b_AON
 * @{
 * @brief rtl8735b_AON Register Declaration
 *****************************************************************************/
typedef struct {
	__O  uint32_t RESV0;
	__IO uint32_t AON_REG_AON_ISO_CTRL ;                   /*!<   register,  Address offset: 0x004 */
	__IO uint32_t AON_REG_AON_FUNC_CTRL ;                  /*!<   register,  Address offset: 0x008 */
	__IO uint32_t AON_REG_AON_CLK_CTRL ;                   /*!<   register,  Address offset: 0x00C */
	__O  uint32_t RESV1[27];
	__IO uint32_t AON_REG_AON_SRC_CLK_CTRL ;               /*!<   register,  Address offset: 0x07C */
	__O  uint32_t RESV2[2];
	__IO uint32_t AON_REG_AON_PLL_CTRL0 ;                  /*!<   register,  Address offset: 0x088 */
	__IO uint32_t AON_REG_AON_SD_CTRL0 ;                   /*!<   register,  Address offset: 0x08C */
	__O  uint32_t RESV3[6];
	__IO uint32_t AON_REG_AON_LSFIF_CMD ;                  /*!<   register,  Address offset: 0x0A8 */
	__IO uint32_t AON_REG_AON_LSFIF_RWD ;                  /*!<   register,  Address offset: 0x0AC */
	__O  uint32_t RESV4;
	__IO uint32_t AON_REG_AON_WDT_TIMER ;                  /*!<   register,  Address offset: 0x0B4 */
	__O  uint32_t RESV5[8];
	__IO uint32_t AON_REG_AON_OTP_SYSCFG5 ;                /*!<   register,  Address offset: 0x0D8 */
	__O  uint32_t RESV6[32];
} AON_TypeDef;
/** @} */

#endif
