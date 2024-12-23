
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
- [开发流程](#开发流程)
    - [协议序列化](#协议序列化)
    - [实现](#实现)
        - [序列化](#序列化)
        - [协议定义与使用](#协议定义与使用)
        - [协议定义与使用-pb](#协议定义与使用-pb)
        - [协议类型处理](#协议类型处理)
    - [配置-xml](#配置-xml)
        - [加载管理](#加载管理)
        - [使用实现](#使用实现)
    - [配置-ini](#配置-ini)
        - [加载管理](#加载管理-1)
        - [使用实现](#使用实现-1)
- [中间件](#中间件)
    - [Redis](#redis)
    - [Kafka](#kafka)
        - [内容](#内容)
        - [实现](#实现-1)
    - [MySQL](#mysql)

<!-- /TOC -->

<div STYLE="page-break-after: always;"></div>

# 开始
https://172.28.23.3/sgs/sgx/game_server_v3
---3312比赛服

比赛只合3312
热更的内容3311合3312都要合

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
1. 解压Depend到当前文件夹
`.\Depend\Depend.rar`

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

# 开发流程

## 协议序列化

- 协议的作用
1. **数据断句**：RPC 需要将对象序列化成二进制数据，协议用于标识请求数据的结束位置，即消息边界。
2. **数据识别**：服务提供方需要从 TCP 通道中识别出哪些二进制数据属于第一个请求。
3. **避免语义不一致**：通过设定边界，避免数据合并或拆分导致的错误。

- 协议的设计
1. **性能要求**：RPC 需要高性能，HTTP 协议由于数据包大小和无状态特性，不适合 RPC。
1. **固定部分与协议头长度**：设计支持扩展的协议，包括固定部分和协议头长度，以支持平滑升级。
2. **协议头与协议体内容**：协议头包含长度和序列化方式，协议体包含请求和扩展属性。
3. **二进制传输**：网络传输需要二进制数据，对象需要转换为二进制。
4. **可逆过程**：序列化是可逆的，服务提供方可以根据请求类型和序列化类型还原对象。

- JSON
1. **文本型序列化框架**：典型的 key-value 方式，没有数据类型。
2. **问题**：额外空间开销大，没有类型信息，需要反射。

- Protobuf
1. **轻便、高效**：Google 的数据标准，支持多种语言。
2. **IDL 定义**：使用 IDL 定义数据结构，生成序列化工具类。

- RPC 框架使用注意事项
1. **对象构造复杂**：避免属性多、嵌套深的对象。
2. **对象过于庞大**：避免大 List 或 Map，影响性能和请求耗时。
3. **序列化框架不支持的类**：选择原生、常用的集合类。
4. **复杂的继承关系**：避免复杂的继承关系，影响性能和序列化问题。


## 实现
### 序列化
```cpp
static bool PackNetMsg(PbWarpMsg* pTempMsg, unsigned int msg_id, const google::protobuf::Message& msg, unsigned int user_id = 0, UINT64 seqid = 0)
{
    if (!pTempMsg)
    {
        return false;
    }
    auto pbLen = msg.ByteSize();
    pTempMsg->OPCode = msg_id;
    pTempMsg->Len = sizeof(PbWarpMsg) + pbLen;
    pTempMsg->user_id = user_id;
    pTempMsg->MsgType = 1;
    pTempMsg->SeqId = seqid;
    if (!msg.SerializePartialToArray((char*)(pTempMsg)+sizeof(PbWarpMsg), pbLen))
    {
        APPLOG_ERROR("Serialize 错误!" << FUN_FILE_LINE);
        return false;
    }
    return true;
}
```
使用Protocol Buffers库来序列化消息.PackNetMsg 函数只负责打包消息.SendNetMsg 函数不仅打包消息，还负责发送:
1. 使用 CSgsAllocXBlockMg::single()->malloc 分配内存，大小为 PbWarpMsg 结构体的大小加上消息的字节大小。
1. 使用 CAutoCallFun 对象确保在函数结束时自动释放内存。
1. 设置 PbWarpMsg 结构体的各个字段，包括操作码 OPCode、长度 Len、用户ID user_id、消息类型 MsgType 和序列号 SeqId。
1. 使用 SerializePartialToArray 方法将 msg 序列化到分配的内存中。
1. 调用 INiceNet::Instance().SendMsg 发送消息。

### 协议定义与使用
例：`DBS_GS_COMPLETE_ACH_NOTICE`

1. **定义协议结构体,实现协议构造函数**
   - 创建一个新的结构体来表示协议数据，继承自`PacketBase`，包含所有必要的字段。
   - 初始化列表中设置操作码（`OPCode`）和结构体大小（`sizeof`）。
   - 在新结构体中实现构造函数，初始化所有成员变量。
```cpp
struct PacketBase
{
	PacketBase(unsigned int opcode, unsigned int len):OPCode(opcode),Len(len),user_id(sc_invalid_user_temp_id){}
	unsigned int OPCode;          //操作类别  
	unsigned int Len;             //整个消息包长度
	unsigned int user_id;         //modify by zht
};
//服务器之间通知,战功完成
struct DbsGsCompleteAchNotice : PacketBase
{
	char szAccount[64];
	unsigned short uAchId;
	unsigned int   uCompleteTime;
	DbsGsCompleteAchNotice() : PacketBase(DBS_GS_COMPLETE_ACH_NOTICE, sizeof(DbsGsCompleteAchNotice))
	{
		memset(szAccount, 0, sizeof(szAccount));
		uAchId = 0;
		uCompleteTime = 0;
	}
};
```

2. **添加操作码枚举**
- 在`OPCode`枚举中添加新的操作码，确保其唯一性。
```cpp
enum OPCode
{
DBS_GS_COMPLETE_ACH_NOTICE, //Dbs通知GS战功完成
}
```

3. **处理函数中添加case分支,实现协议逻辑**：
   - 在消息处理函数（如`hand_msg_dbserver`）中添加`case`分支
   - 在`case`分支中实现具体的业务逻辑，如解析协议数据、调用相关函数等。
```cpp
bool CGsNetMg::hand_msg_dbserver(unsigned int conid,const void *pData,unsigned int data_len)
{
case  DBS_GS_COMPLETE_ACH_NOTICE:
    {
        const DbsGsCompleteAchNotice *pMsg = (const DbsGsCompleteAchNotice *)pMsgHeader;
        CGsUser *pUser = CUserMg::Single()->Find( pMsgHeader->user_id );
        if( pUser != NULL)
        {
            pUser->GetAchievement().NetMsgDbsGsCompleteAchNotice(pMsg);
        }
    }
}
```
### 协议定义与使用-pb
1. **定义协议结构体,实现协议构造函数**
```cpp
struct PacketBaseV2 : public PacketBase
{
	UINT8 MsgType;      //消息类型区分pb还是结构体	
	UINT64 SeqId;       //Request请求使用 唯一ID，用于管理Request请求，定位回调等 
	PacketBaseV2(unsigned int opcode, unsigned int len, UINT8 msgType = em_msg_type_struct) :PacketBase(opcode, len), MsgType(msgType), SeqId(0)
	{
	}
};

struct PbWarpMsg :public PacketBaseV2
{
	PbWarpMsg() :PacketBaseV2(0, sizeof(PbWarpMsg))
	{
	}
};
```
2. **处理函数中填充协议字段，序列化处理并发送**：
```cpp
PbWarpMsg *pTempMsg = (PbWarpMsg*)CSgsAllocXBlockMg::single()->malloc(sizeof(PbWarpMsg) + pbLen);
CAutoCallFun autocb(boost::bind(&CSgsAllocXBlockMg::free, CSgsAllocXBlockMg::single(), (char*)pTempMsg, 43508));

pTempMsg->OPCode = QUERY_USER_ID_RSP;
pTempMsg->user_id = (unsigned int)uid;
pTempMsg->Len = sizeof(PbWarpMsg) + pbLen;
pTempMsg->MsgType = 1;
pTempMsg->SeqId = SeqId;
if (msg.SerializePartialToArray((char*)(pTempMsg)+sizeof(PbWarpMsg), pbLen))
{
    INiceNet::Instance().SendMsg(con_id, pTempMsg, pTempMsg->Len);
}
```
### 协议类型处理
```cpp
void ReplayRecordMgr::WritePacketData(const PacketBase* pMsg)
```
- 协议转换
检查是否有消息适配器：通过 `CNetMsgAdapter::single().HasMsgAdapter(pMsg->OPCode)` 检查当前消息是否有适配器。如果有适配器，尝试将消息适配到 `m_adapterbuf` 缓冲区中。适配成功后，将 `new_msg` 指向适配后的消息。

- 写入操作
根据消息的 `OPCode` 判断消息类型：
    1. **PB消息**：如果 `OPCode` 大于 `PACKET_V2_BEGIN`，表示这是一个 PB 消息。
    - 长度检查：检查消息长度是否在合理范围内。如果不在，记录错误日志并返回。
    - 写入消息：将消息的各个字段写入记录，包括消息 ID、扩展数据、消息版本号、消息类型、序列 ID、消息长度和消息内容。
    2. **结构体消息**：如果 `OPCode` 小于等于 `PACKET_V2_BEGIN`，表示这是一个结构体消息。
    - 写入消息：将消息的各个字段写入记录，包括消息 ID、扩展数据、消息版本号、消息长度和消息内容。

- 安全检查
    1. **转换结果检查**：
   ```cpp
   if (!CNetMsgAdapter::single().AdaptMsg(pMsg, m_adapterbuf, 40960, sc_sgs_curversion, use_buf))
   ```
   - 检查消息转换是否成功，记录错误日志并返回。

    2. **协议版本检查**：
    ```cpp
    if (new_msg->OPCode > PACKET_V2_BEGIN)
    ```
    - 检查操作码是否大于`PACKET_V2_BEGIN`，即是否为PB协议版本的消息。

    3. **消息长度检查**：
    ```cpp
    if (new_msg->Len < sizeof(PacketBaseV2) || new_msg->Len > m_max_protocol_len)
    ```
    - 检查消息长度是否在合法范围内，记录错误日志并返回。

    4. **指针转换检查**：
    ```cpp
    const PacketBaseV2* pMsgV2 = static_cast<const PacketBaseV2*>(new_msg);
    if (!pMsgV2)
    ```
    - 尝试将`new_msg`转换为`PacketBaseV2`类型，并检查转换后的指针是否为空，记录错误日志并返回。

## 配置-xml
### 加载管理
例：`GeneralTrialConfig`
武将试炼配置。采用单例模式，通过读取XML文件来加载配置数据，提供一系列的接口来获取配置数据

- load函数
从XML文件中加载配置数据。首先获取配置文件的路径，使用`path +=""`更改路径，然后使用TinyXML库解析XML文件。解析过程中，会根据不同的XML节点调用相应的加载函数
```C++
bool GeneralTrialConfig::load()
{
    // ...
}
```

- 配置加载函数
负责解析XML文件中的特定节点，并存储在相应的数据结构中
```C++
bool GeneralTrialConfig::LoadCommonCfg(const TiXmlElement& rElement)
{
    // ...
}
```

- 配置数据访问函数
用于访问加载后的配置数据（如`GetBaseCompleteList`、`GetRefreshNode`等）
```C++
const std::vector<GeneralTrialConfig::BaseCompleteNode>& GeneralTrialConfig::GetBaseCompleteList()
{
    return m_base_complete_list;
}
```
### 使用实现
使用`m_data.getGeneralTrialData()`获取当前活动的体力数据，存储在`p_pbTrialData`中
```cpp
auto rCommonCfg = GeneralTrialConfig::single()->GetCommonData();
unsigned int uAddValue = 0;
if (pbTrialData.valuetimerefresh() == 0)
{
    uAddValue = rCommonCfg.uDailyRenew;
}
```
## 配置-ini

### 加载管理
- ini文件
字段分区，键值表

```ini
[app]
server_id =40000
localname =电信测试区
localid=113
IsDevelop=1
```

- CBaseConfig类
getxxx，例如getuint从一个配置文件中获取一个指定字段（field）和键（key）对应的值，并将这个值转换为无符号整型（unsigned int）。
```cpp
unsigned int  CBaseConfig::getuint(	string file,string field,string key, unsigned int def/*=0 */)
{
	string strvalue = _getstring(file,field,key);
	if( strvalue.empty() )
		return def;
	return boost::lexical_cast<unsigned int>(strvalue);
}
```


### 使用实现
例：读取`m_localname`

配置宏定义，getstring读取配置中的string
```cpp
#define CS_CONFIGFILE   "cs_config.ini"

void CNetMg<XNetServerMg>::init_localparam()
{
	m_localname = CBaseConfig::getstring(CS_CONFIGFILE,"app","localname","" );
	if (!m_localname.size())
	{
		APPLOG_FATAL("[读大区名字失败!] 文件名："<<CS_CONFIGFILE<<";app中localname字段");
	}
```

# 中间件
## Redis

- `RedisClient`
RedisClient 类用于与Redis服务器进行交互。它包含一个嵌套的Handle结构体，用于表示订阅的句柄。提供了连接、执行命令、订阅和发布消息等功能。使用了Boost.Asio库来处理异步操作

- `RedisWorkerGroup`
RedisWorker 类负责单个 Redis 连接的建立和命令执行，而 RedisWorkerGroup 类则管理多个 RedisWorker 实例，提供了方便的方式来处理多个 Redis 连接。

- `RedisMg`
RedisMg 类使用单例模式，方便全局访问和管理。通过DoCommand方法执行各种Redis命令，支持不同参数形式。


## Kafka

### 内容
1. Topic
 
- **定义**：一个Topic对应一个消息队列，Kafka支持多生产者、多消费者模式。
 
- **特性**：为实现可伸缩性，Kafka将Topic分为多个Partition，消息被平均分配到各Partition中。
 
2. Partition
 
- **定义**：Partition是Kafka存储消息的基本单位。
 
- **特性**：每个Partition内消息通过偏移量(offset)唯一标识。Kafka保证Partition内消息有序，但全局无序。
 
3. 消费模型
 
- **队列模式（点对点模式）**：
  - 多个消费者共同消费同一队列，每条消息仅发送给一个消费者。
 
- **发布/订阅模式**：
  - 消息会发送给所有订阅主题的消费者。
 
- **Consumer Group**：
  - Kafka引入Consumer Group概念，组内消费者以队列模式工作，但整个Group以发布/订阅模式消费Topic。
 
4. Partition与消费模型
 
- **消息存储**：Partition中的消息可被不同Consumer Group多次消费，消息除非到期否则不会被删除。
 
- **消费位置**：Partition会为每个Consumer Group保存消费位置偏移量。
 
- **消费规则**：同一Consumer Group内，一个Partition只能被一个Consumer消费。
  - Consumer数量多于Partition时会有空闲Consumer。
  - Consumer数量少于Partition时，一个Consumer可能消费多个Partition。
 
5. 物理存储
 
- **Broker**：Partition存储在Broker上，Broker可以是物理机或集群。
 
- **副本机制**：为提高可用性，Partition会有多个副本存储在不同Broker中。
  - 通过Zookeeper的Leader选举机制选出主副本负责读写。
  - 其他副本同步主副本消息。
 
6. 总结
 
- Kafka通过Topic、Partition、Consumer Group等概念实现了高效的消息处理机制。
- Kafka保证Partition内消息有序，但全局无序。
- Partition副本机制提高了系统的可用性和容错性。

### 实现
- `KafkaProducerMail`
用于在Web邮件系统中，通过Kafka实现邮件的异步发送和处理
    - 单例模式:使用单例模式,确保全局只有一个Kafka生产者实例
    - 初始化:Init初始化Kafka生产者和topic
    - 发送消息：通过不同的 Mail2XXX 方法发送邮件消息到Kafka，这些消息可以被Kafka的消费者处理，实现邮件的异步处理和分发。

- `KafkaMsgUtil`
用于生成各种事件信息的JSON字符串，包括服务器状态、用户行为、系统错误，通过Kafka发送这些信息可以实现集中式日志记录和监控。
    - JSON构造：使用CTinyJson类（基于cJSON库）创建和操作JSON对象。
    - 事件数据封装：为每种类型的事件定义一个方法，以及枚举，将事件相关数据添加到JSON对象中。
    - 格式化输出：将JSON对象转换为字符串，以便发送。

## MySQL
- `CMySQLClient`
CMySQLClient类封装了与MySQL数据库的连接和操作，继承线程类，通过线程的方式异步处理SQL请求，提高并发性能
1. 方法

        ConnectDB方法用于连接到MySQL数据库，第一个重载版本接受数据库的主机名、端口号、数据库名、用户名和密码作为参数，第二个重载版本使用默认参数连接到数据库。
        PushSqlReq方法将SQL请求添加到请求队列中。
        real_escape_string方法用于转义SQL字符串，防止SQL注入攻击。
        empty方法检查请求队列是否为空。
        CheckMySqlIsConnect方法检查MySQL连接是否有效。
        IsConnectNotPing方法检查MySQL连接是否有效且未超时。

2. 成员

        m_pDBSystem是一个指向CMySQLDBSystem类的指针，用于管理数据库系统。
        m_pMySqlConn是一个指向CMySQLConnection类的指针，表示MySQL连接。

- `CMySQLDBSystem`
用于管理MySQL数据库操作的模块，通过线程池来提高性能，并且提供了多种方式添加SQL请求,提供接口用于监控和管理数据库连接及操作。
1. 系统接口

        SetMysqlServerListInfo：设置MySQL服务器连接信息。
        Startup：启动数据库系统。
        Stop：停止数据库系统。
        RunMsg：运行消息处理，处理最多imaxcount条消息。
        AddMysqlConnect：添加MySQL连接。
        CheckMysqlConnect：检查MySQL连接。
        ClearDbRunLog：清除数据库运行日志。
2. 用户接口

        AddSQLRequest_byaccount：通过用户账号添加SQL请求。
        AddSQLRequest_bytblname：通过表名添加SQL请求。
        AddSQLRequest_byrandom：随机选择一个客户端添加SQL请求。
        AddSQLRequest_BySqlClient：通过指定的SQL客户端添加SQL请求。
        AddSQLRequest_syslog：添加数据库日志请求。

- `CMySQLConnection`和`CMySQLRecordSet`
用于封装MSSQL和MySQL数据库接口。操作记录集和连接,简化MySQL数据库的操作,提供更高层次的接口


- `dbsop_assist.h`
数据库操作相关的一系列结构和类，包括数据库连接信息、查询类型、用户数据标记、查询请求和响应、查询内存池以及数据库性能信息

        结构体 tagDBConnectInfo，用于存储数据库连接信息，包括服务器名称、端口、数据库名称、用户名和密码。
        结构体 TMysqlQuest，用于封装MySQL查询任务。

        类 CsqlstringRequest，用于封装MySQL查询请求和响应。
        类 CRequestPool 和 CSqlReqPoolMg，用于管理查询请求的内存池。
        类 CSysLog_DBInfo，用于生成数据库的性能信息。
        类 CHashInfo，用于管理需要哈希的表。

        枚举类型 EmDbopQueryType，表示数据库操作的类型，包括读取玩家数据、创建昵称、插入新角色等。
        枚举类型 emUserDataMark，用于标记用户基本数据的修改类型。
