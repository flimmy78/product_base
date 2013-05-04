require("genwrapper_API")

-- Using generic lua wrapper
function readReg(regAddr)
    ret, values = cpssGenWrapper(
        "prvCpssDrvHwPpReadRegister", {
            { "IN", "GT_U8", 0 },
            { "IN", "GT_U32", regAddr },
            { "OUT","GT_U32" }
        })
    print("ret=",ret)
    if ret == 0 then
        print("regData=",values[3]);
    end
end

function writeReg(regAddr, regData)
    ret, values = cpssGenWrapper(
        "prvCpssDrvHwPpWriteRegister", {
            { "IN", "GT_U8", 0 },
            { "IN", "GT_U32", regAddr },
            { "IN", "GT_U32", regData }
        })
    print("ret=",ret)
end

cmdLuaCLI_addCommand(nil, "readReg", {
    func = readReg,
    types = { "number" }
})
cmdLuaCLI_addCommand(nil, "writeReg", {
    func = readReg,
    types = { "number", "number" }
})





-- Command implementation using generic CLI wrapper

cmdLuaCLI_addCommand(nil, "greadReg", {
    types = { "number" },
    func = cpssGenWrapperCLI,
    args = {
        "prvCpssDrvHwPpReadRegister",
        {
            { "IN", "GT_U8", "deviceId", "const", 0 },
            { "IN", "GT_U32","regAddr",  "param", 1 },
            { "OUT","GT_U32","regData" }
        }
    },
	help = "Read PP register.\nParameters:\n\tregAddr  - register address"
})

cmdLuaCLI_addCommand(nil, "gwriteReg", {
    types = { "number" },
    func = cpssGenWrapperCLI,
    args = {
        "prvCpssDrvHwPpWriteRegister",
        {
            { "IN", "GT_U8", "deviceId", "const", 0 },
            { "IN", "GT_U32","regAddr",  "param", 1 },
            { "IN", "GT_U32","regData",  "query", "Data:" }
        }
    }
})
