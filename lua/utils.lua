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