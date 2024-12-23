
<!-- TOC -->

- [TCP](#tcp)
    - [Client](#client)
        - [主函数](#主函数)
    - [Server](#server)
        - [主函数](#主函数-1)
        - [编码解码](#编码解码)
- [WebSocket](#websocket)
    - [总结](#总结)
    - [Ws特点](#ws特点)
    - [Client](#client-1)
        - [主函数](#主函数-2)
    - [Server](#server-1)
        - [主函数](#主函数-3)
        - [连接中心](#连接中心)
        - [连接](#连接)
        - [网页](#网页)
    - [语法](#语法)
        - [html](#html)

<!-- /TOC -->

<div STYLE="page-break-after: always;"></div>

# TCP
## Client
### 主函数
`client.go`
- 发送消息
```go
func sendAuto()
```
1. 创建一个TCP连接，延迟关闭
1. `proto`编码消息，将编码后的消息写入连接

- 命令行输入发送消息
```go
func sendBob()
reader := bufio.NewReader(os.Stdin)
```
1. 创建一个TCP连接，延迟关闭
1. 无限循环持续读取用户输入的文本消息
1. `proto`编码消息，将编码后的消息写入连接

## Server
### 主函数
`server.go`
- 全局
```go
import (
	"bufio"
	"fmt"
	"goChat/proto"
	"io"
	"net"
)
```

- 主函数
```go
func main()
```
1. 创建一个TCP服务器监听指定IP端口，延迟关闭
1. 等待一个一个一个客户端连接，开启新协程

- 处理client发送的消息
```go
func process(conn net.Conn)
```
1. 缓冲区读取器
1. `proto`解码消息，若读取到文件末尾io.EOF返回

### 编码解码
`proto.go`
- 编码
```go
func Encode(message string) ([]byte, error)
```
1. 读取消息的长度，转换成int32类型（占4个字节）
1. 写入消息头和消息实体

- 解码
```go
func Decode(reader *bufio.Reader) (string, error)
```
1. 读取消息的长度-前4个字节的数据
1. 读取消息的数据

# WebSocket
## 总结
connection：用户连接。包含用户信息和连接
tmessage：收发消息信息。包含收发双方，内容，时间，类型
hub：所有连接。包含连接映射和三种消息。其中连接信息为connection，广播消息为tmessage。对象为h

1. read读协程
`c.ws.ReadMessage()`读取消息并匹配，并填充`tmessage`字段输入到`h.broadcast`
1. hub协程
当hub监听到管道消息时，如`hub.broadcast`，则映射到`hub.connections`中，消息输入到`c.send`中
1. write写协程
当connection监听到`c.send`管道消息时，则`c.write(websocket.TextMessage, message)`

## Ws特点
1. WebSocket是一种在单个TCP连接上进行全双工通信的协议
1. WebSocket使得客户端和服务器之间的数据交换变得更加简单，允许服务端主动向客户端推送数据
1. WebSocket API中，浏览器和服务器只需要完成一次握手，两者之间就直接可以创建持久性的连接，并进行双向数据传输

## Client
### 主函数
`client.go`
- 全局
```go
import (
	"flag"
	"github.com/gorilla/websocket"
	"log"
	"net/url"
	"os"
	"os/signal"
	"time"
)
```
1. 延迟执行关闭前逻辑
1. HTTP服务器的地址
1. 监听操作系统的中断信号
1. 拼接并建立服务器连接
1. 新建协程，读取显示服务器消息（延迟执行关闭前逻辑）
1. 尝试内测用户登录
1. 处理中断通道

## Server

### 主函数
`main.go`
- 全局
```go
import (
	"flag"
	"log"
	"net/http"
	"text/template"
)
```
1. 服务器地址
1. 前端页面

- 处理HTTP请求的入口
```go
func serveHome(w http.ResponseWriter, r *http.Request)
```
1. 匹配检查：类型为get，路径为/
1. 渲染页面

- 主函数
```go
func main()
```
1. 开启协程，收发消息
1. 给http服务器添加路由和处理函数
1. 监听服务器并显示错误

### 连接中心
`hub.go`
- 全局
```go

```
1. 结构体-消息：内容，收，发，类型，时间
1. 结构体-中心：连接映射，注册，非注册，广播
1. 实例-结构体-中心：管道

- 收发消息中心
```go
func (h *hub) run()
```
1. 注册：建立映射
1. 非注册：删除映射
1. 广播：循环处理管道
1. 广播-根据消息类型确定发送内容
1. 广播-根据群发标识确定单发群发，单发须先循环到指定用户

### 连接
`conn.go`
- 全局
```go
import (
	"github.com/gorilla/websocket"
	"log"
	"net/http"
	"regexp"
	"strings"
	"time"
)
```
1. 常量：写入等待，pong等待，ping周期，最大字节，验证密码
1. 变量：服务器配置信息
1. 结构体-连接：ws连接，S-C出站消息缓存，客户端验证布尔，用户名

- 处理客户端对websocket请求的入口
```go
func serveWs(w http.ResponseWriter, r *http.Request)
```
1. 将HTTP请求升级为WebSocket连接
1. 初始化connection并加入注册通道
1. 开启服务器的读写协程

- 写协程
```go
func (c *connection) writePump()
func (c *connection) write(mt int, payload []byte) error//设置连接对象的写入超时时间
```
1. 定时发送心跳包，延迟执行关闭前逻辑
1. 使用for循环和select语句监听通道2
    1. 匹配客户端发送内容：错误，关闭连接；正确，ws写文本消息
    1. 匹配心跳包：ws写ping消息

- 读协程
```go
func (c *connection) readPump()
```
1. 设置pong处理器，延迟执行关闭前逻辑
1. 根据消息类型，匹配消息内容
1. 发送给指定接收方

### 网页
`home.html`
- head-js
1. 输出，`function appendLog(msg) {`
2. 输入，`$("#form").submit(function () {`
3. 浏览器ws检查

- body

1. log
```html
<div id="log"></div>
```
2. form
```html
<form id="form">
	<input type="text" id="msg" size="64"/>
    <input type="submit" value="Send"/>
</form>
```

## 语法
### html
```go
$(function() { ... }); 
```
是jQuery的文档就绪函数，确保代码在HTML文档完全加载和解析后执行。

```go
var msg = $("#msg");
```
这是jQuery的选择器语法。$符号是jQuery的简写，用于调用jQuery函数。#msg表示选择id为msg的HTML元素。

```go
$("#form").submit(function() { ... });
```
这行代码为ID为form的表单元素绑定了一个提交事件的处理函数。当表单被提交时，这个函数将被调用。

```go
conn.onmessage = function (evt) {
	appendLog($("<div/>").text(evt.data))
}
```
当WebSocket接收到消息时，会触发onmessage事件，将onmessage绑定函数
函数内容：使用jQuery库来创建一个新的<div>元素，并将其文本内容设置为evt.data的值
