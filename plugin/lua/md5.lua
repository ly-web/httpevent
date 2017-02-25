local md5 = require "md5"
httpevent.response:send_head('Content-Type','text/plain;charset=UTF-8')
  :send_body(md5.sumhexa('hello, luarocks!'))
