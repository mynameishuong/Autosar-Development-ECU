# Makefile cho STM32F103 (Blue Pill)

# Cấu hình cơ bản
TARGET = stm32_project
DEVICE = STM32F10X_MD  # MD cho STM32F103C8T6 (64K Flash)
CC      = arm-none-eabi-gcc
BUILD_DIR = build
# Flags biên dịch
CFLAGS  = -mcpu=cortex-m3 -mthumb -Wall -Og -g \
          -DSTM32F10X_MD -DUSE_STDPERIPH_DRIVER \
          -Ilib/CMSIS/CM3/CoreSupport \
          -Ilib/CMSIS/CM3/DeviceSupport/ST/STM32F10x \
		  -IMCAL/Port_Driver \
		  -IMCAL/DIO_Driver \
		  -IMCAL/PWM_Driver \
		  -IMCAL/ADC_Driver \
		  -ITimer \
          -Ilib/SPL/inc

# Flags linker
LDFLAGS = -T ./linker/stm32f103.ld -nostartfiles -Wl,--gc-sections

# File nguồn
SRCS_C = \
    main.c \
    lib/SPL/src/stm32f10x_rcc.c \
    lib/SPL/src/stm32f10x_gpio.c \
	lib/SPL/src/stm32f10x_tim.c \
	lib/SPL/src/stm32f10x_adc.c \
	lib/SPL/src/stm32f10x_dma.c \
	lib/SPL/src/Det.c \
	lib/SPL/src/misc.c \
	MCAL/Port_Driver/Port_Cfg.c \
	MCAL/Port_Driver/Port.c \
	MCAL/DIO_Driver/Dio.c \
	MCAL/PWM_Driver/Pwm.c \
	MCAL/PWM_Driver/Pwm_cfg.c \
	MCAL/ADC_Driver/Adc.c \
	MCAL/ADC_Driver/Adc_Cfg.c \
	MCAL/ADC_Driver/Adc_HW.c \
	Timer/timer.c \
    lib/CMSIS/CM3/DeviceSupport/ST/STM32F10x/system_stm32f10x.c  # Thêm file system

SRCS_S = lib/CMSIS/CM3/DeviceSupport/ST/STM32F10x/startup/startup_stm32f103.s  # Đường dẫn đầy đủ
OBJS   = $(SRCS_C:.c=.o) $(SRCS_S:.s=.o)
$(shell mkdir -p $(BUILD_DIR))

# Rules
all: $(BUILD_DIR)/$(TARGET).bin  $(BUILD_DIR)/$(TARGET).hex

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.s
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $@
	@echo "======================Firmware Size==========================="
	@arm-none-eabi-size $@
	@echo "=============================================================="
	@rm -f $(OBJS)


$(BUILD_DIR)/$(TARGET).bin: $(BUILD_DIR)/$(TARGET).elf
	arm-none-eabi-objcopy -O binary $< $@

$(BUILD_DIR)/$(TARGET).hex: $(BUILD_DIR)/$(TARGET).elf
	arm-none-eabi-objcopy -O ihex $< $@

flash: $(BUILD_DIR)/$(TARGET).bin
	openocd -f interface/stlink.cfg -f target/stm32f1x.cfg -c "program $(BUILD_DIR)/$(TARGET).bin 0x08000000 verify reset exit"

clean:
	rm -rf $(OBJS) $(BUILD_DIR)

.PHONY: all clean flash