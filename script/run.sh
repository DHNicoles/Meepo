#########################################################################
# File Name: run.sh
# Author: Chegf
# mail: cheguangfu1@jd.com
# Created Time: 2018年08月07日 星期二 20时55分57秒
#########################################################################
#!/bin/bash
source /opt/intel/computer_vision_sdk/bin/setupvars.sh
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/workspace/Meepo/lib:/home/intel/inference_engine_samples_build/intel64/Release/lib:/opt/intel/computer_vision_sdk_2018.2.299/deployment_tools/inference_engine/lib/ubuntu_16.04/intel64:/opt/intel/computer_vision_sdk_2018.2.299/opencv/lib/
cd ~/workspace/Meepo/build/bin
./meepo
