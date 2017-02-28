local hello={}
hello['enable_cache']=true
function hello.main()
  httpevent.response:send_head("Content-Type", "text/plain;charset=UTF-8")
    :send_body('hello,world')
end

return hello
