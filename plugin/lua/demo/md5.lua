local MD5 = require "md5"
local md5={}

function md5.main()
  httpevent.response:send_head('Content-Type','text/plain;charset=UTF-8')
    :send_body(MD5.sumhexa('hello, luarocks!'))

  httpevent.util_tool:submit(200,'OK')
end

return md5
