#!/usr/bin/perl

use strict;

my(@files)=`(cd ../Source/Engine/; find . -iname "*.cpp" -type f)`;
my(@sourceDirs)= qw(../Source/Engine/ ../Source/Engine/rapidxml-1.13/ ../Source/Engine/GUI/ ../Source/Engine/Scripting/);
my($SOURCES) = join(' ', map { chomp $_; $_ =~ s/^\.\///; $_; } @files);
my($OBJS) = join(' ', map { $_ =~ s/cpp$/bc/; $_ =~ s/^[^\/]+\///; $_; } @files);
my($HEADERS) = join(' ', map { "-I" . $_; } @sourceDirs);
my($RULES) = join("\n", map { "%.bc: $_%.cpp\n\t\$(COMPILER) -c -emit-llvm \$(HEADERS) \$<\n" } @sourceDirs);

open(MFILE, ">Makefile") || die("Can\'t open Makefile $!");

print MFILE <<EOF;
COMPILER := clang++ -stdlib=libc++ -std=c++11
HEADERS := $HEADERS -I../Libraries/LeapSDK/include/ -I../Libraries/netLink/include/ -I/usr/local/include/bullet/
LINKER := -o Olypsum.out -L/usr/lib/Leap/ -lLeap -lv8 -lopenal -lvorbis -lvorbisfile -lGL -lSDL -lSDL_image -lSDL_ttf -lLinearMath -lBulletCollision -lBulletDynamics -lBulletSoftBody -lBulletMultiThreaded ../Libraries/netLink/out/libNetLink.bc
SOURCES := $SOURCES
OBJS := $OBJS

$RULES

build: \$(OBJS)
	\$(COMPILER) \$(LINKER) \$(OBJS)

clean:
	rm -f *.bc *.out

EOF

close(MFILE);

system "make build -j8";