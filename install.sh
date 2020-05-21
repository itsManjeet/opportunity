#!/bin/sh

DESTDIR=${DESTDIR:-"/"}
PREFIX=${PREFIX:-"usr"}
BINDIR=${BINDIR:-"$PREFIX/bin/"}
LIBEXECDIR=${LIBEXECDIR:-"$PREFIX/libexec"}
DATADIR=${DATADIR:-"$PREFIX/share"}

install -vDm755 src/opportunity.py $DESTDIR/$BINDIR/opportunity

sed -i "s|data/ui.glade|/$DATADIR/opportunity/ui.glade|g" $DESTDIR/$BINDIR/opportunity

install -vDm755 data/installer.sh -t $DESTDIR/$LIBEXECDIR/opportunity/
install -vDm644 data/{logo.jpg,ui.glade} -t $DESTDIR/$DATADIR/opportunity

install -vDm755 data/opportunity.desktop -t $DESTDIR/$DATADIR/applications