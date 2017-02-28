local lustache = require "lustache"
local tpl={}

tpl.view_model = {
  title = "Joe",
  calc = function ()
    return 2 + 4;
  end
}

function tpl.main()
  local output = lustache:render("{{title}} spends {{calc}}", tpl.view_model)
  httpevent.response:send_head('Content-Type','text/plain;charset=UTF-8')
    :send_body(output)
end

return tpl
