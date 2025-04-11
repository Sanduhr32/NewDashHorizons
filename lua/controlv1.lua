-- Power generation Controller for GT:New Horizons
-- based on GT++ tanks, GT++ LSC and XL Gas Turbines
local comp = require("component")
require("utils")

---The function to be run at intervals
---@param turbine gt_machine
---@param supercap gt_machine
local function control_turbine(turbine, supercap, pump_tank, bounds, max_prod)
    print("entered control function")
    local cap_sen = supercap.getSensorInformation()
    local tur_sen = turbine.getSensorInformation()
    local stored_power = parse(cap_sen[2])
    local total_power = parse(cap_sen[5])
    local avg5sout = cap_sen[10]
    local avg5sin = cap_sen[11]
    local drain = parse(cap_sen[7]) + parse(avg5sin:sub(1, avg5sin:find("(", 1, true) or -1))
    local max_p = math.max(max_prod[1], parse(avg5sout:sub(1, avg5sout:find("(", 1, true) or -1)))

    max_prod[1] = max_p

    local turbine_before = turbine.isWorkAllowed()
    local turbine_after = turbine.isWorkAllowed()

    if stored_power > (total_power * bounds[2]) then
        turbine_after = false
    elseif drain > (max_p * bounds[3]) and stored_power < (total_power * 0.5) then
        turbine_after = true
    elseif stored_power < (total_power * bounds[1]) then
        turbine_after = true
    end

    if turbine_before == turbine_after then
        return nil
    end
    
    turbine.setWorkAllowed(turbine_after)
    pump_tank.setWorkAllowed(turbine_after)

    if turbine_after then
        return "Turbine on at " .. os.date()
    else
        return "Turbine off at " .. os.date()
    end
end

-- Coroutine to manage the interval
local co = coroutine.create(function(cgs)
    local yield_vals = {}
    while true do
        for _, cg in pairs(cgs) do
            local yield_val = control_turbine(cg.turbine, cg.supercap, cg.pump_tank, cg.bounds, cg.max_prod)        -- Call the function
                -- Yield control (optional, depends on use case)
            table.insert(yield_vals, yield_val)
        end
        coroutine.yield(yield_vals)
    end
end)

---comment
---@param bounds table<number> controlling bounds
---@return table<string, string>|nil
local function find_cg(bounds, used_addr)
    local turbine = get_gt_proxy("multimachine.largergasturbine", used_addr)
    local supercap = get_gt_proxy("multimachine.supercapacitor", used_addr)
    local pump_tank = get_gt_proxy("fluidtank.tier.01", used_addr)  -- block is "low voltage fluid tank"

    if not turbine then
        print("turbine not found")
        return nil
    else
        print("found turbine at " .. turbine.address)
    end
    if not supercap then
        print("lapotronic supercapacitor not found")
        return nil
    else
        print("found supercap at " .. supercap.address)
    end

    if not pump_tank then
        print("tank with fluid regulator not found")
        return nil
    end

    local px, py, pz = pump_tank.getCoordinates()
    local tx, ty, tz = turbine.getCoordinates()

    local dx = math.abs(tx - px)
    local dy = math.abs(ty - py)
    local dz = math.abs(tz - pz)

    while (math.sqrt(dx*dx + dy*dy + dz*dz) > 10.5 and (pump_tank ~= nil and pump_tank.address ~= nil)) do
        print("tank distance: " .. math.sqrt(dx*dx + dy*dy + dz*dz))
        -- print("tank: " .. string(pump_tank.address))
        pump_tank = get_gt_proxy("fluidtank.tier.01", used_addr)

        if not pump_tank then
            break
        end

        px, py, pz = pump_tank.getCoordinates()
    
        dx = math.abs(tx - px)
        dy = math.abs(ty - py)
        dz = math.abs(tz - pz)
    end
    
    if not pump_tank then
        print("tank with fluid regulator not found")
        return nil
    else
        print("found tank with fluid regulator at " .. pump_tank.address)
    end

    return {turbine = turbine, supercap = supercap, pump_tank = pump_tank, bounds = bounds, max_prod = { 0 }}
end

-- local turbine = get_gt_proxy("multimachine.largergasturbine")
-- local supercap = get_gt_proxy("multimachine.supercapacitor")
-- local pump_tank = get_gt_proxy("fluidtank.tier.01") -- block is "low voltage fluid tank"
-- local bounds = {} -- lower power, upper power, above drain of max production
-- local max_prod = { 0 }

local control_groups = {}
local used_addr = {}

local count = 1

repeat
    local b = {}
    if count == 1 then
        b = { .10, .98 , 1/4 }
        count = count + 1
    else 
        b = { .20, .90 , 1/2 }
    end
    local found = find_cg(b, used_addr)
    if found then
        table.insert(used_addr, found.turbine.address)
        table.insert(used_addr, found.pump_tank.address)
        table.insert(control_groups, found)
    end
until found == nil

while true do
    local success, vals = coroutine.resume(co, control_groups)     -- Resume the coroutine
    if success and vals then
        for _, val in pairs(vals) do
            print(val)
        end
    elseif not success then
        print(success)
        print("something went wrong")
        print(vals)
        break
    end

    os.sleep(15) -- wait 15s
end

--coroutine.close(co)
