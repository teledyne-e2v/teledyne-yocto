SUMMARY = "Resolution switching demo"
DESCRIPTION = "Demo application for fast resolution switching with V4L2"
LICENSE = "CLOSED"

inherit meson

SRC_URI = " \
    file://demo.c \
    file://meson.build \
    "

DEPENDS = " \
    v4l-utils \
    "

S = "${WORKDIR}"

do_install() {
    install -d ${D}${bindir}
    cp ${B}/fast-resolution-switching-demo ${D}${bindir}/fast-resolution-switching-demo
}

FILES_${PN} += " \
    ${bindir}/fast-resolution-switching-demo \
    "
