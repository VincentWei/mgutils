#!/bin/sh

cat m4/*.m4 > acinclude.m4
aclocal
automake --add-missing --foreign --copy
autoconf
