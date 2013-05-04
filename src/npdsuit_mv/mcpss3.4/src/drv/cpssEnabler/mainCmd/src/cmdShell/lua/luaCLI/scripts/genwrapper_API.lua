cmdLuaCLI_registerCfunction("GenWrapper_init")
GenWrapper_init()

-- *** backend ***
-- function cpssGenWrapper(cpssAPIname, params)
--
-- cpssGenWrapper
--
-- Description:
--          Call CPSS API function
--
-- Parameters:
--  cpssAPIname     - Fx: "prvCpssDrvHwPpReadRegister"
--  params          - table:
--                    each row is table of 3 elements:
--                    1. direction (IN | OUT | INOUT)
--                    2. parameter type
--                       the valid types:
--                       "int"
--                       "bool"
--                       "GT_U8",
--                       "GT_U16",
--                       "GT_U32",
--                       "string"
--                       "custom_type_name" with a set of functions:
--                          mgmType_to_c_${custom_type_name}(param)
--                                  - convert lua variable/table to
--                                    userdata object
--                          mgmType_to_lua_${custom_type_name}(param)
--                                  - convert userdata object to lua
--                    3. parameter name
--                    4. parameter value(optional, for IN and INOUT only)
--
--
-- Returns:
--  GT_STATUS, array of output values
--  string if error
--
--
-- Example
-- status, values = cpssGenWrapper(
--                      "prvCpssDrvHwPpReadRegister", {
--                           { "IN", "GT_U8", "devNum", 0 },
--                           { "IN", "GT_U32","regAddr", 0x200000 },
--                           { "OUT","GT_U32","regData" }
--                      })
-- regData = values.regData



--
-- CPSS API descriptions table
--
-- Format:
--      cpssAPIs[cpssAPIname] = paramsDescription
--                          See cpssGenWrapper() description for details
--                          Only three columns required
cpssAPIs={}

--
-- Map common function to cpss API by device type
-- Format:
--      cpssAPICommon[deviceFamily][name] = cpssAPIname
cpssAPICommon={}
cpssAPICommon["DX"]={}
cpssAPICommon["SX"]={}
cpssAPICommon["EX"]={}
cpssAPICommon["EXMXPM"]={}

--
-- Map deviceId to family
cpssDeviceFamily={}



-- cpssAPIcall
--
-- Description:
--          Call CPSS API function.
--          This function search for parameter description table
--          in cpssAPIs table
--          
--
-- Parameters:
--  cpssAPIname     - Fx: "prvCpssDrvHwPpReadRegister"
--  params          - table:
--                    parameter name -> value
--  strict          - when this parameter is true then
--                    raise error when IN or INOUT parameter is not defined
--
--
-- Returns:
--  GT_STATUS, array of output values
--  string if error
--
--
-- Example
--      result, values = cpssAPIcall("cpssDxChBrgFdbMacEntryRead", {
--                  devNum = 0,
--                  index = indexCnt
--              })
--
function cpssAPIcall(cpssAPIname, params, strict)
    if type(params) ~= "table" then
        error("cpssAPIcall(): the second parameter must be table", 2)
    end
    -- map common function to family specific (optional)
    if params["devNum"] ~= nil then
        local family = cpssDeviceFamily[params["devNum"]]
        if family ~= nil then
            if cpssAPICommon[family] ~= nil then
                if cpssAPICommon[family][cpssAPIname] ~= nil then
                    cpssAPIname = cpssAPICommon[family][cpssAPIname]
                end
            end
        end
    end
    local desc = cpssAPIs[cpssAPIname]
    if desc == nil then
        error("cpssAPIcall(): no description for "..cpssAPIname, 2)
    end
    if type(desc) ~= "table" then
        error("cpssAPIcall(): wrong description for "..cpssAPIname, 2)
    end
    local cparams = {}
    local n
    for n = 1, #desc do
        if type(desc[n]) ~= "table" then
            error("cpssAPIcall(): wrong description for "..cpssAPIname, 2)
        end
        cparams[n] = {}
        cparams[n][1] = desc[n][1]
        cparams[n][2] = desc[n][2]
        cparams[n][3] = desc[n][3]
        if desc[n][1] == "IN" or desc[n][1] == "INOUT" then
            if params[desc[n][3]] == nil and strict then
                error("cpssAPIcall(): parameter '"..desc[n][3].."' not defined for "..cpssAPIname, 2)
            end
            cparams[n][4] = params[desc[n][3]]
        end
    end
    return cpssGenWrapper(cpssAPIname, cparams)
end














-- cpssGenWrapperCLI
--
-- Description:
--          CLI for Call CPSS API function
--
-- Parameters:
--  cpssAPIname     - Fx: "prvCpssDrvHwPpReadRegister"
--  params          - table:
--                    each row is table of 3 elements:
--                    1. parameter name
--                    2. parameter value type (for IN, INOUT):
--                       const | var | param | query
--                    3. value (if const)
--                       variable name (if var)
--                       parameter index (if param)
--                       prompt for input (if query)
--
-- Prints:
--  GT_STATUS
--  output values with names
--
--
-- Example
-- currentDeviceId = 0
-- cpssGenWrapperCLI(
--           "prvCpssDrvHwPpReadRegister", {
--              { "deviceId", "const", 0 },
--              { "regAddr",  "param", 1 },
--           },
--           0x2000000)
-- will print:
-- ret=0x0
-- regData=0x12345678
function cpssGenWrapperCLI(cpssAPIname, params, ...)
    -- prepare parameters
    local cparams={}
    local parr={...}
    local k,v
    for k = 1, #params do
        local p = params[k]
        if #p < 3 then
            print("Wrong parameter description for parameter " .. tostring(k))
            return
        end
        if p[2] == "const" then
            cparams[p[1]] = p[3]
        elseif p[2] == "var" then
            cparams[p[1]] = _G[p[3]]
        elseif p[2] == "param" then
            cparams[k][4] = parr[p[3]]
        elseif p[2] == "query" then
            cparams[p[1]] = cmdLuaCLI_readLine(p[3])
        else
            print("Wrong parameter description for parameter " .. tostring(k))
            return
        end
    end
    ret, values = cpssAPIcall(cpssAPIname, cparams)
    if type(ret) == "string" then
        print("error while call cpssGenWrapper(): " .. ret)
        return
    end
    print(string.format("ret=0x%x", ret))

    for k, v in pairs(values) do
        local val
        if type(v) == "number" then
            val = string.format("0x%x",v)
        else
            val = cmdLuaCLI_to_string(v)
        end
        print(k.."="..val)
    end
end
