#!/bin/bash

NGINX_VERSION="1.15.4" #has to be hardcoded

LIB_ROOT="src/libs" 
STATIC_ROOT="src/static"
NGINX_ROOT="nginx"
LOGS_ROOT="logs" #only used for building

MAKE_BACKUP_ROOT="makeBackupRoot"

UWS_NAME="uws"
RP3D_NAME="reactphysics3d"
TOL_NAME="tinyObjectLoader"

# create folders for neccesary libraries
mkdir --parents $LIB_ROOT/$UWS_NAME
mkdir --parents $LIB_ROOT/$RP3D_NAME
mkdir --parents $LIB_ROOT/$TOL_NAME

mkdir --parents $STATIC_ROOT/$UWS_NAME
mkdir --parents $STATIC_ROOT/$RP3D_NAME

# create folders for logs
mkdir $LOGS_ROOT

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
	
	make > ../$LOGS_ROOT/$UWS_NAME.log # make uWS
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
	make > ../$LOGS_ROOT/$RP3D_NAME.log # make it
	TEMP_LIB_NAME=`ls lib` # get name of static lib
	mv lib/$TEMP_LIB_NAME lib/$RP3D_NAME.so # rename it
	mv lib/$RP3D_NAME.so $STATIC_ROOT/$RP3D_NAME # move to static directory
	#make install # maybe
	
	mv src ../$LIB_ROOT/$RP3D_NAME # move headers to the header folder

	cd ../
	rm -r temp
}

downloadTOL() {
	echo "-----downloading Tiny Object Loader-----"
	mkdir temp
	git clone https://github.com/syoyo/tinyobjloader.git temp
	cd temp
	TAG=`getLatestRelease syoyo/tinyobjloader` # get latest release
	git checkout tags/$TAG
	
	mv tiny_obj_loader.h ../$LIB_ROOT/$TOL_NAME # move the one header to the header folder

	cd ../
	rm -r temp
}

downloadNginxDependencies() {
	#pcre 8.42 download
	wget ftp://ftp.csx.cam.ac.uk/pub/software/programming/pcre/pcre-8.42.tar.gz
	tar -zxf pcre-8.42.tar.gz
	cd pcre-8.42
	./configure
	make > ../../$LOGS_ROOT/pcre.log #send output to log
	sudo make install #maybe not
	
	cp Makefile Makefile-pcre
	cp Makefile-pcre $MAKE_BACKUP_ROOT #move makefile to backup so pcre can be uninstalled later with `make uninstall`
	rm Makefile-pcre
	
	cd ../
	
	#zlib 1.2.11 download
	wget http://zlib.net/zlib-1.2.11.tar.gz
	tar -zxf zlib-1.2.11.tar.gz
	cd zlib-1.2.11
	./configure
	make > ../../$LOGS_ROOT/zlib.log #send output to log
	sudo make install #maybe not
	
	cp Makefile Makefile-zlib
	cp Makefile-zlib $MAKE_BACKUP_ROOT #move makefile to backup so zlib can be uninstalled later with `make uninstall`
	rm Makefile-zlib
	
	cd ../
	
	#openssl 1.1.1 (latest) download
	wget https://www.openssl.org/source/openssl-1.1.1.tar.gz
	tar -zxf openssl-1.1.1.tar.gz
	cd openssl-1.1.1
	./config --prefix=/usr
	make > ../../$LOGS_ROOT/openssl.log #send output to log
	sudo make install #maybe not
	
	cp Makefile Makefile-openssl
	cp Makefile-openssl $MAKE_BACKUP_ROOT #move makefile to backup so openssl can be uninstalled later with `make uninstall`
	rm Makefile-openssl
	
	cd ../
}

removeNginxSourceAndDependenciesSource() {
	rm -rf $NGINX_ROOT/nginx-$NGINX_VERSION
	rm -rf $NGINX_ROOT/pcre-8.42
	rm -rf $NGINX_ROOT/zlib-1.2.11
	rm -rf $NGINX_ROOT/openssl-1.1.1
}

downloadNGINX() {
	cd $NGINX_ROOT #go into nginx folder, IMPORTANT!
	downloadNginxDependencies #download Nginx dependencies
	wget https://nginx.org/download/nginx-$NGINX_VERSION.tar.gz #start Nginx download
	tar zxf nginx-$NGINX_VERSION.tar.gz
	cd nginx-$NGINX_VERSION
	echo "---CONFIG---" > ../../$LOGS_ROOT/$NGINX_ROOT.log
	./configure --prefix=../ \ #the directory to build in is the $NGINX_ROOT folder, not THIS FOLDER!
		#modules disabled because I want lightweight!
		-–without-http_proxy_module \ #not sure yet
		--without-http_empty_gif_module \
		--without-http_ssi_module \
		--without-http_auth_basic_module \
		--without-http_mirror_module \
		--without-http_autoindex_module \
		--without-http_geo_module \
		--without-http_map_module \
		--without-http_split_clients_module \
		--without-http_fastcgi_module \
		--without-http_uwsgi_module \
		--without-http_scgi_module \
		--without-http_grpc_module \
		--without-http_memcached_module \
		--without-http_browser_module \
		--without-mail_pop3_module \
		--without-mail_pop3_module \
		--without-mail_smtp_module \
		#modules enabled because I also want speed!
		-–with-http_ssl_module \
		--with-pcre=../pcre-8.42 \
		--with-zlib=../zlib-1.2.11 \
		--with-file-aio \
		--with-http_gunzip_module \ #can send files gzipped already (example bestimage.png.gz)
		--with-http_gzip_static_module \
		#send output to log
		> ../../$LOGS_ROOT/$NGINX_ROOT.log #send to nginx log
	
	echo "---END_CONFIG---" > ../../$LOGS_ROOT/$NGINX_ROOT.log
	echo "---MAKE---" > ../../$LOGS_ROOT/$NGINX_ROOT.log
	make > ../../$LOGS_ROOT/$NGINX_ROOT.log #send to nginx log
	echo "---END_MAKE---" > ../../$LOGS_ROOT/$NGINX_ROOT.log
	
	cd ../../
	removeNginxSourceAndDependenciesSource #libs are built into the operating system, so its fine
	
}

echo "Now's the time to relax and read a book, because this will take a long time, undoubtably"
echo "Wait for `BUILDING DONE` to know when the building is done"
downloadUWS # download uWS and put the neccesary files
downloadRP3D # download React Physics 3D and put the neccesary files in their folders
downloadTOL # download Tiny Object Loader (one file) and move it to its header location
downloadNGINX # download Nginx + dependencies
# |\___ $NGINX_ROOT/sbin/nginx -- executable
# |\___ $NGINX_ROOT/conf/nginx.conf -- configuration file
# |\___ $NGINX_ROOT/logs/nginx.pid -- PID path
# |\___ $NGINX_ROOT/logs/error.log -- error log
#  \___ $NGINX_ROOT/logs/access.log -- request log
echo "BUILDING DONE, we hope :)"
