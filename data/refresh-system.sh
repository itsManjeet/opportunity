#!/bin/sh

pwconv
grpconv

gdk-pixbuf-query-loaders --update-cache
glib-compile-schemas /usr/share/glib-2.0/schemas
update-mime-database /usr/share/mime/
update-desktop-database

sys-usrgrp fix