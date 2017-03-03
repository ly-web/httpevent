local config={}

function config.main()
  local res=httpevent.response
  --local req=httpevent.request
  --local route_data=httpevent.ROUTE
  local config_data = httpevent.CONFIG
  --local form_tool= httpevent.form_tool
  --local cookie_tool= httpevent.cookie_tool
  --local session_tool = httpevent.session_tool
  local util_tool = httpevent.util_tool
  res:send_head("Content-Type", "text/plain;charset=UTF-8")
  res:send_body('config_data:\n\n')
  for i,v in pairs(config_data) do
    res:send_body(i..':\n')
    for j,z in pairs(v) do
      res:send_body('\t'..j..'\t'..z..'\n')
    end
  end
  util_tool:submit(200,'OK')
end

return config
