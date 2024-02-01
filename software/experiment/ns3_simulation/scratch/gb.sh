#!/bin/bash

sudo rm -rf vPIFOResult
sudo mkdir vPIFOResult

sudo ./waf --run scratch/myTopo
#sudo ./waf --run scratch/inversion
#sudo ./waf --pyrun scratch/calTailPktDelay.py


