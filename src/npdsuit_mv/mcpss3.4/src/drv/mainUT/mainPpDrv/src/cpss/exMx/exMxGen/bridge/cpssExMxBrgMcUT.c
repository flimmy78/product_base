/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxBrgMcUT.c
*
* DESCRIPTION:
*       Unit tests for cpssExMxBrgMc.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/
/* includes */
#include <cpss/exMx/exMxGen/config/private/prvCpssExMxInfo.h>
#include <cpss/exMx/exMxGen/bridge/cpssExMxBrgMc.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* Tests use this vidx as default value for testing     */
#define BRG_MC_TESTED_VIDX_CNS          50

/* Tests use this value as default valid value for port */
#define BRG_MC_VALID_PHY_PORT_CNS       0


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgMcEntryWrite
(
    IN GT_U8                devNum,
    IN GT_U16               vidx,
    IN GT_BOOL              isCpuMember,
    IN CPSS_PORTS_BMP_STC   *portBitmapPtr
);
*/
UTF_TEST_CASE_MAC(cpssExMxBrgMcEntryWrite)
{
/*
ITERATE_DEVICES
1.1. Call function with vidx [0], non-NULL portBitmapPtr [ports[0] = 0x000000AB, ports[1] = 0xFF0000FF] and isCpuMember [GT_TRUE]. Expected: GT_OK.
1.2. Call cpssExMxBrgMcEntryRead with vidx [0], non-NULL portBitmapPtr and isCpuMemberPtr. Expected: GT_OK and portBitmapPtr [ports[0] =0x000000AB, ports[1] = 0xFF0000FF] and isCpuMemberPtr [GT_TRUE].
1.3. Call function with vidx [0], non-NULL portBitmapPtr [ports[0] = 0xFFFFFF00, ports[1] = 0x00000001] and isCpuMember [GT_FALSE]. Expected: GT_OK.
1.4. Call cpssExMxBrgMcEntryRead with vidx [0], non-NULL portBitmapPtr and isCpuMemberPtr. Expected: GT_OK and portBitmapPtr [ports[0] = 0xFFFFFF00, ports[1] = 0x00000001] and isCpuMemberPtr [GT_FALSE].
1.5. Call function with vidx [maxMcGroups - 1], non-NULL portBitmapPtr [ports[0] = 0x000000AB, ports[1] = 0xFF0000FF] and isCpuMember [GT_TRUE]. Expected: GT_OK.
1.6. Call function with out of range vidx [maxMcGroups], isCpuMember [GT_FALSE], non-NULL portBitmapPtr. Expected: NON GT_OK.
1.7. Call function with portBitmapPtr [NULL], vidx [0], isCpuMember [GT_TRUE]. Expected: GT_BAD_PTR.
*/
    GT_STATUS           st = GT_OK;
    GT_U8               dev;
    GT_U16              vidx;
    GT_BOOL             isCpuMember;
    CPSS_PORTS_BMP_STC  portBitmap;
    GT_U8               numVirtualPorts;


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        GT_BOOL             isCpuMemberWritten;
        CPSS_PORTS_BMP_STC  portBitmapWritten;
        GT_U16              maxMcGroups;

        st = prvUtfNumVirtualPortsGet(dev,&numVirtualPorts);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        st = prvUtfMaxMcGroupsGet(dev, &maxMcGroups);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. Call function with vidx [0], non-NULL portBitmapPtr                                    */
        /* [ports[0] = 0x000000AB, ports[1] = 0xFF0000FF] and isCpuMember [GT_TRUE].                   */
        /* Expected: GT_OK.                                                                            */
        vidx = BRG_MC_TESTED_VIDX_CNS;
        portBitmap.ports[0] = 0x000000AB;
        if(numVirtualPorts > 32)
        {
            portBitmap.ports[1] = 0x000000FF;
        }
        else
        {
            portBitmap.ports[1] = 0;/* don't care ! */
        }
        isCpuMember = GT_TRUE;

        st = cpssExMxBrgMcEntryWrite(dev, vidx, isCpuMember, &portBitmap);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vidx, isCpuMember);

        /* 1.2. Call cpssExMxBrgMcEntryRead with vidx [0], non-NULL portBitmapPtr and isCpuMemberPtr    */
        /* Expected: GT_OK and portBitmapPtr [ports[0] =0x000000AB, ports[1] = 0xFF0000FF]              */
        /* and isCpuMemberPtr [GT_TRUE].                                                                */
        st = cpssExMxBrgMcEntryRead(dev, vidx, &isCpuMemberWritten, &portBitmapWritten);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxBrgMcEntryRead: %d, %d", dev, vidx);
        UTF_VERIFY_EQUAL3_PARAM_MAC(portBitmap.ports[0], portBitmapWritten.ports[0], dev, vidx, isCpuMember);
        if(numVirtualPorts > 32)
        {
            UTF_VERIFY_EQUAL3_PARAM_MAC(portBitmap.ports[1], portBitmapWritten.ports[1], dev, vidx, isCpuMember);
        }
        UTF_VERIFY_EQUAL3_PARAM_MAC(isCpuMember, isCpuMemberWritten, dev, vidx, isCpuMember);

        /* 1.3. Call function with vidx [0], non-NULL portBitmapPtr                                     */
        /* [ports[0] = 0xFFFFFF00, ports[1] = 0x00000001] and isCpuMember [GT_FALSE].                   */
        /* Expected: GT_OK.                                                                             */
        vidx = BRG_MC_TESTED_VIDX_CNS;
        portBitmap.ports[0] = 0xFFFFFF00;
        if(numVirtualPorts > 32)
        {
            portBitmap.ports[1] = 0x00000001;
        }
        isCpuMember = GT_FALSE;

        st = cpssExMxBrgMcEntryWrite(dev, vidx, isCpuMember, &portBitmap);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vidx, isCpuMember);

        /* 1.4. Call cpssExMxBrgMcEntryRead with vidx [0], non-NULL portBitmapPtr and isCpuMemberPtr.   */
        /* Expected: GT_OK and portBitmapPtr [ports[0] = 0xFFFFFF00, ports[1] = 0x00000001]             */
        /* and isCpuMemberPtr [GT_FALSE].                                                               */
        st = cpssExMxBrgMcEntryRead(dev, vidx, &isCpuMemberWritten, &portBitmapWritten);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxBrgMcEntryRead: %d, %d", dev, vidx);
        UTF_VERIFY_EQUAL3_PARAM_MAC(portBitmap.ports[0], portBitmapWritten.ports[0], dev, vidx, isCpuMember);
        if(numVirtualPorts > 32)
        {
            UTF_VERIFY_EQUAL3_PARAM_MAC(portBitmap.ports[1], portBitmapWritten.ports[1], dev, vidx, isCpuMember);
        }
        UTF_VERIFY_EQUAL3_PARAM_MAC(isCpuMember, isCpuMemberWritten, dev, vidx, isCpuMember);

        /* 1.5. Call function with vidx [maxMcGroups - 1], non-NULL portBitmapPtr                       */
        /* [ports[0] = 0x000000AB, ports[1] = 0xFF0000FF] and isCpuMember [GT_TRUE].                    */
        /* Expected: GT_OK.                                                                             */
        vidx = maxMcGroups - 1;
        portBitmap.ports[0] = 0x000000AB;
        if(numVirtualPorts > 32)
        {
            portBitmap.ports[1] = 0x000000FF;
        }
        isCpuMember = GT_TRUE;

        st = cpssExMxBrgMcEntryWrite(dev, vidx, isCpuMember, &portBitmap);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vidx, isCpuMember);

        /* 1.6. Call function with out of range vidx [maxMcGroups], isCpuMember [GT_FALSE],             */
        /* non-NULL portBitmapPtr.                                                                      */
        /* Expected: NON GT_OK.                                                                         */
        vidx = maxMcGroups;
        portBitmap.ports[0] = 0x000000AB;
        if(numVirtualPorts > 32)
        {
            portBitmap.ports[1] = 0x000000FF;
        }
        isCpuMember = GT_FALSE;

        st = cpssExMxBrgMcEntryWrite(dev, vidx, isCpuMember, &portBitmap);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, vidx, isCpuMember);

        /* 1.7. Call function with portBitmapPtr [NULL], vidx [0], isCpuMember [GT_TRUE].               */
        /* Expected: GT_BAD_PTR.                                                                        */
        vidx = BRG_MC_TESTED_VIDX_CNS;
        isCpuMember = GT_TRUE;

        st = cpssExMxBrgMcEntryWrite(dev, vidx, isCpuMember, NULL);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PTR, st, "%d, %d, %d, NULL", dev, vidx, isCpuMember);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    vidx = BRG_MC_TESTED_VIDX_CNS;
    portBitmap.ports[0] = 0xFFFFFF00;
    portBitmap.ports[1] = 0x00000001;
    isCpuMember = GT_FALSE;

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxBrgMcEntryWrite(dev, vidx, isCpuMember, &portBitmap);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgMcEntryWrite(dev, vidx, isCpuMember, &portBitmap);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgMcMemberAdd
(
    IN GT_U8    devNum,
    IN GT_U16   vidx,
    IN GT_U8    portNum
);
*/
UTF_TEST_CASE_MAC(cpssExMxBrgMcMemberAdd)
{
/*
ITERATE_DEVICES_PHY_CPU_PORTS
1.1.1. Call function with vidx [0]. Expected: GT_OK.
1.1.2. Call function with vidx [maxMcGroups - 1]. Expected: GT_OK.
1.1.3. Call function with out of range vidx [maxMcGroups]. Expected: NON GT_OK.
*/
    GT_STATUS st = GT_OK;

    GT_U8       dev;
    GT_U8       port;
    IN GT_U16   vidx;


    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            GT_U16  maxMcGroups;

            st = prvUtfMaxMcGroupsGet(dev, &maxMcGroups);
            UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

            /* 1.1.1. Call function with vidx [0].                          */
            /* Expected: GT_OK.                                             */
            vidx = BRG_MC_TESTED_VIDX_CNS;

            st = cpssExMxBrgMcMemberAdd(dev, vidx, port);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vidx, port);

            /* 1.1.2. Call function with vidx [maxMcGroups - 1].            */
            /* Expected: GT_OK.                                             */
            vidx = maxMcGroups - 1;

            st = cpssExMxBrgMcMemberAdd(dev, vidx, port);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vidx, port);

            /* 1.1.3. Call function with out of range vidx [maxMcGroups].   */
            /* Expected: NON GT_OK.                                         */
            vidx = maxMcGroups;

            st = cpssExMxBrgMcMemberAdd(dev, vidx, port);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, vidx, port);
        }

        /* 1.2. Check that function handles CPU port    */
        port = CPSS_CPU_PORT_NUM_CNS;

        /* 1.2.1. <Check other parameters for each active device id and CPU port>. */
        vidx = BRG_MC_TESTED_VIDX_CNS;

        st = cpssExMxBrgMcMemberAdd(dev, vidx, port);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vidx, port);

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.3. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.3.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssExMxBrgMcMemberAdd(dev, vidx, port);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.4. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxBrgMcMemberAdd(dev, vidx, port);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = BRG_MC_VALID_PHY_PORT_CNS;
    vidx = BRG_MC_TESTED_VIDX_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxBrgMcMemberAdd(dev, vidx, port);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgMcMemberAdd(dev, vidx, port);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgMcMemberDelete
(
    IN GT_U8          devNum,
    IN GT_U16         vidx,
    IN GT_U8          portNum
);
*/
UTF_TEST_CASE_MAC(cpssExMxBrgMcMemberDelete)
{
/*
ITERATE_DEVICES_PHY_CPU_PORTS
1.1.1. Call function with vidx [0]. Expected: GT_OK.
1.1.2. Call function with vidx [maxMcGroups - 1]. Expected: GT_OK.
1.1.3. Call function with out of range vidx [maxMcGroups]. Expected: NON GT_OK.
*/
    GT_STATUS st = GT_OK;

    GT_U8       dev;
    GT_U8       port;
    GT_U16      vidx;


    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            GT_U16  maxMcGroups;

            st = prvUtfMaxMcGroupsGet(dev, &maxMcGroups);
            UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

            /* 1.1.1. Call function with vidx [0].                          */
            /* Expected: GT_OK.                                             */
            vidx = BRG_MC_TESTED_VIDX_CNS;

            st = cpssExMxBrgMcMemberDelete(dev, vidx, port);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vidx, port);

            /* 1.1.2. Call function with vidx [maxMcGroups - 1].            */
            /* Expected: GT_OK.                                             */
            vidx = maxMcGroups - 1;

            st = cpssExMxBrgMcMemberDelete(dev, vidx, port);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vidx, port);

            /* 1.1.3. Call function with out of range vidx [maxMcGroups].   */
            /* Expected: NON GT_OK.                                         */
            vidx = maxMcGroups;

            st = cpssExMxBrgMcMemberDelete(dev, vidx, port);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, vidx, port);
        }

        /* 1.2. Check that function handles CPU port    */
        port = CPSS_CPU_PORT_NUM_CNS;

        /* 1.2.1. <Check other parameters for each active device id and CPU port>. */
        vidx = BRG_MC_TESTED_VIDX_CNS;

        st = cpssExMxBrgMcMemberDelete(dev, vidx, port);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vidx, port);

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.3. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.3.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssExMxBrgMcMemberDelete(dev, vidx, port);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.4. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxBrgMcMemberDelete(dev, vidx, port);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = BRG_MC_VALID_PHY_PORT_CNS;
    vidx = BRG_MC_TESTED_VIDX_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxBrgMcMemberDelete(dev, vidx, port);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgMcMemberDelete(dev, vidx, port);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgMcGroupDelete
(
    IN GT_U8    devNum,
    IN GT_U16   vidx
);
*/
UTF_TEST_CASE_MAC(cpssExMxBrgMcGroupDelete)
{
/*
ITERATE_DEVICES
1.1. Call function with vidx [0]. Expected: GT_OK.
1.2. Call function with vidx [maxMcGroups - 1]. Expected: GT_OK.
1.3. Call function with out of range vidx [maxMcGroups]. Expected: NON GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_U16      vidx;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        GT_U16  maxMcGroups;

        st = prvUtfMaxMcGroupsGet(dev, &maxMcGroups);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. Call function with vidx [0].                            */
        /* Expected: GT_OK.                                             */
        vidx = BRG_MC_TESTED_VIDX_CNS;

        st = cpssExMxBrgMcGroupDelete(dev, vidx);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, vidx);

        /* 1.2. Call function with vidx [maxMcGroups - 1].              */
        /* Expected: GT_OK.                                             */
        vidx = maxMcGroups - 1;

        st = cpssExMxBrgMcGroupDelete(dev, vidx);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, vidx);

        /* 1.3. Call function with out of range vidx [maxMcGroups].     */
        /* Expected: NON GT_OK.                                         */
        vidx = maxMcGroups;

        st = cpssExMxBrgMcGroupDelete(dev, vidx);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, vidx);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    vidx = BRG_MC_TESTED_VIDX_CNS;

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxBrgMcGroupDelete(dev, vidx);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgMcGroupDelete(dev, vidx);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgMcEntryRead
(
    IN  GT_U8                   devNum,
    IN  GT_U16                  vidx,
    OUT GT_BOOL                 *isCpuMemberPtr,
    OUT CPSS_PORTS_BMP_STC      *portBitmapPtr
);
*/
UTF_TEST_CASE_MAC(cpssExMxBrgMcEntryRead)
{
/*
ITERATE_DEVICES
1.1. Call function with vidx [0], non-NULL portBitmapPtr and isCpuMemberPtr. Expected: GT_OK.
1.2. Call function with vidx [maxMcGroups - 1], non-NULL portBitmapPtr and isCpuMemberPtr. Expected: GT_OK.
1.3. Call function with out of range vidx [maxMcGroups] and non-NULL portBitmapPtr and isCpuMemberPtr. Expected: NON GT_OK.
1.4. Call function with portBitmapPtr [NULL], vidx [0], non-NULL isCpuMemberPtr. Expected: GT_BAD_PTR.
1.5. Call function with isCpuMemberPtr [NULL], vidx [0], non-NULL portBitmapPtr. Expected: GT_BAD_PTR.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_U16                  vidx;
    GT_BOOL                 isCpuMember;
    CPSS_PORTS_BMP_STC      portBitmap;


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        GT_U16  maxMcGroups;

        st = prvUtfMaxMcGroupsGet(dev, &maxMcGroups);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. Call function with vidx [0], non-NULL portBitmapPtr and isCpuMemberPtr.     */
        /* Expected: GT_OK.                                                                 */
        vidx = BRG_MC_TESTED_VIDX_CNS;

        st = cpssExMxBrgMcEntryRead(dev, vidx, &isCpuMember, &portBitmap);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, vidx);

        /* 1.2. Call function with vidx [maxMcGroups - 1],                                  */
        /* non-NULL portBitmapPtr and isCpuMemberPtr.                                       */
        /* Expected: GT_OK.                                                                 */
        vidx = maxMcGroups - 1;

        st = cpssExMxBrgMcEntryRead(dev, vidx, &isCpuMember, &portBitmap);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, vidx);

        /* 1.3. Call function with out of range vidx [maxMcGroups]                          */
        /* and non-NULL portBitmapPtr and isCpuMemberPtr.                                   */
        /* Expected: NON GT_OK.                                                             */
        vidx = maxMcGroups;

        st = cpssExMxBrgMcEntryRead(dev, vidx, &isCpuMember, &portBitmap);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, vidx);

        /* 1.4. Call function with portBitmapPtr [NULL], vidx [0], non-NULL isCpuMemberPtr  */
        /* Expected: GT_BAD_PTR.                                                            */
        vidx = BRG_MC_TESTED_VIDX_CNS;

        st = cpssExMxBrgMcEntryRead(dev, vidx, &isCpuMember, NULL);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PTR, st, "%d, %d, %d, NULL", dev, vidx, isCpuMember);

        /* 1.5. Call function with isCpuMemberPtr [NULL], vidx [0],                         */
        /* non-NULL portBitmapPtr.                                                          */
        /* Expected: GT_BAD_PTR.                                                            */
        st = cpssExMxBrgMcEntryRead(dev, vidx, NULL, &portBitmap);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, vidx);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    vidx = BRG_MC_TESTED_VIDX_CNS;

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxBrgMcEntryRead(dev, vidx, &isCpuMember, &portBitmap);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgMcEntryRead(dev, vidx, &isCpuMember, &portBitmap);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cpssExMxBrgMc suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxBrgMc)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgMcEntryWrite)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgMcMemberAdd)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgMcMemberDelete)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgMcGroupDelete)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgMcEntryRead)
UTF_SUIT_END_TESTS_MAC(cpssExMxBrgMc)

