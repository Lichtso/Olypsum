#!/usr/bin/perl
use strict;

my($version) = "0.5.6";
my(@libs) = qw(/usr/lib/libc++.so.1 /usr/lib/x86_64-linux-gnu/libopenal.so.1 ../Libraries/JavaScriptCore/bin64/libjavascriptcoregtk-3.0.so.0 /usr/local/lib/libSDL2-2.0.so.0 /usr/local/lib/libSDL2_ttf-2.0.so.0 /usr/local/lib/libSDL2_image-2.0.so.0 /usr/local/lib/libBulletCollision.so.2.82 /usr/local/lib/libBulletDynamics.so.2.82 /usr/local/lib/libBulletSoftBody.so.2.82 /usr/local/lib/libBulletMultiThreaded.so.2.82 /usr/local/lib/libLinearMath.so.2.82);
my(@files)=`(cd ../Source/Engine/; find . -iname "*.cpp" -type f)`;
my(@sourceDirs)= qw(../Source/Engine/ ../Source/Engine/cityhash/ ../Source/Engine/rapidxml/ ../Source/Engine/GUI/ ../Source/Engine/Menu/ ../Source/Engine/Scripting/);
my($SOURCES) = join(' ', map { chomp $_; $_ =~ s/^\.\///; $_; } @files);
my($OBJS) = join(' ', map { $_ =~ s/cpp$/bc/; $_ =~ s/^[^\/]+\///; "build/".$_; } @files);
my($RULES) = join("\n", map { "build/%.bc: $_%.cpp\n\t\$(COMPILER) -o \$@ -c -emit-llvm \$(HEADERS) \$<\n" } @sourceDirs);

system "mkdir -p olypsum/opt/olypsum/lib";
system "cp -R ../Resources olypsum/opt/olypsum/";
foreach my $f (@libs) {
	system "cp $f olypsum/opt/olypsum/lib/";
}

open(Makefile, ">Makefile") || die("Can\'t open Makefile $!");
print Makefile <<EOF;
BIN := olypsum/opt/olypsum/bin/olypsum
COMPILER := clang++ -stdlib=libc++ -std=c++11 -DVERSION='"$version"'
HEADERS := -I../Source/Engine/ -I../Libraries/ -I../Libraries/bulletPhysics/src/
LINKER := -o \$(BIN) -Lolypsum/opt/olypsum/lib/ -Wl,-rpath,'\$\$ORIGIN/../lib' -ljavascriptcoregtk-3.0 -lopenal -lvorbis -lvorbisfile -lGL -lSDL2 -lSDL2_image -lSDL2_ttf -lLinearMath -lBulletCollision -lBulletDynamics -lBulletSoftBody -lBulletMultiThreaded ../Libraries/netLink/out/libNetLink.bc
SOURCES := $SOURCES
OBJS := $OBJS

$RULES

\$(BIN): \$(OBJS)
	\$(COMPILER) \$(LINKER) \$(OBJS)

package:
	strip olypsum/opt/olypsum/bin/olypsum
	dpkg-deb -z8 -Zgzip --build olypsum

clean:
	rm -Rf build/ olypsum/
	rm -f olypsum.deb Makefile
EOF
close(Makefile);

system "mkdir -p olypsum/usr/share/applications";
open(desktop, ">olypsum/usr/share/applications/olypsum.desktop") || die("Can\'t open olypsum.desktop $!");
print desktop <<EOF;
[Desktop Entry]
Version=$version
Name=Olypsum
GenericName=Game Engine
Exec=/opt/olypsum/bin/olypsum
Icon=/opt/olypsum/Resources/Icon64.png
Terminal=false
Type=Application
Categories=Games
EOF
close(desktop);

system "mkdir -p olypsum/DEBIAN";
open(control, ">olypsum/DEBIAN/control") || die("Can\'t open control $!");
print control <<EOF;
Package: olypsum
Section: games
Maintainer: Alexander Meißner <AlexanderMeissner\@gmx.net>
Architecture: amd64
Depends: libc6 (>= 2.15)
Version: $version-1
Homepage: http://gamefortec.net/
Description: Gamefortecs 3D-Game-Engine
Installed-Size: 50000
EOF
close(control);

system "mkdir -p build olypsum/opt/olypsum/bin";
system "make -j8";