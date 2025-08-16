#!/bin/bash

VERSION=$(grep 'define FLING_VERSION' version.h | sed 's/.*"\(.*\)".*/\1/')
GIT_DESC=$(git describe --tags --always --dirty 2>/dev/null)

if [[ "$GIT_DESC" == "v$VERSION" ]]; then
    echo "$VERSION"
else
    echo "$VERSION-$GIT_DESC"
fi
