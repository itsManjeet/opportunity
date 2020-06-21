#!/bin/sh

DESTDIR=${DESTDIR:-"/"}
PREFIX=${PREFIX:-"usr"}
BINDIR=${BINDIR:-"$PREFIX/bin/"}
LIBEXECDIR=${LIBEXECDIR:-"$PREFIX/libexec"}
DATADIR=${DATADIR:-"$PREFIX/share"}

mkdir -pv build/
cd src
go build -o ../build/opportunity
cd ..
install -vDm755 build/opportunity $DESTDIR/$BINDIR/opportunity

sed -i "s|data/ui.glade|/$DATADIR/opportunity/ui.glade|g" $DESTDIR/$BINDIR/opportunity

install -vDm755 data/releax-post-process.sh $DESTDIR/$BINDIR/releax-post-process
install -vDm644 data/{logo.png,ui.glade} -t $DESTDIR/$DATADIR/opportunity
install -vDm755 data/opportunity.desktop -t $DESTDIR/$DATADIR/applications
