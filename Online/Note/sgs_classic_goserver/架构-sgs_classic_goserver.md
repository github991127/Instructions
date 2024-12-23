
<!-- TOC -->

- [开始](#开始)
    - [编译时间](#编译时间)
    - [运行](#运行)
- [start RksD.exe](#start-rksdexe)
    - [分支](#分支)
    - [位置](#位置)
    - [配置](#配置)
    - [QA](#qa)
- [战报数据](#战报数据)
- [邀请数据](#邀请数据)
- [服务](#服务)
    - [网络接入](#网络接入)
        - [main](#main)
        - [NewApplication](#newapplication)
        - [Init](#init)
    - [服务注册](#服务注册)
    - [服务发现](#服务发现)
    - [服务内容](#服务内容)
- [协议](#协议)
    - [协议定义与使用](#协议定义与使用)
    - [协议定义与使用-pb](#协议定义与使用-pb)
    - [协议实现](#协议实现)
- [配置](#配置-1)
    - [加载管理](#加载管理)
    - [使用实现](#使用实现)
- [activity](#activity)
    - [翻翻乐](#翻翻乐)
        - [处理客户端请求-框架](#处理客户端请求-框架)
        - [处理客户端请求-实现逻辑](#处理客户端请求-实现逻辑)
- [rogue](#rogue)
    - [初始化](#初始化)
    - [处理客户端请求-框架](#处理客户端请求-框架-1)
    - [处理客户端请求-逻辑](#处理客户端请求-逻辑)
- [ants](#ants)
    - [原理](#原理)
    - [实现](#实现)
    - [应用](#应用)
        - [main](#main-1)

<!-- /TOC -->

<div STYLE="page-break-after: always;"></div>

# 开始
https://code.dobest.com/sgsclassic/sgs_classic_goserver

以太网适配器 以太网:

   连接特定的 DNS 后缀 . . . . . . . : dobest.corp
   本地链接 IPv6 地址. . . . . . . . : fe80::16ac:8e4f:d407:fdb9%7
   IPv4 地址 . . . . . . . . . . . . : 10.225.23.85
   子网掩码  . . . . . . . . . . . . : 255.255.252.0
   默认网关. . . . . . . . . . . . . : 10.225.20.1
## 编译时间
- game_server_v3
GS：241218
OTHERS：241218

- EncodingAndTesting\game_server_v3
GS：241217
OTHERS：

## 运行
1. 依赖
- 更改镜像
go env -w GOPROXY=https://mirrors.aliyun.com/goproxy/,direct

- `proto_pub\gen_go_sample.bat`
修改bat中的goserver路径，然后运行

2. 修改bat
- `.\vc9bin\d.bat`
删除
#start RksD.exe
修改
vc9bin路径

- `.\vc9bin\关闭服务器.bat`
新增    
taskkill /im AISvrD.exe /F /T
taskkill /im uploadrecord.exe /F /T

3. 修改服务器id
`.\vc9bin\Server\cs_config.ini`
设置localid=113

4. 生成

5. 登录启动
`.\Gateway\gate_msg_client.cpp`
调试附加进程GatewayD

6. 关闭服务器
`.\vc9bin\关闭服务器`

## 分支
- 建立
Branch
game_server_v3_branches/game_server_v3_wjb

- 查看
Repo-browser
https://172.28.23.3/sgs/sgx/game_server_v3_branches/game_server_v3_wjb

- 拉取
SVN checkout
https://172.28.23.3/sgs/sgx/game_server_v3_branches/game_server_v3_wjb

- 合并

## 位置
- 日志
`.\vc9bin\Server\Logiclogdir\*.log`
- 配置
`\vc9bin\Server\xmlconfig\cha_spellextend.xml`
技能数值配置
- 个人服务器
ID113
账号：changpu
密码：null
- GM命令
`.\DBS\User\dbs_dowith_gmcmd.cpp`

## 配置
- 编码：GB2312
- 分隔：LF
- 缩进：制表符

## QA

- 数据库连接
`dbs_config.ini`
#战报数据
[gameinforedis]
ip = 127.0.0.1
port = 6379
clientcnt = 1

#邀请数据
[invitecoderedis]
ip=127.0.0.1
port=6379
clientcnt=1

# 服务
例：`rogue`
## 网络接入
### main
`cmd/rogue/main.go`
- 配置文件路径设置： `app_conf_path` 下的 `cs_config.ini`。
- 创建一个新的应用程序实例。设置应用程序配置路径，游戏数据路径，AI数据路径。
- 运行应用程序。
```go
func main() {
	app := ccframe.NewApplication(*csConfig, "Svr-"+*appName)
	app.CsConf.AppConfPath = *app_conf_path
	app.CsConf.GameDataPath = *game_data_path
	app.CsConf.AiDataPath = *ai_data_path

	err := rogue.Init(app)

	app.Run()
}
```
### NewApplication
`app.go`
```go
func NewApplication(csConfigFile string, name string) *Application
```
1. 解析配置文件
使用 `metanet.ParseAppConfig` 函数解析配置文件，获取应用配置。如果解析失败，则打印错误信息并退出程序。

2. 初始化 Application 对象
初始化 `Application` 对象的各个字段，包括名称、ID、服务器类型、配置信息等。

3. 初始化日志，记录应用启动信息
如果未禁用全局日志初始化，则配置并初始化日志系统。
使用日志记录器记录应用启动信息。

4. 初始化网络服务
```go
a.svrEventHandlers = make(map[uint32]SvrEventHandler)
a.worker = ioservice.NewIOService(fmt.Sprintf("app_%s_main", name), 102400)
a.worker.Init()
```
初始化服务事件处理器和网络 I/O 服务。

5. 初始化请求客户端
```go
a.reqc = protoreq.NewClient(a.Post)
a.reqc.OnNotFind = func(seqid int64, resp interface{}, err error) {
logger := logrus.WithField("seqid", seqid)
if err != nil {
logger.WithError(err).Warn("Resp not find ", a.id)
} else {
logger.WithField("respType", reflect.TypeOf(resp)).Warn("Resp not find ", a.id)
}
}
a.respMsgMap = make(map[uint32]bool)
```
初始化请求客户端，并设置请求未找到时的回调函数。

6. 初始化网络
```go
a.net = metanet.NewMetaNet()
a.net.Init(appConfig, a.worker)
```
初始化网络对象，并设置网络事件监听器，包括连接、关闭、工作线程退出和写错误等事件。
```go
type MetaNet struct {
	// 配置
	Config *AppConfig
	// 事件
	io ioservice.IOService
	// 消息管理
	connectHandler OnNetConnect
	closeHandler   OnNetClose
	bytesHandler   OnNetBytes
	errorHandle    OnWriteError
	WorkerExitHandler OnWorkerExit
	// 服务和客户端
	NetMutex sync.Mutex
	Clients  map[string]*MetaClient
	Server   *MetaServer
	ID2ConnItem   map[uint32]*ConnItem
	Conn2ConnItem map[Connection]*ConnItem
	*MsgHandlers
}
```

7. 启动本地服务器
```go
svrConfig := &metanet.ServerConfig{
ServerID:      appConfig.ServerID,
ServerType:    appConfig.ServerType,
Name:          fmt.Sprintf("%s", a.name),
ListenAddr:    ":" + strconv.Itoa(int(appConfig.ListenPort)),
DisableCrypto: false,
}
if a.net.Listen(svrConfig, a.StartTime) == nil {
return nil
}
```

8. 连接到中心服务器
```go
csConf := &metanet.ClientConfig{
ServerID:    appConfig.CsConf.ServerID,
ServerType:  uint32(CenterServer),
Name:        strconv.Itoa(int(appConfig.CsConf.ServerID)),
ConnectAddr: appConfig.CsConf.Net_listenip + ":" + strconv.Itoa(int(appConfig.CsConf.Net_listenport)),
}
a.net.Connect(csConf, a.StartTime)
```

### Init
`rogue.go`
1. **数据库连接**：
   - 使用`gameutil.OpenDBWithMaxMinConn`函数打开数据库连接，设置最大和最小连接数。

2. **缓存配置**：
   - 创建一个`map[string]*config.RedisConfig`类型的变量`cfg`，用于存储不同类型的Redis配置。
   - 为每种类型的Redis配置（如`user`、`backup`、`invite`等）设置最大连接数、地址、密码和最大空闲连接数。
   - 初始化`cacheMgr`并传入配置信息。

3. **初始化管理**：
   - 初始化任务管理器`antsMgr`，用于管理任务。
   - 创建配置管理`confMgr`，用于加载和管理游戏配置。
   - 加载多个配置文件，如`character.xml`。
   - 初始化用户管理`userMgr`，用于管理用户。
   - 初始化排行榜管理`rogueRankData`，用于存储排行榜数据。

4. **消息监听**：
   - 监听各种消息和服务器事件，如`ccproto.SS_SS_GM_CMD_NTF`、`metanet.CS_Notice_NewServerStartUp`等。
   - 根据不同的事件类型执行相应的操作，如连接到新的服务器、处理数据库断开连接等。

5. **定时任务**：
    - 创建`cronMgr`，用于管理定时任务。
    - 添加每隔10秒和每分钟执行的任务，启动定时任务。

6. **运行应用**：
    - 调用`Run`函数，开始运行应用。

## 服务注册
1. **初始化 `Application` 对象**
详见[NewApplication](#newapplication)

2. **启动一个服务器**
```go
metanet.go
func (net *MetaNet) Listen(config *ServerConfig, startTime int64) *MetaServer
```
- 线程安全：加锁
- 空检查：io字段
- 重复检查：`MetaNet`实例
- 配置文件：`bin/cs_config-sample.ini`

3. **创建一个通用的服务器实例**
    1. 创建MetaServer实例
        ```go
        s := new(MetaServer)
        ```
        创建一个新的`MetaServer`实例。

    2. 初始化MetaServer的字段
    3. 定义连接和关闭处理函数
        - `fconnect`: 当连接建立时，启动一个定时器，如果超时则关闭连接。
        - `fclose`: 当连接关闭时，停止定时器并从`heartCheckTimers`中删除该连接。
        
    4. 创建打包器和处理器MetaPackager和MetaProcessor
    5. 设置MetaProcessor的处理函数
    6. 创建TCPServer
```go
meta_server.go
func NewCommonServer(appConfig *AppConfig, serverConfig *ServerConfig, io ioservice.IOService,
	connectHandler ServerConnectHandler, closeHandler CloseHandler,
	bytesHandler BytesHandler, errorHandle WriteErrorHandler, msgHandlers GetMsgHandler, exitHandler WorkerExitHandler, appStartTime int64,
	msgServerReqHeartBeatHandler MsgHandler) *MetaServer 
```
## 服务发现
`app.go`
- `GetServer` 和 `GetAvailableServer`：获取服务器实例。
- `GetAvailableServerIDs`：获取所有可用的服务节点ID。
- `GetServerType` 和 `IsServerAvailable`：获取服务器类型和检查服务器是否可用。

- `OnExitHandler` 和 `OnFiniHandler`：设置应用程序退出时的回调函数。
- `Post` 和 `TryPost`：将任务派发到服务中执行。
- `AfterPost`：在指定时间后，将任务派发到服务中执行。
- `ListenServerEvent`：监听服务器事件。
- `ListenMsg` 和 `ListenRequest`：监听消息和请求。
- `RegisterResponse`：注册响应消息。

## 服务内容

|ServerType|Description|
|-------------------------|------------------------------|
|ErrorServer|错误服务器|
|LoginServer|登录服务器|
|GameServer|游戏服务器|
|MySQLServer|MySQL数据库服务器|
|ShopServer|商店服务器|
|Gateway|网关服务器|
|FriendServer|好友服务器|
|CenterServer|中心服务器|
|MonitorServer|监控服务器|
|SupportServer|支持服务器|
|ImPostServer|IM中转服务|
|Client|客户端|
|WebService|web服务|
|HaoFangSvr|好友房服务器|
|OpcTool|工具服务器|
|BfSgsBsipSvr|sgs服务器|
|PhoneShopSvr|手机商店服务器|
|FcmServer|FirebaseCloudMessaging|
|RankServer|排名服务器|
|GuildServer|公会服务器|
|LogServer|日志服务器|
|StatsServer|数据统计服务器|
|AIServer|AI服务器|
|ViewServer|旁观服务器|
|SBMServer|比赛服务器|
|CustomLobbyServer|自定义游戏大厅服务器|
|CustomGameServer|自定义游戏服务器|
|WebFcmServer|web防沉迷服务器|
|AdminServer|管理服务器|
|CApiServer|CAPI服务器|
|ActServer|活动服务器|
|OrderServer|订单服务器|
|EntityServer|实体服务器|
|TableServer|表格服务器|
|RecommendServer|推荐服务器|
|AIAgentServer|AI代理服务器|
|ActRogueServer|活动rogue服务器|
|MatchServer|匹配服务器|
|PlanServer|计划服务器|
|HttpServer|HTTP服务器|
|CompServer|竞赛服务器|
|KVServer|键值存储服务器|
|PayServer|充值回调服务|
|ServiceMax|服务最大值（枚举结束值）|

# 协议
例：`treasure`
## 协议定义与使用
1. **定义协议结构体**
```go
type PacketBase struct {
	OPCode  uint32 //操作类别
	Len     uint32 //整个消息包长度
	User_id uint32
}
type ClientTreasureNTF struct {
	metanet.PacketBase
	Id             uint32
	Cost1          uint32                       //单抽消耗
	Cost5          uint32                       //5抽消耗
	Gift_geted     uint8                        //是否领取免费道具 0:无 1:已领取
	Time_next_free uint32                       //0值是没有免费开 非0值时大于该时间可免费
	Open5_get      uint32                       //开了5个的奖励分
	Op             uint8                        //对应操作类型
	Result         uint32                       //对应操作结果
	Items          [5]TClientTreasureNTFOneItem //5张牌数据
}
```
2. **添加操作码枚举**
```go
const (
	Treasure_op_req     = 0
	Treasure_op_reset   = 1
	Treasure_op_getfree = 2
)
```
3. **处理函数中填充协议字段，发送消息**：
```go
app.ListenMsg(ccproto.CLIENT_TREASURE_INFO_RESET, func(sender ccframe.Server, seqId int64, _msg metanet.Message) {
    resp := &ccproto.ClientTreasureNTF{}
    resp.User_id = msg.User_id
    resp.Op = ccproto.Treasure_op_reset
    defer sender.SendMsg(resp)
}
```
## 协议定义与使用-pb

1. **定义协议结构体**
```go
type PacketBaseV2 struct {
	PacketBase
	MsgType uint8
	SeqId   int64
}
type PbMessage struct {
	PacketBaseV2
	Pb proto.Message
}
```

2. **处理函数中填充协议字段，对服务进行请求调用**：
```go
treasure_logic.go
warp := &metanet.PbMessage{
    Pb: req,
}
warp.User_id = u.Uid
r, rerr := app.GetServer(u.SvrDbs, service.MySqlServer).RequestCall(warp, time.Second*15)
```

## 协议实现

1. **添加操作码枚举**

```go
`msgid.go`
const (
	CLIENT_SGS_PROTOCOL_BEGIN = 20000 + iota //协议分段
	SELF_CS_ReportNetType                //服务器自己到中心服务器注册自己的服务类型
	CS_Notice_NewServerStartUp           //cs通知别的服务器有新的服务器启动
	SERVER_TOSERVER_ALIVE_SELFSTRESS_RPT //服务器向中心服务器报告自己的参数
	DBS_TOCS_USERS_OFFLINE_RPT           //DBS向中心服务器报告物理下线的用户
	SS_CS_SELF_FAULT_RPT                 //向cs报告我的故障，我该如何处理
	CS_SS_FAULT_DEALWITH_NTF             //cs通知我如何处理故障
	SS_SS_MYUSING_SERVERID_NTF           //我使用的服务器
```
2. **注册**
```go
register_game.go
metanet.RegisterMessage(SS_SS_MYUSING_SERVERID_NTF, (*SSSS_MeUsingServeridNtf)(nil))
```
3. **监听**
```go
gs_msg_dbserver.go
app.ListenMsg(ccproto.SS_SS_MYUSING_SERVERID_NTF, func(sender ccframe.Server, seqId int64, msg metanet.Message)
```

# 配置

## 加载管理
`conf_manager.go`
加载和重载配置文件，提供线程安全的访问和修改配置的方法。通过回调函数通知配置加载完成，并使用读写锁来保护配置的并发访问
- ConfManager
    - `app`: 指向ccframe框架的Application实例。
    - `Paths`: 配置文件路径。
    - `Develop`: 是否处于开发模式。
    - `loadCallback`: 配置加载完成后的回调函数。
    - `confLock`: 读写锁，用于保护配置的并发访问。
    - `gameConf`: 当前加载的游戏配置。

- 加载配置
调用`gameconfig.go`的`Load`方法加载配置文件
```go
func (p *GameConfig) Load() error {
t := reflect.ValueOf(p).Elem()
for i := 0; i < t.NumField(); i++ {
    fieldinfo := t.Type().Field(i)
    filename := fieldinfo.Tag.Get("name")
    if p.IsSet(filename) {
        tmp := t.Field(i).Interface()
        cfg, ok := tmp.(ConfigBase)
        if ok {
            result := cfg.Load(path.Join(p.CfgPath, "xmlconfig/", filename))
            if result != nil {
                return fmt.Errorf("load %s fail:%v", filename, result)
            }
        }
    }
}
return nil
}
```

- 重载配置
```go
func (p *ConfManager) ReloadXml(vctResult []string, pmsg *ccproto.SSSS_GmCmdNtf, app *ccframe.Application) bool {
	// ...
}
```
根据传入的命令参数重新加载配置文件。如果命令是`config all`，则重新加载所有配置文件；如果是其他命令，则只重新加载指定的配置文件。

- 获取配置
```go
func (p *ConfManager) GetConfig() *conf.GameConfig {
	// ...
}
```
返回当前加载的游戏配置。

## 使用实现
- `Init`初始化加载配置
```go
_, err = confMgr.LoadConf("character.xml", "hd_roguelike.xml", "cha_spell.xml", "sys_ss_gamelabel_config.xml", "ff_dbs_pay_gift.xml", "sys_ss_gamelabel_config.xml", "sys_h5_task_main.xml",
    "hd_all_active.xml", "h5_global_conf.xml", "h5_bar_control.xml")
if err != nil {
    logrus.WithError(err).Error("LoadConf")
    return err
}
```
- 命令行
根据输入使用`path.Join`更改路径
```go
flag.Parse()
if *csConfig == "cs_config.ini" && len(*app_conf_path) != 0 {
    *csConfig = path.Join(*app_conf_path, *csConfig)
}
```

# activity
## 翻翻乐
### 处理客户端请求-框架

`activity/treasure_handle.go`

	CLIENT_TREASURE_INFO_REQ   = 26401 //请求信息
	CLIENT_TREASURE_INFO_RESET = 26402 //重置
	CLIENT_TREASURE_OPEN_REQ   = 26403 //开卡
	CLIENT_TREASURE_GIFT_REQ   = 26404 //商店领取免费道具

注册4个消息监听器，当接收到指定类型的消息时，触发处理函数，参数包括消息发送者sender、消息序列号seqId和消息内容_msg

- 请求信息
1. **消息监听**：
   ```go
   app.ListenMsg(ccproto.CLIENT_TREASURE_INFO_REQ, func(sender ccframe.Server, seqId int64, _msg metanet.Message) {
   ```
   注册了消息监听器，当接收到`ccproto.CLIENT_TREASURE_INFO_REQ`类型的消息时，会触发这个回调函数。

2. **获取用户会话**：
   ```go
   u, ok := userMgr.GetSession(_msg.GetUID())
   if !ok {
       logrus.Debugf("user[%d] not online", _msg.GetUID())
       return
   }
   ```
   通过消息中的用户ID获取用户的会话信息，如果用户不在线，返回。

3. **异步任务处理**：
   ```go
   antsMgr.PostGetUser(u.Acc, func(t int64, userinfo *dbs.WxUserBrief) {
   ```
   使用`antsMgr`异步获取用户信息，`PostGetUser`方法会异步执行

4. **消息类型检查**：
   ```go
   msg, ok := _msg.(*ccproto.ClientTreasureReq)
   if !ok {
       return
   }
   ```
   断言检查：消息类型是否为`ccproto.ClientTreasureReq`

5. **响应消息初始化**：
   ```go
   resp := &ccproto.ClientTreasureNTF{}
   resp.User_id = msg.User_id
   defer sender.SendMsg(resp)
   ```
   初始化响应消息，并设置用户ID，`defer`发送响应消息。

6. **时间检查**：
   ```go
   now := time.Now().Unix()
   if t < now && (now-t) > manager.AntsTaskWaitTimeout {
       resp.Result = ccproto.Treasure_result_err_busy
       return
   }
   ```
   界限检查：当前时间与任务时间的关系，是否超时。

7. **活动信息检查**：
   ```go
   cur := treasureMgr.CheckCurAct()
   if cur == nil {
       resp.Result = 0 //ccproto.Treasure_result_err_id
       return
   }
   ```
   功能检查：当前活动是否存在。

8. **用户活动锁定**：
   ```go
   lock, err := cacheMgr.TryLockUserAct(treasureMgr.actType, cur.VId, msg.User_id, 1)
   if err != nil {
       resp.Result = ccproto.Treasure_result_err_userbusy
       return
   }
   defer cacheMgr.Unlock(lock)
   ```
   锁定用户活动，防止并发操作，如果锁定失败则设置错误结果并返回。`defer`解锁。

9. **加载活动数据**：
   ```go
   actData, err := treasureMgr.Load(msg.User_id, cur.VId)
   if err != nil {
       logrus.WithError(err).Errorf("Treasure Load %d %d Error", msg.User_id, cur.VId)
       resp.Result = ccproto.Treasure_result_err_unknown
       return
   }
   ```
   加载当前用户的活动数据。

10. **活动数据初始化**：
    ```go
    if actData == nil || actData.Version != TreasureDataVer {
        actData = &db.DBActTreasureData{ActId: cur.VId, Uid: msg.User_id, Version: TreasureDataVer}
        resp.Result = treasureMgr.ResetRound(u, cur, actData)
        if 0 != resp.Result {
            return
        }
    }
    ```
    匹配检查：如果活动数据为空或版本不匹配，则初始化活动数据并重置活动轮次。

11. **填充响应数据**：
    ```go
    treasureMgr.FillNtf(u, cur, actData, resp, userinfo)
    return
    ```
    填充响应数据并返回。

- 重置
基本同上，消息相关改为重置卡片

- 开卡
基本同上，消息相关改为打开卡片
根据请求消息中的Cnt和Cost字段设置响应消息的操作类型，即：翻开1或4张，免费或用券翻开

- 礼物
基本同上，消息相关改为打开礼物

### 处理客户端请求-实现逻辑
`activity/treasure_logic.go`
- const
	- `TreasureDataVer`：版本号，值为`db.DBActTreasureData_Ver2022070201`。
	- `TreasureDataKeepTime`：数据的保留时间，40天（`time.Second * 86400 * 40`）。
- struct
	- `actType`：活动类型，值为`conf.ActTypeTreasure`。
	- `Cur`：指向`conf.VTreasureSection`的切片，存储当前数据。
	- `lastCheck`：最后一次检查的时间戳。
	- `lock`：互斥锁，保护数据的一致性。
```go
type TreasureManager struct {
	actType uint32
	Cur     []*conf.VTreasureSection
	lastCheck int64
	lock      sync.Mutex
}
```
- 获取活动配置
`func (p *TreasureManager) CheckCurAct() *conf.VTreasureSection`
	- 匹配检查：时间戳
	- 从配置管理器`confMgr`中获取当前活动配置

- 反序列化活动消息
`func (p *TreasureManager) Load(uid uint32, actId uint32) (*db.DBActTreasureData, error)`
	- 从缓存管理器`cacheMgr`中获取指定用户`uid`和活动`actId`的活动数据。
	- 使用`proto.Unmarshal`将内容反序列化为`DBActTreasureData`结构体。

```go
content, err := cacheMgr.GetActData(p.actType, actId, uid)
data := &db.DBActTreasureData{}
err = proto.Unmarshal(content, data)
if err != nil {
	logrus.WithError(err).Errorf("ActData %d %d %d UnmarshalFail:%v", p.actType, actId, uid, data)
	return nil, err
}
return data, nil
}
```

- 序列化活动消息
`func (p *TreasureManager) Save(data *db.DBActTreasureData)`
	- 使用`proto.Marshal`将`DBActTreasureData`结构体序列化为字节流。
	- 使用`cacheMgr.SetActData`将序列化后的数据保存到缓存中，并设置保存时间`TreasureDataKeepTime`。
```go
content, err := proto.Marshal(data)
err = cacheMgr.SetActData(p.actType, data.ActId, data.Uid, content, TreasureDataKeepTime)
}
```

- 重置卡片
`func (p *TreasureManager) ResetRound(u *manager.UserState, c *conf.VTreasureSection, data *db.DBActTreasureData) (err uint32)`
	- 延迟保存数据
	- 界限检查：活动时间，已翻开卡片数
	- 根据首轮和次轮，生成卡片
	- 增加轮次、重置打开次数、重置奖励
	- 生成规定数量的三种卡，打乱
	- 将重置操作和卡片信息记录到`infos`中，以便日志记录。

```go
infos = append(infos, "reset")
for _, c := range data.Cards {
    infos = append(infos, fmt.Sprintf("%d", c.Card))
}
```

- 打开卡片
`func (p *TreasureManager) OpenCard(u *manager.UserState, c *conf.VTreasureSection, data *db.DBActTreasureData, msg *ccproto.ClientTreasureOpenReq, userinfo *dbs.WxUserBrief) (err uint32)`
	- 延迟保存数据
	- 界限检查：活动时间，已翻开卡片数
    - 索引开启单个或循环开启全部
    - 修改：记录消耗和获得道具信息
    - RequestCall发送请求，处理响应结果
    
```go
r, rerr := app.GetServer(u.SvrDbs, service.MySqlServer).RequestCall(warp, time.Second*15)
resp, ok := r.GetPb().(*dbs.DbsGoodsOptRep)
```

- 打开礼物
`func (p *TreasureManager) GetGift(u *manager.UserState, c *conf.VTreasureSection, data *db.DBActTreasureData, msg *ccproto.ClientTreasureGiftReq) (err uint32)`
基本同上

- 填充响应数据
`func (p *TreasureManager) FillNtf(u *manager.UserState, c *conf.VTreasureSection, data *db.DBActTreasureData, msg *ccproto.ClientTreasureNTF, userinfo *dbs.WxUserBrief)`
    - 调用 `CanDoTreasureFree` 方法判断用户是否可以进行免费宝藏活动，如果可以，根据 `data.TimeLastFree` 和 `c.VFreeDuration` 计算下一次免费时间
    - 遍历 `data.Cards`，将前5个非零的卡片信息填充到 `msg.Items` 

# rogue
## 初始化
`rogue.go`
详见[Init](#init)

## 处理客户端请求-框架
`roguelike_handle.go`

1. **注册响应**：使用 `app.RegisterResponse` 方法注册一系列消息响应

2. **监听消息**：使用 `app.ListenMsg` 方法监听各种客户端请求的消息类型，并为每种消息类型定义了一个处理函数

3. **处理函数**：
   - 从会话管理器 `userMgr` 中获取用户信息
   - 使用 `antsMgr.PostGetUser` 方法异步获取用户详细信息
   - 将消息转换为具体的协议缓冲区（protobuf）消息类型
   - 调用 `rogueLikeMgr` 中的相应方法处理具体游戏逻辑
```go
app.ListenMsg(ccproto.CLIENT_ACT_RAND_GENERAL_POOL, func(sender ccframe.Server, seqId int64, _msg metanet.Message) {
    user, ok := userMgr.GetSession(_msg.GetUID())
    if !ok {
        logrus.Debugf("user[%d] not online", _msg.GetUID())
        return
    }
    antsMgr.PostGetUser(user.Acc, func(t int64, userinfo *dbs.WxUserBrief) {
        msg, ok := _msg.GetPb().(*roguelike.ClientRogueLikeGeneralPoolRandomReq)
        if !ok {
            return
        }
        rogueLikeMgr.RogueLikeGeneralPoolRandom(user, msg, _msg.GetOPCode(), userinfo)
    })
})
```

## 处理客户端请求-逻辑
`roguelike_logic.go`
- struct
```go
type RogueLikeManager struct {
	actType uint32

	lastCheck int64
	lock      sync.Mutex
}
```
    - `actType`存储活动的类型
    - `lastCheck`记录最后一次检查活动的时间戳
    - `lock`互斥锁

- 检查赛季ID
`func (p *RogueLikeManager) CheckCurAct(seasonId uint32) uint32`

    - 从`confMgr`获取游戏配置
    - 从`gameConf.RogueConf`中获取与`seasonId`对应的轮次ID信息

- 从DBS将池发将
1. **空检查**：通过`CheckCurAct`函数检查当前赛季信息，从配置管理器`confMgr`获取赛季和回合的配置信息
2. **用户活动数据锁**：尝试获取用户活动数据的锁，以防止并发写入
3. **请求玩家活动数据**：向数据库服务请求玩家的活动数据，包括赛季数据、武将池数据、游戏数据和历史数据，然后处理响应
4. **匹配检查**：

    - 检查玩家的体力值
    - 检查请求的赛季ID是否与玩家当前赛季ID一致
    - 检查玩家是否已经有武将
    - 检查武将池数据是否需要刷新，如果不需要，则直接返回旧数据

5. **获取武将池数据**：向数据库服务请求新的武将池数据，如果请求失败，则设置错误码并返回
```go
reqPool := &roguelike.RogueLikeGeneralPoolRandomReq{SeasonId: msg.SeasonId, ChangeGenId: cfgSeasonNode.IsChange}
wrapPool := &metanet.PbMessage{
    Pb: reqPool,
}
wrapPool.User_id = u.Uid
rPool, dbPoolErr := app.GetServer(u.SvrDbs, service.MySqlServer).RequestCall(wrapPool, time.Second*15)
```
6. **数据修改和写回**：修改玩家数据，并将新的武将池数据写回数据库
7. **通知客户端**：向客户端发送新的武将池数据

- 数据接口
`roguelike_data.go`
肉鸽数据通用操作接口，方便复用

- 用户消息处理
`user_handles.go`
初始化用户消息处理器
注册消息监听器，当接收到特定类型的消息时，会调用相应的回调函数

# ants
## 原理
Golang 的协程池，两个goroutine
- goworker 的 run 的 goruotine，for 循环中不断获取任务执行
- 业务 submit goroutine，不断投递任务。submit 投递的时候，一旦达到了容量，就使用 wait 阻塞住，或者返回已经过载的错误

![alt text](ants.svg)

1. 使用 sync.Pool 初始化 goworker
    Q：goWorker 的初始化、回收是一个非常频繁的动作，这种动作消耗非常大
    A：使用对象池 sync.Pool 来优化初始化。这样这种大量的获取回收 worker 的行为可以直接从 pool 中获取，降低内存的消耗回收


2. goWorker 的存取同时支持队列和栈方式
    - 预先分配：如果设置了 PreAlloc，则使用循环队列（loopQueue）的方式存取这个 workers。在初始化 pool 的时候，就初始化了 capacity 长度的循环队列，取的时候从队头取，插入的时候往队列尾部插入，整体 queue 保持在 capacity 长度。
    - 用时分配：如果没有设置 PreAlloc，则使用堆栈（stack）的方式存取这个 workers。初始化的时候不初始化任何的 worker，在第一次取的时候在 stack 中取不到，则会从 sync.Pool 中初始化并取到对象，然后使用完成后插入到 当前这个栈中。下次取就直接从 stack 中再获取了。

3. 自定义自旋锁
    遵循指数退避（Exponential Backoff）策略，取锁频率递减

4. 时间戳使用 ticker 来更新
    避免频繁使用 time.Now()对底层负荷过大
5. 无序执行
    ants 并不保证提交的任务被执行的顺序，执行的顺序也不是和提交的顺序保持一致，因为在 ants 是并发地处理所有提交的任务，提交的任务会被分派到正在并发运行的 workers 上去，因此那些任务将会被并发且无序地被执行




## 实现
实现了一个基于ants库的任务管理器，可以管理和调度异步任务。支持不带用户信息和带用户信息的任务处理，并提供任务的提交和停止功能
```go
func (p *AntsManager) Post(f AntsTaskHandle) {
	p.wg.Add(1)
	p.a.Invoke(&AntsTaskInfo{F: f, T: time.Now().Unix()})
}

func (p *AntsManager) PostGetUser(acc string, f AntsTaskHandleWithUser) {
	p.wg.Add(1)
	p.a.Invoke(&AntsTaskInfo{FU: f, T: time.Now().Unix(), A: acc})
}
```

## 应用
### main
任务处理，进入协程池。由于ants无序执行，因此要数据加锁
```go
antsMgr.PostGetUser(u.Acc, func(t int64, userinfo *dbs.WxUserBrief) {
    //...
    //获得当前活动数据
    actData, err := treasureMgr.Load(msg.User_id, cur.VId)
    if err != nil {
        logrus.WithError(err).Errorf("Treasure Load %d %d Error", msg.User_id, cur.VId)
        resp.Result = ccproto.Treasure_result_err_unknown
        return
    }
    if actData == nil || actData.Version != TreasureDataVer {
        actData = &db.DBActTreasureData{ActId: cur.VId, Uid: msg.User_id, Version: TreasureDataVer}
        resp.Result = treasureMgr.ResetRound(u, cur, actData)
        if 0 != resp.Result {
            return
        }
    }
    treasureMgr.FillNtf(u, cur, actData, resp, userinfo)
    return
})
```



