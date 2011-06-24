#!/bin/sh

set -fex

SRCROOT="${PWD}"

dmg_template="${SRCROOT}/dmg-template.dmg"
temp_dmg="${SRCROOT}/tmp.dmg"
product_dmg="${SRCROOT}/Spaces-Utility.dmg"
mnt_volume="/Volumes/Spaces-Utility"

cp "${dmg_template}" "${temp_dmg}"
chmod -R a+rwx "${temp_dmg}"

device=$(hdiutil attach -readwrite -noverify -noautoopen "${temp_dmg}" | \
         egrep '^/dev/' | sed 1q | awk '{print $1}')
sleep 2
cp "${SRCROOT}/spaces-util" "${mnt_volume}/"
chmod -Rf a+r  "${mnt_volume}"
chmod -Rf go-w "${mnt_volume}"

sync
sync
hdiutil detach ${device}

if [[ -f "${product_dmg}" ]]; then
    rm "${product_dmg}"
fi

hdiutil convert "${temp_dmg}" -format UDZO \
    -imagekey zlib-level=9 -o "${product_dmg}"

rm "${temp_dmg}"
echo "Complete!"

#rm -rf "$dir"
#mkdir "$dir"
#cp -R "$BUILT_PRODUCTS_DIR/$PROJECT_NAME" "$dir"
#cp -R "$SRCROOT/dmg-additional/" "$dir"
#rm -f "$dmg"
#hdiutil create -srcfolder "$dir" -volname "$PROJECT_NAME" "$dmg"
#hdiutil internet-enable -yes "$dmg"
#rm -rf "$dir"
