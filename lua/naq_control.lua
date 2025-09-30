require("utils")
local event = require("event")
local thread = require("thread")

local supercap = get_gt_proxy("multimachine.supercapacitor")
local naq_reac = get_gt_proxy("nag")

local cleanup = thread.create(function()
    event.pull("interrupted")
    print("Got signal to terminate!")
    naq_reac.setWorkAllowed(false)
end)

local monitor = thread.create(function()
    local min_percent = 0.1
    local max_percent = 0.98
    while true do
        local data = {}
        local stored_power = parse(cap_sen[2])
        local total_power = parse(cap_sen[5])
        local avg5sout = cap_sen[10]
        local avg5sin = cap_sen[11]
        local drain = parse(cap_sen[7]) + parse(avg5sin:sub(1, avg5sin:find("(", 1, true) or -1))
        local prod = parse(avg5sout:sub(1, avg5sout:find("(", 1, true) or -1))
        data["eu"] = stored_power
        data["eu_max"] = total_power
        data["eu_in"] = prod
        data["eu_out"] = drain
        --send(data, "Naquadah-Reaktor", "power")

        local state = naq_reac.isWorkAllowed()
        local nextstate = state

        if stored_power > (total_power * max_percent) then
            nextstate = false
        elseif stored_power < (total_power * min_percent) then
            nextstate = true
        end

        if nextstate ~= state then
            naq_reac.setWorkAllowed(nextstate)
        end
        os.sleep(15)
    end
end)

thread.waitForAny({cleanup, monitor})
os.exit(0)
