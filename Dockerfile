FROM base/devel:latest as build-base

# Update system and install dependencies
RUN pacman --noconfirm -Syyu && \
    pacman-db-upgrade && \
    pacman --noconfirm -S reflector && \
    reflector --latest 100 --save /etc/pacman.d/mirrorlist && \
    pacman --noconfirm -S sed git cmake jshon expac python-pip python-pyjwt python2-setuptools python-setuptools python-yaml python-fasteners python-bottle python-pylint python-future python-pygments python-astroid python-deprecation

RUN usermod --home /tmp/nobody --shell /bin/sh nobody

COPY dev-script/aurInstallDependencies /usr/bin/
RUN chmod a+x /usr/bin/aurInstallDependencies
RUN /bin/sh -c "aurInstallDependencies"

RUN mkdir /builddir 
COPY ./ /builddir/
RUN cd builddir && \
    sed -i 's?/.*/SpiritSensorGateway?'`pwd`'?g' cmake-build-debug/CMakeCache.txt && \
    ./dev-script/conanUpdateDependencies && \
    sed -i s/libstdc++/libstdc++11/g ~/.conan/profiles/default &&\
    ./dev-script/conanUpdateDependencies && \
    sed -i 's?/.*/SpiritSensorGateway?'`pwd`'?g' cmake-build-debug/Makefile && \
    ls -lAh
    #cmake --build cmake-build-debug 

#CMD ['ls && ./cmake-build-debug/test/runtests']
CMD ls builddir

