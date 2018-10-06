#!/bin/bash

LIB_ROOT="src/libs" 
STATIC_ROOT="src/static"

UWS_NAME="uws"
RP3D_NAME="reactphysics3d"

# create folders for neccesary libraries
mkdir --parents $LIB_ROOT/$UWS_NAME
mkdir --parents $LIB_ROOT/$RP3D_NAME

mkdir --parents $STATIC_ROOT/$UWS_NAME
mkdir --parents $STATIC_ROOT/$RP3D_NAME

getLatestRelease() {
	curl --silent "https://api.github.com/repos/$1/releases/latest" | # Get latest release from GitHub api
		grep '"tag_name":' |                                            # Get tag line
		sed -E 's/.*"([^"]+)".*/\1/'                                    # Pluck JSON value
}

downloadUWS() {
	echo "-----downloading uWebSockets-----"
	mkdir temp
	git clone https://github.com/uNetworking/uWebSockets.git temp
	cd temp
	TAG=`getLatestRelease uNetworking/uWebSockets` # get latest release
	git checkout tags/$TAG
	
	make # make uWS
	cp libuWS.so $STATIC_ROOT/$UWS_NAME # move static lib
	
	mv src ../$LIB_ROOT/$UWS_NAME
	cd ../
	rm -r temp
}

downloadRP3D() {
	echo "-----downloading React Physics 3D-----"
	mkdir temp
	git clone https://github.com/DanielChappuis/reactphysics3d.git temp
	cd temp
	TAG=`getLatestRelease DanielChappuis/reactphysics3d` # get latest release
	git checkout tags/$TAG
	
	# where the darn confusing compiling happens
	cmake . # cmake it (no parameters needed)
	make # make it
	TEMP_LIB_NAME=`ls lib` # get name of static lib
	mv lib/$TEMP_LIB_NAME lib/$RP3D_NAME.so # rename it
	mv lib/$RP3D_NAME.so $STATIC_ROOT/$RP3D_NAME # move to static directory
	#make install # maybe
	
	mv src ../$LIB_ROOT/$RP3D_NAME # move headers to the header folder

	cd ../
	rm -r temp
}

downloadUWS # download uWS and put the neccesary files
downloadRP3D # download React Physics 3D and put the neccesary files
