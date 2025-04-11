local comp = require("component")
local sh = require("shell")
local libraries = {
    json = {
        source = "https://raw.githubusercontent.com/Vurv78/qjson.lua/refs/heads/master",
        files = {"qjson.lua"},
        dir = "./qjson_lua/"
    },
    itself = {
        source = "https://raw.githubusercontent.com/Sanduhr32/NewDashHorizons/refs/heads/master/lua",
        files = {"utils.lua", "env.lua"},
        dir = "./"
    }
}

for name, lib_data in pairs(libraries) do
    print("| Installing " .. name)
    print("| from: " .. lib_data.source)
    local src = lib_data.source;
    for i, filename in ipairs(lib_data.files) do
        print("|- Downloading required file " .. filename)
        local command = "wget -qf \"%s\" \"%s\""
        local succ, fck = sh.execute(string.format(command, src .. "/" .. filename, lib_data.dir .. filename))
        if not succ then
            print(fck)
            return
        end
    end
    print("|============")
end