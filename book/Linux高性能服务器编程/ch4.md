# 第4章 TCP/IP通信案例：访问Internet上的Web服务器

## 4.5 本地名称查询

1.
当wget访问某个Web服务器时，它先读取环境变量http_proxy。如果该环境变量被设置，并且我们没有阻止wget使用代理服务，则wget将通过http_proxy指定的代理服务器来访问Web服务。
