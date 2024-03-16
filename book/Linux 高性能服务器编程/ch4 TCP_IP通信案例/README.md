# 第4章 TCP/IP 通信案例：访问 Internet 上的 Web 服务器

## 4.2 部署代理服务器

### 4.2.1 HTTP 代理服务器的工作原理

1. 在 HTTP 通信链上，客户端和目标服务器之间通常存在某些中转代理服务器，它们提供对目标资源的中转访问。一个 HTTP 请求可能被多个代理服务器转发，后面的服务器称为前面服务的上游服务器。代理服务器按照其使用方式和作用，分为正向代理服务器、反向代理服务器和透明代理服务器：
   * 正向代理要求客户端自己设置代理服务器的地址。客户的每次请求都将直接发送到该代理服务器，并由代理服务器来请求目标资源
   * 反向代理则被设置在服务器端，指用代理服务器来接收 Internet 上的连接请求，然后将请求转发给内部网络上的服务器，并将从内部服务器上得到的结果返回给客户端
   * 透明服务器只能设置在网关上

## 4.5 本地名称查询

1. 当 `wget` 访问某个 Web 服务器时，它先读取环境变量 `http_proxy`。如果该环境变量被设置，并且我们没有阻止 `wget` 使用代理服务，则 `wget`将通过 `http_proxy` 指定的代理服务器来访问 Web 服务。

## 4.6 HTTP 通信

### 4.6.1 HTTP 请求

HTTP 请求的部分内容如下：

```text
GET HTTP://baidu.com/index.html HTTP/1.0
User-Agent: Wget/1.12 (linux-gun)
Host: www.baidu.com
Connection: close
```

第一行是请求行，其中 `GET` 是请求方法

常见的 HTTP 请求方法

| 请求方法  |      含义     |
| :------:|:------------- |
| GET     | 申请获取资源，而不对服务器产生任何其他影响 |
| HEAD    | 和 GET 类似，不过仅要求服务器返回头部信息，而不需要传输任何实际内容 |
| POST    | 客户端向服务器提交数据的方法，这种方法会影响服务器：服务器可能根据收到的数据动态创建新的资源，也可能更新原有资源 |
| PUT     | 上传某个资源 |
| DELETE  | 删除某个资源 |
| TRACE   | 要求目标服务器返回原始 HTTP 请求的内容，可用来查看中间服务器对 HTTP 请求的影响 |
| OPTIONS | 查看服务器对某个特定 URL 都支持哪些请求方法 |
| CONNECT | 用于某些代理服务器，它们能把请求的连接转化为一个安全隧道 |
| PATCH   | 对某个资源做部分修改 |

1. 这些方法中，HEAD、GET、OPTIONS 和 TRACE 被视为安全的方案，因为它们只是从服务器获得资源或信息，而不对服务器进行任何修改。而 POST、PUT、DELETE 和 PATCH 则会影响服务器上的资源
2. `http://www.baidu.com/index.html` 是目标文件的 URL。其中 `http` 是所谓的 scheme，表示获取目标资源需要使用的应用层协议。其他常见的 scheme 还有 ftp、rtsp 和 file 等。`www.baidu.com` 指定资源文件所在的目标主机。`index.html` 指定资源文件的名称。
3. `HTTP/1.0` 表示客户端（wget程序）使用的是 HTTP 的版本号是 1.0
4. `User-Agent: Wget/1.12(linux-gun)` 表示客户端使用的程序是 wegt
5. `Host: www.baidu.com` 表示目标主机名是 `www.baidu.com`。HTTP 协议规定 HTTP 请求中必须包含的头部字段就是目标主机名
6. `Connection: close` 用于告诉服务器处理完这个 HTTP 请求之后就关闭连接，keep-alive 则表示保持一段时间以等待后续请求

在所有头部字段之后，HTTP 请求必须包含一个空行，以表示头部字段的结束。请求行和每个头部字段都必须以 `<CR><LF>` 结束（回车和换行）；而空行则必须只包含一个 `<CR><LF>`，不能有其他字符，甚至是空白字符

在空行之后，HTTP 请求可以包含可选的消息体。如果消息体非空，则 HTTP 请求的头部字段中必须包含描述该消息体长度的字段 `Content-Length`

### 4.6.2 HTTP 应答

```text
HTTP/1.0 200 OK
Server: BWS/1.0
Content-Length: 8024
Content-Type: text/html;chaset =gbk
Set-Cookie: BAIDUID=A5B6C72D68CF639CE8896FD79A03FBD8:FG=1; expires=Wed,
            04 -Jul-42 00:10;47 GMT; path=/; domain=.baidu.com
Via: 1.0 localhost (squid/3.0 STABLE18)
```

第一行是状态行。`HTTP/1.0` 是服务器使用的 HTTP 协议的版本号。通常，服务器需要使用和客户端相同的 HTTP 协议版本

常见的 HTTP 状态码和状态信息及其含义如下表

| 状态类型 | 状态码和状态信息 | 含义 |
| :-----: |:------------: |:---- |
| 1xx 信息 | 100 Continue | 服务器收到了客户端的请求行和头部信息，告诉客户端继续发送数据部分。客户端通常要先发送 Expect: 100-continue 头部字段告诉服务器自己还有数据要发送 |
| 2xx 成功 | 200 OK | 资源请求成功 |
| 3xx 重定向 | 301 Moved Permanently | 资源被转移了，请求将被重定向 |
| 3xx 重定向 | 302 Found | 通知客户端资源能在其他地方找到，但需要使用 GET 方法来获得它 |
| 3xx 重定向 | 303 Not Modified | 表示被申请的资源没有更新，和之前获取的相同 |
| 3xx 重定向 | 304 Temporary Redirect | 通知客户端资源能在其他地方找到。与 302 不同的是，客户端可以使用和原始请求相同的请求方法访问该资源 |
| 4xx 客户端错误 | 400 Bad Request | 通用客户请求错误 |
| 4xx 客户端错误 | 401 Unauthorized | 请求需要认证信息 |
| 4xx 客户端错误 | 403 Forbidden | 访问被服务器禁止，通常是由于客户端没有权限访问该资源 |
| 4xx 客户端错误 | 404 Not Found | 资源没找到 |
| 4xx 客户端错误 | 407 Proxy Authentication Required | 客户端需要先获得代理服务器的认证 |
| 5xx 服务端错误 | 500 Internal Server Error | 通用服务器错误 |
| 5xx 服务端错误 | 503 Service Unavailable | 暂时无法访问服务器 |

Cookie 是服务器发送给客户端的特殊信息，客户端每次向服务器发送请求的时候都需要带上这些信息
