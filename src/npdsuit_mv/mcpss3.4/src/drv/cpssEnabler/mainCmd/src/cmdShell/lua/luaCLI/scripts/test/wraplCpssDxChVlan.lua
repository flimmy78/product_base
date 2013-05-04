--
-- CPSS DxCh Vlan functions
--

--
-- Includes
-- 
require("wraplCpssCommon")

--
-- Import Lua C functions from wraplCpssDxChVlan.c
--
cmdLuaCLI_registerCfunction("wrlCpssDxChVlanExists")
cmdLuaCLI_registerCfunction("wrlCpssDxChVlanList")
cmdLuaCLI_registerCfunction("wrlCpssDxChGetVlanInfo")
cmdLuaCLI_registerCfunction("wrlCpssDxChVlanEntrySet")

--
-- Functions
--


-- ************************************************************************
--  check_param_vlanid
--
--  DESCRIPTION:
--        Check parameter that it is valid vlan Id
--        (number in range 0..4095)
--
--  INPUTS:
--        param     - parameter string
--
--  RETURNS:
--        1. boolean check status
--        2. number:vlanid or string:error_string if failed
--
-- ************************************************************************
function check_param_vlanid(param)
    local vlanid
    vlanid = tonumber(param)
    if type(vlanid) ~= "number" or vlanid < 0 or vlanid > 4095 then
        return false, "Wrong VLAN Id"
    end
    return true, vlanid
end

-- ************************************************************************
--  check_param_vlan
--
--  DESCRIPTION:
--        Check parameter that it is valid and configured vlan Id
--
--  INPUTS:
--        param     - parameter string
--
--  RETURNS:
--        1. boolean check status
--        2. number:vlanid or string:error_string if failed
--
-- ************************************************************************
function check_param_vlan(param)
    local f
    local vlan
    f, vlan = check_param_vlanid(param)
    if f == false then
        return f, vlan
    end
    local devlist
    devlist = wrlDevList()
    local k, dev
    for k, dev in pairs(devlist) do
        if wrlCpssDxChVlanExists(dev, vlan) then
            return true, vlan
        end
    end
    return false, string.format("VLAN %d not configured", vlan)
end


--
-- Create CLI mode: vlan_configuration
--
cmdLuaCLI_modes["vlan_confuguration"] = {
    prompt = "(config-vlan)#"
}

-- CLI_PRD 8.1
-- Add command to switch from configuration to vlan_configuration
--
cmdLuaCLI_addCommand("configuration", "vlan database", {
    func = cmdLuaCLI_change_mode_push,
    args = { "vlan_confuguration" }
})

-- CLI_PRD 8.15
-- implement "show vlan tag xxx"
function show_vlan_tag(vlan)
    local k, dev
    local s
    s = string.format("%-10d", vlan)
    for k, dev in pairs(wrlDevList()) do
        local vlanInfo
        vlanInfo = wrlCpssDxChGetVlanInfo(dev, vlan)
        if type(vlanInfo) ~= "nil" and #(vlanInfo.portsMembers) then
            s = s .. string.format(" %d/", dev)
            s = s .. list_to_string(vlanInfo.portsMembers)
        end
    end
    print(s)
end
-- register "show vlan tag xxx"
cmdLuaCLI_addCommand("vlan_confuguration", "show vlan tag", {
    func = show_vlan_tag,
    types = { "vlan" }
})


-- CLI_PRD 8.15
-- implement "show vlan"
function show_vlan()
    -- merge vlan list for all devices
    local vlanlist
    vlanlist = wrlCpssDxChVlanList(0)
    --
    local k, vlan
    for k, vlan in pairs(vlanlist) do
        show_vlan_tag(vlan)
    end
end
-- register "show vlan"
cmdLuaCLI_addCommand("vlan_confuguration", "show vlan", {
    func = show_vlan
})


-- implement "set vlan entry _vlanid_ _device_/_port_range_ [_device_/_port_range_ ...]"
function set_vlan_entry(vlanid,...)
    local l = {...}
    local i
    for i = 1,#l do
        local s,e,dev,rc,vlaninfo
        s,e,dev=string.find(l[i],"(%d+)/")
        if type(s) == "nil" then
            print("Wrong ports list: ( " .. l[i] .. " ): device number required")
            break
        end
        dev = tonumber(dev)
        if wrlIsDevIdValid(dev) == false then
            print("device "..dev.." is not configured")
            break
        end
        vlaninfo = {}
        vlaninfo.portsMembers = string_to_list(string.sub(l[i],e+1))
        rc = wrlCpssDxChVlanEntrySet(dev,vlanid,vlaninfo)
        if rc ~= 0 then
            print("error " .. rc .. " writing vlan entry " .. vlanid .. " device " .. dev)
        end
    end 
end
-- register "set vlan entry _vlanid_ ..."
cmdLuaCLI_addCommand("vlan_confuguration", "set vlan entry", {
    func = set_vlan_entry,
    types = { "vlanid" }
})
