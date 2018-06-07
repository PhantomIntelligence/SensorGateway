FROM base/devel:latest

# Update system and install dependencies
RUN pacman --noconfirm -Syyu && \
    pacman-db-upgrade && \
    pacman --noconfirm -S reflector && \
    reflector --latest 100 --save /etc/pacman.d/mirrorlist && \
    pacman --noconfirm -S sed wget git cmake jshon expac python-pip python-pyjwt python2-setuptools python-setuptools python-yaml python-fasteners python-bottle python-pylint python-future python-pygments python-astroid python-deprecation

RUN pacman --noconfirm -Syyu &&\
    pacman --noconfirm -S base-devel linux-headers 

RUN ls -lAh /lib/modules/

# Versioning problem with urllib3. Check if conan still needs this dependency
RUN pip install --force-reinstall msgpack-python urllib3==1.21.1

RUN usermod --home /tmp/nobody --shell /bin/sh nobody

COPY dev-script/aurInstallDependencies /usr/bin/
RUN chmod a+x /usr/bin/aurInstallDependencies
RUN /bin/sh -c "aurInstallDependencies"

WORKDIR /builddir
COPY ./ /builddir/

# Install Kvaser's canlib
RUN wget http://canlandbucket.s3-website-eu-west-1.amazonaws.com/productionResourcesFiles/47b24f0a-8f35-4fc5-bd7e-444e805caa9c/linuxcan.tar.gz &&\
    tar -xzf linuxcan.tar.gz &&\
    cd linuxcan &&\
    ln -s /lib/modules/`ls --format=single-column --color=never /lib/modules | grep -v extra` /lib/modules/`uname -r` &&\
    ls /lib/modules/ -lAh &&\ 
    make 


ENTRYPOINT sed -i 's?/.*/SpiritSensorGateway?'`pwd`'?g' cmake-build-debug/CMakeCache.txt &&\
    ./dev-script/conanUpdateDependencies &&\
    sed -i s/libstdc++/libstdc++11/g ~/.conan/profiles/default &&\
    ./dev-script/conanUpdateDependencies &&\
    sed -i 's?/.*/SpiritSensorGateway?'`pwd`'?g' cmake-build-debug/Makefile &&\
    cmake --build /builddir/cmake-build-debug --target runtests -- -j 4 &&\
    /builddir/cmake-build-debug/test/runtests

