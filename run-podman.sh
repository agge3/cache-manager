#!/usr/bin/env bash

NAME="cm"

podman build -t "$NAME" .
podman run --privileged -it --rm "$NAME"
