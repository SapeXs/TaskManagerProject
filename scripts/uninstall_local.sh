#!/usr/bin/env bash

set -euo pipefail

systemctl --user stop taskmanagerd.service 2>/dev/null || true
systemctl --user disable taskmanagerd.service 2>/dev/null || true

rm -f "$HOME/.config/systemd/user/taskmanagerd.service"
rm -f "$HOME/.local/bin/taskmanagerd"
rm -f "$HOME/.local/bin/taskctl"

systemctl --user daemon-reload

echo "TaskManager service and binaries removed."
echo
echo "User data was not removed:"
echo "  $HOME/.local/share/taskmanager"
echo
echo "To remove user data manually:"
echo "  rm -rf \"$HOME/.local/share/taskmanager\""
