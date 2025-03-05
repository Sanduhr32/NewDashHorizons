local json = require("qjson_lua.qjson")
local comp = require("component")
local internet = comp.internet
local env = require("env")
require("utils")

local function find_ME_System(ignore)
    for addr in comp.list("me_interface") do
        local ae2 = comp.proxy(addr, "me_interface")
        if not stringInTable(addr, ignore) then
            return ae2
        end
    end
    return nil
end

---Sends data to a specified system or default URL endpoint.
---@param data table The data to be sent, expected to be a table.
---@param system string|nil Optional. The system identifier to target a specific system.
---If provided, the URL is appended with `?system=<system>`.
---If not provided, the data is sent to the default URL.
local function send(data, system)

    local uri = ""
    if system then
        uri = env.url .. "/api?system=" .. system
    else
        uri = env.url .. "/api"
    end

    local json_data = json.encode(data)
    local headers = {["Content-Type"] = "application/json", ["X-drogon-auth"] = env.secret}

    local req = internet.request(uri, json_data, headers, "POST")
    req.finishConnect()
end

local data = {}
local me_sys = find_ME_System({})

--data["power"] = {}
--data.power["in"] = me_sys.getAvgPowerInjection()
--data.power["max"] = me_sys.getMaxStoredPower()
--data.power["current"] = me_sys.getStoredPower()
--data.power["idle"] = me_sys.getIdlePowerUsage()
--data.power["use"] = me_sys.getAvgPowerUsage()
--send(data.power, "me-power")
data["items"] = me_sys.getItemsInNetwork()
local temp = {}
local count = 0
for i, v in ipairs(data.items) do
    temp[i] = v

    if (i > 0 and i % 20 == 0) then
        send(temp, "me-items-" .. tostring(count))
        count = count + 1
        temp = {}
    end
end
data = {}
--data["fluids"] = me_sys.getFluidsInNetwork()
--send(data.fluids, "me-fluids")
--data = {}
--data["craftable"] = me_sys.getCraftables()
--send(data.craftable, "me-crafts")
--data = {}

--data["cpus"] = {}
--
--local cpus = me_sys.getCpus()
--for i, v in ipairs(cpus) do
--    local cpu = v
--    if (not cpu.busy) then
--        cpu.activeItems = ""
--        cpu.storedItems = ""
--        cpu.pendingItems = ""
--        cpu.finalOutput = ""
--    end
--    table.insert(data.cpus, i, cpu)
--end

--send(data.cpus, "me-cpus")