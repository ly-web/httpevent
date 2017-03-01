--local res=httpevent.response
--local req=httpevent.request
--local route_data=httpevent.ROUTE
--local config_data = httpevent.CONFIG
--local form_tool= httpevent.form_tool
--local cookie_tool= httpevent.cookie_tool
--local session_tool = httpevent.session_tool
--local util_tool = httpevent.util_tool

function httpevent.show_error(err)
  httpevent.util_tool:error(501,"Could not run script")
end

function httpevent.dispatch(str)
  local m = dofile (httpevent.CONFIG.http.luaDirectory .. '/' .. str..'.lua')
  xpcall(m.main,httpevent.show_error)
end


httpevent.dispatch(httpevent.ROUTE[2])
