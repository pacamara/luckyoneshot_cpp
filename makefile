# Copyright (C) 2017 Cryobrew Software (http://www.cryobrew.com)
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

LIBS := -lzbar -lopencv_stitching -lopencv_features2d -lopencv_flann -lopencv_imgcodecs -lopencv_videoio -lopencv_xphoto -lopencv_highgui -lopencv_imgproc -lopencv_freetype -lopencv_core -lopencv_dpm -lopencv_face -lopencv_bioinspired -lopencv_ccalib -lopencv_dnn_modern -lopencv_aruco -lopencv_bgsegm -lopencv_superres -lopencv_videostab -lopencv_photo

CPP_SRCS += \
../src/QRScanner.cpp \
../src/dangampola_grad_method.cpp \
../src/luckyoneshot_main.cpp 

OBJS += \
./build/QRScanner.o \
./build/dangampola_grad_method.o \
./build/luckyoneshot_main.o 

CPP_DEPS += \
./build/QRScanner.d \
./build/dangampola_grad_method.d \
./build/luckyoneshot_main.d 

./build/%.o: ./src/%.cpp
	@echo 'Building file: $<'
	mkdir -p build
	g++ -I/usr/local/include/opencv -I/usr/local/include -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

all: luckyoneshot_cpp

luckyoneshot_cpp: $(OBJS)
	@echo 'Building target: $@'
	g++ -L/usr/local/lib -o "build/luckyoneshot_cpp" $(OBJS) $(LIBS)
	@echo 'Finished building target: $@'

clean:
	rm ./build/*
	rmdir build

