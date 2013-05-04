/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* trafficDemoUt.c
*
* DESCRIPTION:
*       demo to test check the UT traffic engine capabilities
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#include <stdarg.h>

#include <gtOs/gtOsMem.h>

#include <utf/private/prvUtfExtras.h>
#include <utf/utfMain.h>

#include <trafficEngine/trafficTableUt.h>
#include <trafficEngine/trafficEngineUt.h>
#include <trafficEngine/testTypesUt.h>
#include <trafficEngine/trafficGeneratorUt.h>
#include <appDemo/utils/appDemoFdbUpdateLock.h>

/*flag to force test to fail for the demo */
static GT_U32   forceTestFailForDemo = 0;

/* name of current test */
char utTestCurrentTestName[256]={0};
/* state of current test */
GT_U32  utTestCurrentTestFail = 0;

static GT_U32   demoInitDone = 0;
/* this is IPv4 data take from test of :
   Bug #87117 01.05 Tunnel Start: Ipv4 over MPLS - Basic*/
static GT_U8    utTestPacketPayLoadIpv4Bytes[UT_TEST_BUFFER_MAX_SIZE_CNS]={
 0x45, 0x00 ,0x00 ,0x2A ,0x00 ,0x00 ,0x00 ,0x00 ,0x40 ,0x04 ,0x76 ,0xCB ,0x01 ,0x01,
 0x01, 0x01 ,0x01 ,0x01 ,0x01 ,0x03 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00, 0x00, 0x00,
 0x00, 0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00
};

static UT_TRAFFIC_ENGINE_PACKET_L2_STC          utTestPacketL2Part={{0},{0}};
static UT_TRAFFIC_ENGINE_PACKET_ETHERTYPE_STC   utTestPacketEtherTypePart={UT_ETHERTYPE_0800_IPV4_TAG_CNS};
static UT_TRAFFIC_ENGINE_PACKET_VLAN_TAG_STC    utTestPacketVlanTagPart={UT_ETHERTYPE_8100_TAG_CNS,0,0,0};
static UT_TRAFFIC_ENGINE_PACKET_PAYLOAD_STC     utTestPacketPayLoadPart={UT_TEST_BUFFER_MAX_SIZE_CNS,utTestPacketPayLoadIpv4Bytes};

UT_TRAFFIC_ENGINE_PACKET_L2_STC          *utTestPacketL2PartPtr = &utTestPacketL2Part;
UT_TRAFFIC_ENGINE_PACKET_ETHERTYPE_STC   *utTestPacketEtherTypePartPtr = &utTestPacketEtherTypePart;
UT_TRAFFIC_ENGINE_PACKET_VLAN_TAG_STC    *utTestPacketVlanTagPartPtr = &utTestPacketVlanTagPart;
UT_TRAFFIC_ENGINE_PACKET_PAYLOAD_STC     *utTestPacketPayLoadPartPtr = &utTestPacketPayLoadPart;


static UT_PACKET_PART_STC utTestPacketIpv4VlanTaggedPartsArray[] ={
        {UT_PACKET_PART_L2_E,&utTestPacketL2Part},
        {UT_PACKET_PART_VLAN_TAG_E,&utTestPacketVlanTagPart},
        {UT_PACKET_PART_ETHERTYPE_E,&utTestPacketEtherTypePart},
        {UT_PACKET_PART_PAYLOAD_E,&utTestPacketPayLoadPart}
    };

/* number of parts in utTestPacketIpv4VlanTaggedPartsArray */
#define  UT_TEST_PACKET_IPV4_VLAN_TAGGED_NUM_PARTS   \
    sizeof(utTestPacketIpv4VlanTaggedPartsArray) /  \
    sizeof(utTestPacketIpv4VlanTaggedPartsArray[0])

static UT_PACKET_PART_STC utTestPacketIpv4VlanUnTaggedPartsArray[] ={
        {UT_PACKET_PART_L2_E,&utTestPacketL2Part},
        {UT_PACKET_PART_VLAN_TAG_E,&utTestPacketVlanTagPart},
        {UT_PACKET_PART_ETHERTYPE_E,&utTestPacketEtherTypePart},
        {UT_PACKET_PART_PAYLOAD_E,&utTestPacketPayLoadPart}
    };

/* number of parts in utTestPacketIpv4VlanUnTaggedPartsArray */
#define  UT_TEST_PACKET_IPV4_VLAN_UNTAGGED_NUM_PARTS   \
    sizeof(utTestPacketIpv4VlanUnTaggedPartsArray) /    \
    sizeof(utTestPacketIpv4VlanUnTaggedPartsArray[0])

static UT_TRAFFIC_ENGINE_PACKET_STC utTestPacketIpv4VlanTagged =
    UT_TEST_PACKET_IP_MAC(UT_TEST_BUFFER_MAX_SIZE_CNS,UT_TEST_PACKET_IPV4_VLAN_TAGGED_NUM_PARTS,utTestPacketIpv4VlanTaggedPartsArray);
static UT_TRAFFIC_ENGINE_PACKET_STC utTestPacketIpv4VlanUnTagged=
    UT_TEST_PACKET_IP_MAC(UT_TEST_BUFFER_MAX_SIZE_CNS,UT_TEST_PACKET_IPV4_VLAN_UNTAGGED_NUM_PARTS,utTestPacketIpv4VlanUnTaggedPartsArray);

/* pointer to predefined IPv4 packet with vlan tag */
UT_TRAFFIC_ENGINE_PACKET_STC   *utTestPacketIpv4VlanTaggedPtr = &utTestPacketIpv4VlanTagged;
/* pointer UT_TRAFFIC_ENGINE_PACKET_STC predefined IPv4 packet without vlan tag */
UT_TRAFFIC_ENGINE_PACKET_STC   *utTestPacketIpv4VlanUnTaggedPtr = &utTestPacketIpv4VlanUnTagged;

/* calc b percentage of a */
#define PERCENTEGE(a,b) ((a * b)/100)

/*******************************************************************************
*   utTestTitleTrace
*
* DESCRIPTION:
*       trace the title of the test and it's description
*
* INPUTS:
*       start - started/ended
*       testName - name of the test
*       testDescription - description of the test
*
* OUTPUTS:
*       None.
*
* RETURNS:
*
* COMMENTS:
*
*
*******************************************************************************/
void utTestTitleTrace
(
    IN GT_BOOL  start,
    IN char *   testName,
    IN char *   testDescription,
    IN ...
)
{
    char buff[2048];
    va_list args;
    GT_U32  strLen;

    if(start == GT_TRUE)
    {
        if(testName == NULL)
        {
            return;
        }

        strLen = osStrlen(testName);

        if(strLen > sizeof(utTestCurrentTestName)-1)
        {
            strLen = sizeof(utTestCurrentTestName)-1;
        }

        /* save name of last test for abnormal exit from the test */
        osMemCpy(utTestCurrentTestName,testName,strLen);
        utTestCurrentTestName[strLen] = 0;

        /* set test as 'did not fail' */
        utTestCurrentTestFail = 0;
    }
    else
    {
        if(testName == NULL)
        {
            /* take the name from the current running test */
            testName = utTestCurrentTestName;
        }
    }

    if(testDescription)
    {
        va_start(args, testDescription);
        vsprintf(buff, testDescription, args);
        va_end(args);
    }
    else
    {
        buff[0] = 0;
    }

    if(start == GT_TRUE)
    {
        osPrintf("test [%s] [%s] - description[%s] \n" ,
                                "started",
                                testName,
                                buff);
    }
    else
    {
        osPrintf("test [%s] [%s] [%s] \n" ,
                                "ended",
                                utTestCurrentTestFail ? "--FAILED--" : "PASSED",
                                testName);
    }
}

/*******************************************************************************
*   utTestPhaseTrace
*
* DESCRIPTION:
*       trace the phase number and format
*
* INPUTS:
*       format - format for printing.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*
* COMMENTS:
*
*
*******************************************************************************/
void utTestPhaseTrace
(
    IN GT_U32   phaseNum,
    IN char *   phaseFormat,
    IN ...
)
{
    char buff[2048];
    va_list args;

    va_start(args, phaseFormat);
    vsprintf(buff, phaseFormat, args);
    va_end(args);

    osPrintf("phase %d - %s \n" ,phaseNum,buff);
}

/*******************************************************************************
*   utTestExpectedResultsTrace
*
* DESCRIPTION:
*       trace the expected results
*
* INPUTS:
*       actual - actual value
*       expected - expected value
*       format - format for printing.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*
* COMMENTS:
*
*
*******************************************************************************/
void utTestExpectedResultsTrace
(
    IN GT_U32   actual,
    IN GT_U32   expected,
    IN char *   format,
    IN ...
)
{
    char buff[2048];
    va_list args;

    if(format)
    {
        va_start(args, format);
        vsprintf(buff, format, args);
        va_end(args);
    }
    else
    {
        buff[0] = 0;
    }

    osPrintf("expected result error: actual[0x%8.8x] expected[0x%8.8x] - %s \n" ,actual,expected,buff);
}


/*******************************************************************************
*   utTestExpectedResultCheck
*
* DESCRIPTION:
*       check the expected result against the actual results
*
* INPUTS:
*       fieldName - filed name (string)
*       actualResult   - actual Result
*       expectedResult - expected result
*       numCompares    - number of comparisons
*       compareParamsArray - (array of) comparisons info
*       extraInfo - extra info to print when error
*                   use value of UT_TEST_EXPECTED_EXTRA_INFO_UNUSED_CNS
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - compare failed , and test should fail on first fail
*       GT_BAD_PTR      - on NULL pointer
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS utTestExpectedResultCheck
(
    IN  char*                           fieldName,
    IN  GT_U32                          actualResult,
    IN  GT_U32                          expectedResult,
    IN  GT_U32                          numCompares,
    IN  UT_TEST_COMPARE_PARAM_STC       compareParamsArray[],
    IN  GT_U32                          extraInfo
)
{
    GT_U32  ii;
    GT_U32  error = 0;

    CPSS_NULL_PTR_CHECK_MAC(compareParamsArray);

    if(numCompares == 0)
    {
        return GT_BAD_PARAM;
    }

    for(ii = 0 ; (error == 0) && (ii < numCompares); ii++)
    {
        switch(compareParamsArray[ii].type)
        {
            case UT_TEST_COMPARE_TYPE_NONE_E:
                break;
            case UT_TEST_COMPARE_TYPE_VALUE_E:
                switch (compareParamsArray[ii].operator)
                {
                    case UT_TEST_COMPARE_OPERATOR_DIFF_ABSOLUTE_E:
                        if(((expectedResult + compareParamsArray[ii].value) < actualResult) ||
                           ((expectedResult - compareParamsArray[ii].value) > actualResult))
                        {
                            error = 1;
                        }
                        break;
                    case UT_TEST_COMPARE_OPERATOR_DIFF_ABOVE_E:
                        if((expectedResult + compareParamsArray[ii].value) < actualResult)
                        {
                            error = 1;
                        }
                        else if(expectedResult > actualResult)
                        {
                            error = 1;
                        }
                        break;
                    case UT_TEST_COMPARE_OPERATOR_DIFF_BELOW_E:
                        if((expectedResult - compareParamsArray[ii].value) > actualResult)
                        {
                            error = 1;
                        }
                        else if(expectedResult < actualResult)
                        {
                            error = 1;
                        }
                        break;
                    default:
                        return GT_BAD_PARAM;
                }
                break;
            case UT_TEST_COMPARE_TYPE_PERCENTAGE_E:
                if(compareParamsArray[ii].value >= 100)
                {
                    return GT_BAD_PARAM;
                }

                switch (compareParamsArray[ii].operator)
                {
                    case UT_TEST_COMPARE_OPERATOR_DIFF_ABSOLUTE_E:
                        if((PERCENTEGE(expectedResult,100+compareParamsArray[ii].value) < actualResult) ||
                           (PERCENTEGE(expectedResult,100-compareParamsArray[ii].value) > actualResult))
                        {
                            error = 1;
                        }
                        break;
                    case UT_TEST_COMPARE_OPERATOR_DIFF_ABOVE_E:
                        if(PERCENTEGE(expectedResult,100+compareParamsArray[ii].value) < actualResult)
                        {
                            error = 1;
                        }
                        else if(expectedResult > actualResult)
                        {
                            error = 1;
                        }
                        break;
                    case UT_TEST_COMPARE_OPERATOR_DIFF_BELOW_E:
                        if(PERCENTEGE(expectedResult,100-compareParamsArray[ii].value) > actualResult)
                        {
                            error = 1;
                        }
                        else if(expectedResult < actualResult)
                        {
                            error = 1;
                        }
                        break;
                    default:
                        return GT_BAD_PARAM;
                }
                break;
            case UT_TEST_COMPARE_TYPE_BIGGER_THEN_E:
                if(expectedResult >= actualResult)
                {
                    error = 1;
                }
                break;
            case UT_TEST_COMPARE_TYPE_BIGGER_EQUAL_THEN_E:
                if(expectedResult > actualResult)
                {
                    error = 1;
                }
                break;
            case UT_TEST_COMPARE_TYPE_SMALLER_THEN_E:
                if(expectedResult <= actualResult)
                {
                    error = 1;
                }
                break;
            case UT_TEST_COMPARE_TYPE_SMALLER_EQUAL_THEN_E:
                if(expectedResult < actualResult)
                {
                    error = 1;
                }
                break;
            case UT_TEST_COMPARE_TYPE_EQUAL_TO_E:
                if(expectedResult != actualResult)
                {
                    error = 1;
                }
                break;
            case UT_TEST_COMPARE_TYPE_NOT_EQUAL_TO_E:
                if(expectedResult == actualResult)
                {
                    error = 1;
                }
                break;
            default:
                return GT_BAD_PARAM;
        }
    }

    if(error)
    {
        if(extraInfo == UT_TEST_EXPECTED_EXTRA_INFO_UNUSED_CNS)
        {
            utTestExpectedResultsTrace(actualResult,expectedResult,fieldName);
        }
        else
        {
            char    tmpBuff[256];

            osSprintf(tmpBuff,"%s extraInfo[0x%8.8x]",fieldName,extraInfo);
            utTestExpectedResultsTrace(actualResult,expectedResult,tmpBuff);
        }

        return GT_BAD_PARAM;
    }

    return GT_OK;
}

/*******************************************************************************
* utTrafficDemoInit
*
* DESCRIPTION:
*       init the traffic demo
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS utTrafficDemoInit(
    void
)
{
    GT_STATUS   rc = GT_OK;

    if(demoInitDone)
    {
        return GT_OK;
    }

#ifdef CHX_FAMILY
    rc = appDemoDxChNetRxPacketCbRegister(rxPacketReceiveCb);
#endif /*CHX_FAMILY*/
    demoInitDone = 1;

    return rc;
}

/*******************************************************************************
* utTestPortsAndTagsArrayToBmpConvert
*
* DESCRIPTION:
*       convert the list of ports and taggs to bmp of ports nad tags in device
*
* APPLICABLE DEVICES: All DxCh devices
*
* INPUTS:
*       devNum              - device number
*       numPorts     - number of ports to set to vlan
*       portsArray   - array of ports
*       taggedArray  - array of tagging of the ports
*       portsMembersPtr     - (pointer to) bmp of ports members in vlan
*       portsTaggingPtr     - (pointer to) bmp of ports tagged in the vlan
*       portsTaggingCmd     - (pointer to) ports tagging commands
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong port type
*       GT_BAD_PTR               - one portsMembersPtr NULL pointer or portsArray
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS utTestPortsAndTagsArrayToBmpConvert(
    IN  GT_U8                       devNum,
    IN  GT_U32                      numPorts,
    IN  CPSS_INTERFACE_INFO_STC     portsArray[],
    IN  GT_BOOL                     taggedArray[],
    OUT CPSS_PORTS_BMP_STC          *portsMembersPtr,
    OUT CPSS_PORTS_BMP_STC          *portsTaggingPtr,
    OUT UT_TEST_BRG_VLAN_PORTS_TAG_CMD_STC *portsTaggingCmd
)
{
    GT_U32  ii;

    /* check 2 pointer that must not be NULL */
    CPSS_NULL_PTR_CHECK_MAC(portsArray);
    CPSS_NULL_PTR_CHECK_MAC(portsMembersPtr);
    /* the other 2 pointer may be NULL */

    portsMembersPtr->ports[1] =  portsMembersPtr->ports[0] = 0;
    if(portsTaggingPtr)
    {
        portsTaggingPtr->ports[1] =  portsTaggingPtr->ports[0] = 0;
    }

    for(ii = 0 ; ii < numPorts; ii++)
    {
        if(portsArray[ii].type != CPSS_INTERFACE_PORT_E)
        {
            return GT_BAD_PARAM;
        }

        if(portsArray[ii].devPort.devNum == devNum)
        {
            CPSS_PORTS_BMP_PORT_SET_MAC(portsMembersPtr, portsArray[ii].devPort.portNum);
            if(taggedArray && portsTaggingPtr)
            {
                if(taggedArray[ii] == GT_TRUE)
                {
                    CPSS_PORTS_BMP_PORT_SET_MAC(portsTaggingPtr, portsArray[ii].devPort.portNum);
                    portsTaggingCmd->portsCmd[ii] = UT_TEST_BRG_VLAN_PORT_TAG0_CMD_E;
                }
                else
                {
                    portsTaggingCmd->portsCmd[ii] = UT_TEST_BRG_VLAN_PORT_UNTAGGED_CMD_E;
                }
            }
        }
    }

    return GT_OK;
}

#ifdef CHX_FAMILY
/*******************************************************************************
* utTestBrgVlanInfoConvertDxCh
*
* DESCRIPTION:
*       convert the list of ports and taggs to bmp of ports nad tags in device
*
* APPLICABLE DEVICES: All DxCh devices
*
* INPUTS:
*       vlanParamPtr - (pointer to) VLAN specific information
*
* OUTPUTS:
*       dxChVlanParamPtr - (pointer to) DXCH VLAN specific information
*
* RETURNS:
*       GT_OK                    - on success
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS utTestBrgVlanInfoConvertDxCh(
    IN UT_CONVERT_DIRECTION_ENT convert,
    IN  UT_TEST_BRG_VLAN_INFO_STC       *vlanParamPtr,
    OUT CPSS_DXCH_BRG_VLAN_INFO_STC     *dxChVlanParamPtr
)
{
    GT_U32  swap = (convert == UT_CONVERT_DIRECTION_CPSS_TO_UT_E) ? 1 : 0;

    UT_TEST_FIELD_COPY_MAC(swap,dxChVlanParamPtr,vlanParamPtr,unkSrcAddrSecBreach  );
    UT_TEST_FIELD_COPY_MAC(swap,dxChVlanParamPtr,vlanParamPtr,unregNonIpMcastCmd   );
    UT_TEST_FIELD_COPY_MAC(swap,dxChVlanParamPtr,vlanParamPtr,unregIpv4McastCmd    );
    UT_TEST_FIELD_COPY_MAC(swap,dxChVlanParamPtr,vlanParamPtr,unregIpv6McastCmd    );
    UT_TEST_FIELD_COPY_MAC(swap,dxChVlanParamPtr,vlanParamPtr,unkUcastCmd          );
    UT_TEST_FIELD_COPY_MAC(swap,dxChVlanParamPtr,vlanParamPtr,unregIpv4BcastCmd    );
    UT_TEST_FIELD_COPY_MAC(swap,dxChVlanParamPtr,vlanParamPtr,unregNonIpv4BcastCmd );
    UT_TEST_FIELD_COPY_MAC(swap,dxChVlanParamPtr,vlanParamPtr,ipv4IgmpToCpuEn      );
    UT_TEST_FIELD_COPY_MAC(swap,dxChVlanParamPtr,vlanParamPtr,mirrToRxAnalyzerEn   );
    UT_TEST_FIELD_COPY_MAC(swap,dxChVlanParamPtr,vlanParamPtr,ipv6IcmpToCpuEn      );
    UT_TEST_FIELD_COPY_MAC(swap,dxChVlanParamPtr,vlanParamPtr,ipCtrlToCpuEn        );
    UT_TEST_FIELD_COPY_MAC(swap,dxChVlanParamPtr,vlanParamPtr,ipv4IpmBrgMode       );
    UT_TEST_FIELD_COPY_MAC(swap,dxChVlanParamPtr,vlanParamPtr,ipv6IpmBrgMode       );
    UT_TEST_FIELD_COPY_MAC(swap,dxChVlanParamPtr,vlanParamPtr,ipv4IpmBrgEn         );
    UT_TEST_FIELD_COPY_MAC(swap,dxChVlanParamPtr,vlanParamPtr,ipv6IpmBrgEn         );
    UT_TEST_FIELD_COPY_MAC(swap,dxChVlanParamPtr,vlanParamPtr,ipv6SiteIdMode       );
    UT_TEST_FIELD_COPY_MAC(swap,dxChVlanParamPtr,vlanParamPtr,ipv4UcastRouteEn     );
    UT_TEST_FIELD_COPY_MAC(swap,dxChVlanParamPtr,vlanParamPtr,ipv4McastRouteEn     );
    UT_TEST_FIELD_COPY_MAC(swap,dxChVlanParamPtr,vlanParamPtr,ipv6UcastRouteEn     );
    UT_TEST_FIELD_COPY_MAC(swap,dxChVlanParamPtr,vlanParamPtr,ipv6McastRouteEn     );
    UT_TEST_FIELD_COPY_MAC(swap,dxChVlanParamPtr,vlanParamPtr,stgId                );
    UT_TEST_FIELD_COPY_MAC(swap,dxChVlanParamPtr,vlanParamPtr,autoLearnDisable     );
    UT_TEST_FIELD_COPY_MAC(swap,dxChVlanParamPtr,vlanParamPtr,naMsgToCpuEn         );
    UT_TEST_FIELD_COPY_MAC(swap,dxChVlanParamPtr,vlanParamPtr,mruIdx               );
    UT_TEST_FIELD_COPY_MAC(swap,dxChVlanParamPtr,vlanParamPtr,bcastUdpTrapMirrEn   );
    UT_TEST_FIELD_COPY_MAC(swap,dxChVlanParamPtr,vlanParamPtr,vrfId                );

    return GT_OK;
}

/*******************************************************************************
* utTestBrgTagCommandConvertDxCh
*
* DESCRIPTION:
*       convert ports tag command to/from  DxCh cpss format
*
* APPLICABLE DEVICES: All DxCh devices
*
* INPUTS:
*       portsTaggingCmdPtr - (pointer to) ports tagging command
*
* OUTPUTS:
*       dxChPortsTaggingCmdPtr - (pointer to) DXCH ports tagging command
*
* RETURNS:
*       GT_OK                    - on success
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS utTestBrgTagCommandConvertDxCh(
    IN UT_TEST_BRG_VLAN_PORTS_TAG_CMD_STC     *portsTaggingCmdPtr,
    OUT CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC  *dxChPortsTaggingCmdPtr
)
{
    GT_U32  i; /* loop iterator */

    for (i = 0; i < UT_TEST_MAX_PORTS_NUM_CNS; i++)
    {
        dxChPortsTaggingCmdPtr->portsCmd[i] = (CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT) portsTaggingCmdPtr->portsCmd[i];        
    }

    return GT_OK;
}

/*******************************************************************************
* utTestBrgVlanEntryWriteDxCh
*
* DESCRIPTION:
*       Builds and writes vlan entry to HW. -- DxCh
*
* APPLICABLE DEVICES: All DxCh devices
*
* INPUTS:
*       vlanId       - VLAN Id
*       numPorts     - number of ports to set to vlan
*       portsArray   - array of ports
*       taggedArray  - array of tagging of the ports
*       vlanParamPtr - (pointer to) VLAN specific information
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong vid
*       GT_OUT_OF_RANGE          - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS   utTestBrgVlanEntryWriteDxCh(
    IN  UT_VLAN_ID              vlanId,
    IN  GT_U32                  numPorts,
    IN  CPSS_INTERFACE_INFO_STC portsArray[],
    IN  GT_BOOL                 taggedArray[],
    IN  UT_TEST_BRG_VLAN_INFO_STC *vlanParamPtr
)
{
    GT_STATUS   rc;
    GT_U8       devNum;
    CPSS_PORTS_BMP_STC  portsBmp;
    CPSS_PORTS_BMP_STC  tagsBmp;
    CPSS_DXCH_BRG_VLAN_INFO_STC dxchVlanParam;
    UT_TEST_BRG_VLAN_PORTS_TAG_CMD_STC   portsTaggingCmd;
    CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC dxChPortsTaggingCmd;

    /* convert UT-test format to DXCH format */
    rc = utTestBrgVlanInfoConvertDxCh(UT_CONVERT_DIRECTION_UT_TO_CPSS_E,vlanParamPtr,&dxchVlanParam);
    UT_TEST_RC_CHECK(rc);

    /* prepare iterator for go over all active devices */
    rc = prvUtfNextDeviceReset(&devNum, UTF_ALL_FAMILY_E);
    UT_TEST_RC_CHECK(rc);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        rc = utTestPortsAndTagsArrayToBmpConvert(devNum,numPorts,portsArray,taggedArray,&portsBmp,&tagsBmp, &portsTaggingCmd);
        UT_TEST_RC_CHECK(rc);

        /* convert UT-test format to DXCH format */
        rc = utTestBrgTagCommandConvertDxCh(&portsTaggingCmd,&dxChPortsTaggingCmd);
        UT_TEST_RC_CHECK(rc);

        rc = cpssDxChBrgVlanEntryWrite(devNum,vlanId,&portsBmp,&tagsBmp,&dxchVlanParam, &dxChPortsTaggingCmd);
        UT_TEST_RC_CHECK(rc);
    }

    return GT_OK;
}

/*******************************************************************************
* utTestBrgVlanLearningStateSetDxCh
*
* DESCRIPTION:
*       Sets state of VLAN based learning to specified VLAN on specified device.
*
* APPLICABLE DEVICES:  DxCh2 and above
*
* INPUTS:
*       vlanId    - vlan Id
*       status    - GT_TRUE for enable  or GT_FALSE otherwise.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong vlanId
*
* COMMENTS:
*******************************************************************************/
static GT_STATUS utTestBrgVlanLearningStateSetDxCh
(
    IN GT_U16   vlanId,
    IN GT_BOOL  status
)
{
    GT_STATUS   rc;
    GT_U8       devNum;

    /* prepare iterator for go over all active devices */
    rc = prvUtfNextDeviceReset(&devNum, UTF_CH2_E | UTF_CH3_E);
    UT_TEST_RC_CHECK(rc);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        rc = cpssDxChBrgVlanLearningStateSet(devNum,vlanId,status);
        UT_TEST_RC_CHECK(rc);
    }

    return GT_OK;
}

/*******************************************************************************
* utTestBrgFdbMacEntrySetDxCh
*
* DESCRIPTION:
*       Create new or update existing entry in Hardware MAC address table through
*       Address Update message.(AU message to the PP is non direct access to MAC
*       address table).
*       The function use New Address message (NA) format.
*       The function checks that AU messaging is ready  before using it.
*       The function does not check that AU message was processed by PP.
*
*
* INPUTS:
*       devNum          - physical device number
*       macEntryPtr     - pointer mac table entry
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - aging timeout exceeds hardware limitation.
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE - the PP is not ready to get a message from CPU.
*                      (PP still busy with previous message)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Application should synchronize call of utTestBrgFdbMacEntrySet,
*       utTestBrgFdbQaSend and utTestBrgFdbMacEntryDelete functions.
*       Invocations of these functions should be mutual exclusive because they
*       use same HW resources.
*       Application can check that the AU message processing has completed by
*       utTestBrgFdbFromCpuAuMsgStatusGet.
*       If the entry type is IPv6, then the Application should set the four
*       bytes for SIP and DIP that selected by  utTestBrgMcIpv6BytesSelectSet.
*
*******************************************************************************/
GT_STATUS utTestBrgFdbMacEntrySetDxCh
(
    IN CPSS_MAC_ENTRY_EXT_STC       *macEntryPtr
)
{
    GT_STATUS   rc;
    GT_U8       devNum;

    /* prepare iterator for go over all active devices */
    rc = prvUtfNextDeviceReset(&devNum, UTF_ALL_FAMILY_E);
    UT_TEST_RC_CHECK(rc);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        APPDEMO_FDB_UPDATE_LOCK();
        rc = cpssDxChBrgFdbMacEntrySet(devNum,macEntryPtr);
        APPDEMO_FDB_UPDATE_UNLOCK();
        UT_TEST_RC_CHECK(rc);
    }

    return GT_OK;
}

/*******************************************************************************
* utTestTunnelStartEntryConvertDxCh
*
* DESCRIPTION:
*       convert the TS from UT format to DXCH format
*
* APPLICABLE DEVICES: All DxCh devices
*
* INPUTS:
*       tunnelType    - type of the tunnel; valid options:
*                       CPSS_TUNNEL_X_OVER_IPV4_E,
*                       CPSS_TUNNEL_X_OVER_GRE_IPV4_E,
*                       CPSS_TUNNEL_X_OVER_MPLS_E
*       tsPtr - (pointer to) UT TS entry format
*
* OUTPUTS:
*       dxChTsPtr - (pointer to) DXCH TS entry format
*
* RETURNS:
*       GT_OK                    - on success
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS utTestTunnelStartEntryConvertDxCh(
    IN UT_CONVERT_DIRECTION_ENT convert,
    IN  CPSS_TUNNEL_TYPE_ENT                    tunnelType,
    IN  UT_TEST_TUNNEL_START_CONFIG_UNT       *tsPtr,
    OUT CPSS_DXCH_TUNNEL_START_CONFIG_UNT     *dxChTsPtr
)
{
    GT_U32  swap = (convert == UT_CONVERT_DIRECTION_CPSS_TO_UT_E) ? 1 : 0;

    switch(tunnelType)
    {
        case CPSS_TUNNEL_X_OVER_IPV4_E:
        case CPSS_TUNNEL_X_OVER_GRE_IPV4_E:
            UT_TEST_FIELD_COPY_MAC(swap,dxChTsPtr,tsPtr,ipv4Cfg.tagEnable       );
            UT_TEST_FIELD_COPY_MAC(swap,dxChTsPtr,tsPtr,ipv4Cfg.vlanId          );
            UT_TEST_FIELD_COPY_MAC(swap,dxChTsPtr,tsPtr,ipv4Cfg.upMarkMode      );
            UT_TEST_FIELD_COPY_MAC(swap,dxChTsPtr,tsPtr,ipv4Cfg.up              );
            UT_TEST_FIELD_COPY_MAC(swap,dxChTsPtr,tsPtr,ipv4Cfg.dscpMarkMode    );
            UT_TEST_FIELD_COPY_MAC(swap,dxChTsPtr,tsPtr,ipv4Cfg.dscp            );
            UT_TEST_FIELD_COPY_MAC(swap,dxChTsPtr,tsPtr,ipv4Cfg.macDa           );
            UT_TEST_FIELD_COPY_MAC(swap,dxChTsPtr,tsPtr,ipv4Cfg.dontFragmentFlag);
            UT_TEST_FIELD_COPY_MAC(swap,dxChTsPtr,tsPtr,ipv4Cfg.ttl             );
            UT_TEST_FIELD_COPY_MAC(swap,dxChTsPtr,tsPtr,ipv4Cfg.autoTunnel      );
            UT_TEST_FIELD_COPY_MAC(swap,dxChTsPtr,tsPtr,ipv4Cfg.autoTunnelOffset);
            UT_TEST_FIELD_COPY_MAC(swap,dxChTsPtr,tsPtr,ipv4Cfg.destIp          );
            UT_TEST_FIELD_COPY_MAC(swap,dxChTsPtr,tsPtr,ipv4Cfg.srcIp           );
            UT_TEST_FIELD_COPY_MAC(swap,dxChTsPtr,tsPtr,ipv4Cfg.cfi             );
            break;
        case CPSS_TUNNEL_X_OVER_MPLS_E:
            UT_TEST_FIELD_COPY_MAC(swap,dxChTsPtr,tsPtr,mplsCfg.tagEnable       );
            UT_TEST_FIELD_COPY_MAC(swap,dxChTsPtr,tsPtr,mplsCfg.vlanId          );
            UT_TEST_FIELD_COPY_MAC(swap,dxChTsPtr,tsPtr,mplsCfg.upMarkMode      );
            UT_TEST_FIELD_COPY_MAC(swap,dxChTsPtr,tsPtr,mplsCfg.up              );
            UT_TEST_FIELD_COPY_MAC(swap,dxChTsPtr,tsPtr,mplsCfg.macDa           );
            UT_TEST_FIELD_COPY_MAC(swap,dxChTsPtr,tsPtr,mplsCfg.numLabels       );
            UT_TEST_FIELD_COPY_MAC(swap,dxChTsPtr,tsPtr,mplsCfg.ttl             );
            UT_TEST_FIELD_COPY_MAC(swap,dxChTsPtr,tsPtr,mplsCfg.label1          );
            UT_TEST_FIELD_COPY_MAC(swap,dxChTsPtr,tsPtr,mplsCfg.exp1MarkMode    );
            UT_TEST_FIELD_COPY_MAC(swap,dxChTsPtr,tsPtr,mplsCfg.exp1            );
            UT_TEST_FIELD_COPY_MAC(swap,dxChTsPtr,tsPtr,mplsCfg.label2          );
            UT_TEST_FIELD_COPY_MAC(swap,dxChTsPtr,tsPtr,mplsCfg.exp2MarkMode    );
            UT_TEST_FIELD_COPY_MAC(swap,dxChTsPtr,tsPtr,mplsCfg.exp2            );
            UT_TEST_FIELD_COPY_MAC(swap,dxChTsPtr,tsPtr,mplsCfg.retainCRC       );
            UT_TEST_FIELD_COPY_MAC(swap,dxChTsPtr,tsPtr,mplsCfg.cfi             );
            break;
        default:
            return GT_BAD_PARAM;

        }

    return GT_OK;
}

/*******************************************************************************
* utTestTunnelStartEntrySetDxCh
*
* DESCRIPTION:
*       Set a tunnel start entry.
*
*
* INPUTS:
*       routerArpTunnelStartLineIndex - line index for the tunnel start entry
*                       in the router ARP / tunnel start table (0..1023)
*       tunnelType    - type of the tunnel; valid options:
*                       CPSS_TUNNEL_X_OVER_IPV4_E,
*                       CPSS_TUNNEL_X_OVER_GRE_IPV4_E,
*                       CPSS_TUNNEL_X_OVER_MPLS_E
*       configPtr     - points to tunnel start configuration
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Tunnel start entries table and router ARP addresses table reside at
*       the same physical memory. The table contains 1K lines.
*       Each line can hold:
*       - 1 tunnel start entry
*       - 4 router ARP addresses entries
*       Indexes for tunnel start range is (0..1023); Indexes for router ARP
*       addresses range is (0..4095).
*       Tunnel start entry at index n and router ARP addresses at indexes
*       4*n..4*n+3 share the same memory. For example tunnel start entry at
*       index 100 and router ARP addresses at indexes 400..403 share the same
*       physical memory.
*
*******************************************************************************/
static GT_STATUS utTestTunnelStartEntrySetDxCh
(
    IN  GT_U32                              routerArpTunnelStartLineIndex,
    IN  CPSS_TUNNEL_TYPE_ENT                tunnelType,
    IN  UT_TEST_TUNNEL_START_CONFIG_UNT     *configPtr
)
{
    GT_STATUS   rc;
    GT_U8       devNum;
    CPSS_DXCH_TUNNEL_START_CONFIG_UNT     dxChConfig;

    /* convert UT-test format to DXCH format */
    rc = utTestTunnelStartEntryConvertDxCh(UT_CONVERT_DIRECTION_UT_TO_CPSS_E,tunnelType,configPtr,&dxChConfig);
    UT_TEST_RC_CHECK(rc);

    /* prepare iterator for go over all active devices */
    rc = prvUtfNextDeviceReset(&devNum, UTF_CH2_E | UTF_CH3_E);
    UT_TEST_RC_CHECK(rc);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        rc = cpssDxChTunnelStartEntrySet(devNum,routerArpTunnelStartLineIndex,tunnelType,&dxChConfig);
        UT_TEST_RC_CHECK(rc);
    }

    return GT_OK;
}

/*******************************************************************************
* utTestIpCntSetInterfaceConvertDxCh
*
* DESCRIPTION:
*       convert the IP counter set interface from UT format to DXCH format
*
* APPLICABLE DEVICES: All DxCh devices
*
* INPUTS:
*       interfaceModeCfgPtr - (pointer to) UT IP counter set interface
*
* OUTPUTS:
*       dxChInterfaceModeCfgPtr - (pointer to) DXCH IP counter set interface
*
* RETURNS:
*       GT_OK                    - on success
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS utTestIpCntSetInterfaceConvertDxCh(
    IN UT_CONVERT_DIRECTION_ENT convert,
    IN  UT_TEST_IP_COUNTER_SET_INTERFACE_CFG_STC *interfaceModeCfgPtr,
    OUT CPSS_DXCH_IP_COUNTER_SET_INTERFACE_CFG_STC *dxChInterfaceModeCfgPtr
)
{
    GT_U32  swap = (convert == UT_CONVERT_DIRECTION_CPSS_TO_UT_E) ? 1 : 0;

    UT_TEST_FIELD_COPY_MAC(swap,dxChInterfaceModeCfgPtr,interfaceModeCfgPtr,portTrunkCntMode );
    UT_TEST_FIELD_COPY_MAC(swap,dxChInterfaceModeCfgPtr,interfaceModeCfgPtr,ipMode           );
    UT_TEST_FIELD_COPY_MAC(swap,dxChInterfaceModeCfgPtr,interfaceModeCfgPtr,vlanMode         );
    UT_TEST_FIELD_COPY_MAC(swap,dxChInterfaceModeCfgPtr,interfaceModeCfgPtr,devNum           );
    UT_TEST_FIELD_COPY_MAC(swap,dxChInterfaceModeCfgPtr,interfaceModeCfgPtr,portTrunk.trunk  );
    UT_TEST_FIELD_COPY_MAC(swap,dxChInterfaceModeCfgPtr,interfaceModeCfgPtr,vlanId           );

    return GT_OK;
}

/*******************************************************************************
* utTestIpCntSetModeSetDxCh
*
* DESCRIPTION:
*      Sets a counter set's bounded interface and interface mode.
*
* APPLICABLE DEVICES: DxCh2 and above.
*
* INPUTS:
*       cntSet        - the counter set
*       cntSetMode    - the counter set bind mode (interface or Next hop)
*       interfaceModeCfgPtr - if cntSetMode = UT_TEST_IP_CNT_SET_INTERFACE_MODE_E
*                          this is the interface configuration
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS utTestIpCntSetModeSetDxCh
(
    IN  CPSS_IP_CNT_SET_ENT                      cntSet,
    IN  UT_TEST_IP_CNT_SET_MODE_ENT              cntSetMode,
    IN  UT_TEST_IP_COUNTER_SET_INTERFACE_CFG_STC *interfaceModeCfgPtr
)
{
    GT_STATUS   rc;
    GT_U8       devNum;
    CPSS_DXCH_IP_COUNTER_SET_INTERFACE_CFG_STC dxChInterfaceModeCfg;

    /* convert UT-test format to DXCH format */
    rc = utTestIpCntSetInterfaceConvertDxCh(UT_CONVERT_DIRECTION_UT_TO_CPSS_E,interfaceModeCfgPtr,&dxChInterfaceModeCfg);
    UT_TEST_RC_CHECK(rc);

    /* prepare iterator for go over all active devices */
    rc = prvUtfNextDeviceReset(&devNum, UTF_CH2_E | UTF_CH3_E);
    UT_TEST_RC_CHECK(rc);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        rc = cpssDxChIpCntSetModeSet(devNum,cntSet,cntSetMode,&dxChInterfaceModeCfg);
        UT_TEST_RC_CHECK(rc);
    }

    return GT_OK;
}

/*******************************************************************************
* utTestIpUcRouteEntriesConvertDxCh
*
* DESCRIPTION:
*       convert the array of IP uc route from UT format to DXCH format
*
* APPLICABLE DEVICES: All DxCh devices
*
* INPUTS:
*       numOfRouteEntries - number of entries in the array
*       routeEntriesArray - UT Array IP uc route
*
* OUTPUTS:
*       dxChRouteEntriesArray -  DXCH array IP uc route
*
* RETURNS:
*       GT_OK                    - on success
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS utTestIpUcRouteEntriesConvertDxCh(
    IN UT_CONVERT_DIRECTION_ENT convert,
    IN GT_U32                          numOfRouteEntries,
    IN UT_TEST_IP_UC_ROUTE_ENTRY_STC   routeEntriesArray[],
    OUT CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC dxChRouteEntriesArray[]
)
{
    GT_U32  ii,jj;

    GT_U32  swap = (convert == UT_CONVERT_DIRECTION_CPSS_TO_UT_E) ? 1 : 0;

    for(ii = 0 ; ii < numOfRouteEntries; ii++)
    {
        dxChRouteEntriesArray[ii].type               = routeEntriesArray[ii].type ;
        switch(routeEntriesArray[ii].type)
        {
            case UT_TEST_IP_UC_ROUTE_ENTRY_E:
                UT_TEST_FIELD_COPY_MAC(swap,&dxChRouteEntriesArray[ii],&routeEntriesArray[ii],entry.regularEntry.cmd                              );
                UT_TEST_FIELD_COPY_MAC(swap,&dxChRouteEntriesArray[ii],&routeEntriesArray[ii],entry.regularEntry.cpuCodeIdx                       );
                UT_TEST_FIELD_COPY_MAC(swap,&dxChRouteEntriesArray[ii],&routeEntriesArray[ii],entry.regularEntry.appSpecificCpuCodeEnable         );
                UT_TEST_FIELD_COPY_MAC(swap,&dxChRouteEntriesArray[ii],&routeEntriesArray[ii],entry.regularEntry.unicastPacketSipFilterEnable     );
                UT_TEST_FIELD_COPY_MAC(swap,&dxChRouteEntriesArray[ii],&routeEntriesArray[ii],entry.regularEntry.ttlHopLimitDecEnable             );
                UT_TEST_FIELD_COPY_MAC(swap,&dxChRouteEntriesArray[ii],&routeEntriesArray[ii],entry.regularEntry.ttlHopLimDecOptionsExtChkByPass  );
                UT_TEST_FIELD_COPY_MAC(swap,&dxChRouteEntriesArray[ii],&routeEntriesArray[ii],entry.regularEntry.ingressMirror                    );
                UT_TEST_FIELD_COPY_MAC(swap,&dxChRouteEntriesArray[ii],&routeEntriesArray[ii],entry.regularEntry.qosProfileMarkingEnable          );
                UT_TEST_FIELD_COPY_MAC(swap,&dxChRouteEntriesArray[ii],&routeEntriesArray[ii],entry.regularEntry.qosProfileIndex                  );
                UT_TEST_FIELD_COPY_MAC(swap,&dxChRouteEntriesArray[ii],&routeEntriesArray[ii],entry.regularEntry.qosPrecedence                    );
                UT_TEST_FIELD_COPY_MAC(swap,&dxChRouteEntriesArray[ii],&routeEntriesArray[ii],entry.regularEntry.modifyUp                         );
                UT_TEST_FIELD_COPY_MAC(swap,&dxChRouteEntriesArray[ii],&routeEntriesArray[ii],entry.regularEntry.modifyDscp                       );
                UT_TEST_FIELD_COPY_MAC(swap,&dxChRouteEntriesArray[ii],&routeEntriesArray[ii],entry.regularEntry.countSet                         );
                UT_TEST_FIELD_COPY_MAC(swap,&dxChRouteEntriesArray[ii],&routeEntriesArray[ii],entry.regularEntry.trapMirrorArpBcEnable            );
                UT_TEST_FIELD_COPY_MAC(swap,&dxChRouteEntriesArray[ii],&routeEntriesArray[ii],entry.regularEntry.sipAccessLevel                   );
                UT_TEST_FIELD_COPY_MAC(swap,&dxChRouteEntriesArray[ii],&routeEntriesArray[ii],entry.regularEntry.dipAccessLevel                   );
                UT_TEST_FIELD_COPY_MAC(swap,&dxChRouteEntriesArray[ii],&routeEntriesArray[ii],entry.regularEntry.ICMPRedirectEnable               );
                UT_TEST_FIELD_COPY_MAC(swap,&dxChRouteEntriesArray[ii],&routeEntriesArray[ii],entry.regularEntry.scopeCheckingEnable              );
                UT_TEST_FIELD_COPY_MAC(swap,&dxChRouteEntriesArray[ii],&routeEntriesArray[ii],entry.regularEntry.siteId                           );
                UT_TEST_FIELD_COPY_MAC(swap,&dxChRouteEntriesArray[ii],&routeEntriesArray[ii],entry.regularEntry.mtuProfileIndex                  );
                UT_TEST_FIELD_COPY_MAC(swap,&dxChRouteEntriesArray[ii],&routeEntriesArray[ii],entry.regularEntry.isTunnelStart                    );
                UT_TEST_FIELD_COPY_MAC(swap,&dxChRouteEntriesArray[ii],&routeEntriesArray[ii],entry.regularEntry.nextHopVlanId                    );
                UT_TEST_FIELD_COPY_MAC(swap,&dxChRouteEntriesArray[ii],&routeEntriesArray[ii],entry.regularEntry.nextHopInterface                 );
                UT_TEST_FIELD_COPY_MAC(swap,&dxChRouteEntriesArray[ii],&routeEntriesArray[ii],entry.regularEntry.nextHopARPPointer                );
                UT_TEST_FIELD_COPY_MAC(swap,&dxChRouteEntriesArray[ii],&routeEntriesArray[ii],entry.regularEntry.nextHopTunnelPointer             );
                break;
            case UT_TEST_IP_UC_ECMP_RPF_E:
                for(jj = 0 ; jj < 8; jj++)
                {
                    UT_TEST_FIELD_COPY_MAC(swap,&dxChRouteEntriesArray[ii],&routeEntriesArray[ii],entry.ecmpRpfCheck.vlanArray[jj]);
                }
                break;
            default:
                return GT_BAD_PARAM;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* utTestIpUcRouteEntriesWriteDxCh
*
* DESCRIPTION:
*    Writes an array of uc route entries to hw.
*
* APPLICABLE DEVICES: DxCh2 and above.
*
* INPUTS:
*       baseRouteEntryIndex   - the index from which to write the array
*       routeEntriesArray     - the uc route entries array
*       numOfRouteEntries     - the number route entries in the array (= the
*                               number of route entries to write)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on devNum not active.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       In case uRPF ECMP/QOS is globally enabled, then for ECMP/QOS block
*       with x route entries, additional route entry should be included after
*       the block with the uRPF route entry information.
*
*
*******************************************************************************/
static GT_STATUS utTestIpUcRouteEntriesWriteDxCh
(
    IN GT_U32                          baseRouteEntryIndex,
    IN UT_TEST_IP_UC_ROUTE_ENTRY_STC   *routeEntriesArray,
    IN GT_U32                          numOfRouteEntries
)
{
    GT_STATUS   rc;
    GT_U8       devNum;
    CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC   *dxChRouteEntriesArray;

    if(numOfRouteEntries == 0)
    {
        return GT_OK;
    }

    dxChRouteEntriesArray = osMalloc(sizeof(UT_TEST_IP_UC_ROUTE_ENTRY_STC)*numOfRouteEntries);
    if(dxChRouteEntriesArray == NULL)
    {
        return GT_OUT_OF_CPU_MEM;
    }

    /* convert UT-test format to DXCH format */
    rc = utTestIpUcRouteEntriesConvertDxCh(UT_CONVERT_DIRECTION_UT_TO_CPSS_E,numOfRouteEntries,routeEntriesArray,dxChRouteEntriesArray);
    if(rc != GT_OK)
    {
        goto exit_cleanly_lbl;
    }

    /* prepare iterator for go over all active devices */
    rc = prvUtfNextDeviceReset(&devNum, UTF_CH2_E | UTF_CH3_E);
    if(rc != GT_OK)
    {
        goto exit_cleanly_lbl;
    }

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        rc = cpssDxChIpUcRouteEntriesWrite(devNum,baseRouteEntryIndex,dxChRouteEntriesArray,numOfRouteEntries);
        if(rc != GT_OK)
        {
            goto exit_cleanly_lbl;
        }
    }

exit_cleanly_lbl:
    osFree(dxChRouteEntriesArray);

    UT_TEST_RC_CHECK(rc);

    return GT_OK;

}

/*******************************************************************************
* utTestIpCntConvertDxCh
*
* DESCRIPTION:
*       convert the IP counters from DXCH format to UT format
*
* APPLICABLE DEVICES: All DxCh devices
*
* INPUTS:
*       dxChCountersPtr -  DXCH IP counters
*
* OUTPUTS:
*       countersPtr - UT IP counters
*
* RETURNS:
*       GT_OK                    - on success
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS utTestIpCntConvertDxCh(
    IN UT_CONVERT_DIRECTION_ENT convert,
    IN CPSS_DXCH_IP_COUNTER_SET_STC *dxChCountersPtr,
    OUT UT_TEST_IP_COUNTER_SET_STC  *countersPtr
)
{
    GT_U32  swap = (convert == UT_CONVERT_DIRECTION_CPSS_TO_UT_E) ? 1 : 0;

    UT_TEST_FIELD_COPY_MAC(swap,dxChCountersPtr,countersPtr,inUcPkts                 );
    UT_TEST_FIELD_COPY_MAC(swap,dxChCountersPtr,countersPtr,inMcPkts                 );
    UT_TEST_FIELD_COPY_MAC(swap,dxChCountersPtr,countersPtr,inUcNonRoutedExcpPkts    );
    UT_TEST_FIELD_COPY_MAC(swap,dxChCountersPtr,countersPtr,inUcNonRoutedNonExcpPkts );
    UT_TEST_FIELD_COPY_MAC(swap,dxChCountersPtr,countersPtr,inMcNonRoutedExcpPkts    );
    UT_TEST_FIELD_COPY_MAC(swap,dxChCountersPtr,countersPtr,inMcNonRoutedNonExcpPkts );
    UT_TEST_FIELD_COPY_MAC(swap,dxChCountersPtr,countersPtr,inUcTrappedMirrorPkts    );
    UT_TEST_FIELD_COPY_MAC(swap,dxChCountersPtr,countersPtr,inMcTrappedMirrorPkts    );
    UT_TEST_FIELD_COPY_MAC(swap,dxChCountersPtr,countersPtr,mcRfpFailPkts            );
    UT_TEST_FIELD_COPY_MAC(swap,dxChCountersPtr,countersPtr,outUcRoutedPkts          );

    return GT_OK;
}

/*******************************************************************************
* utTestIpCntGetDxCh
*
* DESCRIPTION:
*       Return the IP counter set requested.
*
* APPLICABLE DEVICES: DxCh2 and above.
*
* INPUTS:
*       cntSet - counters set to retrieve.
*
* OUTPUTS:
*       countersPtr - (pointer to) counter values
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - on error
*       GT_NOT_INITIALIZED       - The library was not initialized.
*       GT_BAD_PARAM             - Illegal parameter in function called
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This function doesn't handle counters overflow.
*
*******************************************************************************/
GT_STATUS utTestIpCntGetDxCh
(
    IN  CPSS_IP_CNT_SET_ENT          cntSet,
    OUT UT_TEST_IP_COUNTER_SET_STC  *countersPtr
)
{
    GT_STATUS   rc;
    GT_U8       devNum;
    CPSS_DXCH_IP_COUNTER_SET_STC dxchCounters;
    UT_TEST_IP_COUNTER_SET_STC   tmpCounters;


    countersPtr->inUcPkts                    = 0 ;
    countersPtr->inMcPkts                    = 0 ;
    countersPtr->inUcNonRoutedExcpPkts       = 0 ;
    countersPtr->inUcNonRoutedNonExcpPkts    = 0 ;
    countersPtr->inMcNonRoutedExcpPkts       = 0 ;
    countersPtr->inMcNonRoutedNonExcpPkts    = 0 ;
    countersPtr->inUcTrappedMirrorPkts       = 0 ;
    countersPtr->inMcTrappedMirrorPkts       = 0 ;
    countersPtr->mcRfpFailPkts               = 0 ;
    countersPtr->outUcRoutedPkts             = 0 ;


    /* prepare iterator for go over all active devices */
    rc = prvUtfNextDeviceReset(&devNum, UTF_CH2_E | UTF_CH3_E);
    UT_TEST_RC_CHECK(rc);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        rc = cpssDxChIpCntGet(devNum,cntSet,&dxchCounters);
        UT_TEST_RC_CHECK(rc);

        /* convert  DXCH format to UT-test format */
        rc = utTestIpCntConvertDxCh(UT_CONVERT_DIRECTION_CPSS_TO_UT_E,&dxchCounters,&tmpCounters);
        UT_TEST_RC_CHECK(rc);


        countersPtr->inUcPkts                     += tmpCounters.inUcPkts                  ;
        countersPtr->inMcPkts                     += tmpCounters.inMcPkts                  ;
        countersPtr->inUcNonRoutedExcpPkts        += tmpCounters.inUcNonRoutedExcpPkts     ;
        countersPtr->inUcNonRoutedNonExcpPkts     += tmpCounters.inUcNonRoutedNonExcpPkts  ;
        countersPtr->inMcNonRoutedExcpPkts        += tmpCounters.inMcNonRoutedExcpPkts     ;
        countersPtr->inMcNonRoutedNonExcpPkts     += tmpCounters.inMcNonRoutedNonExcpPkts  ;
        countersPtr->inUcTrappedMirrorPkts        += tmpCounters.inUcTrappedMirrorPkts     ;
        countersPtr->inMcTrappedMirrorPkts        += tmpCounters.inMcTrappedMirrorPkts     ;
        countersPtr->mcRfpFailPkts                += tmpCounters.mcRfpFailPkts             ;
        countersPtr->outUcRoutedPkts              += tmpCounters.outUcRoutedPkts           ;
    }

    return GT_OK;
}

/*******************************************************************************
* utTestIpCntSetDxCh
*
* DESCRIPTION:
*       Set the IP counter set requested.
*
* APPLICABLE DEVICES: DxCh2 and above.
*
* INPUTS:
*       cntSet - counters set to retrieve.
*
* OUTPUTS:
*       countersPtr - (pointer to) counter values
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - on error
*       GT_NOT_INITIALIZED       - The library was not initialized.
*       GT_BAD_PARAM             - Illegal parameter in function called
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This function doesn't handle counters overflow.
*
*******************************************************************************/
GT_STATUS utTestIpCntSetDxCh
(
    IN  CPSS_IP_CNT_SET_ENT          cntSet,
    OUT UT_TEST_IP_COUNTER_SET_STC  *countersPtr
)
{
    GT_STATUS   rc;
    GT_U8       devNum;
    CPSS_DXCH_IP_COUNTER_SET_STC dxchCounters;

    /* prepare iterator for go over all active devices */
    rc = prvUtfNextDeviceReset(&devNum, UTF_CH2_E | UTF_CH3_E);
    UT_TEST_RC_CHECK(rc);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* convert UT format to DXCH  format */
        rc = utTestIpCntConvertDxCh(UT_CONVERT_DIRECTION_UT_TO_CPSS_E,&dxchCounters,countersPtr);
        UT_TEST_RC_CHECK(rc);

        rc = cpssDxChIpCntSet(devNum,cntSet,&dxchCounters);
        UT_TEST_RC_CHECK(rc);
    }

    return GT_OK;
}

/*******************************************************************************
* utTestIpLpmIpv4UcPrefixConvertDxCh
*
* DESCRIPTION:
*       convert the IP LPM Ipv4 UC prefix from DXCH format to UT format
*
* APPLICABLE DEVICES: All DxCh devices
*
* INPUTS:
*       dxChNextHopInfoPtr -  DXCH IP LPM Ipv4 UC prefix
*
* OUTPUTS:
*       nextHopInfoPtr - UT IP LPM Ipv4 UC prefix
*
* RETURNS:
*       GT_OK                    - on success
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS utTestIpLpmIpv4UcPrefixConvertDxCh(
    IN UT_CONVERT_DIRECTION_ENT convert,
    INOUT CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT *dxChNextHopInfoPtr,
    INOUT UT_TEST_IP_TCAM_ROUTE_ENTRY_INFO_UNT  *nextHopInfoPtr
)
{
    GT_U32  swap = (convert == UT_CONVERT_DIRECTION_CPSS_TO_UT_E) ? 1 : 0;

    UT_TEST_FIELD_COPY_MAC(swap,dxChNextHopInfoPtr,nextHopInfoPtr,ipLttEntry.routeType                  );
    UT_TEST_FIELD_COPY_MAC(swap,dxChNextHopInfoPtr,nextHopInfoPtr,ipLttEntry.numOfPaths                 );
    UT_TEST_FIELD_COPY_MAC(swap,dxChNextHopInfoPtr,nextHopInfoPtr,ipLttEntry.routeEntryBaseIndex        );
    UT_TEST_FIELD_COPY_MAC(swap,dxChNextHopInfoPtr,nextHopInfoPtr,ipLttEntry.ucRPFCheckEnable           );
    UT_TEST_FIELD_COPY_MAC(swap,dxChNextHopInfoPtr,nextHopInfoPtr,ipLttEntry.sipSaCheckMismatchEnable   );
    UT_TEST_FIELD_COPY_MAC(swap,dxChNextHopInfoPtr,nextHopInfoPtr,ipLttEntry.ipv6MCGroupScopeLevel      );

    return GT_OK;
}

/*******************************************************************************
* utTestIpLpmIpv4UcPrefixAddDxCh
*
* DESCRIPTION:
*   Creates a new or override an existing Ipv4 prefix in a Virtual Router for
*   the specified LPM DB.
*
* APPLICABLE DEVICES: All DxCh Devices.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router id.
*                         future support only , currently only vrId = 0 is
*                         supported.
*       ipAddr          - The destination IP address of this prefix.
*       prefixLen       - The number of bits that are actual valid in the ipAddr.
*       nextHopInfoPtr  - the route entry info associated with this UC prefix.
*       override        - override an existing entry for this mask if it already
*                         exists, or don't override and return error.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_RANGE         -  If prefix length is too big, or
*       GT_ERROR                 - If the vrId was not created yet, or
*       GT_OUT_OF_CPU_MEM        - If failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM         - If failed to allocate TCAM memory, or
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       To change the default prefix for the VR use prefixLen = 0.
*
*******************************************************************************/
GT_STATUS utTestIpLpmIpv4UcPrefixAddDxCh
(
    IN GT_U32                                   lpmDBId,
    IN GT_U32                                   vrId,
    IN GT_IPADDR                                ipAddr,
    IN GT_U32                                   prefixLen,
    IN UT_TEST_IP_TCAM_ROUTE_ENTRY_INFO_UNT     *nextHopInfoPtr,
    IN GT_BOOL                                  override
)
{
    GT_STATUS rc;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT dxChNextHopInfo;

    rc = utTestIpLpmIpv4UcPrefixConvertDxCh(UT_CONVERT_DIRECTION_UT_TO_CPSS_E,&dxChNextHopInfo,nextHopInfoPtr);
    UT_TEST_RC_CHECK(rc);

    rc = cpssDxChIpLpmIpv4UcPrefixAdd(lpmDBId,vrId,ipAddr,prefixLen,&dxChNextHopInfo,override);
    UT_TEST_RC_CHECK(rc);

    return rc;
}

#endif/*CHX_FAMILY*/

/*******************************************************************************
* utTestBrgVlanEntryWrite
*
* DESCRIPTION:
*       Builds and writes vlan entry to HW.
*
* APPLICABLE DEVICES: All DxCh devices
*
* INPUTS:
*       vlanId       - VLAN Id
*       numPorts     - number of ports to set to vlan
*       portsArray   - array of ports
*       vlanParamPtr - (pointer to) VLAN specific information
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong vid
*       GT_OUT_OF_RANGE          - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS   utTestBrgVlanEntryWrite(
    IN  UT_VLAN_ID              vlanId,
    IN  GT_U32                  numPorts,
    IN  CPSS_INTERFACE_INFO_STC portsArray[],
    IN  GT_BOOL                 taggedArray[],
    IN  UT_TEST_BRG_VLAN_INFO_STC *vlanParamPtr
)
{
    GT_STATUS rc = GT_OK;
    UT_TEST_PARAM_NUT_USED(vlanId);
    UT_TEST_PARAM_NUT_USED(numPorts);
    UT_TEST_PARAM_NUT_USED(portsArray);
    UT_TEST_PARAM_NUT_USED(taggedArray);
    UT_TEST_PARAM_NUT_USED(vlanParamPtr);

#ifdef EXMXPM_FAMILY
#endif /*EXMXPM_FAMILY*/

#ifdef EX_FAMILY
#endif /*EX_FAMILY*/

#ifdef CHX_FAMILY
    rc = utTestBrgVlanEntryWriteDxCh(vlanId,numPorts,portsArray,taggedArray,vlanParamPtr);
    UT_TEST_RC_CHECK(rc);
#endif /*CHX_FAMILY*/

    return rc;
}


/*******************************************************************************
* utTestBrgVlanLearningStateSet
*
* DESCRIPTION:
*       Sets state of VLAN based learning to specified VLAN on specified device.
*
* INPUTS:
*       vlanId    - vlan Id
*       status    - GT_TRUE for enable  or GT_FALSE otherwise.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong vlanId
*
* COMMENTS:
*******************************************************************************/
GT_STATUS utTestBrgVlanLearningStateSet
(
    IN GT_U16   vlanId,
    IN GT_BOOL  status
)
{
    GT_STATUS rc = GT_OK;

    UT_TEST_PARAM_NUT_USED(vlanId);
    UT_TEST_PARAM_NUT_USED(status);

#ifdef EXMXPM_FAMILY
#endif /*EXMXPM_FAMILY*/

#ifdef EX_FAMILY
#endif /*EX_FAMILY*/

#ifdef CHX_FAMILY
    rc = utTestBrgVlanLearningStateSetDxCh(vlanId,status);
    UT_TEST_RC_CHECK(rc);
#endif /*CHX_FAMILY*/

    return rc;
}

/*******************************************************************************
* utTestBrgFdbMacEntrySet
*
* DESCRIPTION:
*       Create new or update existing entry in Hardware MAC address table through
*       Address Update message.(AU message to the PP is non direct access to MAC
*       address table).
*       The function use New Address message (NA) format.
*       The function checks that AU messaging is ready  before using it.
*       The function does not check that AU message was processed by PP.
*
*
* INPUTS:
*       devNum          - physical device number
*       macEntryPtr     - pointer mac table entry
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - aging timeout exceeds hardware limitation.
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE - the PP is not ready to get a message from CPU.
*                      (PP still busy with previous message)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Application should synchronize call of utTestBrgFdbMacEntrySet,
*       utTestBrgFdbQaSend and utTestBrgFdbMacEntryDelete functions.
*       Invocations of these functions should be mutual exclusive because they
*       use same HW resources.
*       Application can check that the AU message processing has completed by
*       utTestBrgFdbFromCpuAuMsgStatusGet.
*       If the entry type is IPv6, then the Application should set the four
*       bytes for SIP and DIP that selected by  utTestBrgMcIpv6BytesSelectSet.
*
*******************************************************************************/
GT_STATUS utTestBrgFdbMacEntrySet
(
    IN CPSS_MAC_ENTRY_EXT_STC       *macEntryPtr
)
{
    GT_STATUS rc = GT_OK;

    UT_TEST_PARAM_NUT_USED(macEntryPtr);

#ifdef EXMXPM_FAMILY
#endif /*EXMXPM_FAMILY*/

#ifdef EX_FAMILY
#endif /*EX_FAMILY*/

#ifdef CHX_FAMILY
    rc = utTestBrgFdbMacEntrySetDxCh(macEntryPtr);
    UT_TEST_RC_CHECK(rc);
#endif /*CHX_FAMILY*/

    return rc;
}

/*******************************************************************************
* utTestTunnelStartEntrySetDxCh
*
* DESCRIPTION:
*       Set a tunnel start entry.
*
*
* INPUTS:
*       routerArpTunnelStartLineIndex - line index for the tunnel start entry
*                       in the router ARP / tunnel start table (0..1023)
*       tunnelType    - type of the tunnel; valid options:
*                       CPSS_TUNNEL_X_OVER_IPV4_E,
*                       CPSS_TUNNEL_X_OVER_GRE_IPV4_E,
*                       CPSS_TUNNEL_X_OVER_MPLS_E
*       configPtr     - points to tunnel start configuration
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Tunnel start entries table and router ARP addresses table reside at
*       the same physical memory. The table contains 1K lines.
*       Each line can hold:
*       - 1 tunnel start entry
*       - 4 router ARP addresses entries
*       Indexes for tunnel start range is (0..1023); Indexes for router ARP
*       addresses range is (0..4095).
*       Tunnel start entry at index n and router ARP addresses at indexes
*       4*n..4*n+3 share the same memory. For example tunnel start entry at
*       index 100 and router ARP addresses at indexes 400..403 share the same
*       physical memory.
*
*******************************************************************************/
GT_STATUS utTestTunnelStartEntrySet
(
    IN  GT_U32                              routerArpTunnelStartLineIndex,
    IN  CPSS_TUNNEL_TYPE_ENT                tunnelType,
    IN  UT_TEST_TUNNEL_START_CONFIG_UNT     *configPtr
)
{
    GT_STATUS rc = GT_OK;

    UT_TEST_PARAM_NUT_USED(routerArpTunnelStartLineIndex);
    UT_TEST_PARAM_NUT_USED(tunnelType);
    UT_TEST_PARAM_NUT_USED(configPtr);

#ifdef EXMXPM_FAMILY
#endif /*EXMXPM_FAMILY*/

#ifdef EX_FAMILY
#endif /*EX_FAMILY*/

#ifdef CHX_FAMILY
    rc = utTestTunnelStartEntrySetDxCh(routerArpTunnelStartLineIndex,tunnelType,configPtr);
    UT_TEST_RC_CHECK(rc);
#endif /*CHX_FAMILY*/

    return rc;
}

/*******************************************************************************
* utTestIpCntSetModeSet
*
* DESCRIPTION:
*      Sets a counter set's bounded interface and interface mode.
*
* APPLICABLE DEVICES: DxCh2 and above.
*
* INPUTS:
*       cntSet        - the counter set
*       cntSetMode    - the counter set bind mode (interface or Next hop)
*       interfaceModeCfgPtr - if cntSetMode = UT_TEST_IP_CNT_SET_INTERFACE_MODE_E
*                          this is the interface configuration
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS utTestIpCntSetModeSet
(
    IN  CPSS_IP_CNT_SET_ENT                      cntSet,
    IN  UT_TEST_IP_CNT_SET_MODE_ENT              cntSetMode,
    IN  UT_TEST_IP_COUNTER_SET_INTERFACE_CFG_STC *interfaceModeCfgPtr
)
{
    GT_STATUS rc = GT_OK;

    UT_TEST_PARAM_NUT_USED(cntSet);
    UT_TEST_PARAM_NUT_USED(cntSetMode);
    UT_TEST_PARAM_NUT_USED(interfaceModeCfgPtr);

#ifdef EXMXPM_FAMILY
#endif /*EXMXPM_FAMILY*/

#ifdef EX_FAMILY
#endif /*EX_FAMILY*/

#ifdef CHX_FAMILY
    rc = utTestIpCntSetModeSetDxCh(cntSet,cntSetMode,interfaceModeCfgPtr);
    UT_TEST_RC_CHECK(rc);
#endif /*CHX_FAMILY*/

    return rc;
}


/*******************************************************************************
* utTestIpPortRoutingEnable
*
* DESCRIPTION:
*       Enable multicast/unicast IPv4/v6 routing on a port.
*
* APPLICABLE DEVICES: DxCh2 and above.
*
* INPUTS:
*       devNum        - the device number
*       portNum       - the port to enable on
*       ucMcEnable    - routing type to enable Unicast/Multicast
*       protocolStack - what type of traffic to enable ipv4 or ipv6 or both.
*       enableRouting - enable IP routing for this port
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       none
*
*
*******************************************************************************/
GT_STATUS utTestIpPortRoutingEnable
(
    IN GT_U8                            devNum,
    IN GT_U8                            portNum,
    IN CPSS_IP_UNICAST_MULTICAST_ENT    ucMcEnable,
    IN CPSS_IP_PROTOCOL_STACK_ENT       protocolStack,
    IN GT_BOOL                          enableRouting
)
{
    GT_STATUS rc = GT_OK;

    UT_TEST_PARAM_NUT_USED(devNum);
    UT_TEST_PARAM_NUT_USED(portNum);
    UT_TEST_PARAM_NUT_USED(ucMcEnable);
    UT_TEST_PARAM_NUT_USED(protocolStack);
    UT_TEST_PARAM_NUT_USED(enableRouting);

#ifdef EXMXPM_FAMILY
#endif /*EXMXPM_FAMILY*/

#ifdef EX_FAMILY
#endif /*EX_FAMILY*/

#ifdef CHX_FAMILY
    rc = cpssDxChIpPortRoutingEnable(devNum,portNum,ucMcEnable,protocolStack,enableRouting);
    UT_TEST_RC_CHECK(rc);
#endif /*CHX_FAMILY*/

    return rc;
}

/*******************************************************************************
* utTestBrgVlanIpUcRouteEnable
*
* DESCRIPTION:
*       Enable/Disable IPv4/Ipv6 Unicast Routing on Vlan
*
* APPLIOCABLE DEVICES: All DxCh devices
*
* INPUTS:
*       devNum      - device number
*       vlanId      - Vlan ID
*       protocol    - ipv4 or ipv6
*       value       - GT_TRUE - enable ip unicast routing;
*                     GT_FALSE -  disable ip unicast routing.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS utTestBrgVlanIpUcRouteEnable
(
    IN GT_U8                        devNum,
    IN GT_U16                       vlanId,
    IN CPSS_IP_PROTOCOL_STACK_ENT   protocol,
    IN GT_BOOL                      enable
)
{
    GT_STATUS rc = GT_OK;

    UT_TEST_PARAM_NUT_USED(devNum);
    UT_TEST_PARAM_NUT_USED(vlanId);
    UT_TEST_PARAM_NUT_USED(protocol);
    UT_TEST_PARAM_NUT_USED(enable);

#ifdef EXMXPM_FAMILY
#endif /*EXMXPM_FAMILY*/

#ifdef EX_FAMILY
#endif /*EX_FAMILY*/

#ifdef CHX_FAMILY
    rc = cpssDxChBrgVlanIpUcRouteEnable(devNum,vlanId,protocol,enable);
    UT_TEST_RC_CHECK(rc);
#endif /*CHX_FAMILY*/

    return rc;
}

/*******************************************************************************
* utTestIpUcRouteEntriesWrite
*
* DESCRIPTION:
*    Writes an array of uc route entries to hw.
*
* APPLICABLE DEVICES: DxCh2 and above.
*
* INPUTS:
*       baseRouteEntryIndex   - the index from which to write the array
*       routeEntriesArray     - the uc route entries array
*       numOfRouteEntries     - the number route entries in the array (= the
*                               number of route entries to write)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on devNum not active.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       In case uRPF ECMP/QOS is globally enabled, then for ECMP/QOS block
*       with x route entries, additional route entry should be included after
*       the block with the uRPF route entry information.
*
*
*******************************************************************************/
GT_STATUS utTestIpUcRouteEntriesWrite
(
    IN GT_U32                          baseRouteEntryIndex,
    IN UT_TEST_IP_UC_ROUTE_ENTRY_STC   *routeEntriesArray,
    IN GT_U32                          numOfRouteEntries
)
{
    GT_STATUS rc = GT_OK;

    UT_TEST_PARAM_NUT_USED(baseRouteEntryIndex);
    UT_TEST_PARAM_NUT_USED(routeEntriesArray);
    UT_TEST_PARAM_NUT_USED(numOfRouteEntries);

#ifdef EXMXPM_FAMILY
#endif /*EXMXPM_FAMILY*/

#ifdef EX_FAMILY
#endif /*EX_FAMILY*/

#ifdef CHX_FAMILY
    rc = utTestIpUcRouteEntriesWriteDxCh(baseRouteEntryIndex,routeEntriesArray,numOfRouteEntries);
    UT_TEST_RC_CHECK(rc);
#endif /*CHX_FAMILY*/

    return rc;
}

/*******************************************************************************
* utTestIpCntGet
*
* DESCRIPTION:
*       Return the IP counter set requested.
*
* APPLICABLE DEVICES: DxCh2 and above.
*
* INPUTS:
*       cntSet - counters set to retrieve.
*
* OUTPUTS:
*       countersPtr - (pointer to) counter values
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - on error
*       GT_NOT_INITIALIZED       - The library was not initialized.
*       GT_BAD_PARAM             - Illegal parameter in function called
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This function doesn't handle counters overflow.
*
*******************************************************************************/
GT_STATUS utTestIpCntGet
(
    IN  CPSS_IP_CNT_SET_ENT          cntSet,
    OUT UT_TEST_IP_COUNTER_SET_STC  *countersPtr
)
{
    GT_STATUS rc = GT_OK;

    UT_TEST_PARAM_NUT_USED(cntSet);
    UT_TEST_PARAM_NUT_USED(countersPtr);

#ifdef EXMXPM_FAMILY
#endif /*EXMXPM_FAMILY*/

#ifdef EX_FAMILY
#endif /*EX_FAMILY*/

#ifdef CHX_FAMILY
    rc = utTestIpCntGetDxCh(cntSet,countersPtr);
    UT_TEST_RC_CHECK(rc);
#endif /*CHX_FAMILY*/

    return rc;
}
/*******************************************************************************
* utTestIpCntSet
*
* DESCRIPTION:
*       set IP counter set requested.
*
* APPLICABLE DEVICES: DxCh2 and above.
*
* INPUTS:
*       cntSet - counters set to retrieve.
*
* OUTPUTS:
*       countersPtr - (pointer to) counter values
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - on error
*       GT_NOT_INITIALIZED       - The library was not initialized.
*       GT_BAD_PARAM             - Illegal parameter in function called
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This function doesn't handle counters overflow.
*
*******************************************************************************/
GT_STATUS utTestIpCntSet
(
    IN  CPSS_IP_CNT_SET_ENT          cntSet,
    IN  UT_TEST_IP_COUNTER_SET_STC  *countersPtr
)
{
    GT_STATUS rc = GT_OK;

    UT_TEST_PARAM_NUT_USED(cntSet);
    UT_TEST_PARAM_NUT_USED(countersPtr);

#ifdef EXMXPM_FAMILY
#endif /*EXMXPM_FAMILY*/

#ifdef EX_FAMILY
#endif /*EX_FAMILY*/

#ifdef CHX_FAMILY
    rc = utTestIpCntSetDxCh(cntSet,countersPtr);
    UT_TEST_RC_CHECK(rc);
#endif /*CHX_FAMILY*/

    return rc;
}

/*******************************************************************************
* utTestIpLpmIpv4UcPrefixAdd
*
* DESCRIPTION:
*   Creates a new or override an existing Ipv4 prefix in a Virtual Router for
*   the specified LPM DB.
*
* APPLICABLE DEVICES: All DxCh Devices.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router id.
*                         future support only , currently only vrId = 0 is
*                         supported.
*       ipAddr          - The destination IP address of this prefix.
*       prefixLen       - The number of bits that are actual valid in the ipAddr.
*       nextHopInfoPtr  - the route entry info associated with this UC prefix.
*       override        - override an existing entry for this mask if it already
*                         exists, or don't override and return error.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_RANGE         -  If prefix length is too big, or
*       GT_ERROR                 - If the vrId was not created yet, or
*       GT_OUT_OF_CPU_MEM        - If failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM         - If failed to allocate TCAM memory, or
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       To change the default prefix for the VR use prefixLen = 0.
*
*******************************************************************************/
GT_STATUS utTestIpLpmIpv4UcPrefixAdd
(
    IN GT_U32                                   lpmDBId,
    IN GT_U32                                   vrId,
    IN GT_IPADDR                                ipAddr,
    IN GT_U32                                   prefixLen,
    IN UT_TEST_IP_TCAM_ROUTE_ENTRY_INFO_UNT     *nextHopInfoPtr,
    IN GT_BOOL                                  override
)
{
    GT_STATUS rc = GT_OK;

    UT_TEST_PARAM_NUT_USED(lpmDBId);
    UT_TEST_PARAM_NUT_USED(vrId);
    UT_TEST_PARAM_NUT_USED(ipAddr);
    UT_TEST_PARAM_NUT_USED(prefixLen);
    UT_TEST_PARAM_NUT_USED(nextHopInfoPtr);
    UT_TEST_PARAM_NUT_USED(override);

#ifdef EXMXPM_FAMILY
#endif /*EXMXPM_FAMILY*/

#ifdef EX_FAMILY
#endif /*EX_FAMILY*/

#ifdef CHX_FAMILY
    rc = utTestIpLpmIpv4UcPrefixAddDxCh(lpmDBId,vrId,ipAddr,prefixLen,nextHopInfoPtr,override);
    UT_TEST_RC_CHECK(rc);
#endif /*CHX_FAMILY*/

    return rc;
}


/*******************************************************************************
* utTrafficDemoTestRun
*
* DESCRIPTION:
*       run a test - 01.05 Tunnel Start: Ipv4 over MPLS - Basic
*
* INPUTS:
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS UT_TEST_NAME_MAC(1,5)
( /*01.05 Tunnel Start: Ipv4 over MPLS - Basic*/
    void
)
{
    GT_STATUS   rc;
    GT_U32      phase = 0;
    GT_U32      ii,jj;
    UT_VLAN_ID  vidArray[2] = {5,6};
    CPSS_INTERFACE_INFO_STC portsArray[2][2] = {{UT_TEST_PORT_0_0,
                                                 UT_TEST_PORT_0_8},
                                                {UT_TEST_PORT_0_18,
                                                 UT_TEST_PORT_0_23}};
    GT_BOOL     taggedArray[2][2] = {{GT_FALSE,GT_FALSE},
                                     {GT_FALSE,GT_TRUE}};
    GT_U32  vlanNumPorts = 2;
    UT_TEST_BRG_VLAN_INFO_STC   vlanParam;
    CPSS_MAC_ENTRY_EXT_STC      macEntry = UT_TEST_MAC_ENTRY_DEFAULT_MAC;
    UT_MAC_ADDR mac3402 = UT_TEST_MAC_ADDR_DA_000000003402_CNS;
    UT_MAC_ADDR mac3405 = UT_TEST_MAC_ADDR_DA_000000003402_CNS;
    UT_MAC_ADDR macSa0001 = UT_TEST_MAC_ADDR_SA_000000000001_CNS;
    GT_U32                              tsIndex;
    CPSS_TUNNEL_TYPE_ENT                tunnelType;
    UT_TEST_TUNNEL_START_CONFIG_UNT     tsEntry;
    UT_MAC_ADDR tsMac = {0x66,0x77,0x88,0x11,0x22,0x33};
    UT_TEST_IP_CNT_SET_MODE_ENT              cntSetMode;
    UT_TEST_IP_COUNTER_SET_INTERFACE_CFG_STC interfaceModeCfg;
    GT_U32                          baseRouteEntryIndex;
    UT_TEST_IP_UC_ROUTE_ENTRY_STC   routeEntriesArray[1];
    GT_U32                          numOfRouteEntries;
    GT_U32                                   lpmDBId;
    GT_U32                                   vrId;
    GT_IPADDR                                ipAddr;
    GT_U32                                   prefixLen;
    UT_TEST_IP_TCAM_ROUTE_ENTRY_INFO_UNT     ucNextHop;
    GT_BOOL                                  override;
    UT_TRAFFIC_ENGINE_PACKET_STC    *packetInfoPtr;/* pointer to the packet info */
    GT_U32                          burstCount;/* number of frames to send in the burst */
    GT_U32                          numVfd;/* number of VFDs (override of the background in specific offsets) */
    UT_TRAFFIC_ENGINE_VFD_STC       *vfdArrayPtr;/* pointer to array of VFDs :
                                                        {Mode,
                                                         Offset,
                                                         Pattern
                                                         CycleCount}*/
    CPSS_PORT_MAC_COUNTER_SET_STC   counters[4];/* counters for the 4 ports */
    UT_TEST_COMPARE_PARAM_STC       compareParams;
    GT_U32                          numCompares = 1;/* number of compares */
    CPSS_IP_CNT_SET_ENT             ipCounterSet; /* IP counter set */
    UT_TEST_IP_COUNTER_SET_STC      ipCounterSetValues;/* IP counter set values*/
    UT_TEST_IP_COUNTER_SET_STC      zeroIpCounterSetValues;/* counters with zero value */
    GT_U32                          currentFrameInCpu;/* number of frames received in CPU*/
    GT_U32                          matchingFrames;/* number of frames that I wait for */
    GT_U32                          extraInfo;
    GT_U32                          egressTsPacketLen;/* length of the TS packet that should egress from the port (without CRC)*/
    GT_U32                          payloadLen;/*length of the payload (passenger)*/
    GT_BOOL                         getFirst;/* parameter for get first frame or next */
    UT_TRAFFIC_ENGINE_VFD_STC       triggerVfdArray[2];/* VFDs for triggering */
    GT_U32                          numVfdTriggers = 2;/* number of VFDs for triggering */
    GT_U32                          numActualTriggers;/* number of actual packets that meet the trigger */

    /* init Array */
    UT_TEST_RESET_ARRAY_MAC(routeEntriesArray);

    /* init parameters */
    UT_TEST_RESET_PARAM_MAC(vlanParam);
    UT_TEST_RESET_PARAM_MAC(tsEntry);
    UT_TEST_RESET_PARAM_MAC(zeroIpCounterSetValues);
    UT_TEST_RESET_PARAM_MAC(ucNextHop);


    /* set port 0 as the port that traffic ingress from it */
    utTrafficGeneratorPortLoopbackModeEnableSet(&portsArray[0][0],/*port 0*/
                                                GT_TRUE);

    /* force link up on all ports in test */
    utTrafficGeneratorPortForceLinkUpEnableSet(&portsArray[0][0],/*port 0*/
                                                GT_TRUE);
    utTrafficGeneratorPortForceLinkUpEnableSet(&portsArray[0][1],/*port 8*/
                                                GT_TRUE);
    utTrafficGeneratorPortForceLinkUpEnableSet(&portsArray[1][0],/*port 18*/
                                                GT_TRUE);
    utTrafficGeneratorPortForceLinkUpEnableSet(&portsArray[1][1],/*port 23*/
                                                GT_TRUE);


    utTestTitleTrace(GT_TRUE,"Bug #87117 01.05 Tunnel Start: Ipv4 over MPLS - Basic",
                     NULL);

    /* phase 0 */
    utTestPhaseTrace(UT_TEST_PHASE(phase++),"Create vlan[%d] with ports (%d,%d), (%d,%d) and vlan [%d] with ports (%d,%d), (%d,%d)",
                    vidArray[0],
                    portsArray[0][0].devPort.devNum,
                    portsArray[0][0].devPort.portNum,
                    portsArray[0][1].devPort.devNum,
                    portsArray[0][1].devPort.portNum,
                    vidArray[1],
                    portsArray[1][0].devPort.devNum,
                    portsArray[1][0].devPort.portNum,
                    portsArray[1][1].devPort.devNum,
                    portsArray[1][1].devPort.portNum
                    );


    /* set the vlans with the needed ports */
    for(ii = 0 ; ii < 2 ; ii++)
    {
        rc = utTestBrgVlanEntryWrite(vidArray[ii],vlanNumPorts,
                                portsArray[ii],taggedArray[ii],
                                &vlanParam);
        UT_TEST_RC_CHECK(rc);
    }


    /* phase 1 */
    utTestPhaseTrace(UT_TEST_PHASE(phase++),"CPU Capture");

    rc = utTrafficRxSetMode(UT_TRAFFIC_RX_CYCLIC_E,
                        0x600,/*buferSize*/
                        0x30);  /*numOfEnteries*/
    UT_TEST_RC_CHECK(rc);

    rc = utTrafficRxPktClearTbl();
    UT_TEST_RC_CHECK(rc);

    rc = utTrafficRxCapture(GT_TRUE);
    UT_TEST_RC_CHECK(rc);

    /* phase 2 */
    utTestPhaseTrace(UT_TEST_PHASE(phase++),"Set FDB Table");

    rc = utTestBrgVlanLearningStateSet(vidArray[0],GT_TRUE);
    UT_TEST_RC_CHECK(rc);

    UT_TEST_MAC_ADDR_SET(macEntry.key.key.macVlan.macAddr.arEther,
                         mac3402);

    macEntry.key.key.macVlan.vlanId = vidArray[0];
    macEntry.dstInterface = portsArray[0][1];/*port 8*/
    macEntry.isStatic = GT_TRUE;
    macEntry.daRoute = GT_TRUE;
    macEntry.age = GT_TRUE;

    rc = utTestBrgFdbMacEntrySet(&macEntry);
    UT_TEST_RC_CHECK(rc);

    /* phase 3 */
    utTestPhaseTrace(UT_TEST_PHASE(phase++),"Configure TS");

    tsIndex = 8;
    tunnelType = CPSS_TUNNEL_X_OVER_MPLS_E;
    tsEntry.mplsCfg.tagEnable = GT_TRUE;
    tsEntry.mplsCfg.vlanId = vidArray[1];
    UT_TEST_MAC_ADDR_SET(tsEntry.mplsCfg.macDa.arEther,
                         tsMac);
    tsEntry.mplsCfg.numLabels = 1;
    tsEntry.mplsCfg.ttl = 0x33;

    tsEntry.mplsCfg.label1 = 3;
    tsEntry.mplsCfg.exp1 = 5;
    /* WA for bug in simulation */
    tsEntry.mplsCfg.label2 = tsEntry.mplsCfg.label1;
    tsEntry.mplsCfg.exp2 = tsEntry.mplsCfg.exp1;

    rc = utTestTunnelStartEntrySet(tsIndex,tunnelType,&tsEntry);
    UT_TEST_RC_CHECK(rc);

    /* phase 4 */
    utTestPhaseTrace(UT_TEST_PHASE(phase++),"Set IP Counters Mode");

    /* set all IP counter to count only matching from the Router */
    cntSetMode = UT_TEST_IP_CNT_SET_ROUTE_ENTRY_MODE_E;

    /* check IP counters */
    for(ipCounterSet = CPSS_IP_CNT_SET0_E ; ipCounterSet <= CPSS_IP_CNT_SET3_E ; ipCounterSet++)
    {
        rc = utTestIpCntSetModeSet(ipCounterSet, cntSetMode,&interfaceModeCfg);
        UT_TEST_RC_CHECK(rc);
    }

    /* phase 5 */
    utTestPhaseTrace(UT_TEST_PHASE(phase++),"Enable Routing");

    rc = utTestIpPortRoutingEnable(portsArray[0][0].devPort.devNum,
                                   portsArray[0][0].devPort.portNum,
                                   CPSS_IP_UNICAST_E,
                                   CPSS_IP_PROTOCOL_IPV4_E,
                                   GT_TRUE);
    UT_TEST_RC_CHECK(rc);

    rc = utTestBrgVlanIpUcRouteEnable(portsArray[0][0].devPort.devNum,
                                      vidArray[0],
                                      CPSS_IP_PROTOCOL_IPV4_E,
                                      GT_TRUE);
    UT_TEST_RC_CHECK(rc);


    /* phase 6 */
    utTestPhaseTrace(UT_TEST_PHASE(phase++),"IP Configuration");

    baseRouteEntryIndex = 5;
    numOfRouteEntries = 1;

    routeEntriesArray[0].type = UT_TEST_IP_UC_ROUTE_ENTRY_E;
    if(forceTestFailForDemo)
    {
        /* don't do routing , trap it to CPU */
        routeEntriesArray[0].entry.regularEntry.cmd = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
    }
    else
    {
        routeEntriesArray[0].entry.regularEntry.cmd = CPSS_PACKET_CMD_ROUTE_E;
    }
    routeEntriesArray[0].entry.regularEntry.isTunnelStart = GT_TRUE;
    routeEntriesArray[0].entry.regularEntry.nextHopVlanId = vidArray[1];
    routeEntriesArray[0].entry.regularEntry.nextHopInterface = portsArray[1][1];/*port 23*/
    routeEntriesArray[0].entry.regularEntry.nextHopTunnelPointer = 8;

    rc = utTestIpUcRouteEntriesWrite(baseRouteEntryIndex,routeEntriesArray,numOfRouteEntries);
    UT_TEST_RC_CHECK(rc);

    lpmDBId = 0;
    vrId = 0;
    ipAddr.arIP[0] = 0x01;
    ipAddr.arIP[1] = 0x01;
    ipAddr.arIP[2] = 0x01;
    ipAddr.arIP[3] = 0x03;
    prefixLen = 32;
    override = GT_TRUE;/* to allow running test several times without 'reset' */

    /* keep all value of ucNextHop.ipLttEntry = 0
       set the rout index */
    ucNextHop.ipLttEntry.routeEntryBaseIndex = baseRouteEntryIndex;

    /* set LTT entry */
    rc = utTestIpLpmIpv4UcPrefixAdd(lpmDBId,vrId,ipAddr,prefixLen,&ucNextHop,override);
    UT_TEST_RC_CHECK(rc);

    /* phase 7 */
    utTestPhaseTrace(UT_TEST_PHASE(phase++),"Set Trigger");

    /* set port 23 with 'capture' .
       by this we can check triggers on traffic captured ...*/
    rc = utTrafficGeneratorPortTxEthCaptureSet(&portsArray[1][1],GT_TRUE);
    UT_TEST_RC_CHECK(rc);

    /* trigger on the DA mac of the packet (from the TS entry) */
    triggerVfdArray[0].mode = UT_TRAFFIC_ENGINE_VFD_MODE_STATIC_E;
    triggerVfdArray[0].offset = 0;
    triggerVfdArray[0].patternPtr[0] = tsMac[0];
    triggerVfdArray[0].patternPtr[1] = tsMac[1];
    triggerVfdArray[0].patternPtr[2] = tsMac[2];
    triggerVfdArray[0].patternPtr[3] = tsMac[3];
    triggerVfdArray[0].patternPtr[4] = tsMac[4];
    triggerVfdArray[0].patternPtr[5] = tsMac[5];
    triggerVfdArray[0].cycleCount = 6;

    triggerVfdArray[1].mode = UT_TRAFFIC_ENGINE_VFD_MODE_STATIC_E;
    triggerVfdArray[1].offset = 12+4;/* after mac sa,da and vlan tag */
    triggerVfdArray[1].patternPtr[0] = 0x88;/* the MPLS etherType */
    triggerVfdArray[1].patternPtr[1] = 0x47;/* the MPLS etherType */
    triggerVfdArray[1].patternPtr[2] = 0x00;/* the MPLS tunnel label */
    triggerVfdArray[1].patternPtr[3] = 0x00;/* the MPLS tunnel label */
    triggerVfdArray[1].patternPtr[4] = 0x3B;/* the MPLS label bytes */
    triggerVfdArray[1].patternPtr[5] = 0x33;/* the MPLS label bytes */
    triggerVfdArray[1].cycleCount = 6;

    /* phase 8 */
    utTestPhaseTrace(UT_TEST_PHASE(phase++),"Reset Counters");

    for(ii = 0 ; ii < 2 ; ii++)
    {
        for(jj = 0 ; jj < 2 ; jj++)
        {
            rc = utTrafficGeneratorPortCountersEthReset(&portsArray[ii][jj]);
            UT_TEST_RC_CHECK(rc);
        }
    }

    /* phase 9 */
    utTestPhaseTrace(UT_TEST_PHASE(phase++),"Generate Traffic");

    /* attach the packet info to already structured packet */
    packetInfoPtr = utTestPacketIpv4VlanTaggedPtr;
    packetInfoPtr->totalLen = 60;/* not include CRC */
    packetInfoPtr->partsArray = utTestPacketIpv4VlanTaggedPartsArray;

    /* set the values needed in the packet */

    /* set L2 parameter */
    UT_TEST_MAC_ADDR_SET(utTestPacketL2Part.daMac,
                         mac3402);

    UT_TEST_MAC_ADDR_SET(utTestPacketL2Part.saMac,
                         macSa0001);

    /* set vlan tag parameter */
    utTestPacketVlanTagPart.vpt = 3;
    utTestPacketVlanTagPart.cfi = 0;
    utTestPacketVlanTagPart.vid = vidArray[0];/* vid 5 */

    /* set ether type parameter */
    utTestPacketEtherTypePart.etherType = UT_ETHERTYPE_0800_IPV4_TAG_CNS;
    /* keep the pay load unchanged -- bind it to IPv4 as payload --
       because Engine not support parsing IPv4 yet ... */
    utTestPacketPayLoadPart.bytesPtr = utTestPacketPayLoadIpv4Bytes;

    burstCount = 1;
    numVfd = 0;
    vfdArrayPtr = NULL;

    /* transmit the traffic */
    rc = utTrafficGeneratorPortTxEthTransmit(&portsArray[0][0],/*port 0*/
                                        packetInfoPtr,
                                        burstCount,
                                        numVfd,
                                        vfdArrayPtr);
    UT_TEST_RC_CHECK(rc);


    /* phase 10 */
    utTestPhaseTrace(UT_TEST_PHASE(phase++),"Check Results");

    payloadLen = packetInfoPtr->totalLen - (12 + 4 + 2); /* mac + vlan + ethertype */

    egressTsPacketLen = 12 + /* mac addresses */
                        ((tsEntry.mplsCfg.tagEnable == GT_TRUE) ? 4 : 0) + /*vlan tag */
                        2 + /* mpls ether type */
                        (tsEntry.mplsCfg.numLabels * 4) +/* MPLS labels*/
                        payloadLen;

    for(ii = 0 ; ii < 2 ; ii++)
    {
        for(jj = 0 ; jj < 2 ; jj++)
        {
            rc = utTrafficGeneratorPortCountersEthRead(&portsArray[ii][jj],
                    &counters[(ii * 2) + jj]);
            UT_TEST_RC_CHECK(rc);
        }
    }

    /* absolute diff by value */
    compareParams.type      = UT_TEST_COMPARE_TYPE_VALUE_E;
    compareParams.operator  = UT_TEST_COMPARE_OPERATOR_DIFF_ABSOLUTE_E;
    compareParams.value     = 0;

    extraInfo = UT_TEST_EXPECTED_EXTRA_INFO_UNUSED_CNS;

    /* port 0 */
    rc = utTestExpectedResultCheck(UT_TEST_FIELD_NAME_AND_VALUE_MAC(counters[0].goodPktsRcv.l[0]),burstCount,numCompares,&compareParams,extraInfo);
    UT_TEST_RC_CHECK(rc);
    rc = utTestExpectedResultCheck(UT_TEST_FIELD_NAME_AND_VALUE_MAC(counters[0].goodPktsRcv.l[1]),0,numCompares,&compareParams,extraInfo);
    UT_TEST_RC_CHECK(rc);

    /* port 8 */
    rc = utTestExpectedResultCheck(UT_TEST_FIELD_NAME_AND_VALUE_MAC(counters[1].goodPktsSent.l[0]),0,numCompares,&compareParams,extraInfo);
    UT_TEST_RC_CHECK(rc);
    rc = utTestExpectedResultCheck(UT_TEST_FIELD_NAME_AND_VALUE_MAC(counters[1].goodPktsSent.l[1]),0,numCompares,&compareParams,extraInfo);
    UT_TEST_RC_CHECK(rc);

    /* port 18 */
    rc = utTestExpectedResultCheck(UT_TEST_FIELD_NAME_AND_VALUE_MAC(counters[2].goodPktsSent.l[0]),0,numCompares,&compareParams,extraInfo);
    UT_TEST_RC_CHECK(rc);
    rc = utTestExpectedResultCheck(UT_TEST_FIELD_NAME_AND_VALUE_MAC(counters[2].goodPktsSent.l[1]),0,numCompares,&compareParams,extraInfo);
    UT_TEST_RC_CHECK(rc);

    /* port 23 */
    /* check packets */
    rc = utTestExpectedResultCheck(UT_TEST_FIELD_NAME_AND_VALUE_MAC(counters[3].goodPktsSent.l[0]),burstCount,numCompares,&compareParams,extraInfo);
    UT_TEST_RC_CHECK(rc);
    rc = utTestExpectedResultCheck(UT_TEST_FIELD_NAME_AND_VALUE_MAC(counters[3].goodPktsSent.l[1]),0,numCompares,&compareParams,extraInfo);
    UT_TEST_RC_CHECK(rc);

    /* check bytes */
    rc = utTestExpectedResultCheck(UT_TEST_FIELD_NAME_AND_VALUE_MAC(counters[3].goodOctetsSent.l[0]),
        burstCount*(egressTsPacketLen+UT_CRC_LEN_CNS),numCompares,&compareParams,extraInfo);
    UT_TEST_RC_CHECK(rc);
    rc = utTestExpectedResultCheck(UT_TEST_FIELD_NAME_AND_VALUE_MAC(counters[3].goodOctetsSent.l[1]),0,numCompares,&compareParams,extraInfo);
    UT_TEST_RC_CHECK(rc);

    /* get and check triggers on the port */
    rc = utTrafficGeneratorPortTxEthTriggerCountersGet(&portsArray[1][1],numVfdTriggers, triggerVfdArray,&numActualTriggers);
    UT_TEST_RC_CHECK(rc);

    rc = utTestExpectedResultCheck(UT_TEST_FIELD_NAME_AND_VALUE_MAC(numActualTriggers),burstCount,numCompares,&compareParams,extraInfo);
    UT_TEST_RC_CHECK(rc);


    /* check IP counters */
    for(ipCounterSet = CPSS_IP_CNT_SET0_E ; ipCounterSet <= CPSS_IP_CNT_SET3_E ; ipCounterSet++)
    {
        rc = utTestIpCntGet(ipCounterSet,&ipCounterSetValues);
        UT_TEST_RC_CHECK(rc);

        extraInfo = ipCounterSet;/* extra info to know , which counter failed in the loop */
        rc = utTestExpectedResultCheck(UT_TEST_FIELD_NAME_AND_VALUE_MAC(ipCounterSetValues.inUcPkts),
                    (ipCounterSet == CPSS_IP_CNT_SET0_E) ? burstCount : 0,
                    numCompares,&compareParams,extraInfo);
        UT_TEST_RC_CHECK(rc);

        /* set the counters to 0 so we can run again the test */
        rc = utTestIpCntSet(ipCounterSet,&zeroIpCounterSetValues);
        UT_TEST_RC_CHECK(rc);
    }

    extraInfo = UT_TEST_EXPECTED_EXTRA_INFO_UNUSED_CNS;

    /* phase 11 */
    utTestPhaseTrace(UT_TEST_PHASE(phase++),"config unknown UC in vlan to CPU");

    /* add a reason for a packet to go to the CPU ,
       so we can test the 'CPU Rx table' */
    vlanParam.unkUcastCmd = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;

    /* set the vlans with the needed ports */
    for(ii = 0 ; ii < 2 ; ii++)
    {
        rc = utTestBrgVlanEntryWrite(vidArray[ii],vlanNumPorts,
                                portsArray[ii],taggedArray[ii],
                                &vlanParam);
        UT_TEST_RC_CHECK(rc);
    }

    /* phase 11 */
    utTestPhaseTrace(UT_TEST_PHASE(phase++),"send unkUc in vlan[%d]",vidArray[0]);
    mac3405[5] = 0x05;
    /* set L2 parameter */
    UT_TEST_MAC_ADDR_SET(utTestPacketL2Part.daMac,
                         mac3405);

    /* transmit the traffic */
    rc = utTrafficGeneratorPortTxEthTransmit(&portsArray[0][0],/*port 0*/
                                        packetInfoPtr,
                                        burstCount,
                                        numVfd,
                                        vfdArrayPtr);
    UT_TEST_RC_CHECK(rc);

    utTestPhaseTrace(UT_TEST_PHASE(phase++),"check Rx packets at CPU");

    currentFrameInCpu = 0;
    matchingFrames = 0;
    getFirst = GT_TRUE;
    /* check packets sent to CPU */
    while(1)
    {
        GT_U32 packetActualLength;
        GT_U8   packetBuff[UT_TRAFFIC_RX_BUFFER_MAX_SIZE_CNS];
        GT_U32 buffLen = UT_TRAFFIC_RX_BUFFER_MAX_SIZE_CNS;
        GT_U8  devNum;
        GT_U8  queue;
        UT_TRAFFIC_GENERATOR_NET_RX_PARAMS_STC rxParam;

        rc = utTrafficGeneratorRxInCpuGet(
                    UT_TRAFFIC_GENERATOR_PACKET_TYPE_REGULAR_E,
                    getFirst, packetBuff,
                    &buffLen, &packetActualLength,
                    &devNum,&queue,
                    &rxParam);
        if(rc != GT_OK)
        {
            break;
        }

        getFirst = GT_FALSE; /* now we get the next */

        extraInfo = currentFrameInCpu;/* extra info to know , which frame failed in the loop */
        currentFrameInCpu++;

        /* check the RX parameters (if needed) */

        /* check that the DSA is 'TO_CPU' */
        rc = utTestExpectedResultCheck(UT_TEST_FIELD_NAME_AND_VALUE_MAC(rxParam.dsaParam.dsaType),
            UT_TRAFFIC_GENERATOR_NET_DSA_CMD_TO_CPU_E,numCompares,&compareParams,extraInfo);
        UT_TEST_RC_CHECK(rc);

        /* check that the packet came from the needed port,device (not trunk) */
        rc = utTestExpectedResultCheck(UT_TEST_FIELD_NAME_AND_VALUE_MAC(rxParam.dsaParam.dsaInfo.toCpu.srcIsTrunk),
            0,numCompares,&compareParams,extraInfo);
        UT_TEST_RC_CHECK(rc);

        /* check that the packet came from the needed port,device (not trunk) */
        rc = utTestExpectedResultCheck(UT_TEST_FIELD_NAME_AND_VALUE_MAC(rxParam.dsaParam.dsaInfo.toCpu.devNum),
            portsArray[0][0].devPort.devNum,numCompares,&compareParams,extraInfo);
        UT_TEST_RC_CHECK(rc);

        /* check that the packet came from the needed port,device (not trunk) */
        rc = utTestExpectedResultCheck(UT_TEST_FIELD_NAME_AND_VALUE_MAC(rxParam.dsaParam.dsaInfo.toCpu.interface.portNum),
            portsArray[0][0].devPort.portNum,numCompares,&compareParams,extraInfo);
        UT_TEST_RC_CHECK(rc);

        if(rxParam.dsaParam.dsaInfo.toCpu.srcIsTrunk == 0 &&
           rxParam.dsaParam.dsaInfo.toCpu.devNum == portsArray[0][0].devPort.devNum &&
           rxParam.dsaParam.dsaInfo.toCpu.interface.portNum == portsArray[0][0].devPort.portNum)
        {
            /* the packet came from where I expected it to be */
            matchingFrames++;
        }
    }

    extraInfo = UT_TEST_EXPECTED_EXTRA_INFO_UNUSED_CNS;
    /*check that we got burstCount packets*/
    rc = utTestExpectedResultCheck(UT_TEST_FIELD_NAME_AND_VALUE_MAC(matchingFrames),
        burstCount,numCompares,&compareParams,extraInfo);
    UT_TEST_RC_CHECK(rc);

    /* trace the 'End of test' */
    utTestTitleTrace(GT_FALSE,NULL,NULL);

    return GT_OK;
}

/*******************************************************************************
* utTrafficDemoTestRun
*
* DESCRIPTION:
*       run a test - 01.05 Tunnel Start: Ipv4 over MPLS - Basic
*           running it with configuration that will fail it --> for DEMO purposes
*
* INPUTS:
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS UT_TEST_NAME_MAC(1,1005)
( /*01.05 Tunnel Start: Ipv4 over MPLS - Basic*/
    void
)
{
    GT_STATUS rc;

    forceTestFailForDemo = 1;
    rc = UT_TEST_NAME_MAC(1,5)();
    forceTestFailForDemo = 0;

    return rc;
}


/*******************************************************************************
* utTrafficDemoTestRun
*
* DESCRIPTION:
*       run a test
*
* INPUTS:
*       testClass - the class of test
*       testNum   - number of the test (in the class)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_NOT_IMPLEMENTED - test number not implemented
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS utTrafficDemoTestRun(
    IN GT_U32   testClass,
    IN GT_U32   testNum
)
{

    switch(testClass)
    {
        case 1:
            switch(testNum)
            {
                case 5:
                    return UT_TEST_NAME_MAC(1,5)();
                case 1005:
                    return UT_TEST_NAME_MAC(1,1005)();
                default:
                    break;
            }
            break;
        default:
            break;
    }

    return GT_NOT_IMPLEMENTED;
}

