local json = require("qjson_lua.qjson")
local comp = require("component")
local internet = comp.internet
local env = require("env")
require("utils")

---Finds the first ME Interface system that is not in the ignore list.
---@param ignore table A table of addresses to ignore when searching for ME Interface systems.
---@return table|nil Returns a proxy object representing the ME Interface system, or `nil` if not found.
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
    local headers = {["Content-Type"] = "application/json"}
    local request = internet.request(uri, json.encode(data), headers, "POST")
    request.finishConnect()
end

local data = {}
local me_sys = find_ME_System({})

table.insert(data, "power", {})
table.insert(data.power, "in", me_sys.getAvgPowerInjection())
table.insert(data.power, "max", me_sys.getMaxStoredPower())
table.insert(data.power, "current", me_sys.getStoredPower())
table.insert(data.power, "idle", me_sys.getIdlePowerUsage())
table.insert(data.power, "use", me_sys.getAvgPowerUsage())
table.insert(data, "items", me_sys.getItemsInNetwork())
table.insert(data, "fluids", me_sys.getFluidsInNetwork())
table.insert(data, "craftable", me_sys.getCraftables())

table.insert(data, "cpus", {})

local cpus = me_sys.getCpus()
for i, v in ipairs(cpus) do
    local cpu = v
    if (not cpu.busy) then
        cpu.activeItems = ""
        cpu.storedItems = ""
        cpu.pendingItems = ""
        cpu.finalOutput = ""
    end
    table.insert(data.cpus, i, cpu)
end

send(data, "me")