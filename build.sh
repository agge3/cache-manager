#!/usr/bin/env bash

#!/bin/bash
set -e

echo "=== Building cache-manager artifacts ==="

# Build the artifacts
echo "Building cache-manager library..."
docker build --platform linux/amd64 -f Dockerfile.build -t cache-manager-builder .

# Extract tarball
echo "Extracting tarball..."
rm -rf dist
mkdir -p dist

# Run container and copy tarball out
docker run --platform linux/amd64 --rm \
    -v $(pwd)/dist:/output \
    cache-manager-builder \
    bash -c "cp /cache-manager-dist.tar.gz /output/"

echo ""
echo "✓ Build complete!"
echo "  Tarball: dist/cache-manager-dist.tar.gz"
echo ""
echo "To use:"
echo "  tar xzf dist/cache-manager-dist.tar.gz"
echo "  # lib/cache-manager.cpp.o"
echo "  # lib/libtbb.so*"
echo "  # include/..."
