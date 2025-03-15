################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../libraries/my_lib/MMC_SD.c \
../libraries/my_lib/bmp.c \
../libraries/my_lib/diskio.c \
../libraries/my_lib/ff.c \
../libraries/my_lib/ffsystem.c \
../libraries/my_lib/ffunicode.c \
../libraries/my_lib/font.c \
../libraries/my_lib/spi.c 

COMPILED_SRCS += \
libraries/my_lib/MMC_SD.src \
libraries/my_lib/bmp.src \
libraries/my_lib/diskio.src \
libraries/my_lib/ff.src \
libraries/my_lib/ffsystem.src \
libraries/my_lib/ffunicode.src \
libraries/my_lib/font.src \
libraries/my_lib/spi.src 

C_DEPS += \
libraries/my_lib/MMC_SD.d \
libraries/my_lib/bmp.d \
libraries/my_lib/diskio.d \
libraries/my_lib/ff.d \
libraries/my_lib/ffsystem.d \
libraries/my_lib/ffunicode.d \
libraries/my_lib/font.d \
libraries/my_lib/spi.d 

OBJS += \
libraries/my_lib/MMC_SD.o \
libraries/my_lib/bmp.o \
libraries/my_lib/diskio.o \
libraries/my_lib/ff.o \
libraries/my_lib/ffsystem.o \
libraries/my_lib/ffunicode.o \
libraries/my_lib/font.o \
libraries/my_lib/spi.o 


# Each subdirectory must supply rules for building sources it contributes
libraries/my_lib/MMC_SD.src: ../libraries/my_lib/MMC_SD.c libraries/my_lib/subdir.mk
	cctc -cs --dep-file="$(*F).d" --misrac-version=2004 -D__CPU__=tc37x "-fD:/AURIX/ADS_finaltest/Seekfree_TC377_Opensource_Library/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc37x -Y0 -N0 -Z0 -o "$@" "$<"
libraries/my_lib/MMC_SD.o: libraries/my_lib/MMC_SD.src libraries/my_lib/subdir.mk
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
libraries/my_lib/bmp.src: ../libraries/my_lib/bmp.c libraries/my_lib/subdir.mk
	cctc -cs --dep-file="$(*F).d" --misrac-version=2004 -D__CPU__=tc37x "-fD:/AURIX/ADS_finaltest/Seekfree_TC377_Opensource_Library/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc37x -Y0 -N0 -Z0 -o "$@" "$<"
libraries/my_lib/bmp.o: libraries/my_lib/bmp.src libraries/my_lib/subdir.mk
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
libraries/my_lib/diskio.src: ../libraries/my_lib/diskio.c libraries/my_lib/subdir.mk
	cctc -cs --dep-file="$(*F).d" --misrac-version=2004 -D__CPU__=tc37x "-fD:/AURIX/ADS_finaltest/Seekfree_TC377_Opensource_Library/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc37x -Y0 -N0 -Z0 -o "$@" "$<"
libraries/my_lib/diskio.o: libraries/my_lib/diskio.src libraries/my_lib/subdir.mk
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
libraries/my_lib/ff.src: ../libraries/my_lib/ff.c libraries/my_lib/subdir.mk
	cctc -cs --dep-file="$(*F).d" --misrac-version=2004 -D__CPU__=tc37x "-fD:/AURIX/ADS_finaltest/Seekfree_TC377_Opensource_Library/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc37x -Y0 -N0 -Z0 -o "$@" "$<"
libraries/my_lib/ff.o: libraries/my_lib/ff.src libraries/my_lib/subdir.mk
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
libraries/my_lib/ffsystem.src: ../libraries/my_lib/ffsystem.c libraries/my_lib/subdir.mk
	cctc -cs --dep-file="$(*F).d" --misrac-version=2004 -D__CPU__=tc37x "-fD:/AURIX/ADS_finaltest/Seekfree_TC377_Opensource_Library/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc37x -Y0 -N0 -Z0 -o "$@" "$<"
libraries/my_lib/ffsystem.o: libraries/my_lib/ffsystem.src libraries/my_lib/subdir.mk
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
libraries/my_lib/ffunicode.src: ../libraries/my_lib/ffunicode.c libraries/my_lib/subdir.mk
	cctc -cs --dep-file="$(*F).d" --misrac-version=2004 -D__CPU__=tc37x "-fD:/AURIX/ADS_finaltest/Seekfree_TC377_Opensource_Library/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc37x -Y0 -N0 -Z0 -o "$@" "$<"
libraries/my_lib/ffunicode.o: libraries/my_lib/ffunicode.src libraries/my_lib/subdir.mk
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
libraries/my_lib/font.src: ../libraries/my_lib/font.c libraries/my_lib/subdir.mk
	cctc -cs --dep-file="$(*F).d" --misrac-version=2004 -D__CPU__=tc37x "-fD:/AURIX/ADS_finaltest/Seekfree_TC377_Opensource_Library/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc37x -Y0 -N0 -Z0 -o "$@" "$<"
libraries/my_lib/font.o: libraries/my_lib/font.src libraries/my_lib/subdir.mk
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
libraries/my_lib/spi.src: ../libraries/my_lib/spi.c libraries/my_lib/subdir.mk
	cctc -cs --dep-file="$(*F).d" --misrac-version=2004 -D__CPU__=tc37x "-fD:/AURIX/ADS_finaltest/Seekfree_TC377_Opensource_Library/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc37x -Y0 -N0 -Z0 -o "$@" "$<"
libraries/my_lib/spi.o: libraries/my_lib/spi.src libraries/my_lib/subdir.mk
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"

clean: clean-libraries-2f-my_lib

clean-libraries-2f-my_lib:
	-$(RM) libraries/my_lib/MMC_SD.d libraries/my_lib/MMC_SD.o libraries/my_lib/MMC_SD.src libraries/my_lib/bmp.d libraries/my_lib/bmp.o libraries/my_lib/bmp.src libraries/my_lib/diskio.d libraries/my_lib/diskio.o libraries/my_lib/diskio.src libraries/my_lib/ff.d libraries/my_lib/ff.o libraries/my_lib/ff.src libraries/my_lib/ffsystem.d libraries/my_lib/ffsystem.o libraries/my_lib/ffsystem.src libraries/my_lib/ffunicode.d libraries/my_lib/ffunicode.o libraries/my_lib/ffunicode.src libraries/my_lib/font.d libraries/my_lib/font.o libraries/my_lib/font.src libraries/my_lib/spi.d libraries/my_lib/spi.o libraries/my_lib/spi.src

.PHONY: clean-libraries-2f-my_lib

