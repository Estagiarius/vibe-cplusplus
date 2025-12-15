#!/bin/bash

# Setup Script for Academic Management System
# Supported: Ubuntu, Pop!_OS, Debian

echo "=========================================="
echo " Setting up Environment for AcademicManager"
echo "=========================================="

if [ "$EUID" -ne 0 ]
  then echo "Please run as root (sudo ./setup.sh)"
  exit
fi

echo "[1/3] Updating repositories..."
apt-get update

echo "[2/3] Installing Build Tools (CMake, G++)..."
apt-get install -y build-essential cmake

echo "[3/3] Installing Qt 6 and Dependencies..."
# Try installing standard packages.
# Note: Some distros use 'libqt6network6' instead of 'libqt6network6t64'.
# We try a broad install to cover bases.

apt-get install -y \
    qt6-base-dev \
    qt6-base-dev-tools \
    libqt6sql6-sqlite \
    libqt6network6t64 \
    libqt6test6t64 \
    libqt6widgets6t64 \
    libgl1-mesa-dev

# Fallback for older Ubuntu versions/Debian that don't have t64 suffix
if [ $? -ne 0 ]; then
    echo "Trying alternative package names for compatibility..."
    apt-get install -y \
        libqt6network6 \
        libqt6test6 \
        libqt6widgets6
fi

echo "=========================================="
echo " Setup Complete!"
echo " You can now build the project:"
echo "   mkdir build && cd build"
echo "   cmake .. && make"
echo "=========================================="
