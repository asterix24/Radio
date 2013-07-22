#
# User makefile.
# Edit this file to change compiler options and related stuff.
#

# Programmer interface configuration, see http://dev.bertos.org/wiki/ProgrammerInterface for help
radio_PROGRAMMER_TYPE = none
radio_PROGRAMMER_PORT = none

# Files included by the user.
radio_USER_CSRC = \
	$(radio_SRC_PATH)/main.c \
	bertos/cpu/cortex-m3/drv/spi_stm32.c \
	bertos/drv/spi.c \
	bertos/drv/cc1101.c \
	$(radio_SRC_PATH)/hw/hw_cc1101.c \
	#

# Files included by the user.
radio_USER_PCSRC = \
	#

# Files included by the user.
radio_USER_CPPASRC = \
	#

# Files included by the user.
radio_USER_CXXSRC = \
	#

# Files included by the user.
radio_USER_ASRC = \
	#

# Flags included by the user.
radio_USER_LDFLAGS = \
	#

# Flags included by the user.
radio_USER_CPPAFLAGS = \
	#

# Flags included by the user.
radio_USER_CPPFLAGS = \
	-fno-strict-aliasing \
	-fwrapv \
	-Os
	#
