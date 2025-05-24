# path to cube LL HAL
STM_LIBS_PATH	=	$(HOME)/libs/STM32CubeG0/Drivers/

# path to libs_embedded
EMBEDDED_PATH	= $(HOME)/libs/stm32_libs/libs_embedded/

USER_PATH    	= usr/

COMMON_PATH   	= $(EMBEDDED_PATH)/common/
MATH_PATH   	= $(EMBEDDED_PATH)/tmath/
DEVICE_PATH   	= $(EMBEDDED_PATH)/stm32g0xx/

# paths to stm libs : CMSIS, HAL ... 
CMSIS_PATH		= $(STM_LIBS_PATH)/CMSIS/
HAL_PATH		= $(STM_LIBS_PATH)/STM32G0xx_HAL_Driver/



GCCPREFIX  = arm-none-eabi-
OUTPUT     = bin/build


TARGET     = -mcpu=cortex-m0plus -mthumb -DSTM32G031xx -DUSE_HAL_DRIVER

LDSCRIPT   = $(DEVICE_PATH)/device/linker_script.ld

# Includes
INC = -I$(CMSIS_PATH)/Device/ST/STM32G0xx/Include/
INC+= -I$(CMSIS_PATH)/Include/
INC+= -I$(CMSIS_PATH)/Core/Include/
INC+= -I$(HAL_PATH)/Inc/

INC+= -I$(COMMON_PATH)
INC+= -I$(MATH_PATH)
INC+= -I$(DEVICE_PATH)/device/
INC+= -I$(DEVICE_PATH)/drivers/



INC+= -I$(USER_PATH)


# asm sources
ASM_SRCS = $(shell find $(DEVICE_PATH)/device/ -name '*.s')

# C sources
C_SRCS  = $(shell find $(DEVICE_PATH) -name '*.c')
#C_SRCS+= $(shell find $(HAL_PATH)/Src/ -name '*.c' ! -name '*_template.c')

# cpp sources
CXX_SRCS = $(shell find $(USER_PATH) -name '*.cpp')
CXX_SRCS+= $(shell find $(COMMON_PATH) -name '*.cpp')
CXX_SRCS+= $(shell find $(DEVICE_PATH)/drivers -name '*.cpp')



AS 		 = $(GCCPREFIX)gcc
CC       = $(GCCPREFIX)gcc
CXX      = $(GCCPREFIX)g++
LD       = $(GCCPREFIX)g++
OBJCOPY  = $(GCCPREFIX)objcopy
OBJDUMP  = $(GCCPREFIX)objdump
SIZE     = $(GCCPREFIX)size	

	
ASFLAGS	 = $(TARGET) -c	
CCFLAGS  = $(TARGET) $(INC) -c -O2 -Os -Wall -Wextra -Wpedantic -std=c99 -ffreestanding -nostdlib -fdata-sections -ffunction-sections
CXXFLAGS = $(TARGET) $(INC) -c -O2 -Os -Wall -Wextra -Wpedantic -std=c++14 -fno-rtti -fno-exceptions -fdata-sections -ffunction-sections
LDFLAGS = -T$(LDSCRIPT) $(TARGET) -nostdlib -Wl,--gc-sections -lgcc -ffreestanding

ASM_OBJS  = $(ASM_SRCS:.s=.o)	
C_OBJS    = $(C_SRCS:.c=.o)
CXX_OBJS  = $(CXX_SRCS:.cpp=.o)

all: $(OUTPUT)
	$(OBJCOPY) -O ihex $<.elf $(OUTPUT).hex
	$(OBJCOPY) -O binary $<.elf $(OUTPUT).bin
	$(OBJDUMP) --disassemble $<.elf > $(OUTPUT).asm
	$(SIZE) $(OUTPUT).elf

	

$(OUTPUT): $(C_OBJS) $(CXX_OBJS) $(ASM_OBJS)
	$(LD) -o $@.elf $(C_OBJS) $(CXX_OBJS) $(ASM_OBJS) $(LDFLAGS)


%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

%.o: %.c
	$(CC) $(CCFLAGS) $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@



clean:
	rm -f $(C_OBJS) $(CXX_OBJS) $(OUTPUT)*