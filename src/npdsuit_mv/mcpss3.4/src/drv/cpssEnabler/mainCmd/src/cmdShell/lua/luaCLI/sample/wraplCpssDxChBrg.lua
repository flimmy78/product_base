--
-- CPSS DxCh Bridge functions
--

--
-- Includes
-- 
require("wraplCpssCommon")
require("genwrapper_API")

--
-- Import Lua C functions from wraplCpssDxChBrg.c
--
cmdLuaCLI_registerCfunction("mgmType_to_lua_CPSS_MAC_ENTRY_EXT_STC")
cmdLuaCLI_registerCfunction("mgmType_to_c_CPSS_MAC_ENTRY_EXT_STC")

--
-- Create CLI mode: fdb_configuration
--
cmdLuaCLI_modes["fdb_configuration"] = {
    prompt = "(config-fdb)#"
}

--
-- Add command to switch from configuration to vlan_configuration
--
cmdLuaCLI_addCommand("configuration", "fdb configuration", {
    func = cmdLuaCLI_change_mode_push,
    args = { "fdb_configuration" }
})

cmdLuaCLI_addCommand("fdb_configuration", "bridge aging-time", {
    types = { "number" },
    func = cpssGenWrapperCLI,
    args = {
        "cpssDxChBrgFdbAgingTimeoutSet",
        {
            { "IN", "GT_U8", "deviceId", "const", 0 },
            { "IN", "GT_U32","timeOut",  "param", 1 }
        }
    }
})
cmdLuaCLI_addCommand("fdb_configuration", "show bridge aging-time", {
    func = cpssGenWrapperCLI,
    args = {
        "cpssDxChBrgFdbAgingTimeoutGet",
        {
            { "IN", "GT_U8", "deviceId", "const", 0 },
            { "OUT","GT_U32","timeOut"}
        }
    }
})

function show_fdb()
    local indexCnt
    local fdbEnd
    indexCnt = 0
    fdbEnd = false
    local result, values

    repeat
        result, values = cpssGenWrapper(
            "cpssDxChBrgFdbMacEntryRead", {
                { "IN", "GT_U8", 0 }, -- devNum
                { "IN", "GT_U32", indexCnt },
                { "OUT","GT_BOOL" }, -- *validPtr,
                { "OUT","GT_BOOL" }, -- *skipPtr,
                { "OUT","GT_BOOL" }, -- *agedPtr,
                { "OUT","GT_U8" },   -- *associatedDevNumPtr,
                { "OUT","CPSS_MAC_ENTRY_EXT_STC" } --  *entryPtr
            })

        if result == 0 then
            if values[3] then -- valid
                print(string.format("index=%d entry=",indexCnt)..cmdLuaCLI_to_string(values[7], 1))
            end
        end
        if result == 3 then -- GT_OUT_OF_RANGE
            fdbEnd = true
        end

        indexCnt = indexCnt + 1
    until fdbEnd

end

cmdLuaCLI_addCommand("fdb_configuration", "show fdb", {
    func = show_fdb
})
