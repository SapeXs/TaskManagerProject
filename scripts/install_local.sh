#!/usr/bin/env bash

set -euo pipefail

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

cmake -S "$PROJECT_ROOT" -B "$PROJECT_ROOT/build" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build "$PROJECT_ROOT/build" -j

mkdir -p "$HOME/.local/bin"
mkdir -p "$HOME/.local/share/taskmanager"
mkdir -p "$HOME/.config/systemd/user"

cp "$PROJECT_ROOT/build/taskmanagerd" "$HOME/.local/bin/taskmanagerd"
cp "$PROJECT_ROOT/build/taskctl" "$HOME/.local/bin/taskctl"
cp "$PROJECT_ROOT/packaging/systemd/taskmanagerd.service" "$HOME/.config/systemd/user/taskmanagerd.service"

systemctl --user daemon-reload
systemctl --user enable taskmanagerd.service
systemctl --user restart taskmanagerd.service

echo "TaskManager installed and started."
echo
echo "Status:"
echo "  systemctl --user status taskmanagerd.service"
echo
echo "Logs:"
echo "  journalctl --user -u taskmanagerd.service -f"
echo
echo "CLI:"
echo "  taskctl help"
