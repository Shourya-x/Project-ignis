#!/usr/bin/env bash

set -e

if [ $# -ne 1 ]; then
    echo "Usage: ./new_module.sh <module_name>"
    exit 1
fi

NAME="$1"

# Ensure script runs relative to the project root where new_module.sh lives
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

HEADER=$(echo "$NAME" | tr '[:lower:]' '[:upper:]')

mkdir -p "Modules/$NAME"

cat > "Modules/$NAME/$NAME.h" <<EOF
#ifndef ${HEADER}_H
#define ${HEADER}_H

#ifdef __cplusplus
extern "C" {
#endif

void ${NAME}_Init(void);

#ifdef __cplusplus
}
#endif

#endif
EOF

cat > "Modules/$NAME/$NAME.c" <<EOF
#include "${NAME}.h"

void ${NAME}_Init(void)
{

}
EOF

echo "Created:"
echo "  Modules/$NAME/$NAME.c"
echo "  Modules/$NAME/$NAME.h"

# Refresh CMake build & clangd compile_commands.json if CMakePresets.json exists
if [ -f "CMakePresets.json" ] && command -v cube-cmake >/dev/null 2>&1; then
    cube-cmake --preset Debug >/dev/null
    echo "Updated CMake & IntelliSense (compile_commands.json) for module '$NAME'."
fi
