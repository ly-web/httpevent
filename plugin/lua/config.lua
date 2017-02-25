local res = httpevent.response
local config_data = httpevent.CONFIG

res:send_head("Content-Type", "text/plain;charset=UTF-8")

res:send_body('config_data:\n')
for i,v in pairs(config_data) do
  res:send_body('\t'):send_body(i):send_body('='):send_body(v):send_body('\n')
end
