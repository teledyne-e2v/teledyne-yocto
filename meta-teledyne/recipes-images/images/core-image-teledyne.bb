DESCRIPTION = "Image with teledyne custom configuration"

LICENSE = "MIT"

inherit core-image

IMAGE_FEATURES += "ssh-server-openssh"

#
# Conman packages are required for networking
#
CONMANPKGS ?= " \
    connman \
    connman-plugin-loopback \
    connman-plugin-ethernet \
    connman-plugin-wifi \
    connman-client \
    "

#
# Graphical packages are added for GUI
#
GRAPHICAL ?= " \
    wayland \
    weston \
    weston-init \
    weston-xwayland \
    "

GSTREAMER ?= " \
    gstreamer1.0 \
    gstreamer1.0-libav \
    gstreamer1.0-plugins-bad \
    gstreamer1.0-plugins-base \
    gstreamer1.0-plugins-good \
    gstreamer1.0-plugins-ugly \
    gstd \
    "

IMAGE_INSTALL += " \
    ${CONMANPKGS} \
    ${GRAPHICAL} \
    ${GSTREAMER} \
    fast-resolution-switching-demo \
    i2c-tools \
    v4l-utils \
    xauth \
    "
