#!/usr/bin/perl

use strict;

# /usr/lib/x86_64-linux-gnu/libogg.so.0 /usr/lib/x86_64-linux-gnu/libvorbis.so.0 /usr/lib/x86_64-linux-gnu/libvorbisfile.so.3
my(@libs) = qw(/usr/lib/libc++.so.1 /usr/lib/libv8.so /usr/lib/Leap/libLeap.so /usr/local/lib/libSDL2-2.0.so.0 /usr/local/lib/libSDL2_ttf-2.0.so.0 /usr/local/lib/libSDL2_image-2.0.so.0 /usr/local/lib/libBulletCollision.so.2.81  /usr/local/lib/libBulletDynamics.so.2.81 /usr/local/lib/libBulletSoftBody.so.2.81 /usr/local/lib/libBulletMultiThreaded.so.2.81 /usr/local/lib/libLinearMath.so.2.81);
my(@files)=`(cd ../Source/Engine/; find . -iname "*.cpp" -type f)`;
my(@sourceDirs)= qw(../Source/Engine/ ../Source/Engine/rapidxml-1.13/ ../Source/Engine/GUI/ ../Source/Engine/Scripting/);
my($SOURCES) = join(' ', map { chomp $_; $_ =~ s/^\.\///; $_; } @files);
my($OBJS) = join(' ', map { $_ =~ s/cpp$/bc/; $_ =~ s/^[^\/]+\///; "build/".$_; } @files);
my($HEADERS) = join(' ', map { "-I" . $_; } @sourceDirs);
my($RULES) = join("\n", map { "build/%.bc: $_%.cpp\n\t\$(COMPILER) -o \$@ -c -emit-llvm \$(HEADERS) \$<\n" } @sourceDirs);

system "mkdir -p build ../bin";
foreach my $f (@libs) {
	system "cp $f ../bin/";
}

open(MFILE, ">build/Makefile") || die("Can\'t open Makefile $!");
print MFILE <<EOF;
COMPILER := clang++ -stdlib=libc++ -std=c++11 -DDEBUG
HEADERS := $HEADERS -I../Libraries/LeapSDK/include/ -I../Libraries/netLink/include/ -I/usr/local/include/bullet/
LINKER := -o ../bin/Olypsum.out -Lbin/ -Wl,-rpath,'\$\$ORIGIN' -lLeap -lv8 -lopenal -lvorbis -lvorbisfile -lGL -lSDL2 -lSDL2_image -lSDL2_ttf -lLinearMath -lBulletCollision -lBulletDynamics -lBulletSoftBody -lBulletMultiThreaded ../Libraries/netLink/out/libNetLink.bc
SOURCES := $SOURCES
OBJS := $OBJS

$RULES

../bin/Olypsum.out: \$(OBJS)
	\$(COMPILER) \$(LINKER) \$(OBJS)

clean:
	rm -Rf build/ ../bin/

EOF

close(MFILE);

system "make -j8 -fbuild/Makefile";