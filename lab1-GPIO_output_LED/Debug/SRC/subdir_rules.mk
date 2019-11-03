################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
SRC/%.obj: ../SRC/%.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"D:/ti/ccs910/ccs/tools/compiler/ti-cgt-c2000_18.12.2.LTS/bin/cl2000" -v28 -ml --float_support=fpu32 --include_path="F:/MyProgram/dsplicheng/dsplicheng/y/lab1-GPIO_output_LED/INCLUDE" --include_path="F:/MyProgram/dsplicheng/dsplicheng/y/lab1-GPIO_output_LED/SRC" --include_path="F:/MyProgram/dsplicheng/dsplicheng/y/lab1-GPIO_output_LED/INCLUDE" --include_path="D:/ti/ccs910/ccs/tools/compiler/ti-cgt-c2000_18.12.2.LTS/include" --include_path="/packages/ti/xdais" --advice:performance=all --define=_DEBUG --define=LARGE_MODEL -g --diag_warning=225 --abi=coffabi --preproc_with_compile --preproc_dependency="SRC/$(basename $(<F)).d_raw" --obj_directory="SRC" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

SRC/%.obj: ../SRC/%.asm $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"D:/ti/ccs910/ccs/tools/compiler/ti-cgt-c2000_18.12.2.LTS/bin/cl2000" -v28 -ml --float_support=fpu32 --include_path="F:/MyProgram/dsplicheng/dsplicheng/y/lab1-GPIO_output_LED/INCLUDE" --include_path="F:/MyProgram/dsplicheng/dsplicheng/y/lab1-GPIO_output_LED/SRC" --include_path="F:/MyProgram/dsplicheng/dsplicheng/y/lab1-GPIO_output_LED/INCLUDE" --include_path="D:/ti/ccs910/ccs/tools/compiler/ti-cgt-c2000_18.12.2.LTS/include" --include_path="/packages/ti/xdais" --advice:performance=all --define=_DEBUG --define=LARGE_MODEL -g --diag_warning=225 --abi=coffabi --preproc_with_compile --preproc_dependency="SRC/$(basename $(<F)).d_raw" --obj_directory="SRC" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


