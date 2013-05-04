--
-- CPSS common functions
--



--
-- Import Lua C functions from wraplCpssCommon.c
--
cmdLuaCLI_registerCfunction("wrlIsDevIdValid")
cmdLuaCLI_registerCfunction("wrlDevList")
cmdLuaCLI_registerCfunction("wrlReadRegister")
cmdLuaCLI_registerCfunction("wrlWriteRegister")

--
-- Global variables
--
cpssCurrentDevice = 0

--
-- Functions
--


-- ************************************************************************
--  check_param_deviceid
--
--  DESCRIPTION:
--        Check parameter that it is valid and configured device Id
--
--  INPUTS:
--        param     - parameter string
--
--  RETURNS:
--        1. boolean check status
--        2. number:deviceid or string:error_string if failed
--
-- ************************************************************************
function check_param_deviceid(param)
    local dev
    dev = tonumber(param)
    if type(dev) ~= "number" or dev < 0 or dev > 63 then
        return false, "Wrong device Id"
    end
    if wrlIsDevIdValid(dev) then
        return true, dev
    end
    return false, string.format("Device %d not configured", dev)
end



-- ************************************************************************
--  list_to_string
--
--  DESCRIPTION:
--        Convert list of numbers to comma separated string
--
--  INPUTS:
--        list     - parameter string
--
--  RETURNS:
--        string
--
--  EXAMPLES
--        print(list_to_string({1,4,6,7,8,9,11,20}))
--           1,4,6-9,11,20
--
-- ************************************************************************
function list_to_string(list)
    local s
    local is_range
    is_range = false
    for k = 1, #list do
        if k == 1 then
            s = list[k]
        elseif list[k-1]+1 ~= list[k] then
            if is_range then
                s = s .. "-" .. list[k-1]
            end
            is_range = false
            s = s .. "," .. list[k]
        else
            is_range = true
        end
    end
    if is_range then
        s = s .. "-" .. list[#list]
    end
    return s
end

-- ************************************************************************
--  string_to_list
--
--  DESCRIPTION:
--        Convert string to list of numbers
--
--  INPUTS:
--        s     - parameter string
--
--  RETURNS:
--        list
--
--  EXAMPLES
--        print(table.concat(string_to_list("1,4,6-9,11,20"), ","))
--           1,4,6,7,8,9,11,20
--
-- ************************************************************************
function string_to_list(s)
    local list
    local is,ie,ch,val,prevch,prev
    list = {}
    while true do
        is,ie,ch = string.find(s, "([-,])")
        if type(is) == "nil" then
            val = tonumber(s)
        else
            val = tonumber(string.sub(s,1,is-1))
        end

        if prevch == "-" then
            prev = prev + 1
            while prev < val do
                table.insert(list, prev)
                prev = prev + 1
            end
        end
        table.insert(list,val)

        if type(is) == "nil" then
            break
        else
            prev = val
            prevch = ch
            s = string.sub(s,ie+1)
        end
    end

    return list
end




--
-- Commands
--
function cpssLuaTest_setCurrentDevice(dev)
    cpssCurrentDevice = dev
end

function cmdLuaCLI_showRegister(regAddr)
    regData = wrlReadRegister(cpssCurrentDevice, regAddr)
    if type(regData) ~= "number" then
        print("failed to read register: " .. regData)
    else
        print(string.format("reg[0x%08x]=0x%08x",regAddr, regData)) 
    end
end
function cmdLuaCLI_setRegister(regAddr, regData)
    local rc = wrlWriteRegister(cpssCurrentDevice, regAddr, regData)
    if rc == 0 then
        print("OK")
    else
        print(string.format("Failed to write register: %d",rc))
    end
end

cmdLuaCLI_addCommands({
    {
        syn = "cpss",
        sub = {
            {
                syn = "show",
                sub = {
                    {
                        syn = "register",
                        func = cmdLuaCLI_showRegister,
                        types = { "number" }
                    }
                }
            },
            {
                syn = "set",
                sub = {
                    {
                        syn = "register",
                        func = cmdLuaCLI_setRegister,
                        types = { "number", "number" }
                    },
                    {
                        syn = "device",
                        func = cpssLuaTest_setCurrentDevice,
                        types = { "deviceid" }
                    }
                }
            }
        }
    }
})
