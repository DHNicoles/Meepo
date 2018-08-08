#OpenCV
set(OpenCV_INCLUDE_DIRS ../include/opencv ../include/opencv2)
INCLUDE_DIRECTORIES({OpenCV_INCLUDE_DIRS})
find_library(HIGHGUI opencv_highgui ../lib)
find_library(CORE opencv_core ../lib)
find_library(IMGCODECS opencv_imgcodecs ../lib)
find_library(IMAGE_PROC opencv_imgproc ../lib)
find_library(VIDEO opencv_video ../lib)
find_library(VIDEOIO opencv_videoio ../lib)
set(OpenCV_LIBS ${HIGHGUI} ${CORE} ${IMAGE_PROC} ${VIDEO} ${VIDEOIO} ${IMGCODECS})

#HikVision
find_library(HCNETSDK hcnetsdk ../lib)
find_library(PLAYM4 PlayCtrl ../lib)
find_library(AUDIO_RENDER AudioRender ../lib)
find_library(SUPER_RENDER SuperRender ../lib)
set(Hik_LIBS ${HCNETSDK} ${PLAYM4} ${AUDIO_RENDER} ${SUPER_RENDER})

#OpenVino
find_library(FORMAT_READER format_reader /home/intel/inference_engine_samples_build/intel64/Release/lib)
find_library(CPU_EXTENSION cpu_extension /home/intel/inference_engine_samples_build/intel64/Release/lib)
find_library(INFERENCE_ENGINE inference_engine /opt/intel/computer_vision_sdk_2018.2.299/deployment_tools/inference_engine/lib/ubuntu_16.04/intel64)
SET(OPENVINO_LIBS ${FORMAT_READER} ${CPU_EXTENSION} ${INFERENCE_ENGINE})

set(Other_LIBS libpthread.so libdl.so)
SET(LIBS ${Hik_LIBS} ${OpenCV_LIBS} ${OPENVINO_LIBS} ${Other_LIBS})

set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_BUILD_TYPE "Release")
SET(CMAKE_CXX_FLAGS "-std=c++11 -Wall -msse4.2 -fPIC -o3 -w -fopenmp")
INCLUDE_DIRECTORIES(../include)
INCLUDE_DIRECTORIES(../include/OpenVINO)
INCLUDE_DIRECTORIES(../include/OpenVINO/inference_engine/include/)

file(GLOB_RECURSE SRC_LIST . "*.cpp")  
AUX_SOURCE_DIRECTORY(. SRC_LIST)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ../../lib/)

ADD_EXECUTABLE(meepo ${SRC_LIST})
#ADD_LIBRARY(meepo-rc SHARED ${SRC_LIST})

TARGET_LINK_LIBRARIES(meepo ${LIBS})
#TARGET_LINK_LIBRARIES(meepo-rc ${LIBS})



