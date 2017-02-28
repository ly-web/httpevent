local res = httpevent.response
local config_data = httpevent.CONFIG
local config={}

function config.main()
  res:send_head("Content-Type", "text/plain;charset=UTF-8")
  res:send_body('config_data:\n\n')
  for i,v in pairs(config_data) do
    res:send_body(i..':\n')
    for j,z in pairs(v) do
      res:send_body('\t'..j..'\t'..z..'\n')
    end
  end
end

return config
