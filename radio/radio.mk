#
# Wizard autogenerated makefile.
# DO NOT EDIT, use the radio_user.mk file instead.
#

# Constants automatically defined by the selected modules
radio_DEBUG = 1

# Our target application
TRG += radio

radio_PREFIX = "arm-none-eabi-"

radio_SUFFIX = ""

radio_SRC_PATH = radio

radio_HW_PATH = radio

# Files automatically generated by the wizard. DO NOT EDIT, USE radio_USER_CSRC INSTEAD!
radio_WIZARD_CSRC = \
	bertos/cpu/cortex-m3/drv/adc_stm32.c \
	bertos/cpu/cortex-m3/drv/flash_stm32.c \
	bertos/cpu/cortex-m3/drv/i2c_stm32.c \
	bertos/cpu/cortex-m3/drv/ser_stm32.c \
	bertos/cpu/cortex-m3/drv/timer_cm3.c \
	bertos/drv/adc.c \
	bertos/drv/i2c.c \
	bertos/drv/i2c_bitbang.c \
	bertos/drv/ser.c \
	bertos/drv/timer.c \
	bertos/io/kblock.c \
	bertos/io/kfile.c \
	bertos/io/kfile_block.c \
	bertos/mware/event.c \
	bertos/mware/formatwr.c \
	bertos/mware/hex.c \
	#

# Files automatically generated by the wizard. DO NOT EDIT, USE radio_USER_PCSRC INSTEAD!
radio_WIZARD_PCSRC = \
	 \
	#

# Files automatically generated by the wizard. DO NOT EDIT, USE radio_USER_CPPASRC INSTEAD!
radio_WIZARD_CPPASRC = \
	 \
	#

# Files automatically generated by the wizard. DO NOT EDIT, USE radio_USER_CXXSRC INSTEAD!
radio_WIZARD_CXXSRC = \
	 \
	#

# Files automatically generated by the wizard. DO NOT EDIT, USE radio_USER_ASRC INSTEAD!
radio_WIZARD_ASRC = \
	 \
	#

radio_CPPFLAGS = -D'CPU_FREQ=(16000000UL)' -D'ARCH=(ARCH_DEFAULT)' -D'WIZ_AUTOGEN' -I$(radio_HW_PATH) -I$(radio_SRC_PATH) $(radio_CPU_CPPFLAGS) $(radio_USER_CPPFLAGS)

# Automatically generated by the wizard. PLEASE DO NOT EDIT!
radio_LDFLAGS = $(radio_CPU_LDFLAGS) $(radio_WIZARD_LDFLAGS) $(radio_USER_LDFLAGS)

# Automatically generated by the wizard. PLEASE DO NOT EDIT!
radio_CPPAFLAGS = $(radio_CPU_CPPAFLAGS) $(radio_WIZARD_CPPAFLAGS) $(radio_USER_CPPAFLAGS)

# Automatically generated by the wizard. PLEASE DO NOT EDIT!
radio_CSRC = $(radio_CPU_CSRC) $(radio_WIZARD_CSRC) $(radio_USER_CSRC)

# Automatically generated by the wizard. PLEASE DO NOT EDIT!
radio_PCSRC = $(radio_CPU_PCSRC) $(radio_WIZARD_PCSRC) $(radio_USER_PCSRC)

# Automatically generated by the wizard. PLEASE DO NOT EDIT!
radio_CPPASRC = $(radio_CPU_CPPASRC) $(radio_WIZARD_CPPASRC) $(radio_USER_CPPASRC)

# Automatically generated by the wizard. PLEASE DO NOT EDIT!
radio_CXXSRC = $(radio_CPU_CXXSRC) $(radio_WIZARD_CXXSRC) $(radio_USER_CXXSRC)

# Automatically generated by the wizard. PLEASE DO NOT EDIT!
radio_ASRC = $(radio_CPU_ASRC) $(radio_WIZARD_ASRC) $(radio_USER_ASRC)

# CPU specific flags and options, defined in the CPU definition files.
# Automatically generated by the wizard. PLEASE DO NOT EDIT!
radio_CPU_CPPASRC = bertos/cpu/cortex-m3/hw/crt_cm3.S bertos/cpu/cortex-m3/hw/vectors_cm3.S
radio_CPU_CPPAFLAGS = -g -gdwarf-2 -mthumb -mno-thumb-interwork
radio_CPU_CPPFLAGS = -O0 -g3 -gdwarf-2 -mthumb -mno-thumb-interwork -fno-strict-aliasing -fwrapv -fverbose-asm -Ibertos/cpu/cortex-m3/ -D__ARM_STM32F100C4__
radio_CPU_CSRC = bertos/cpu/cortex-m3/hw/init_cm3.c bertos/cpu/cortex-m3/drv/irq_cm3.c bertos/cpu/cortex-m3/drv/gpio_stm32.c bertos/cpu/cortex-m3/drv/clock_stm32.c
radio_PROGRAMMER_CPU = stm32
radio_CPU_LDFLAGS = -mthumb -mno-thumb-interwork -nostartfiles -Wl,--no-warn-mismatch -Wl,-dT bertos/cpu/cortex-m3/scripts/stm32f100c4_rom.ld
radio_STOPFLASH_SCRIPT = bertos/prg_scripts/arm/stopopenocd.sh
radio_CPU = cortex-m3
radio_STOPDEBUG_SCRIPT = bertos/prg_scripts/arm/stopopenocd.sh
radio_DEBUG_SCRIPT = bertos/prg_scripts/arm/debug.sh
radio_FLASH_SCRIPT = bertos/prg_scripts/arm/flash-stm32f100.sh

include $(radio_SRC_PATH)/radio_user.mk
