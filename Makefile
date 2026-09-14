.PHONY: debug release run test bench

BUILD_DIR := build

debug:
	@cmake -S . -B $(BUILD_DIR) -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER:FILEPATH=/usr/bin/clang++ -DCMAKE_COLOR_DIAGNOSTICS=ON -DCMAKE_CXX_FLAGS="-fcolor-diagnostics"
	@cmake --build $(BUILD_DIR) -j$(shell nproc)
	@printf "\nBinary: $(BUILD_DIR)/bmp (run \"make run\")\n"
	@printf "\nTests: $(BUILD_DIR)/tests/unit_tests (run \"make test\")\n"
	@printf "\nBenchmarks: $(BUILD_DIR)/benchmarks/bench_decode (run \"make bench\")\n"


release:
	@cmake -S . -B $(BUILD_DIR) -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER:FILEPATH=/usr/bin/clang++ -DCMAKE_COLOR_DIAGNOSTICS=ON -DCMAKE_CXX_FLAGS="-fcolor-diagnostics"
	@cmake --build $(BUILD_DIR) -j$(shell nproc)
	@printf "\nBinary: $(BUILD_DIR)/bmp (run \"make run\")\n"
	@printf "\nTests: $(BUILD_DIR)/tests/unit_tests (run \"make test\")\n"
	@printf "\nBenchmarks: $(BUILD_DIR)/benchmarks/bench_decode (run \"make bench\")\n"

run:
	@./$(BUILD_DIR)/bmp

test:
	@./$(BUILD_DIR)/tests/unit_tests

bench:
	@./$(BUILD_DIR)/benchmarks/bench_decode
