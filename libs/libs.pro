TEMPLATE = subdirs

SUBDIRS = \
    hoedown \
    googletest \
    librepcb \
    quazip \
    sexpresso

librepcb.depends = hoedown quazip sexpresso
