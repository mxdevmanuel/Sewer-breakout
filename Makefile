BUILD_DIR := build

.PHONY: all configure run clean

all: $(BUILD_DIR)/build.ninja
	cmake --build $(BUILD_DIR)

# Configure only on first use (or after `make clean`); later runs just build.
$(BUILD_DIR)/build.ninja:
	cmake --preset debug

configure:
	cmake --preset debug

run: all
	./$(BUILD_DIR)/SewerBreakout

clean:
	rm -rf $(BUILD_DIR)
