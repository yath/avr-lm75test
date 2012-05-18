MCU = atmega8
F_CPU = 1000000
TARGET = lm75test

AVR_COMMON_API = 1

WITH_UART  = 1
WITH_RAND  = 0
WITH_LCD   = 1
DEBUG	   = 1

SRC = main.c
include common/common.mk
