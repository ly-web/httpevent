--local res=httpevent.response
--local req=httpevent.request
--local route_data=httpevent.ROUTE
--local config_data = httpevent.CONFIG
--local form_tool= httpevent.form_tool
--local cookie_tool= httpevent.cookie_tool
--local session_tool = httpevent.session_tool
--local util_tool = httpevent.util_tool

function dispatch(str)
  local m = nil
  m = dofile (httpevent.CONFIG.http.luaDirectory .. '/' .. str..'.lua')
  if(m ~= nil) then
    m.main()
    if not m.enable_cache then
      httpevent.util_tool:submit(200,'OK')
    end
  else
    httpevent.util_tool:error(404,'Not found')
  end
end

dispatch(httpevent.ROUTE[2])
