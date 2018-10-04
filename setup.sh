#!/bin/bash

LIB_ROOT="src/libs"

mkdir --parents $LIB_ROOT/uws
mkdir --parents $LIB_ROOT/reactphysics3d

get_latest_release() {
	curl --silent "https://api.github.com/repos/$1/releases/latest" | # Get latest release from GitHub api
		grep '"tag_name":' |                                            # Get tag line
		sed -E 's/.*"([^"]+)".*/\1/'                                    # Pluck JSON value
}

downloadUWS() {
	echo "-----downloading uWebSockets-----"
	mkdir temp
	git clone https://github.com/uNetworking/uWebSockets.git temp
	cd temp
	TAG=`get_latest_release "uNetworking/uWebSockets"` # get latest release
	git checkout tags/$TAG
	mv src ../$LIB_ROOT/uws
	cd ../
	rm -r temp
}

downloadRP3D() {
	echo "-----downloading React Physics 3D-----"
	mkdir temp
	git clone https://github.com/DanielChappuis/reactphysics3d.git temp
	cd temp
	TAG=`get_latest_release "DanielChappuis/reactphysics3d"` # get latest release
	git checkout tags/$TAG
	mv src ../$LIB_ROOT/reactphysics3d
	
	# where the darn confusing compiling happens

	cd ../
	rm -r temp
}

downloadUWS
downloadRP3D