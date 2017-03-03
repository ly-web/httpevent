local hello={}
function hello.main()
  local res=httpevent.response
  --local req=httpevent.request
  --local route_data=httpevent.ROUTE
  --local config_data = httpevent.CONFIG
  --local form_tool= httpevent.form_tool
  --local cookie_tool= httpevent.cookie_tool
  --local session_tool = httpevent.session_tool
  --local util_tool = httpevent.util_tool
  res:send_head("Content-Type", "text/plain;charset=UTF-8")
    :send_body('hello,world')
end

return hello
