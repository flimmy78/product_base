--
-- Lua CLI common definitions
--

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
    {
        mode = "",
        syn = "enable",
        func = cmdLuaCLI_change_mode_push,
        args = { "exec" }
    },
    {
        syn = "disable",
        func = cmdLuaCLI_change_mode_pop_all
    },
    {
        syn = "exit",
        func = cmdLuaCLI_change_mode_pop
    },
    {
        mode = "exec",
        syn = "configure",
        func = cmdLuaCLI_change_mode_push,
        args = { "configuration" }
    }
})
