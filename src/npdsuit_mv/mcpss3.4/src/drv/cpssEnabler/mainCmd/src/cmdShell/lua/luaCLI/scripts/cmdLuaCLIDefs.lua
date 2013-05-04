--
-- Lua CLI common definitions
--

do -- cmdLuaCLI implementation

--
-- cmdLuaCLI_description = {
--    mode = { node [,node ...] }
--    ,mode = ...
-- }
--
-- node = {
--    keyword = "word",
--              keyword
--    sub = { node [,node ...] },
--              the list of nodes for next word in command
--    func = functionreference,
--              function pointer to execute command
--    args = { ... },
--              const arguments to be passed to functionreference()
--              before all other parameters
--    types = { typename [,typename ...] },
--              parameter typenames
--              the function check_param_TYPENAME called to check
--              parameter and possible convert it
--    help = string,
--    parseArgs = {
--       {
--           keyword = string,
--           help = string,
--           name = string,
--                parsed[name] = keyword
--           argtype = string,
--                parsed[keyword] = arg
--           arghelp = string,
--           mandatory = true,
--       } [, ....]
--    }
-- }
--
cmdLuaCLI_description = {}
cmdLuaCLI_mode = "all"

-- ************************************************************************
--  cmdLuaCLI_addCommand
--
--  DESCRIPTION:
--        Add one command specified as string
--
--  INPUTS:
--        mode     - parameter string
--        str      - command string
--        table    - node
--
--  RETURNS:
--        status
--
-- ************************************************************************
function cmdLuaCLI_addCommand(mode, str, node)
    if mode == nil then
        mode = "all"
    end
    local tbl = cmdLuaCLI_description[mode]
    if tbl == nil then
        cmdLuaCLI_description[mode] = {}
        tbl = cmdLuaCLI_description[mode]
    end
    local strArr = splitline(str)
    local k, n, current_node
    for k = 1, #strArr do
        current_node = nil
        for n = 1, #tbl do
            if tbl[n].keyword == strArr[k] then
                current_node = tbl[n]
            end
        end
        if current_node == nil then
            tbl[#tbl+1] = { }
            current_node = tbl[#tbl]
            current_node.keyword = strArr[k]
        end
        if k < #strArr then
            if current_node.sub == nil then
                current_node.sub={}
            end
            tbl = current_node.sub
        end
    end
    for k,v in pairs(node) do
        if k ~= "sub" then
            current_node[k] = v
        end
    end
end

-- ************************************************************************
--  cmdLuaCLI_addCommands
--
--  DESCRIPTION:
--        Add commands (merge tree)
--
--  INPUTS:
--        tree    - tree to merge
--
--  RETURNS:
--        status
--
-- ************************************************************************
function cmdLuaCLI_addCommands(tree)
    local mode,nodes
    for mode,nodes in pairs(tree) do
        -- top level
        local t = cmdLuaCLI_description[mode]
        if t == nil then
            cmdLuaCLI_description[mode] = {}
            t = cmdLuaCLI_description[mode]
        end
        local function merge_nodes(nodes, tree)
            if type(nodes) ~= "table" then
                return
            end
            local k,t
            for n = 1, #nodes do
                local merged = false
                if type(nodes[n].keyword) == "string" then
                    for t = 1, #tree do
                        if nodes[n].keyword == tree[t].keyword then
                            local attr,value
                            for attr,value in pairs(nodes[n]) do
                                if attr ~= "sub" then
                                    tree[t][attr] = value
                                else
                                    if type(attr) == "table" then
                                        if type(tree[t].sub) ~= "table" then
                                            tree[t].sub = value
                                        else
                                            merge_nodes(value,tree[t].sub)
                                        end
                                    end
                                end
                            end
                            merged = true
                        end
                    end
                    if not merged then
                        tree[#tree+1] = nodes[n]
                    end
                end
            end
        end
        merge_nodes(nodes, t)
    end
end

-- ************************************************************************
--  luaCLI_checkPerm
--
--  DESCRIPTION:
--        return permission for node
--
--  INPUTS:
--        none
--
--  RETURNS:
--        0 - denied
--        1 - permitted
--        2 - nothing
--
-- ************************************************************************
local function luaCLI_checkPerm(node)
    return 2;
end

-- ************************************************************************
--  luaCLI_find_node
--
--  DESCRIPTION:
--        find node
--
--  INPUTS:
--        strArr   - command string splitted into words
--
--  RETURNS:
--        node
--        nesting level
--        permitted
--
-- ************************************************************************
local function luaCLI_find_node(strArr)
    local function l_find_node(strArr,tbl)
        local k, n, current_node, found_node
        local permitted = 1
        found_node = nil
        found_level = 0
        if tbl == nil then
            return nil,0,permitted
        end
        for k = 1, #strArr do
            current_node = nil
            for n = 1, #tbl do
                if tbl[n].keyword == strArr[k] then
                    found_node = tbl[n]
                    found_level = k
                    current_node = tbl[n]
                    local p = luaCLI_checkPerm(current_node)
                    if p < 2 then
                        permitted = p
                    end
                end
            end
            if current_node == nil then
                return found_node,found_level,permitted
            end
            if k < #strArr then
                if current_node.sub == nil then
                    return found_node,found_level,permitted
                end
                tbl = current_node.sub
            end
        end
        return found_node,found_level,permitted
    end
    local tbl = cmdLuaCLI_description[cmdLuaCLI_mode]
    local level, node = nil
    if type(tbl) ~= "nil" then
        node,level,permitted = l_find_node(strArr,tbl)
    end
    if node == nil then
        node,level,permitted = l_find_node(strArr,cmdLuaCLI_description["all"])
    end
    return node,level,permitted
end

-- ************************************************************************
--  luaCLI_ac_match
--
--  DESCRIPTION:
--        add keyword to list if it match s
--
--  INPUTS:
--        s   - command string splitted into words
--
--  RETURNS:
--        bool
--
-- ************************************************************************
local function luaCLI_ac_match(s, keyword, matched)
    if string.sub(keyword, 1, string.len(s)) == s then
        matched[#matched+1] = keyword
        return true
    end
    return false
end

-- ************************************************************************
--  luaCLI_ac_complete
--
--  DESCRIPTION:
--        return word to completeist if it match s
--
--  INPUTS:
--        s   - command string splitted into words
--
--  RETURNS:
--        bool
--
-- ************************************************************************
local function luaCLI_ac_complete(s, matched)
    local n
    local compl = matched[1]
    if #matched == 1 then
        compl = compl .. " "
    end
    for n = 2, #matched do
        if string.len(matched[n]) < string.len(compl) then
            compl = string.sub(compl, 1, string.len(matched[n]))
        end
        while compl ~= "" do
            if string.sub(matched[n], 1, string.len(compl)) ~= compl then
                compl = string.sub(compl, 1, string.len(compl)-1)
            else
                break
            end
        end
    end
    return string.sub(compl,string.len(s)+1)
end

-- ************************************************************************
--  luaCLI_parse_parameters
--
--  DESCRIPTION:
--        parse parameters (see parseArgs field)
--
--  INPUTS:
--        
--
--  RETURNS:
--        status   "ok", etc
--        keyword  nil for ok | keyword for "noarg" or "badarg"
--        number of args parsed
--        parsed   (table)
--        missing  { keyword = { }, name = { opts = {...} }
--
-- ************************************************************************
local function luaCLI_parse_parameters(strArr, firstIdx, parseArgs)
    local parsed = {}
    local missing = {}
    local o
    -- build missing
    for o = 1, #parseArgs do
        local p = parseArgs[o]
        if type(p.name) == "string" then
            if type(missing[p.name]) ~= "table" then
                missing[p.name] = {}
                missing[p.name].opts = {}
            end
            table.insert(missing[p.name].opts, p)
            if p.mandatory then
                missing[p.name].mandatory = true
            end
        else
            missing[p.keyword] = p
        end
    end
    -- parse parameters
    last_opt = nil
    local i = 1
    while firstIdx + i <= #strArr do
        local found = false
        for o = 1, #parseArgs do
            local p = parseArgs[o]
            if strArr[firstIdx + i] == p.keyword then
                found = true
                i = i + 1
                if type(p.name) == "string" then
                    if type(parsed[p.name]) == "string" then
                        return "wrong option: \""..p.keyword.."\" - \""..p.name.."\" already defined as \""..parsed[p.name].."\"",
                            nil, i-1, parsed, missing,last_opt
                    end
                    parsed[p.name] = p.keyword
                    missing[p.name] = nil
                else
                    if type(parsed[p.keyword]) ~= "nil" then
                        return "wrong option: \""..p.keyword.."\" - already defined",
                            nil, i-1, parsed, missing,last_opt
                    end
                    missing[p.keyword] = nil
                    parsed[p.keyword] = true
                end
                last_opt = p
                if type(p.argtype) == "string" then
                    -- parse parameter arg
                    if firstIdx + i > #strArr then
                        return "noarg", p.keyword, i-1, parsed, missing,last_opt
                    end
                    local fn = "check_param_"..p.argtype
                    local arg =  strArr[firstIdx+i]
                    if _G[fn] == nil then
                        parsed[p.keyword] = arg
                    else
                        if type(_G[fn]) ~= "function" then
                            return "** "..fn.." is not a function",
                                nil, i-1, parsed, missing,last_opt
                        else
                            -- call check_param_xxx function
                            lstatus,status,v = pcall(_G[fn], arg)
                            if not lstatus then
                                return "** error while executing "..fn.."(\""..arg.."\"): "..status,
                                    nil, i-1, parsed, missing,last_opt
                            end
                            if not status then
                                return "bad parameter for "..p.keyword..": "..v,
                                    p.keyword, i-1, parsed, missing
                            end
                            parsed[p.keyword] = v
                        end
                    end
                    i = i + 1
                end

            end
        end
        if not found then
            break
        end
    end
    return "ok", nil, i-1, parsed, missing,last_opt
end

-- ************************************************************************
--  cmdLuaCLI_complete
--
--  DESCRIPTION:
--        autocomplete
--
--  INPUTS:
--        str     - line to complete
--
--  RETURNS:
--        string  - string for completion
--        array   - array of altenatives
--
-- ************************************************************************
function cmdLuaCLI_complete(str)
    local strArr = splitline(str.."x")
    local s = strArr[#strArr]
    s = string.sub(s, 1, string.len(s) - 1)
    strArr[#strArr] = s
    local node, level
    table.remove(strArr) --
    node,level = luaCLI_find_node(strArr)
    strArr[#strArr+1] = s -- 
    if level == 0 and #strArr > 1 then
        return "",{}
    end
    local matched = {}
    local function lll(s,matched,tbl)
        local n
        if type(tbl) ~= "table" then
            return
        end
        for n = 1, #tbl do
            luaCLI_ac_match(s, tbl[n].keyword, matched)
        end
    end
    if level == 0 then
        lll(s,matched,cmdLuaCLI_description["all"])
        lll(s,matched,cmdLuaCLI_description[cmdLuaCLI_mode])
    else
        if level + 1 == #strArr and type(node.sub) == "table" then
            lll(s,matched,node.sub)
        else
            if type(node.parseArgs) == "table" then
                -- TODO: apply parameters autocompl
                local status, keyword, nargs, parsed, missing
                status, keyword, nargs, parsed, missing = luaCLI_parse_parameters(strArr, level, node.parseArgs)
                if status ~= "ok" and keyword == nil then
                    print("** error parsing parameters: "..status)
                    return
                end
                if level + nargs + 1 == #strArr then
                    local k,v
                    for k,v in pairs(missing) do
                        if type(v.opts) == "table" then
                            for o = 1, #v.opts do
                                luaCLI_ac_match(s,v.opts[o].keyword, matched)
                            end
                        else
                            luaCLI_ac_match(s,k, matched)
                        end
                    end
                end
            end
        end
    end
    if #matched == 0 then
        return "",matched
    end
    return luaCLI_ac_complete(s, matched), matched
end

-- ************************************************************************
--  luaCLI_check_types
--
--  DESCRIPTION:
--        check parameter types
--
--  INPUTS:
--        strArr
--        firstIdx
--        types
--        args
--
--  RETURNS:
--        number of parameters parsed
--
-- ************************************************************************
local function luaCLI_check_types(strArr, firstIdx, types, args)
    local n
    for n = 1, #types do
        if type(types[n]) ~= "string" then
            print(string.format("** Wrong item %d in command_description.types[], must be string",n))
            return n
        end
        if firstIdx + n > #strArr then
            print(string.format("** Parameter %d of type ",n)..types[n].." missing")
            return n
        end
        local fn = "check_param_"..types[n]
        if _G[fn] == nil then
            args[#args+1] = strArr[firstIdx+n]
        else
            if type(_G[fn]) ~= "function" then
                print("** "..fn.." is not a function")
                return n
            else
                -- call check_param_xxx function
                lstatus,status,v = pcall(_G[fn], strArr[firstIdx+n])
                if not lstatus then
                    print("** error while executing "..fn..": "..status)
                    return n
                end
                if not status then
                    print("** "..fn..string.format(" returned failure for parameter %d: ",n)..v)
                    return n
                end
                args[#args+1] = v
            end
        end

    end
    return #types
end

-- ************************************************************************
--  cmdLuaCLI_execute
--
--  DESCRIPTION:
--        execute command
--
--  INPUTS:
--        str     - line to complete
--
--  RETURNS:
--        None
--
-- ************************************************************************
function cmdLuaCLI_execute(str)
    local strArr = splitline(str)
    local node, level
    node,level,permitted = luaCLI_find_node(strArr)
    if type(node) ~= "table" then
        print("** command not found: "..str)
        return
    end
    if permitted == 0 then
        print("** Can't execute command: permission denied")
        return
    end
    if type(node.func) ~= "function" then
        print("** No executor for command: "..table.concat(strArr," ",1,level))
        return
    end
    local n,lstatus,status,v
    -- do parametrized values
    local args = {}
    if type(node.args) == "table" then
        for n = 1, #node.args do
            args[#args+1] = node.args[n]
        end
    end
    -- parse options
    if type(node.parseArgs) == "table" then
        local status, keyword, nargs, parsed, missing
        local k,v
        status, keyword, nargs, parsed, missing = luaCLI_parse_parameters(strArr, level, node.parseArgs)
        if status ~= "ok" then
            if status == "noarg" then
                status = "argument required for \""..keyword.."\" option"
            end
            print("** error parsing parameters: "..status)
            return
        end
        -- check missing mandatory parameters
        local failed = false
        for k, v in pairs(missing) do
            if v.mandatory then
                print("** a mandatory option "..k.." is missing")
                if type(v.opts) == "table" then
                    print("** should be one of:")
                    local i
                    for i = 1, #v.opts do
                        print("**     "..v.opts[i].keyword)
                    end
                end
                failed = true
            end
        end
        if failed then
            return
        end

        level = level + nargs
        args[#args+1] = parsed
    end
    --check parameters
    n = 1
    if type(node.types) == "table" then
        n = luaCLI_check_types(strArr, level, node.types, args)
    end
    -- add the rest of parameters
    for n = level + n, #strArr do
        args[#args+1] = strArr[n]
    end
    lstatus, v = pcall(node.func,unpack(args))
    if not lstatus then
        print("** error while executing: "..v)
    end
end

-- ************************************************************************
--  cmdLuaCLI_help
--
--  DESCRIPTION:
--        get help for partialy completed command
--
--  INPUTS:
--        str     - line to complete
--
--  RETURNS:
--        None
--
-- ************************************************************************
function cmdLuaCLI_help(str)
    local strArr = splitline(str.."x")
    local s = strArr[#strArr]
    s = string.sub(s, 1, string.len(s) - 1)
    strArr[#strArr] = s
    local node, level
    table.remove(strArr) --
    node,level = luaCLI_find_node(strArr)
    strArr[#strArr+1] = s -- 
    if level == 0 and #strArr > 1 then
        return ""
    end
    local matched = {}
    local function luaCLI_help_match(s, keyword, help, matched)
        if string.sub(keyword, 1, string.len(s)) == s then
            matched[#matched+1] = { }
            matched[#matched].keyword = keyword
            matched[#matched].help = help
            return true
        end
        return false
    end
    local function lll(s,matched,tbl)
        local n
        if type(tbl) ~= "table" then
            return
        end
        for n = 1, #tbl do
            luaCLI_help_match(s, tbl[n].keyword, tbl[n].help, matched)
        end
    end
    if level == 0 then
        lll(s,matched,cmdLuaCLI_description["all"])
        lll(s,matched,cmdLuaCLI_description[cmdLuaCLI_mode])
    else
        if level + 1 == #strArr and type(node.sub) == "table" then
            lll(s,matched,node.sub)
        else
            if type(node.parseArgs) == "table" then
                -- TODO: apply parameters autocompl
                local status, keyword, nargs, parsed, missing
                status, keyword, nargs, parsed, missing, last_opt = luaCLI_parse_parameters(strArr, level, node.parseArgs)
                if status ~= "ok" and keyword == nil then
                    print("** error parsing parameters: "..status)
                    return
                end
                if status == "noarg" and keyword ~= nil then
                    luaCLI_help_match(s, keyword, last_opt.help, matched)
                else
                    if level + nargs == #strArr and type(last_opt) == "table" then
                        if type(last_opt.argtype) == "string" and type(last_opt.arghelp) == "string" then
                            luaCLI_help_match(last_opt.arghelp, last_opt.arghelp, nil, matched)
                        else
                            luaCLI_help_match(s, last_opt.keyword, last_opt.help, matched)
                        end
                    else
                        if level + nargs + 1 == #strArr then
                            local k,v
                            for k,v in pairs(missing) do
                                if type(v.opts) == "table" then
                                    for o = 1, #v.opts do
                                        luaCLI_help_match(s, v.opts[o].keyword, v.opts[o].help, matched)
                                    end
                                else
                                    luaCLI_help_match(s, k, v.help, matched)
                                end
                            end
                        end
                    end
                end
            end
        end
    end
    help = ""
    for n = 1, #matched do
        if n > 1 then
            help = help .. "\n"
        end
        help = help .. "\t" .. matched[n].keyword
        if type(matched[n].help) == "string" then
            help = help .. "\t" .. matched[n].help
        end
    end
    return help
end

end -- cmdLuaCLI implementation








-- ************************************************************************
--  check_param_number
--
--  DESCRIPTION:
--        Check parameter that it is number
--
--  INPUTS:
--        param     - parameter string
--
--  RETURNS:
--        1. boolean check status
--        2. number:value or string:error_string if failed
--
-- ************************************************************************
function check_param_number(param)
    param = tonumber(param)
    if param == nil then
        return false, "not a number"
    end
    return true, param
end

--
-- Create 3 default CLI modes:
--    "", "exec", "configuration"
--
cmdLuaCLI_modes = {
    [""] = {
        prompt = ">"
    },
    ["exec"] = {
        prompt = "#"
    },
    ["configuration"] = {
        prompt = "(config)#"
    }
}

--
-- Mode change functions
--
function cmdLuaCLI_change_mode(mode)
    local f
    f = _G["cmdLuaCLI_mode_" .. cmdLuaCLI_mode .. "_exit"]
    if type(f) == "function" then
        pcall(f)
    end
    cmdLuaCLI_mode = mode
    if type(cmdLuaCLI_modes[mode]) == "table" then
        cmdLuaCLI_prompt = "Console" .. cmdLuaCLI_modes[mode].prompt .. " "
    end
    f = _G["cmdLuaCLI_mode_" .. cmdLuaCLI_mode .. "_enter"]
    if type(f) == "function" then
        pcall(f)
    end
end

cmdLuaCLI_mode_stack = {}
cmdLuaCLI_mode_stackptr = 0

function cmdLuaCLI_change_mode_push(mode)
    cmdLuaCLI_mode_stackptr = cmdLuaCLI_mode_stackptr + 1
    cmdLuaCLI_mode_stack[cmdLuaCLI_mode_stackptr] = cmdLuaCLI_mode
    cmdLuaCLI_change_mode(mode)
end

function cmdLuaCLI_change_mode_pop()
    if cmdLuaCLI_mode_stackptr < 1 then
        return
    end
    cmdLuaCLI_change_mode(cmdLuaCLI_mode_stack[cmdLuaCLI_mode_stackptr])
    cmdLuaCLI_mode_stackptr = cmdLuaCLI_mode_stackptr - 1
end

function cmdLuaCLI_change_mode_pop_all()
    if cmdLuaCLI_mode_stackptr > 0 then
        cmdLuaCLI_mode_stackptr = 1
        cmdLuaCLI_change_mode(cmdLuaCLI_mode_stack[1])
    else
        cmdLuaCLI_change_mode_push("")
    end

end


--
-- register mode change functions
--
cmdLuaCLI_addCommands({
  all={
    {
        keyword = "enable",
        func = cmdLuaCLI_change_mode_push,
        args = { "exec" }
    },
    {
        keyword = "disable",
        func = cmdLuaCLI_change_mode_pop_all
    },
    {
        keyword = "exit",
        func = cmdLuaCLI_change_mode_pop
    }
  },
  exec={
    {
        keyword = "configure",
        func = cmdLuaCLI_change_mode_push,
        args = { "configuration" }
    }
  }
})


-- ************************************************************************
--  cmdLua_to_string
--
--  DESCRIPTION:
--        Convert value to string
--        This function can be used to print complex tables
--
--  INPUTS:
--        value to convert  - parameter string
--        level             - nesting level (for indent)
--
--  RETURNS:
--        string
--
-- ************************************************************************
function cmdLuaCLI_to_string(val,level)
    if level == nil then
        level = 0
    end
    if val == nil then
        return "nil"
    elseif type(val) == "boolean" then
        if val then
            return "true"
        end
        return "false"
    elseif type(val) == "number" then
        return tostring(val)
    elseif type(val) == "string" then
        return string.format("%q",val)
    elseif type(val) == "table" then
        local s,k,v,not_first,recno
        s = "{"
        not_first = false
        recno = 0
        for k,v in pairs(val) do
            if not_first then s = s .. "," end
            not_first = true
            recno = recno + 1
            s = s .. "\n" .. string.rep("  ",level+1) 
            if type(k) == "string" and string.match(k,"%a[%a%d_]+") == k then
                s = s .. k .. "="
            elseif type(k) == "number" and k == recno then
                
            else
                s = s .. "[" .. cmdLuaCLI_to_string(k,level+1) .. "]="
            end
            s = s .. cmdLuaCLI_to_string(v,level+1)
        end
        s = s .. "\n" .. string.rep("  ",level) .. "}"
        return s
    elseif type(val) == "function" then
        return "<function>"
    elseif type(val) == "userdata" then
        return "<userdata>"
    elseif type(val) == "thread" then
        return "<thread>"
    else
        return "nil"
    end
end
cmdLuaCLI_addCommand(nil,"CLI show",{
    func = function ()
        print(cmdLuaCLI_to_string(cmdLuaCLI_description,0))
    end
})
