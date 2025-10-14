#!/usr/bin/env bash

set -eou pipefail

BUILD_DIR=${BUILD_DIR:-"ctr-build"}
RUN_CMD=${RUN_CMD:-"./cache-manager"}
PERF_MODE=${PERF_MODE:-false}

# Parse CLI args
while [[ $# -gt 0 ]]; do
    case $1 in
        --perf)
            PERF_MODE=true
            shift
            ;;
        --run)
            RUN_CMD="$2"
            shift 2
            ;;
        *)
            echo "Unknown argument: $1"
            exit 1
            ;;
    esac
done

# Build step
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"
cmake ../
make -j$(nproc)

# Run step
if [ "$PERF_MODE" = true ]; then
    echo "Running with perf..."
    perf stat $RUN_CMD
else
    echo "Running: $RUN_CMD"
    exec $RUN_CMD
fi
