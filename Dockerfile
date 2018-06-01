FROM base/devel:latest as build-base

# Update pacman and its mirrors so it's fast

# Update system and install dependencies
RUN pacman --noconfirm -Syyu && \
    pacman-db-upgrade && \
    pacman --noconfirm -S reflector && \
    reflector --latest 100 --save /etc/pacman.d/mirrorlist && \
    pacman --noconfirm -S git cmake

# Install packer to ease aur installation
RUN git clone https://aur.archlinux.org/packer.git && \
    cd packer && \
    makepkg -sirc && \
    #source $PWD/PKGBUILD && pacman -Syu --noconfirm --needed --asdeps "${makedepends[@]}" "${depends[@]}" && \
    cd .. && rm -r packer

# Install conan & its dependencies
RUN su docker -c 'packer -h && packer --noconfirm --noedit conan'

# Install the SpiritSensorGateway
RUN cmake --build $PWD/cmake-build-debug --target runtests -- -j 4

CMD ['ls && ./cmake-build-debug/test/runtests']

