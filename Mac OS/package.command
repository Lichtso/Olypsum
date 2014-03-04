#!/bin/bash

APP_NAME="Olypsum"
DMG_BACKGROUND_IMG="unpacking.png"

# make sure we are in the correct dir when we double-click a .command file
dir=${0%/*}
if [ -d "$dir" ]; then
  cd "$dir"
fi

# you should not need to change these
APP_EXE="${APP_NAME}.app/Contents/MacOS/${APP_NAME}"
DMG_TMP="${APP_NAME}-tmp.dmg"
DMG_FINAL="${APP_NAME}.dmg"
STAGING_DIR="./package"

# clear out any old data
rm -rf "${DMG_TMP}" "${DMG_FINAL}"

# strip the executable
pushd "${STAGING_DIR}"
echo "Stripping ${APP_EXE}..."
strip -u -r "${APP_EXE}"
popd

# figure out how big our DMG needs to be
#  assumes our contents are at least 1M!
SIZE=`du -sh "${STAGING_DIR}" | sed 's/\([0-9\.]*\)M\(.*\)/\1/'` 
SIZE=`echo "${SIZE} + 2.0" | bc | awk '{print int($1+0.5)}'`

if [ $? -ne 0 ]; then
   echo "Error: Cannot compute size of staging dir"
   exit
fi

echo "Dmg has ${SIZE} MB"

# create the temp DMG file
hdiutil create -srcfolder "${STAGING_DIR}" -volname "${APP_NAME}" -fs HFS+ \
      -fsargs "-c c=64,a=16,e=16" -format UDRW -size ${SIZE}M "${DMG_TMP}"

echo "Created DMG: ${DMG_TMP}"

# mount it and save the device
DEVICE=$(hdiutil attach -readwrite -noverify "${DMG_TMP}" | \
         egrep '^/dev/' | sed 1q | awk '{print $1}')

sleep 2

# tell the Finder to resize the window, set the background,
#  change the icon size, place the icons in the right position, etc.
echo '
   tell application "Finder"
     tell disk "'${APP_NAME}'"
           open
           set current view of container window to icon view
           set toolbar visible of container window to false
           set statusbar visible of container window to false
           set the bounds of container window to {100, 100, 800, 600}
           set position of item "'${APP_NAME}'.app" of container window to {203, 250}
           set position of item "Applications" of container window to {512, 250}
           set viewOptions to the icon view options of container window
           set arrangement of viewOptions to not arranged
           set icon size of viewOptions to 100
           set background picture of viewOptions to file ".background:unpacking.png"
           close
           open
           update without registering applications
           delay 2
     end tell
   end tell
' | osascript

# unmount
sync
hdiutil detach "${DEVICE}"

# now make the final image a compressed disk image
echo "Creating compressed image"
hdiutil convert "${DMG_TMP}" -format UDZO -imagekey zlib-level=9 -o "${DMG_FINAL}"

# clean up
rm -rf "${DMG_TMP}"
echo 'Done.'

exit