#!/bin/bash

if [ "$#" -lt 2 ]; then
  echo "Usage: $0 <IC type> <Matter version>"
  echo "IC type       : amebaz2 / amebaz2plus / amebad / ameba-rtos"
  echo "Matter version: v1.3 / v1.4"
  exit 1
fi

AMEBA="$1"
MATTER_VER="$2"

files_to_delete=(
  "$PWD/component/soc/realtek/8710c/misc/utilities/include/ctype.h"
)

BASE_DIR="$PWD/project"

delete_files() {
  for file_path in "${files_to_delete[@]}"; do
    if [ -e "$file_path" ]; then
      rm "$file_path"
      echo "File $file_path removed."
    else
      echo "File $file_path does not exist."
    fi
  done
}

modify_makefiles() {
  find "$BASE_DIR" -type f -name "Makefile" | while read -r FILE; do
    if grep -q "ENABLE_MATTER = 0" "$FILE"; then
      echo "Modifying $FILE"
      sed -i 's/^ENABLE_MATTER = 0/ENABLE_MATTER = 1/' "$FILE"
    fi
  done
}

case "$AMEBA" in
  amebaz2 | amebaz2plus)
    echo "Configuring for $AMEBA"
    delete_files
    if [ "$AMEBA" = "amebaz2plus" ]; then
      modify_makefiles
    fi
    ;;
  amebad | ameba-rtos)
    echo "Configuring for $AMEBA"
    ;;
  *)
    echo "Invalid IC type argument. Expected 'amebaz2', 'amebaz2plus', 'amebad', or 'ameba-rtos'."
    exit 1
    ;;
esac

case "$MATTER_VER" in
  v1.3 | v1.4)
    echo "Matter Version is set to $MATTER_VER"
    ;;
  *)
    echo "Invalid Matter version argument. Expected 'v1.3', or 'v1.4'."
    exit 1
    ;;
esac

if [ ! -d third_party ];then
    mkdir third_party
else
    rm third_party/connectedhomeip
fi

cd third_party
rm -rf connectedhomeip
ln -s ../../connectedhomeip connectedhomeip

cd ../

case "$AMEBA" in
  amebaz2 | amebaz2plus | amebad)
    if [ ! -d component/common/application/matter ] || [ -z "$(find component/common/application/matter -mindepth 1)" ]; then
      mkdir -p component/common/application/matter
      git clone https://github.com/Ameba-AIoT/ameba-rtos-matter.git component/common/application/matter
    fi
    ;;
  ameba-rtos)
    if [ ! -d component/application/matter ] || [ -z "$(find component/application/matter -mindepth 1)" ]; then
      mkdir -p component/application/matter
      git clone https://github.com/Ameba-AIoT/ameba-rtos-matter.git component/application/matter -b ameba-rtos/release/$MATTER_VER
    fi
    ;;
  *)
    echo "Invalid argument. Expected 'amebaz2', 'amebaz2plus', 'amebad', or 'ameba-rtos'."
    exit 1
    ;;
esac
echo "Matter setup complete"