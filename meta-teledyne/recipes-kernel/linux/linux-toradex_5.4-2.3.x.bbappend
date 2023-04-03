FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI += "file://toradex_kernel-5.4.193_verdin-imx8m-mini_topaz2m-v0.6.patch \
            file://topaz2m.cfg \
            "
