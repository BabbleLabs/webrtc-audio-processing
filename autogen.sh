#!/bin/sh

OS="`uname`"

case $OS in
    'Linux')
        echo "Platform: Linux"
        LIBTOOLIZE=libtoolize
        ;;
    'Darwin')
        ARCH="`arch`"
        case $ARCH in
            'arm64')
                echo "Platform: Mac OS ARM"
                ;;
            'i386')
                echo "Platform: Mac OS X64"
                ;;
        esac
        LIBTOOLIZE=glibtoolize
        ;;
esac

$LIBTOOLIZE
aclocal
automake --add-missing --copy
autoconf
./configure ${@}
