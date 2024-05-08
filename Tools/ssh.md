# 远程 SSH 连接功能

在 client 上生成密钥，并 copy 到 remote host：

```shell
ssh-keygen -t ed25519 -f "$HOME\.ssh\id_ed25519-remote-ssh"
```

```shell
export USER_AT_HOST="your-user-name-on-host@hostname"
export PUBKEYPATH="$HOME//.ssh//id_ed25519.pub"
ssh-copy-id -i "$PUBKEYPATH" "$USER_AT_HOST"
```

在 SSH 配置文件添加识别文件地址：

```
Host ubuntu2204
     HostName 192.168.1.2
     User summergift
     IdentityFile "C://Users//.ssh//id_ed25519-remote-ssh"
```

