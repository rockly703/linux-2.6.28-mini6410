/* linux/arch/arm/mach-s5p6440/mach-smdk6440.c
 *
 * Copyright 2008 Openmoko, Inc.
 * Copyright 2008 Simtec Electronics
 *	Ben Dooks <ben@simtec.co.uk>
 *	http://armlinux.simtec.co.uk/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
*/

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/list.h>
#include <linux/timer.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/serial_core.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/i2c.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/partitions.h>
#include <linux/mtd/partitions.h>
#include <linux/module.h>
#include <linux/clk.h>
#include <linux/pwm_backlight.h>
#include <linux/spi/spi.h>

#include <asm/mach/arch.h>
#include <asm/mach/map.h>
#include <asm/mach/irq.h>

#include <mach/hardware.h>
#include <mach/map.h>
#include <mach/regs-mem.h>

#include <asm/irq.h>
#include <asm/mach-types.h>

#include <plat/regs-serial.h>
#include <plat/iic.h>

#include <plat/nand.h>
#include <plat/partition.h>
#include <plat/s5p6440.h>
#include <plat/clock.h>
#include <plat/regs-clock.h>
#include <plat/devs.h>
#include <plat/cpu.h>
#include <plat/ts.h>
#include <plat/adc.h>
#include <plat/pm.h>

#include <plat/regs-rtc.h>

#include <mach/gpio.h>
#include <plat/gpio-cfg.h>

#ifdef CONFIG_USB_SUPPORT
#include <plat/regs-otg.h>
#include <linux/usb/ch9.h>

/* S3C_USB_CLKSRC 0: EPLL 1: CLK_48M */
#define S3C_USB_CLKSRC	1
#define OTGH_PHY_CLK_VALUE      (0x02)  /* UTMI Interface, Cristal, 12Mhz clk for PLL */
#endif

#define UCON S3C2410_UCON_DEFAULT | S3C2410_UCON_UCLK
#define ULCON S3C2410_LCON_CS8 | S3C2410_LCON_PNONE | S3C2410_LCON_STOPB
#define UFCON S3C2410_UFCON_RXTRIG8 | S3C2410_UFCON_FIFOMODE

//static struct samspi_device spidev_b0_cs0;
#if 0
static struct samspi_device ProtocolADriver_b1_cs0;
static struct samspi_device spidev_b0_cs1;
static struct samspi_device ProtocolBDriver_b1_cs1;
static struct samspi_device spidev_b1_cs2;
#endif

static struct spi_board_info __initdata sam_spi_devs[] = {
	[0] = {
		.modalias	 = "spidev", /* Test Interface */
		.mode		 = SPI_MODE_2,	/* CPOL=1, CPHA=0 */
		.max_speed_hz	 = 2468013,
		/* Connected to SPI-0 as 1st Slave */
		.bus_num	 = 0,
		.irq		 = IRQ_SPI0,
		.chip_select	 = 0,
//		.controller_data = (void *)&spidev_b0_cs0,
#if 0
	}, {
		.modalias	= "ProtocolADriver",
		.mode		= SPI_MODE_2,
		.max_speed_hz	= 1357923,
		/* Connected to SPI-1 as 1st Slave */
		.bus_num	= 1,
		.irq		= IRQ_SPI1,
		.chip_select	= 0,
		.controller_data = (void *)&ProtocolADriver_b1_cs0,
	}, {
		.modalias	= "spidev",
		.mode		= SPI_MODE_2,
		.max_speed_hz	= 2357923,
		/* Connected to SPI-0 as 2nd Slave */
		.bus_num	= 0,
		.irq		= IRQ_SPI0,
		.chip_select	= 1,
		.controller_data = (void *)&spidev_b0_cs1,
	}, {
		.modalias	= "ProtocolBDriver",
		.mode		= SPI_MODE_2,
		.max_speed_hz	= 3357923,
		/* Connected to SPI-1 as 2ndst Slave */
		.bus_num	= 1,
		.irq		= IRQ_SPI1,
		.chip_select	= 1,
		.controller_data = (void *)&ProtocolBDriver_b1_cs1,
	}, {
		.modalias	= "spidev",
		.mode		= SPI_MODE_2,
		.max_speed_hz	= 4357923,
		/* Connected to SPI-1 as 3rd Slave */
		.bus_num	= 1,
		.irq		= IRQ_SPI1,
		.chip_select	= 2,
		.controller_data = (void *)&spidev_b1_cs2,
#endif
	},
};

static struct s3c2410_uartcfg smdk6440_uartcfgs[] __initdata = {
	[0] = {
		.hwport	     = 0,
		.flags	     = 0,
		.ucon	     = S3C64XX_UCON_DEFAULT,
		.ulcon	     = S3C64XX_ULCON_DEFAULT,
		.ufcon	     = S3C64XX_UFCON_DEFAULT,
	},
	[1] = {
		.hwport	     = 1,
		.flags	     = 0,
		.ucon	     = S3C64XX_UCON_DEFAULT,
		.ulcon	     = S3C64XX_ULCON_DEFAULT,
		.ufcon	     = S3C64XX_UFCON_DEFAULT,
	},
	[2] = {
		.hwport	     = 2,
		.flags	     = 0,
		.ucon	     = S3C64XX_UCON_DEFAULT,
		.ulcon	     = S3C64XX_ULCON_DEFAULT,
		.ufcon	     = S3C64XX_UFCON_DEFAULT,
	},
	[3] = {
		.hwport	     = 3,
		.flags	     = 0,
		.ucon	     = S3C64XX_UCON_DEFAULT,
		.ulcon	     = S3C64XX_ULCON_DEFAULT,
		.ufcon	     = S3C64XX_UFCON_DEFAULT,
	},	
};

struct map_desc smdk6440_iodesc[] = {};

static struct platform_device *smdk6440_devices[] __initdata = {
#ifdef CONFIG_SMDK6440_SD_CH0
	&s3c_device_hsmmc0,
#endif
#ifdef CONFIG_SMDK6440_SD_CH1
	&s3c_device_hsmmc1,
#endif
#ifdef CONFIG_SMDK6440_SD_CH2
	&s3c_device_hsmmc2,
#endif
	&s3c_device_wdt,
	&s3c_device_rtc,
	&s3c_device_i2c0,
	&s3c_device_i2c1,
	&s3c_device_spi0,
	&s3c_device_spi1,
	&s3c_device_ts,
	&s3c_device_smc911x,
	&s3c_device_lcd,
	&s3c_device_nand,
	&s3c_device_usbgadget,
	&s3c_device_usb_otghcd,

#ifdef CONFIG_S5P64XX_ADC
	&s3c_device_adc,
#endif

#ifdef CONFIG_HAVE_PWM
	&s3c_device_timer[0],
	&s3c_device_timer[1],
#endif
};

static struct i2c_board_info i2c_devs0[] __initdata = {
	{ I2C_BOARD_INFO("24c08", 0x50), },
	//{ I2C_BOARD_INFO("WM8580", 0x10), },
};

static struct i2c_board_info i2c_devs1[] __initdata = {
	{ I2C_BOARD_INFO("24c128", 0x57), },	/* Samsung S524AD0XD1 */
	{ I2C_BOARD_INFO("WM8580", 0x1a), },
	//{ I2C_BOARD_INFO("WM8580", 0x1b), },
};


static struct s3c_ts_mach_info s3c_ts_platform __initdata = {
	.delay 			= 10000,
	.presc 			= 49,
	.oversampling_shift	= 2,
	.resol_bit 		= 12,
	.s3c_adc_con		= ADC_TYPE_2,
};

static struct s3c_adc_mach_info s3c_adc_platform = {
	/* s3c6410 support 12-bit resolution */
	.delay	= 	10000,
	.presc 	= 	49,
	.resolution = 	12,
};

#if defined(CONFIG_HAVE_PWM)
static struct platform_pwm_backlight_data smdk_backlight_data = {
	.pwm_id		= 1,
	.max_brightness	= 255,
	.dft_brightness	= 255,
	.pwm_period_ns	= 78770,
};

static struct platform_device smdk_backlight_device = {
	.name		= "pwm-backlight",
	.dev		= {
		.parent = &s3c_device_timer[1].dev,
		.platform_data = &smdk_backlight_data,
	},
};

static void __init smdk_backlight_register(void)
{
	int ret = platform_device_register(&smdk_backlight_device);
	if (ret)
		printk(KERN_ERR "smdk: failed to register backlight device: %d\n", ret);
}
#else
#define smdk_backlight_register()	do { } while (0)
#endif

void smdk6440_setup_sdhci0 (void);

static void __init smdk6440_map_io(void)
{
	s3c_device_nand.name = "s5p6440-nand";

	s5p64xx_init_io(smdk6440_iodesc, ARRAY_SIZE(smdk6440_iodesc));
	s3c24xx_init_clocks(12000000);
	s3c24xx_init_uarts(smdk6440_uartcfgs, ARRAY_SIZE(smdk6440_uartcfgs));

	smdk6440_setup_sdhci0();
}

static void __init smdk6440_smc911x_set(void)
{
	unsigned int tmp;

	tmp = __raw_readl(S5P64XX_SROM_BW);
	tmp &= ~(S5P64XX_SROM_BW_WAIT_ENABLE1_MASK | S5P64XX_SROM_BW_WAIT_ENABLE1_MASK |
		S5P64XX_SROM_BW_DATA_WIDTH1_MASK);
	tmp |= S5P64XX_SROM_BW_BYTE_ENABLE1_ENABLE | S5P64XX_SROM_BW_WAIT_ENABLE1_ENABLE |
		S5P64XX_SROM_BW_DATA_WIDTH1_16BIT;

	__raw_writel(tmp, S5P64XX_SROM_BW);

	__raw_writel(S5P64XX_SROM_BCn_TACS(0) | S5P64XX_SROM_BCn_TCOS(4) |
			S5P64XX_SROM_BCn_TACC(13) | S5P64XX_SROM_BCn_TCOH(1) |
			S5P64XX_SROM_BCn_TCAH(4) | S5P64XX_SROM_BCn_TACP(6) |
			S5P64XX_SROM_BCn_PMC_NORMAL, S5P64XX_SROM_BC1);
}

static void __init smdk6440_machine_init(void)
{
	s3c_device_nand.dev.platform_data = &s3c_nand_mtd_part_info;

	smdk6440_smc911x_set();

	s3c_i2c0_set_platdata(NULL);
	s3c_i2c1_set_platdata(NULL);

	s3c_ts_set_platdata(&s3c_ts_platform);
	s3c_adc_set_platdata(&s3c_adc_platform);

	i2c_register_board_info(0, i2c_devs0, ARRAY_SIZE(i2c_devs0));
	i2c_register_board_info(1, i2c_devs1, ARRAY_SIZE(i2c_devs1));

	spi_register_board_info(sam_spi_devs, ARRAY_SIZE(sam_spi_devs));

	platform_add_devices(smdk6440_devices, ARRAY_SIZE(smdk6440_devices));

	s5p6440_pm_init();

	smdk_backlight_register();

}

MACHINE_START(SMDK6440, "SMDK6440")
	/* Maintainer: Ben Dooks <ben@fluff.org> */
	.phys_io	= S3C_PA_UART & 0xfff00000,
	.io_pg_offst	= (((u32)S3C_VA_UART) >> 18) & 0xfffc,
	.boot_params	= S5P64XX_PA_SDRAM + 0x100,

	.init_irq	= s5p6440_init_irq,
	.map_io		= smdk6440_map_io,
	.init_machine	= smdk6440_machine_init,
	.timer		= &s3c64xx_timer,
MACHINE_END

#ifdef CONFIG_USB_SUPPORT
/* Initializes OTG Phy. */
void otg_phy_init(void) {

	int err;

	if (gpio_is_valid(S5P64XX_GPN(1))) {
		err = gpio_request(S5P64XX_GPN(1), "GPN");

		if(err)
			printk(KERN_ERR "failed to request GPN1\n");

		gpio_direction_output(S5P64XX_GPN(1), 1);
	}

	writel(readl(S3C_OTHERS)&~S3C_OTHERS_USB_SIG_MASK, S3C_OTHERS);
	writel(0x0, S3C_USBOTG_PHYPWR);		/* Power up */
        writel(OTGH_PHY_CLK_VALUE, S3C_USBOTG_PHYCLK);
	writel(0x1, S3C_USBOTG_RSTCON);

	udelay(50);
	writel(0x0, S3C_USBOTG_RSTCON);
	udelay(50);
}
EXPORT_SYMBOL(otg_phy_init);

/* USB Control request data struct must be located here for DMA transfer */
struct usb_ctrlrequest usb_ctrl __attribute__((aligned(8)));
EXPORT_SYMBOL(usb_ctrl);

/* OTG PHY Power Off */
void otg_phy_off(void) {
	writel(readl(S3C_USBOTG_PHYPWR)|(0x1F<<1), S3C_USBOTG_PHYPWR);
	writel(readl(S3C_OTHERS)&~S3C_OTHERS_USB_SIG_MASK, S3C_OTHERS);

	gpio_free(S5P64XX_GPN(1));
}
EXPORT_SYMBOL(otg_phy_off);

#endif

#if defined(CONFIG_RTC_DRV_S3C)
/* RTC common Function for samsung APs*/
unsigned int s3c_rtc_set_bit_byte(void __iomem *base, uint offset, uint val)
{
	writeb(val, base + offset);

	return 0;
}

unsigned int s3c_rtc_read_alarm_status(void __iomem *base)
{
	return 1;
}

void s3c_rtc_set_pie(void __iomem *base, uint to)
{
	unsigned int tmp;

	tmp = readw(base + S3C2410_RTCCON) & ~S3C_RTCCON_TICEN;

        if (to)
                tmp |= S3C_RTCCON_TICEN;

        writew(tmp, base + S3C2410_RTCCON);
}

void s3c_rtc_set_freq_regs(void __iomem *base, uint freq, uint s3c_freq)
{
	unsigned int tmp;

        tmp = readw(base + S3C2410_RTCCON) & (S3C_RTCCON_TICEN | S3C2410_RTCCON_RTCEN );
        writew(tmp, base + S3C2410_RTCCON);
        s3c_freq = freq;
        tmp = (32768 / freq)-1;
        writel(tmp, base + S3C2410_TICNT);
}

void s3c_rtc_enable_set(struct platform_device *pdev,void __iomem *base, int en)
{
	unsigned int tmp;

	if (!en) {
		tmp = readw(base + S3C2410_RTCCON);
		writew(tmp & ~ (S3C2410_RTCCON_RTCEN | S3C_RTCCON_TICEN), base + S3C2410_RTCCON);
	} else {
		/* re-enable the device, and check it is ok */
		if ((readw(base+S3C2410_RTCCON) & S3C2410_RTCCON_RTCEN) == 0){
			dev_info(&pdev->dev, "rtc disabled, re-enabling\n");

			tmp = readw(base + S3C2410_RTCCON);
			writew(tmp|S3C2410_RTCCON_RTCEN, base+S3C2410_RTCCON);
		}

		if ((readw(base + S3C2410_RTCCON) & S3C2410_RTCCON_CNTSEL)){
			dev_info(&pdev->dev, "removing RTCCON_CNTSEL\n");

			tmp = readw(base + S3C2410_RTCCON);
			writew(tmp& ~S3C2410_RTCCON_CNTSEL, base+S3C2410_RTCCON);
		}

		if ((readw(base + S3C2410_RTCCON) & S3C2410_RTCCON_CLKRST)){
			dev_info(&pdev->dev, "removing RTCCON_CLKRST\n");

			tmp = readw(base + S3C2410_RTCCON);
			writew(tmp & ~S3C2410_RTCCON_CLKRST, base+S3C2410_RTCCON);
		}
	}
}
#endif

