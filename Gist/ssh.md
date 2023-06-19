# 远程 SSH 连接功能

```
ssh-keygen -t ed25519 -f "$HOME\.ssh\id_ed25519-remote-ssh"
```

```
export USER_AT_HOST="your-user-name-on-host@hostname"
export PUBKEYPATH="$HOME/.ssh/id_ed25519.pub"
ssh-copy-id -i "$PUBKEYPATH" "$USER_AT_HOST"
```

