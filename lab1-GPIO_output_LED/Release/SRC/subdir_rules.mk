################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
SRC/%.obj: ../SRC/%.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"D:/ti/ccs910/ccs/tools/compiler/ti-cgt-c2000_18.12.2.LTS/bin/cl2000" --silicon_version=28 -g -O3 --define=LARGE_MODEL --include_path="D:/ti/ccs910/ccs/tools/compiler/ti-cgt-c2000_18.12.2.LTS/include" --include_path="/packages/ti/xdais" --diag_warning=225 --large_memory_model --preproc_with_compile --preproc_dependency="SRC/$(basename $(<F)).d_raw" --obj_directory="SRC" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

SRC/%.obj: ../SRC/%.asm $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"D:/ti/ccs910/ccs/tools/compiler/ti-cgt-c2000_18.12.2.LTS/bin/cl2000" --silicon_version=28 -g -O3 --define=LARGE_MODEL --include_path="D:/ti/ccs910/ccs/tools/compiler/ti-cgt-c2000_18.12.2.LTS/include" --include_path="/packages/ti/xdais" --diag_warning=225 --large_memory_model --preproc_with_compile --preproc_dependency="SRC/$(basename $(<F)).d_raw" --obj_directory="SRC" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


