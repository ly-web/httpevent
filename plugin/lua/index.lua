--local res=httpevent.response
--local req=httpevent.request
--local route_data=httpevent.ROUTE
--local config_data = httpevent.CONFIG
--local form_tool= httpevent.form_tool
--local cookie_tool= httpevent.cookie_tool
--local session_tool = httpevent.session_tool
--local util_tool = httpevent.util_tool

xpcall(
  function()
    local m = dofile (string.format("%s/%s.lua",httpevent.CONFIG.http.luaDirectory,httpevent.ROUTE[2]))
    m.main()
  end
  ,function()
    httpevent.util_tool:error(501,"Could not run script")
  end
)
