FROM base/devel:latest as build-base

# Update pacman and its mirrors so it's fast
RUN pacman --noconfirm -S pacman-mirrorlist reflector && \
    reflector --latest 100 --save /etc/pacman.d/mirrorlist

# Update system and install dependencies
RUN pacman --noconfirm -Syyu && \
    pacman --noconfirm -S git cmake

# Install packer to ease aur installation
RUN git clone https://aur.archlinux.org/packer.git && \
    cd packer && \
    makepkg -sirc && \
    cd .. && rm -r packer

# Install conan & its dependencies
RUN packer --noconfirm --noedit conan

# Install the SpiritSensorGateway
RUN cmake --build $PWD/cmake-build-debug --target runtests -- -j 4

CMD ['ls && ./cmake-build-debug/test/runtests']

