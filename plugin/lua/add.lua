local result=httpevent.lua_test:add(1,2)
httpevent.response:send_head('lua','5.3')
:send_head("Content-Type", "text/plain;charset=UTF-8")
:send_body(result)
