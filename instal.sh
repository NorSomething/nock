#!/usr/bin/env bash
set -euo pipefail

PAMFILE="/etc/pam.d/nock"

if [[ $EUID -ne 0 ]]; then
    echo "Run with sudo: sudo ./install.sh"
    exit 1
fi

if [[ -f "$PAMFILE" ]]; then
    echo "PAM config already exists at $PAMFILE, skipping."
    exit 0
fi

cat > "$PAMFILE" << 'EOF'
auth       include      system-local-login
account    include      system-local-login
EOF

chmod 644 "$PAMFILE"
echo "Installed PAM config to $PAMFILE"
