#!/bin/sh
INSTALL_PREFIX="$1"
OUTPUT_PATH="$2"
echo "#!/bin/sh" > "$OUTPUT_PATH"
echo "source \"$INSTALL_PREFIX/share/npdfr/venv/bin/activate\"" >> "$OUTPUT_PATH"
echo "\"$INSTALL_PREFIX/share/npdfr/npdfr\" \"\$@\"" >> "$OUTPUT_PATH"
chmod +x "$OUTPUT_PATH"
