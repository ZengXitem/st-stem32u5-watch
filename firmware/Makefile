# STM32U575 Firefighter Watch Makefile

# 项目名称
TARGET = firefighter-watch

# 编译器配置
PREFIX = arm-none-eabi-
CC = $(PREFIX)gcc
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S

# MCU配置
CPU = -mcpu=cortex-m33
FPU = -mfpu=fpv5-sp-d16
FLOAT-ABI = -mfloat-abi=hard
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# 源文件目录
BUILD_DIR = build
C_SOURCES = \
Core/Src/main.c \
Core/Src/sensors.c \
Core/Src/wifi_mqtt.c \
Core/Src/stm32u5xx_it.c \
Core/Src/stm32u5xx_hal_msp.c \
Core/Src/system_stm32u5xx.c \
Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal.c \
Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_rcc.c \
Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_gpio.c \
Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_uart.c \
Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_i2c.c \
Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_adc.c \
Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_tim.c \
Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_pwr.c

# ASM文件
ASM_SOURCES = startup_stm32u575xx.s

# 包含目录
C_INCLUDES = \
-ICore/Inc \
-IDrivers/STM32U5xx_HAL_Driver/Inc \
-IDrivers/STM32U5xx_HAL_Driver/Inc/Legacy \
-IDrivers/CMSIS/Device/ST/STM32U5xx/Include \
-IDrivers/CMSIS/Include

# 编译标志
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections
CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

# 调试标志
DEBUG = 1
ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif

# 优化级别
OPT = -Og

# 宏定义
C_DEFS = \
-DUSE_HAL_DRIVER \
-DSTM32U575xx

# 链接器脚本
LDSCRIPT = STM32U575ZITxQ_FLASH.ld

# 库
LIBS = -lc -lm -lnosys
LIBDIR =
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

# 默认目标
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin

# 目标文件列表
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@

$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@

$(BUILD_DIR):
	mkdir $@

# 清理
clean:
	-rm -fR $(BUILD_DIR)

# 依赖
-include $(wildcard $(BUILD_DIR)/*.d)

.PHONY: all clean 