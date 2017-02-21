local result=10000
result=httpevent.lua_test:loop(result)
httpevent.response:send_head('lua','5.3')
:send_head("Content-Type", "text/plain;charset=UTF-8")
:send_body(result)
