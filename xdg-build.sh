#!/bin/bash

REPO_DIR=/home/alvaropg/work/repo/hazlo
HAZLO_BUILD_DIR=$(pwd)/build

rm -rf $HAZLO_BUILD_DIR
mkdir -p $HAZLO_BUILD_DIR
xdg-app build-init $HAZLO_BUILD_DIR org.gnome.Sdk org.gnome.Platform 3.16

# Gom
cd ../gom
xdg-app build $HAZLO_BUILD_DIR ./configure.sh --prefix=/self
xdg-app build $HAZLO_BUILD_DIR make
xdg-app build $HAZLO_BUILD_DIR make install

# Hazlo
cd ../hazlo
xdg-app build $HAZLO_BUILD_DIR ./autogen.sh --prefix=/self
xdg-app build $HAZLO_BUILD_DIR make
xdg-app build $HAZLO_BUILD_DIR make install

xdg-app build-finish --command=hazlo --allow=x11 --allow=host-fs --allow=session-dbus $HAZLO_BUILD_DIR

mkdir -p $REPO_DIR
xdg-app build-export $REPO_DIR $HAZLO_BUILD_DIR org.gnome.Hazlo

rm -rf $HAZLO_BUILD_DIR
