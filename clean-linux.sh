#!/bin/bash
set -e		# exit on error
SCRIPT_DIR="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

echo "Cleaning project..."

echo "Cleaning bin directory..."
rm -rf "$SCRIPT_DIR/bin"
echo "Cleaning build directory..."
rm -rf "$SCRIPT_DIR/build"

echo "Cleaning asio submodule..."
$SCRIPT_DIR/modules/asio/clean-linux.sh

echo "Cleaning spdlog submodule..."
$SCRIPT_DIR/modules/spdlog/clean-linux.sh

echo "Done"
