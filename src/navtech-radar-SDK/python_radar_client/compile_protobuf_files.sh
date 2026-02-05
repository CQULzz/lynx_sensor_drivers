#!/bin/bash         
protobuf_common_folder="./../protobuf/"

echo "Compiling protobuf files"
protofilenames="$protobuf_common_folder*.proto"
for protofilename in $protofilenames
    do
        echo "Compiling file: $protofilename"
        protoc -I=$protobuf_common_folder --python_out=./protobuf/ $protofilename
    done
echo "Done"