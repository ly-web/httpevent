local res=httpevent.response
local req=httpevent.request
local route_data=httpevent.ROUTE
local form_tool= httpevent.form_tool
local cookie_data= httpevent.cookie_tool
local session_tool = httpevent.session_tool

res:send_head("Content-Type", "text/plain;charset=UTF-8")

res:send_body('route_data:\n')
for i,v in ipairs(route_data) do
  res:send_body('\t'):send_body(i):send_body('='):send_body(v):send_body('\n')
end

if form_tool ~= nil then
  res:send_body('form_data:\n')
  if form_tool:has_form('upload') then
    res:send_body('\t'):send_body('upload'):send_body('='):send_body(form_tool:get_form('upload')):send_body('\n')
  else
    res:send_body('\t'):send_body('not found upload\n')
  end
end

if cookie_tool ~= nil then
  res:send_body('session_id:\n')
  res:send_body('\t'):send_body(cookie_tool:get_cookie('HTTPEVENTSESSIONID'))
    :send_body('\n')
end

res:send_body('client:\t'):send_body(req:get_client()):send_body("\n")
  :send_body('uri:\t'):send_body(req:get_uri()):send_body("\n")
  :send_body('method:\t'):send_body(req:get_method()):send_body("\n")
  :send_body('user_agent:\t'):send_body(req:get_user_agent()):send_body("\n")
