.PHONY : all install

all: start

.PHONY : start build

start: build
	./../build/GidUralClient

build:
	   mkdir -p ../build \
	&& cd ../build \
	&& source ~/.bashrc \
	&& /usr/local/cmake/bin/cmake \
	   -DCMAKE_CXX_COMPILER=/opt/rh/devtoolset-9/root/usr/bin/g++ \
	   -DCMAKE_C_COMPILER=/opt/rh/devtoolset-9/root/usr/bin/gcc	.. \
	&& /usr/local/cmake/bin/cmake --build .

install: install/lib/librdkafka install/lib/openssl-1.1.1k

.PHONY : install/lib/librdkafka install/lib/openssl-1.1.1k

install/lib/librdkafka: install/getlibs install/cmake-3.22.2
	cd ../third_party/librdkafka \
	&& /usr/local/cmake/bin/cmake -H. -B_cmake_build \
	&& /usr/local/cmake/bin/cmake --build _cmake_build \
	&& /usr/local/cmake/bin/cmake --build _cmake_build --target install	

install/lib/openssl-1.1.1k:
	   wget https://ftp.openssl.org/source/openssl-1.1.1k.tar.gz \
	&& tar xf openssl*.gz \
	&& cd openssl* \
	&& ./config --prefix=/usr --openssldir=/etc/ssl zlib-dynamic \
	&& make -j$(nproc) \
	&& make install

.PHONY : install/getlibs

install/getlibs:
	   sh ../GitClonner.sh https://github.com/confluentinc/librdkafka ../third_party/librdkafka \
	&& sh ../GitClonner.sh https://github.com/adeharo9/cpp-dotenv.git ../third_party/cpp-dotenv

.PHONY : install/cmake-3.22.2

install/cmake-3.22.2:
	   cd /usr/local/src \
	&& curl -LO https://github.com/Kitware/CMake/releases/download/v3.22.2/cmake-3.22.2-linux-x86_64.tar.gz \
	&& tar -xvf cmake-3.22.2-linux-x86_64.tar.gz \
	&& mv -f cmake-3.22.2-linux-x86_64 /usr/local/cmake
         
.PHONY : preinstall

preinstall: 
	   yum -y update \
	&& yum install -y centos-release-scl \
	&& yum clean all \
	&& yum install -y \
		   devtoolset-9-* \
	       wget \
		   gcc \
		   gcc-c++ \
	       bzip2 \
	       make \
	       curl \
	       git \
	       vim \
	       perl-core \
	       pcre-devel \
	       zlib-devel \
	&& yum remove cmake -y \
	&& scl enable devtoolset-9 bash

	
