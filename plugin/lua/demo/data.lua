local res=httpevent.response
local req=httpevent.request
local route_data=httpevent.ROUTE
local form_tool= httpevent.form_tool
local cookie_tool= httpevent.cookie_tool
local session_tool = httpevent.session_tool
local util_tool = httpevent.util_tool
local session_key = 'HTTPEVENTSESSIONID'

local data={}

function data.main ()

  res:send_head("Content-Type", "text/plain;charset=UTF-8")

  res:send_body('route_data:\n')
  for i,v in ipairs(route_data) do
    res:send_body('\t'..i..'='..v..'\n')
  end

  local form_key={'a','b','upload'}
  res:send_body('form_data:\n')
  for i,v in ipairs(form_key) do
    if form_tool:has(v) then
      res:send_body('\t'..v..'='..form_tool:get(v)..'\n')
    end
  end



  res:send_body('session_id:\n')
  if cookie_tool:has(session_key) then
    res:send_body('\t'..cookie_tool:get(session_key)..'\n')
  else
    res:send_body('\t'..'not found session_id'..'\n')
  end

  res:send_body('client:\t'..req:get_client().."\n")
    :send_body('uri:\t'..req:get_uri().."\n")
    :send_body('method:\t'..req:get_method().."\n")
    :send_body('user_agent:\t'..req:get_user_agent().."\n")

  util_tool:submit(200,'OK')

end


return data
