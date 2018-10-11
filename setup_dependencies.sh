#!/bin/bash

NGINX_VERSION="1.15.4"

LIB_ROOT="src/libs" 
STATIC_ROOT="src/static"
NGINX_ROOT="nginx"

UWS_NAME="uws"
RP3D_NAME="reactphysics3d"
NGINX_NAME="nginx"

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

downloadNginxDependencies() {
	#pcre 8.42 download
	wget ftp://ftp.csx.cam.ac.uk/pub/software/programming/pcre/pcre-8.42.tar.gz
	tar -zxf pcre-8.42.tar.gz
	cd pcre-8.42
	./configure
	make
	sudo make install
	cd ../
	
	#zlib 1.2.11 download
	wget http://zlib.net/zlib-1.2.11.tar.gz
	tar -zxf zlib-1.2.11.tar.gz
	cd zlib-1.2.11
	./configure
	make
	sudo make install
	cd ../
	
	#openssl 1.0.2p download
	wget http://www.openssl.org/source/openssl-1.0.2p.tar.gz
	tar -zxf openssl-1.0.2p.tar.gz
	cd openssl-1.0.2p
	./config --prefix=/usr
	make
	sudo make install
	cd ../
}

downloadNGINX() {
	cd NGINX_ROOT
	downloadNginxDependencies #download Nginx dependencies
	wget https://nginx.org/download/nginx-$NGINX_VERSION.tar.gz
	tar zxf nginx-$NGINX_VERSION.tar.gz
	cd nginx-$NGINX_VERSION
	./configure 
}

downloadUWS # download uWS and put the neccesary files
downloadRP3D # download React Physics 3D and put the neccesary files
