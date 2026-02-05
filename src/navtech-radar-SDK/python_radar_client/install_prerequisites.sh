#!/bin/bash
numpypackagename="numpy"
matplotlibpackagename="matplotlib"
opencvpackagename="opencv_python"
protobufpackagename="protobuf"

numpypackageversion="2.2.2"
matplotlibpackageversion="3.10.0"
opencvpackageversion="4.11.0.86"
protobufpackageversion="5.29.3"

packagenames=($matplotlibpackagename $numpypackagename $opencvpackagename $protobufpackagename)
packageversions=($matplotlibpackageversion $numpypackageversion $opencvpackageversion $protobufpackageversion)
echo $packagenames
echo "Installing prerequisite packages"
for i in "${!packagenames[@]}"
    do
        echo "Installing package: ${packagenames[i]}, version: ${packageversions[i]}"
        pip3 install ${packagenames[i]}==${packageversions[i]}
    done
echo "Done"