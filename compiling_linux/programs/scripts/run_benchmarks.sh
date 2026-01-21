# Run benchmarks
#!/bin/bash
set -euo pipefail

PROGRAMS_DIR="/programs"
MODULES_DIR="/modules"

# Insert kernel modules
echo "Inserting kernel modules from $MODULES_DIR"
for mod in "$MODULES_DIR"/*.ko; do
    echo "Inserting module $mod"
    insmod "$mod"
    MODULE_NAME=$(basename "$mod" .ko)
done

# Run benchmarks
echo "Running benchmarks from $PROGRAMS_DIR"
for prog in "$PROGRAMS_DIR"/*; do
    if [[ -x "$prog" ]]; then
        echo "Running benchmark $prog"
        "$prog"
    else
        echo "Skipping non-executable $prog"
    fi
done
