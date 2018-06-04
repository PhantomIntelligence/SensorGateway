FROM base/devel:latest as build-base

# Update system and install dependencies
RUN pacman --noconfirm -Syyu && \
    pacman-db-upgrade && \
    pacman --noconfirm -S reflector && \
    reflector --latest 100 --save /etc/pacman.d/mirrorlist && \
    pacman --noconfirm -S git cmake jshon expac python-pip python-pyjwt python2-setuptools python-setuptools python-yaml python-fasteners python-bottle python-pylint python-future python-pygments python-astroid python-deprecation


RUN usermod --home /tmp/nobody --shell /bin/sh nobody

COPY dev-script/aurInstallDependencies /usr/bin/
RUN chmod a+x /usr/bin/aurInstallDependencies
RUN /bin/sh -c "aurInstallDependencies"

# Install the SpiritSensorGateway
RUN cmake --build $PWD/cmake-build-debug --target runtests -- -j 4

CMD ['ls && ./cmake-build-debug/test/runtests']

