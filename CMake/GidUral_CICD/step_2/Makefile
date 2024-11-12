.PHONY : install


install: install/lib/openssl-1.1.1k
	   cp librdkafka.so.1 /usr/local/lib \
	&& cp librdkafka++.so.1 /usr/local/lib \
	&& echo "/usr/local/lib" >> /etc/ld.so.conf \
	&& ldconfig \
	&& chmod +x ./GidUralClient

.PHONY : install/lib/openssl-1.1.1k

install/lib/openssl-1.1.1k: preinstall
	   mkdir -p download \
	&& cd download \
	&& wget https://ftp.openssl.org/source/openssl-1.1.1k.tar.gz \
	&& tar xf openssl*.gz \
	&& cd openssl* \
	&& ./config --prefix=/usr --openssldir=/etc/ssl zlib-dynamic \
	&& make -j$(nproc) \
	&& make install \
	&& cd ../../ \
	&& rm -r download
     
.PHONY : preinstall

preinstall: 
	   yum -y update \
	&& yum install -y \
	       wget \
		   gcc \
	       perl-core \
	       zlib-devel

	
