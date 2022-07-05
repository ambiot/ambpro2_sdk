#ifdef CONFIG_BUILD_LIB
#error ERROR, only for application
#endif

#ifndef _RTL8735B_SYSON_S_TYPE_H_
#define _RTL8735B_SYSON_S_TYPE_H_


/**************************************************************************//**
 * @defgroup SYSON_S_REG_SYS_SEC_CTRL
 * @brief                                            (@ 0X074)
 * @{
 *****************************************************************************/
#define SYSON_S_SHIFT_FLASH_SEC_KEY_RDY_EN           4
#define SYSON_S_BIT_FLASH_SEC_KEY_RDY_EN             ((u32)0x00000001 << 4)                              /*!<R/W 0  Flash SEC key_ready control bit(For Engine reset wait for key_ready pull high, then latch key values)  */
#define SYSON_S_SHIFT_SCE_KEY_SEL                    2
#define SYSON_S_MASK_SCE_KEY_SEL                     ((u32)0x00000003 << 2)                              /*!<R/W 0  2'b0 : Only key 1 load into Flash SEC 2 key regions 2'b1 : Only key 2 load into Flash SEC 2 key regions 2'b2 : key 1 load into Flash SEC key 1 region; key 2 load into Flash SEC key 2 region 2'b3 : key 1 load into Flash SEC key 2 region; key 2 load into Flash SEC key 1 region  */
/** @} */

/**************************************************************************//**
 * @defgroup SYSON_S_REG_SYS_LPDDR1_CTRL
 * @brief                                            (@ 0X080)
 * @{
 *****************************************************************************/
#define SYSON_S_SHIFT_HS_LPDDR1_CLK_EN               4
#define SYSON_S_BIT_HS_LPDDR1_CLK_EN                 ((u32)0x00000001 << 4)                              /*!<R/W 0  memory control function enable  */
#define SYSON_S_SHIFT_HS_LPDDR1_EN                   0
#define SYSON_S_BIT_HS_LPDDR1_EN                     ((u32)0x00000001 << 0)                              /*!<R/W 0  memory control function enable  */
/** @} */

/**************************************************************************//**
 * @defgroup SYSON_S_REG_SYS_DDRPHY_CTRL
 * @brief                                            (@ 0X084)
 * @{
 *****************************************************************************/
#define SYSON_S_SHIFT_HS_DDRPHY_CRT_CLK_EN           7
#define SYSON_S_BIT_HS_DDRPHY_CRT_CLK_EN             ((u32)0x00000001 << 7)
#define SYSON_S_SHIFT_HS_DDRPHY_RBUS_CLK_EN          6
#define SYSON_S_BIT_HS_DDRPHY_RBUS_CLK_EN            ((u32)0x00000001 << 6)                              /*!<R/W 0  Rbus interface clock gating control */
#define SYSON_S_SHIFT_DDRPHY_VCCON                   4
#define SYSON_S_BIT_DDRPHY_VCCON                     ((u32)0x00000001 << 4)                              /*!<R/W 0  1: enable core power */
#define SYSON_S_SHIFT_DDRPHY_RBUS_EN                 2
#define SYSON_S_BIT_DDRPHY_RBUS_EN                   ((u32)0x00000001 << 2)                              /*!<R/W 0  1: reset RBUS */
#define SYSON_S_SHIFT_HS_DDRPHY_CRT_RST              0
#define SYSON_S_BIT_HS_DDRPHY_CRT_RST                ((u32)0x00000001 << 0)                              /*!<R/W 0  CRT active low asynchronous reset */
/** @} */

/**************************************************************************//**
 * @defgroup SYSON_S_REG_SYS_PLATFORM_CTRL0
 * @brief                                            (@ 0X09C)
 * @{
 *****************************************************************************/
#define SYSON_S_SHIFT_LXBUS_CLK_EN                   17
#define SYSON_S_BIT_LXBUS_CLK_EN                     ((u32)0x00000001 << 17)                             /*!<R/W 0  1: Enable LX bus CLK */
#define SYSON_S_SHIFT_LXBUS_EN                       16
#define SYSON_S_BIT_LXBUS_EN                         ((u32)0x00000001 << 16)                             /*!<R/W 0  1: Enable LX bus */
/** @} */

/**************************************************************************//**
 * @defgroup SYSON_S_REG_SYS_OTG_CTRL
 * @brief                                            (@ 0X120)
 * @{
 *****************************************************************************/
#define SYSON_S_SHIFT_USB_IBX2MIPI_EN                22
#define SYSON_S_BIT_USB_IBX2MIPI_EN                  ((u32)0x00000001 << 22)                             /*!<R/W 0  1: Enable IBX to MIPI, DDR and EPHY */
#define SYSON_S_SHIFT_SYS_UABG_EN                    19
#define SYSON_S_BIT_SYS_UABG_EN                      ((u32)0x00000001 << 19)                             /*!<R/W 0  1. Enable bandgap */
#define SYSON_S_SHIFT_SYS_UAHV_EN                    18
#define SYSON_S_BIT_SYS_UAHV_EN                      ((u32)0x00000001 << 18)                             /*!<R/W 0  1: USB PHY analog 3.3V power cut enable */

/** @} */

/**************************************************************************//**
 * @defgroup SYSON_S_REG_SYS_RMII_CTRL
 * @brief                                            (@ 0X128)
 * @{
 *****************************************************************************/
#define SYSON_S_SHIFT_SYS_RMII_SCLK_GEN              2
#define SYSON_S_BIT_SYS_RMII_SCLK_GEN                ((u32)0x00000001 << 2)                              /*!<R/W 0  1: Enable RMII 50MHz */
#define SYSON_S_SHIFT_SYS_RMII_CLK_EN                1
#define SYSON_S_BIT_SYS_RMII_CLK_EN                  ((u32)0x00000001 << 1)                              /*!<R/W 0  1: RMII clock enable */
#define SYSON_S_SHIFT_SYS_RMII_EN                    0
#define SYSON_S_BIT_SYS_RMII_EN                      ((u32)0x00000001 << 0)                              /*!<R/W 0  1: enable RMII IP */
/** @} */


/**************************************************************************//**
 * @defgroup rtl8735b_SYSON_S
 * @{
 * @brief rtl8735b_SYSON_S Register Declaration
 *****************************************************************************/
typedef struct {
	__O  uint32_t RSVD0[29];
	__IO uint32_t SYSON_S_REG_SYS_SEC_CTRL ;               /*!<   register,  Address offset: 0x074 */
	__O  uint32_t RSVD1[2];
	__IO uint32_t SYSON_S_REG_SYS_LPDDR1_CTRL ;            /*!<   register,  Address offset: 0x080 */
	__IO uint32_t SYSON_S_REG_SYS_DDRPHY_CTRL ;            /*!<   register,  Address offset: 0x084 */
	__O  uint32_t RSVD2[5];
	__IO uint32_t SYSON_S_REG_SYS_PLATFORM_CTRL0 ;         /*!<   register,  Address offset: 0x09C */
	__O  uint32_t RSVD3[32];
	__IO uint32_t SYSON_S_REG_SYS_OTG_CTRL ;               /*!<   register,  Address offset: 0x120 */
	__O  uint32_t RSVD4;
	__IO uint32_t SYSON_S_REG_SYS_RMII_CTRL ;              /*!<   register,  Address offset: 0x128 */
	__O  uint32_t RSVD5[13];
} SYSON_S_TypeDef;
/** @} */

#endif
