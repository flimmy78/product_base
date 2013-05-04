function file_sequental_processing(filename)
    local fd, e
    fd, e = fs.open(filename)
    if fd == nil then
        print("failed to open file " .. filename .. ": "..e)
        return false
    end
    while true do
        local str
        str, e = fs.gets(fd)
        if str == nil then
            if e == "<EOF>" then
                break
            end
            fs.close(fd)
            print("error reading file: " .. e)
            return false
        end
        --****************************
        --*** parse line from file ***
        --****************************

    end
    fs.close(fd)
    return true
end
