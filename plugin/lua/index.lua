--local res=httpevent.response
--local req=httpevent.request
local route_data=httpevent.ROUTE
local config_data = httpevent.CONFIG
--local form_tool= httpevent.form_tool
--local cookie_tool= httpevent.cookie_tool
--local session_tool = httpevent.session_tool
local util_tool = httpevent.util_tool

local show_error =function (err)
  util_tool:error(501,"Could not run script")
end

local dispatch = function (str)
  local m = dofile (config_data.http.luaDirectory .. '/' .. str..'.lua')
  xpcall(m.main,show_error)
end


dispatch(route_data[2])
