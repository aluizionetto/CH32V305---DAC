################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include User/ff/subdir.mk
-include User/subdir.mk
-include Startup/subdir.mk
-include Peripheral/src/subdir.mk
-include Debug/subdir.mk
-include Core/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
SD_CH32V305RBT6_R1.hex \

SECONDARY_LIST += \
SD_CH32V305RBT6_R1.lst \

SECONDARY_SIZE += \
SD_CH32V305RBT6_R1.siz \


# All Target
all: SD_CH32V305RBT6_R1.elf secondary-outputs

# Tool invocations
SD_CH32V305RBT6_R1.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU RISC-V Cross C Linker'
	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized  -g -T "D:\RISC-V\mrs_workspace\SD_CH32V305RBT6_R1\Ld\Link.ld" -nostartfiles -Xlinker --gc-sections -Wl,-Map,"SD_CH32V305RBT6_R1.map" --specs=nano.specs --specs=nosys.specs -o "SD_CH32V305RBT6_R1.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

SD_CH32V305RBT6_R1.hex: SD_CH32V305RBT6_R1.elf
	@echo 'Invoking: GNU RISC-V Cross Create Flash Image'
	riscv-none-embed-objcopy -O ihex "SD_CH32V305RBT6_R1.elf"  "SD_CH32V305RBT6_R1.hex"
	@echo 'Finished building: $@'
	@echo ' '

SD_CH32V305RBT6_R1.lst: SD_CH32V305RBT6_R1.elf
	@echo 'Invoking: GNU RISC-V Cross Create Listing'
	riscv-none-embed-objdump --all-headers --demangle --disassemble "SD_CH32V305RBT6_R1.elf" > "SD_CH32V305RBT6_R1.lst"
	@echo 'Finished building: $@'
	@echo ' '

SD_CH32V305RBT6_R1.siz: SD_CH32V305RBT6_R1.elf
	@echo 'Invoking: GNU RISC-V Cross Print Size'
	riscv-none-embed-size --format=berkeley "SD_CH32V305RBT6_R1.elf"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(SECONDARY_FLASH)$(SECONDARY_LIST)$(SECONDARY_SIZE)$(ASM_DEPS)$(S_UPPER_DEPS)$(C_DEPS) SD_CH32V305RBT6_R1.elf
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets
