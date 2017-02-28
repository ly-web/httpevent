local res=httpevent.response
local req=httpevent.request
local route_data=httpevent.ROUTE
local form_tool= httpevent.form_tool
local cookie_tool= httpevent.cookie_tool
local session_tool = httpevent.session_tool

local session={}

function session.main()
  local session_key ='TEST'
  local session_value=0
  if session_tool:has(session_key) then
    session_value = tonumber(session_tool:get(session_key))
    session_tool:set(session_key,session_value+1)
  else
    session_tool:set(session_key,session_value)
  end
  res:send_head("Content-Type", "text/plain;charset=UTF-8")
    :send_body(session_key..'\t='..session_value)
end

return  session
