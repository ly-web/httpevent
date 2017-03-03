# httpevent
c++ and lua web framework

## 缘起
httpevent是webcppd的继承者。

由于webcppd内核采用的是Poco内置的多线程服务器，虽然性能不错，但是依然存在不小的提升空间。

httpevent不仅包含webcppd的全部特性，而且改进和增加了新的功能

## 主要改进
httpevent主要在以下三个方面了改进了webcppd：
- 内核采用libevent2，性能倍增，稳定性更好，对信号处理更友好
- handler采用基于libevent2的轻包装类，不再受限于Poco。
- 不仅轻松支持C++开发，而且轻松支持lua开发

## 基本特性
- HTTP 1.1
- HTTPS
- 支持多种服务器类型选择，可通过配置文件轻松配置
    - 0=static
    - 1=0+dynamic
    - 2=1+session
    - 3=1+cache
    - 4=3+session
    - 5=all
    - default=5
- c++ 开发
- lua 开发(支持lua v5.1,v5.2,v5.3以及luajit)
- 会话管理
- 自动缓存管理
- ip动、静态黑名单
- 正则路由(PCRE)
- 防盗链
- 日志


## lua api
使用方法参见`plugin/lua`
- httpevent.response : lua 对象实例
    - send_head
    - send_body

- httpevent.request : lua 对象实例
    - get_client
    - get_method
    - get_uri
    - get_user_agent

- httpevent.ROUTE :  一个 lua 表 


- httpevent.CONFIG : 一个 lua 表

- httpevent.form_tool : lua 对象实例 包含GET,POST,可上传域为upload的文件
    - has
    - get

- httpevent.cookie_tool : lua 对象实例
    - has
    - set
    - get

- httpevent.session_tool : lua 对象实例
    - has
    - set
    - get

- httpevent.util_tool : lua 对象实例
    - error
    - submit
    - redirect


## c++ hello world
```
#include <httpevent.hpp>
namespace httpevent {
    class hello : public view {

        void handler(const request& req, response& res) {
            res.send_head("Content-Type", "text/plain;charset=UTF-8")
                    .send_body("hello,world");
        }

    };
}
```
## lua hello world
```
local hello={}
function hello.main()
  httpevent.response:send_head("Content-Type", "text/plain;charset=UTF-8")
    :send_body('hello,world')
end

return hello

```

## 服务器选型
- 做纯静态文件服务器，推荐0型
- 作后端应用服务器,推荐1、2、3、4型，视乎业务需要选择是否需要缓存管理器或（和）会话管理器
- 作通用服务器，推荐选择默认的5型

## 自动缓存服务
httpevent提供系统级的缓存服务，对于3、4、5型服务器，只要没有调用submit方法，系统都将自动缓存内容。
一旦调用submit方法，系统就会认为该内容无需缓存。

## 压力测试
以下数据以3型服务器为基准，压力测试工具为ab(ubuntu 16.10,i5-5200U,4G,笔记本)

- c++ hello world

        ab -c500 -n 50000 http://localhost/
        Requests per second:    24538.13 [#/sec] (mean)

- lua hello world

        ab -c500 -n 50000 http://localhost/demo/hello.lua
        Requests per second:    20470.11 [#/sec] (mean)


## 依赖

- libevent v2.1.8+
- Poco framework v1.6+
- lua devel v5.1+
- openssl devel

## 安装
`build.sh`

## 配置
`/etc/httpevent/httpevent.properties`

## 运行
`sudo systemctl enable httpevent`

`sudo systemctl (start|stop|restart|status) httpevent`