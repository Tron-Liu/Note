# 第4章 TCP/IP 通信案例：访问 Internet 上的 Web 服务器

## 4.5 本地名称查询

1. 当 `wget` 访问某个 Web 服务器时，它先读取环境变量 `http_proxy`。如果该环境变量被设置，并且我们没有阻止 `wget` 使用代理服务，则 `wget`将通过 `http_proxy` 指定的代理服务器来访问 Web 服务。
