cmdLuaCLI_registerCfunction("mgmType_to_lua_GT_IPADDR")
cmdLuaCLI_registerCfunction("mgmType_to_c_GT_IPADDR")
cmdLuaCLI_registerCfunction("mgmType_to_lua_GT_IPV6ADDR")
cmdLuaCLI_registerCfunction("mgmType_to_c_GT_IPV6ADDR")
cmdLuaCLI_registerCfunction("mgmType_to_lua_CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC")
cmdLuaCLI_registerCfunction("mgmType_to_c_CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC")

function ipprefix_do_config_line(str)
    local ip, prefixlen, nexthop, result
    local re_ip4_addr, re_ipv6_addr, re_prefix_add, re_prefix_del, re_hex
    re_ipv4_addr = "%d+%.%d+%.%d+%.%d+"
    re_prefix_add = "%s+/%s*(%d+)%s+([^%s]+)%s*$"
    re_prefix_del = "%s+/%s*(%d+)%s*$"
    re_hex = "[0-9da-fA-F]+"
    re_ipv6_addr = string.rep(re_hex .. ":",7) .. re_hex
    ip, prefixlen, nexthop = string.match(str, "^+%s*("..re_ipv4_addr..")"..re_prefix_add)
    if ip ~= nil then
        result = cpssGenWrapper(
            "cpssExMxPmIpLpmIpv4UcPrefixAdd", {
                { "IN", "GT_U32",    "lpmDbId", 0 },
                { "IN", "GT_U32",    "vrId", 0 },
                { "IN", "GT_IPADDR", "ipAddr", ip },
                { "IN", "GT_U32",    "prefixLen", tonumber(prefixlen) },
                { "IN", "CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC", "nextHopPointer", {
                            routeEntryBaseMemAddr = tonumber(nexthop),
                            blockSize = 1,
                            routeEntryMethod = "CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E" } },
                { "IN", "GT_BOOL",   "override", false }
            })
        -- check result
        if type(result) == "string" then
            print("error in cpssExMxPmIpLpmIpv4UcPrefixAdd() call: " .. result)
        end

        return -- result
    end
    ip, prefixlen = string.match(str, "^-%s*("..re_ipv4_addr..")"..re_prefix_del)
    if ip ~= nil then
        result = cpssGenWrapper(
            "cpssExMxPmIpLpmIpv4UcPrefixDelete", {
                { "IN", "GT_U32",    "lpmDbId", 0 },
                { "IN", "GT_U32",    "vrId", 0 },
                { "IN", "GT_IPADDR", "ipAddr", ip },
                { "IN", "GT_U32",    "prefixLen", tonumber(prefixlen) }
            })
        -- check result
        if type(result) == "string" then
            print("error in cpssExMxPmIpLpmIpv4UcPrefixDelete() call: " .. result)
        end

        return -- result
    end
    ip, prefixlen, nexthop = string.match(str, "^+%s*("..re_ipv6_addr..")"..re_prefix_add)
    if ip ~= nil then
        result = cpssGenWrapper(
            "cpssExMxPmIpLpmIpv6UcPrefixAdd", {
                { "IN", "GT_U32",      "lpmDbId", 0 },
                { "IN", "GT_U32",      "vrId", 0 },
                { "IN", "GT_IPV6ADDR", "ipAddr", ip },
                { "IN", "GT_U32",      "prefixLen", tonumber(prefixlen) },
                { "IN", "CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC", "nextHopPointer", {
                            routeEntryBaseMemAddr = tonumber(nexthop),
                            blockSize = 1,
                            routeEntryMethod = "CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E" } },
                { "IN", "GT_BOOL",   "override", false }
            })
        -- check result
        if type(result) == "string" then
            print("error in cpssExMxPmIpLpmIpv6UcPrefixAdd() call: " .. result)
        end

        return -- result
    end
    ip, prefixlen = string.match(str, "^-%s*("..re_ipv6_addr..")"..re_prefix_del)
    if ip ~= nil then
        result = cpssGenWrapper(
            "cpssExMxPmIpLpmIpv6UcPrefixDelete", {
                { "IN", "GT_U32",      "lpmDbId", 0 },
                { "IN", "GT_U32",      "vrId", 0 },
                { "IN", "GT_IPV6ADDR", "ipAddr", ip },
                { "IN", "GT_U32",      "prefixLen", tonumber(prefixlen) }
            })
        -- check result
        if type(result) == "string" then
            print("error in cpssExMxPmIpLpmIpv6UcPrefixAdd() call: " .. result)
        end

        return -- result
    end
    print("wrong line: " .. str)
end

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
        ipprefix_do_config_line(str)
    end
    fs.close(fd)
    return true
end
