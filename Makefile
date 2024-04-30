
.PHONY: build upload listen all
all: build upload listen

build:
	arduino-cli compile -b arduino:avr:mega --libraries libs --build-path ./build

upload:
	arduino-cli upload -p /dev/cu.usbmodem14101 --fqbn arduino:avr:mega --input-dir ./build

listen:
	cat /dev/cu.usbmodem14101
