sbin_PROGRAMS = spear_usbloader

spear_usbloader_SOURCES = main.c
spear_usbloader_SOURCES += cmdline.c
spear_usbloader_SOURCES += spear_usb.c

spear_usbloader_CPPFLAGS = -Wall -Werror $(LIBUSB_CFLAGS)
spear_usbloader_LDADD = $(LIBUSB_LIBS)


