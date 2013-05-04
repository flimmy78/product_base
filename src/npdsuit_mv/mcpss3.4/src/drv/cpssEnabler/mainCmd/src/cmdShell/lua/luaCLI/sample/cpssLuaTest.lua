cmdLuaCLI_registerCfunction("cpssLuaTest_Hello")

cmdLuaCLI_addCommands({
    {
        syn = "cpss",
        sub = {
            {
                syn = "test",
                func = cpssLuaTest_Hello
            },
        }
    }
})
