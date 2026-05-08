BUILD_FOLDER = ./build
PROJECT_NAME = pi

CPU_CORE = 0
GOVERNOR = performance
AMD_BOOST = /sys/devices/system/cpu/cpufreq/boost
FIXED_FREQ = 3000MHz 

.PHONY: bench setup_cpu restore_cpu

bench:
	cmake -B build -DCMAKE_BUILD_TYPE=RelWithDebInfo
	cmake --build $(BUILD_FOLDER) -j$$(nproc)
	@$(MAKE) setup_cpu
	sudo taskset -c $(CPU_CORE) chrt -f 99 $(BUILD_FOLDER)/$(PROJECT_NAME) $(OPTS)
	@$(MAKE) restore_cpu

setup_cpu:
	sudo cpupower frequency-set -u $(FIXED_FREQ) -d $(FIXED_FREQ) -g $(GOVERNOR)
	if [ -f $(AMD_BOOST) ]; then echo 0 | sudo tee $(AMD_BOOST); fi

restore_cpu:
	sudo cpupower frequency-set -g powersave
	if [ -f $(AMD_BOOST) ]; then echo 1 | sudo tee $(AMD_BOOST); fi


	