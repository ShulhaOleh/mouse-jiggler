#!/bin/bash
set -e

REPO="ShulhaOleh/mouse-jiggler"
BINARY="mouse-jiggler-x86_64"
INSTALL_DIR="/usr/local/bin"
UDEV_RULES_DIR="/etc/udev/rules.d"
UDEV_RULE="99-mouse-jiggler.rules"

if [ "$EUID" -ne 0 ]; then
    echo "Please run as root: curl -sSL https://raw.githubusercontent.com/$REPO/main/install.sh | sudo bash"
    exit 1
fi

echo "Downloading mouse-jiggler..."
curl -sSL "https://github.com/$REPO/releases/latest/download/$BINARY" -o "$INSTALL_DIR/mouse-jiggler"
chmod +x "$INSTALL_DIR/mouse-jiggler"

echo "Installing udev rule..."
curl -sSL "https://raw.githubusercontent.com/$REPO/main/rules/$UDEV_RULE" -o "$UDEV_RULES_DIR/$UDEV_RULE"

echo "Reloading udev rules..."
udevadm control --reload-rules
udevadm trigger

echo "Done! Run 'mouse-jiggler' to start."
