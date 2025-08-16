#!/bin/bash

VERSION=$(grep 'define FLING_VERSION' version.h | sed 's/.*"\(.*\)".*/\1/')
GIT_DESC=$(git describe --tags --always --dirty 2>/dev/null)

if [[ "$GIT_DESC" == "v$VERSION" ]]; then
    echo "$VERSION"
elif [[ "$GIT_DESC" == "v$VERSION-dirty" ]]; then
    echo "$VERSION-dirty"
else
    echo "$VERSION-$GIT_DESC"
fi
