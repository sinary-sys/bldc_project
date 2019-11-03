################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
CMD/28335_RAM_lnk.exe: ../CMD/28335_RAM_lnk.cmd $(GEN_CMDS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Linker'
	"D:/ti/ccs910/ccs/tools/compiler/ti-cgt-c2000_18.12.2.LTS/bin/cl2000" --silicon_version=28 -g -O3 --define=LARGE_MODEL --diag_warning=225 --large_memory_model -z -m"../CMD/Release/LED.map" --warn_sections -i"D:/ti/ccs910/ccs/tools/compiler/ti-cgt-c2000_18.12.2.LTS/lib" -i"D:/ti/ccs910/ccs/tools/compiler/ti-cgt-c2000_18.12.2.LTS/include" -i"F:/MyProgram/dsplicheng/dsplicheng/y/lab1-GPIO_output_LED" --reread_libs --xml_link_info="LED_linkInfo.xml" --rom_model -o "$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

CMD/DSP2833x_Headers_nonBIOS.exe: ../CMD/DSP2833x_Headers_nonBIOS.cmd $(GEN_CMDS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Linker'
	"D:/ti/ccs910/ccs/tools/compiler/ti-cgt-c2000_18.12.2.LTS/bin/cl2000" --silicon_version=28 -g -O3 --define=LARGE_MODEL --diag_warning=225 --large_memory_model -z -m"../CMD/Release/LED.map" --warn_sections -i"D:/ti/ccs910/ccs/tools/compiler/ti-cgt-c2000_18.12.2.LTS/lib" -i"D:/ti/ccs910/ccs/tools/compiler/ti-cgt-c2000_18.12.2.LTS/include" -i"F:/MyProgram/dsplicheng/dsplicheng/y/lab1-GPIO_output_LED" --reread_libs --xml_link_info="LED_linkInfo.xml" --rom_model -o "$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


