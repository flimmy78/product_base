--
-- Config file support functions
--


-- ************************************************************************
-- config_file_load
--
-- DESCRIPTION:
--      Load and execute commands from file
--
-- INPUTS:
--      filename
--
-- RETURNS:
--      bool status
--
-- ************************************************************************
function config_file_load(filename)
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
        -- remove trailing \n and \r and spaces
        local f = true
        while f do
            f = false
            local c = string.sub(str, -1)
            if c == "\n" then f = true end
            if c == "\r" then f = true end
            if c == "\t" then f = true end
            if c == " "  then f = true end
            if f then
                str = string.sub(str, 1, -2)
            end
        end
        -- remove heading spaces
        local f = true
        while f do
            f = false
            local c = string.sub(str, 1, 1)
            if c == "\t" then f = true end
            if c == " "  then f = true end
            if f then
                str = string.sub(str, 2)
            end
        end
        -- skip empty strings and comments
        if str ~= "" and string.sub(str, 1, 2) ~= "--" then
            -- execute line:
            -- cmdLuaCLI_execute(str)
            print("do cmdLuaCLI_execute(\"" .. str .. "\")")
            -- handle return status
        end
    end
    fs.close(fd)
    return true
end


-- ************************************************************************
--  copy_file
--
--  DESCRIPTION:
--      copy file
--
--  INPUTS:
--      srcname
--      dstname
--
--  RETURNS:
--      bool status
--
-- ************************************************************************
function copy_file(srcname, dstname)
    if not fs.fileexists(srcname) then
        return false
    end
    fs.unlink(dstname)
    local fsrc, fdst, e
    fsrc, e = fs.open(srcname, "r")
    if fsrc == nil then
        print("failed to open file " .. srcname .. ": "..e)
        return false
    end
    fdst, e = fs.open(dstname, "w")
    if fdst == nil then
        print("failed to create file " .. dstname .. ": "..e)
        fs.close(fsrc)
        return false
    end
    while true do
        local data
        data = fs.read(fsrc, 1024);
        if data == nil or data == "" then
            break
        end
        fs.write(fdst, data)
    end
    fs.close(fsrc)
    fs.close(fdst)
    return true
end

-- ************************************************************************
-- config_file_clear
--
-- DESCRIPTION:
--      clear a config file
--
-- INPUTS:
--      filename
--
-- RETURNS:
--      bool status
--
-- ************************************************************************
function config_file_clear(filename)
    fs.unlink(filename)
end

-- ************************************************************************
--  config_file_add_command
--
--  DESCRIPTION:
--      add a command to config file
--
--  INPUTS:
--      filename
--      command
--
--  RETURNS:
--      bool status
--
-- ************************************************************************
function config_file_add_command(filename, command)
    local fd, e
    fd, e = fs.open(filename, "a")
    if fd == nil then
        print("failed to open file " .. filename .. ": "..e)
        return false
    end
    fs.write(fd, command .. "\n")
    fs.close(fd)
    return true
end

-- ************************************************************************
--  config_file_remove_lines
--
--  DESCRIPTION:
--      add a command to config file
--
--  INPUTS:
--      filename
--      pattern 
--
--  RETURNS:
--      bool status
--
-- ************************************************************************
function config_file_remove_lines(filename, pattern)
    if not fs.fileexists(filename) then
        return true
    end
    -- first create a backup file (copy)
    if not copy_file(filename, filename .. ".bak") then
        return false
    end
    -- now copy lines from bak file except matched
    local fd, e, fdbak
    fs.unlink(filename)
    fd, e = fs.open(filename, "w")
    if fd == nil then
        print("failed to create file " .. filename .. ": "..e)
        return false
    end
    fdbak, e = fs.open(filename .. ".bak", "r")
    if fdbak == nil then
        print("failed to open file " .. filename .. ".bak: "..e)
        fs.close(fd)
        return false
    end
    while true do
        local str
        str, e = fs.gets(fdbak)
        if str == nil then
            if e == "<EOF>" then
                break
            end
            fs.close(fd)
            fs.close(fdbak)
            print("error reading file: " .. e)
            return false
        end
        if string.match(str, pattern) == nil then
            fs.write(fd, str)
        end
    end
    fs.close(fd)
    fs.close(fdbak)


    return true
end

-- ************************************************************************
--  create_config_from_history
--
--  DESCRIPTION:
--      create config file from history
--
--  INPUTS:
--      filename
--
--  RETURNS:
--      bool status
--
-- ************************************************************************
function create_config_from_history(filename)
    local filters = {
        "saveconfig"
    }
    -- first create a backup file (copy)
    if fs.fileexists(filename) then
        copy_file(filename, filename .. ".bak")
    end
    fs.unlink(filename)
    local fd, e
    fd, e = fs.open(filename, "w")
    if fd == nil then
        print("failed to open file " .. filename .. ": "..e)
        return false
    end
    local k, j, filter
    for k = 1, #cmdLuaCLI_history do
        filter = false
        for j = 1, #filters do
            if string.match(cmdLuaCLI_history[k], filters[j]) ~= nil then
                filter = true
                break
            end
        end
        if filter then
            fs.write(fd, "-- ")
        end
        fs.write(fd, cmdLuaCLI_history[k] .. "\n")
    end
    fs.close(fd)
    return true
end


--
-- config module registry table
--
-- here key is module name string, value is function
--
-- For example to register function save_config_fdb():
--     save_config_tbl["FDB configuration"] = save_config_fdb
--
save_config_tbl = {}

-- ************************************************************************
-- save_config
--
-- DESCRIPTION:
--      Save config to file (call all registered functions)
--
-- INPUTS:
--      filename
--
-- RETURNS:
--      bool status
--
-- ************************************************************************
function save_config(filename)
    -- first create a backup file (copy)
    if fs.fileexists(filename) then
        copy_file(filename, filename .. ".bak")
    end
    fs.unlink(filename)
    local fd, e
    fd, e = fs.open(filename, "w")
    if fd == nil then
        print("failed to open file " .. filename .. ": "..e)
        return false
    end
    local k, v, status
    status = true
    for k,v in pairs(save_config_tbl) do
        if type(v) ~= "function" then
            print("wrong value in save_config_tbl[" .. string.format("%q",k) .. "]")
        else
            fs.write("-- " .. k .. "\n")
            if not v(fd) then
                status = false
            end
        end
    end
    fs.close(fd)
    return status
end

