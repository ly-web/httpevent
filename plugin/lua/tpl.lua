local lustache = require "lustache"

local view_model = {
  title = "Joe",
  calc = function ()
    return 2 + 4;
  end
}

local output = lustache:render("{{title}} spends {{calc}}", view_model)
httpevent.response:send_head('Content-Type','text/plain;charset=UTF-8')
  :send_body(output)
