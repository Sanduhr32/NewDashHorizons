local comp = require("component")
local json = require("qjson_lua.qjson")

function stringInTable(str, tbl)
    if not tbl or not str then
        return false
    end
    for _, value in ipairs(tbl) do
        if value == str then
            return true
        end
    end
    return false
end

function send(data, system, systype)

    local uri = env.url .. "/api"
    if system then
        uri = uri .. "?system=" .. system
    end
    if systype then
        uri = uri .. "&type=" .. systype
    end

    local json_data = json.encode(data)
    local headers = {["Content-Type"] = "application/json", ["X-drogon-auth"] = env.secret}

    local req = internet.request(uri, json_data, headers, "POST")
    req.finishConnect()
end

---comment
---@param gt_machine_name string
---@param skipper string|nil
---@return gt_machine|nil
function get_gt_proxy(gt_machine_name, skipper)
    for addr in comp.list("gt_machine") do
        local gregger = comp.proxy(addr, "gt_machine")
        if gregger.getName() == gt_machine_name and not stringInTable(gregger.address, skipper) then
            return gregger
        end
    end
    return nil
end


---@param str string
function parse(str)
    return tonumber(str:gsub("[^0-9]", ""), 10)
end

-- Function to simulate waiting for a specific duration (in seconds)
if not os.sleep then
    function os.sleep(seconds)
        local start_time = os.time()
        while os.time() - start_time < seconds do
            -- Simulate idle state
        end
    end
end