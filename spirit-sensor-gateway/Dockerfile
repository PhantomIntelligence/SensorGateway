FROM base/devel:latest

# Update pacman and its mirrors so it's fast
RUN pacman --noconfirm -S pacman-mirrorlist reflector
RUN reflector --latest 100 --save /etc/pacman.d/mirrorlist

# Update system
RUN pacman --noconfirm -Syyu

# Install necessary programs
RUN pacman --noconfirm -S git cmake tar

# Install packer to ease aur installation
RUN git clone https://aur.archlinux.org/packer.git && \
    cd packer && \
    makepkg -sirc && \
    cd .. && rm -r packer

