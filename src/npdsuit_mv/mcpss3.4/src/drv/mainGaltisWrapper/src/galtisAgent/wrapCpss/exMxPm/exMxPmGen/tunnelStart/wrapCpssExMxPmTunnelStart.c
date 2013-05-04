/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxPmTunnelStart.c
*
* DESCRIPTION:
*       TODO: Add proper description of this file here
*
*       DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

/* Common galtis includes */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>

/* Feature specific includes. */
#include <cpss/generic/cpssTypes.h>
#include <cpss/exMxPm/exMxPmGen/tunnelStart/cpssExMxPmTunnelStart.h>

/*******************************************************************************
* cpssExMxPmTunnelStartEgressFilteringEnableSet
*
* DESCRIPTION:
*       Sets globally whether Tunnel Start packets are subject to egress VLAN
*       filtering and egress Spanning Tree filtering.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum            - device number
*       enable            - GT_TRUE enable filtering
*                           GT_FALSE disable filtering
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - wrong device number
*       GT_HW_ERROR       - on hardware error
*       GT_FAIL           - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTunnelStartEgressFilteringEnableSet
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_BOOL enable;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmTunnelStartEgressFilteringEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmTunnelStartEgressFilteringEnableGet
*
* DESCRIPTION:
*       Gets whether Tunnel Start packets are subject to egress VLAN filtering
*       and egress Spanning Tree filtering
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum            - device number
*
* OUTPUTS:
*       enablePtr         - points to enable/disable
*                           GT_TRUE filtering is enabled
*                           GT_FALSE filtering is disabled
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - wrong device number
*       GT_BAD_PTR        - on NULL pointer
*       GT_HW_ERROR       - on hardware error
*       GT_FAIL           - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTunnelStartEgressFilteringEnableGet
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_BOOL enable;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmTunnelStartEgressFilteringEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

static GT_BOOL isExtended=GT_FALSE;/*indication if Puma 2 table is used*/

/*******************************************************************************
* cpssExMxPmTunnelStartEntrySet
*
* DESCRIPTION:
*       Set a tunnel start entry.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*   devNum                        - device number
*   routerArpTunnelStartLineIndex - index of the Tunnel start entry.
*                                   in FDB split and shared mode range (0-4095),
*                                   in FDB external mode range (0-16383).
*   tunnelType                    - type of the tunnel; valid options:
*                                   CPSS_TUNNEL_X_OVER_IPV4_E,
*                                   CPSS_TUNNEL_X_OVER_GRE_IPV4_E,
*                                   CPSS_TUNNEL_X_OVER_MPLS_E,
*                                   CPSS_TUNNEL_MAC_IN_MAC_E
*   entryPtr                      - points to tunnel start entry configuration
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on failure
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Bridge Forwarding Database (FDB table) has few memory modes that
*       determines the memory placement of the FDB table, router ARP addresses
*       and tunnel start entries:
*         - split mode
*           In this mode the FDB resides in internal FDB table and ARP and
*           tunnel start entries share the ARP/tunnel table.
*         - shared mode
*           In this mode the FDB and ARP share the FDB table and ARP/tunnel
*           table is occupied only by tunnel start entries.
*         - external mode
*           In this mode the FDB resides in external memory, the ARP/tunnel
*           start entries resides in internal FDB table.
*       FDB memory modes can be found under
*       CPSS_EXMXPM_PP_CONFIG_FDB_AND_ARP_TABLE_MODE_ENT
*       The ARP/tunnel start table contains 4K lines.
*       Each line can hold:
*       - 1 tunnel start entry.
*       - 4 router ARP addresses entries.
*       Indexes for tunnel start range are (0..4095); Indexes for router ARP
*       addresses range are (0..16383).
*       When ARP and tunnel start entries share the same memory, then tunnel
*       start entry at index n and router ARP addresses at indexes
*       4*n..4*n+3 share the same memory. For example tunnel start entry at
*       index 100 and router ARP addresses at indexes 400..403 share the same
*       physical memory.
*       When in external mode the FDB table can hold tunnel start and ARP
*       entries. Indexes for tunnel start range are (0..16383); Indexes
*       for router ARP addresses range are (0..65535).
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTunnelStartEntrySet_IPV4_CONFIG
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 routerArpTunnelStartLineIndex;
    CPSS_TUNNEL_TYPE_ENT tunnelType;
    CPSS_EXMXPM_TUNNEL_START_ENTRY_UNT entry;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum =(GT_U8)inArgs[0];

    /*exit if it is not ip*/

    if (inArgs[3]&&!isExtended)
    {
        galtisOutput(outArgs, GT_BAD_STATE, "%d", -1);
        return CMD_OK;
    }

    /*get fields*/
    routerArpTunnelStartLineIndex=(GT_U16)inFields[0];
    tunnelType=(CPSS_TUNNEL_TYPE_ENT)inFields[1];
    entry.ipv4Cfg.tagEnable=(GT_BOOL)inFields[2];
    entry.ipv4Cfg.vlanId=(GT_U16)inFields[3];
    entry.ipv4Cfg.upMarkMode=(CPSS_EXMXPM_TUNNEL_START_MARK_MODE_ENT)inFields[4];
    entry.ipv4Cfg.up=(GT_U32)inFields[5];
    entry.ipv4Cfg.dscpMarkMode=(CPSS_EXMXPM_TUNNEL_START_MARK_MODE_ENT)inFields[6];
    entry.ipv4Cfg.dscp=(GT_U32)inFields[7];
    galtisMacAddr(&(entry.ipv4Cfg.macDa),(GT_U8*) inFields[8]);
    entry.ipv4Cfg.dontFragmentFlag=(GT_BOOL)inFields[9];
    entry.ipv4Cfg.ttl=(GT_U32)inFields[10];
    entry.ipv4Cfg.autoTunnel=(GT_BOOL)inFields[11];
    entry.ipv4Cfg.autoTunnelOffset=(GT_U32)inFields[12];
    entry.ipv4Cfg.ethType=(CPSS_TUNNEL_GRE_ETHER_TYPE_ENT)inFields[13];
    galtisIpAddr(&(entry.ipv4Cfg.destIp),(GT_U8*) inFields[14]);
    galtisIpAddr(&(entry.ipv4Cfg.srcIp),(GT_U8*) inFields[15]);
    entry.ipv4Cfg.retainCrc=(GT_BOOL)inFields[16];


    /* call cpss api function */
    result = cpssExMxPmTunnelStartEntrySet(devNum, routerArpTunnelStartLineIndex, tunnelType, &entry);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    isExtended=GT_FALSE;

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmTunnelStartEntrySet
*
* DESCRIPTION:
*       Set a tunnel start entry.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*   devNum                        - device number
*   routerArpTunnelStartLineIndex - index of the Tunnel start entry.
*                                   in FDB split and shared mode range (0-4095),
*                                   in FDB external mode range (0-16383).
*   tunnelType                    - type of the tunnel; valid options:
*                                   CPSS_TUNNEL_X_OVER_IPV4_E,
*                                   CPSS_TUNNEL_X_OVER_GRE_IPV4_E,
*                                   CPSS_TUNNEL_X_OVER_MPLS_E,
*                                   CPSS_TUNNEL_MAC_IN_MAC_E
*   entryPtr                      - points to tunnel start entry configuration
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on failure
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Bridge Forwarding Database (FDB table) has few memory modes that
*       determines the memory placement of the FDB table, router ARP addresses
*       and tunnel start entries:
*         - split mode
*           In this mode the FDB resides in internal FDB table and ARP and
*           tunnel start entries share the ARP/tunnel table.
*         - shared mode
*           In this mode the FDB and ARP share the FDB table and ARP/tunnel
*           table is occupied only by tunnel start entries.
*         - external mode
*           In this mode the FDB resides in external memory, the ARP/tunnel
*           start entries resides in internal FDB table.
*       FDB memory modes can be found under
*       CPSS_EXMXPM_PP_CONFIG_FDB_AND_ARP_TABLE_MODE_ENT
*       The ARP/tunnel start table contains 4K lines.
*       Each line can hold:
*       - 1 tunnel start entry.
*       - 4 router ARP addresses entries.
*       Indexes for tunnel start range are (0..4095); Indexes for router ARP
*       addresses range are (0..16383).
*       When ARP and tunnel start entries share the same memory, then tunnel
*       start entry at index n and router ARP addresses at indexes
*       4*n..4*n+3 share the same memory. For example tunnel start entry at
*       index 100 and router ARP addresses at indexes 400..403 share the same
*       physical memory.
*       When in external mode the FDB table can hold tunnel start and ARP
*       entries. Indexes for tunnel start range are (0..16383); Indexes
*       for router ARP addresses range are (0..65535).
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTunnelStartEntrySet__MPLS_CONFIG
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 routerArpTunnelStartLineIndex;
    CPSS_TUNNEL_TYPE_ENT tunnelType;
    CPSS_EXMXPM_TUNNEL_START_ENTRY_UNT entry;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum =(GT_U8)inArgs[0];

    /*exit if it is not mpls*/

    if (!inArgs[3]&&!isExtended)
    {
        galtisOutput(outArgs, GT_BAD_STATE, "%d", -1);
        return CMD_OK;
    }

    /*get fields*/
    routerArpTunnelStartLineIndex=(GT_U16)inFields[0];
    tunnelType=(CPSS_TUNNEL_TYPE_ENT)inFields[1];
    entry.mplsCfg.tagEnable=(GT_BOOL)inFields[2];
    entry.mplsCfg.vlanId=(GT_U16)inFields[3];
    entry.mplsCfg.upMarkMode=(CPSS_EXMXPM_TUNNEL_START_MARK_MODE_ENT)inFields[4];
    entry.mplsCfg.up=(GT_U32)inFields[5];
    galtisMacAddr(&(entry.mplsCfg.macDa),(GT_U8*) inFields[6]);
    entry.mplsCfg.numLabels=(GT_U32)inFields[7];
    entry.mplsCfg.ttl=(GT_U32)inFields[8];
    entry.mplsCfg.label1=(GT_U32)inFields[9];
    entry.mplsCfg.exp1MarkMode=(CPSS_EXMXPM_TUNNEL_START_MARK_MODE_ENT)inFields[10];
    entry.mplsCfg.exp1=(GT_U32)inFields[11];
    entry.mplsCfg.label2=(GT_U32)inFields[12];
    entry.mplsCfg.exp2MarkMode=(CPSS_EXMXPM_TUNNEL_START_MARK_MODE_ENT)inFields[13];
    entry.mplsCfg.exp2=(GT_U32)inFields[14];
    entry.mplsCfg.retainCrc=(GT_BOOL)inFields[15];

    if(!isExtended)
    /*TBD need to change - hard coded for puma*/
    entry.mplsCfg.ethType = CPSS_EXMXPM_TUNNEL_START_MPLS_ETHER_TYPE_UC_E;
    else
     entry.mplsCfg.ethType =(CPSS_EXMXPM_TUNNEL_START_MPLS_ETHER_TYPE_ENT)inFields[16]; 

    /* call cpss api function */
    result = cpssExMxPmTunnelStartEntrySet(devNum, routerArpTunnelStartLineIndex, tunnelType, &entry);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    isExtended=GT_FALSE;

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmTunnelStartEntrySet
*
* DESCRIPTION:
*       Set a tunnel start entry.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*   devNum                        - device number
*   routerArpTunnelStartLineIndex - index of the Tunnel start entry.
*                                   in FDB split and shared mode range (0-4095),
*                                   in FDB external mode range (0-16383).
*   tunnelType                    - type of the tunnel; valid options:
*                                   CPSS_TUNNEL_X_OVER_IPV4_E,
*                                   CPSS_TUNNEL_X_OVER_GRE_IPV4_E,
*                                   CPSS_TUNNEL_X_OVER_MPLS_E,
*                                   CPSS_TUNNEL_MAC_IN_MAC_E
*   entryPtr                      - points to tunnel start entry configuration
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on failure
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Bridge Forwarding Database (FDB table) has few memory modes that
*       determines the memory placement of the FDB table, router ARP addresses
*       and tunnel start entries:
*         - split mode
*           In this mode the FDB resides in internal FDB table and ARP and
*           tunnel start entries share the ARP/tunnel table.
*         - shared mode
*           In this mode the FDB and ARP share the FDB table and ARP/tunnel
*           table is occupied only by tunnel start entries.
*         - external mode
*           In this mode the FDB resides in external memory, the ARP/tunnel
*           start entries resides in internal FDB table.
*       FDB memory modes can be found under
*       CPSS_EXMXPM_PP_CONFIG_FDB_AND_ARP_TABLE_MODE_ENT
*       The ARP/tunnel start table contains 4K lines.
*       Each line can hold:
*       - 1 tunnel start entry.
*       - 4 router ARP addresses entries.
*       Indexes for tunnel start range are (0..4095); Indexes for router ARP
*       addresses range are (0..16383).
*       When ARP and tunnel start entries share the same memory, then tunnel
*       start entry at index n and router ARP addresses at indexes
*       4*n..4*n+3 share the same memory. For example tunnel start entry at
*       index 100 and router ARP addresses at indexes 400..403 share the same
*       physical memory.
*       When in external mode the FDB table can hold tunnel start and ARP
*       entries. Indexes for tunnel start range are (0..16383); Indexes
*       for router ARP addresses range are (0..65535).
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTunnelStartEntrySet__MIM_CONFIG
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 routerArpTunnelStartLineIndex;
    CPSS_TUNNEL_TYPE_ENT tunnelType;
    CPSS_EXMXPM_TUNNEL_START_ENTRY_UNT entry;



    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum =(GT_U8)inArgs[0];

    /*exit if it is not mpls*/

    if (!inArgs[3]&&!isExtended)
    {
        galtisOutput(outArgs, GT_BAD_STATE, "%d", -1);
        return CMD_OK;
    }

    /*get fields*/
    routerArpTunnelStartLineIndex=(GT_U16)inFields[0];
    tunnelType= CPSS_TUNNEL_MAC_IN_MAC_E;

    entry.mimCfg.tagEnable = (GT_BOOL)inFields[2];
    entry.mimCfg.vlanId = (GT_U16)inFields[3];
    entry.mimCfg.upMarkMode = (CPSS_EXMXPM_TUNNEL_START_MARK_MODE_ENT)inFields[4];
    entry.mimCfg.up = (GT_U32)inFields[5];
    galtisMacAddr(&(entry.mimCfg.macDa),(GT_U8*) inFields[6]);

    entry.mimCfg.ttl = (GT_U32)inFields[8];
    entry.mimCfg.retainCrc = (GT_BOOL)inFields[15];


    if(!isExtended)
       {
        /*TBD hard coded for MIM*/
          entry.mimCfg.iSid = 1;
          entry.mimCfg.iSidAssignMode = CPSS_EXMXPM_TUNNEL_START_MARK_FROM_ENTRY_E;
          entry.mimCfg.iUp = 2;
          entry.mimCfg.iUpMarkMode = CPSS_EXMXPM_TUNNEL_START_MARK_FROM_ENTRY_E;
          entry.mimCfg.iDp = 1;
          entry.mimCfg.iDpMarkMode = CPSS_EXMXPM_TUNNEL_START_MARK_FROM_ENTRY_E;
          entry.mimCfg.iTagReserved = 7;
       }
    else
        {
          entry.mimCfg.iSid =(GT_U32)inFields[7];
          entry.mimCfg.iSidAssignMode =(CPSS_EXMXPM_TUNNEL_START_MARK_MODE_ENT)inFields[9];
          entry.mimCfg.iUp =(GT_U32)inFields[10];
          entry.mimCfg.iUpMarkMode =(CPSS_EXMXPM_TUNNEL_START_MARK_MODE_ENT)inFields[11];
          entry.mimCfg.iDp =(GT_U32)inFields[12];
          entry.mimCfg.iDpMarkMode =(CPSS_EXMXPM_TUNNEL_START_MARK_MODE_ENT)inFields[13];
          entry.mimCfg.iTagReserved =(GT_U32)inFields[14];
         }
    


    /* call cpss api function */
    result = cpssExMxPmTunnelStartEntrySet(devNum, routerArpTunnelStartLineIndex, tunnelType, &entry);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    isExtended=GT_FALSE;

    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmTunnelStartEntrySet
*
* DESCRIPTION:
*       Set a tunnel start entry.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*   devNum                        - device number
*   routerArpTunnelStartLineIndex - index of the Tunnel start entry.
*                                   in FDB split and shared mode range (0-4095),
*                                   in FDB external mode range (0-16383).
*   tunnelType                    - type of the tunnel; valid options:
*                                   CPSS_TUNNEL_X_OVER_IPV4_E,
*                                   CPSS_TUNNEL_X_OVER_GRE_IPV4_E,
*                                   CPSS_TUNNEL_X_OVER_MPLS_E,
*                                   CPSS_TUNNEL_MAC_IN_MAC_E
*   entryPtr                      - points to tunnel start entry configuration
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on failure
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Bridge Forwarding Database (FDB table) has few memory modes that
*       determines the memory placement of the FDB table, router ARP addresses
*       and tunnel start entries:
*         - split mode
*           In this mode the FDB resides in internal FDB table and ARP and
*           tunnel start entries share the ARP/tunnel table.
*         - shared mode
*           In this mode the FDB and ARP share the FDB table and ARP/tunnel
*           table is occupied only by tunnel start entries.
*         - external mode
*           In this mode the FDB resides in external memory, the ARP/tunnel
*           start entries resides in internal FDB table.
*       FDB memory modes can be found under
*       CPSS_EXMXPM_PP_CONFIG_FDB_AND_ARP_TABLE_MODE_ENT
*       The ARP/tunnel start table contains 4K lines.
*       Each line can hold:
*       - 1 tunnel start entry.
*       - 4 router ARP addresses entries.
*       Indexes for tunnel start range are (0..4095); Indexes for router ARP
*       addresses range are (0..16383).
*       When ARP and tunnel start entries share the same memory, then tunnel
*       start entry at index n and router ARP addresses at indexes
*       4*n..4*n+3 share the same memory. For example tunnel start entry at
*       index 100 and router ARP addresses at indexes 400..403 share the same
*       physical memory.
*       When in external mode the FDB table can hold tunnel start and ARP
*       entries. Indexes for tunnel start range are (0..16383); Indexes
*       for router ARP addresses range are (0..65535).
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTunnelStartEntrySet
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    /* switch by tunnelType in inFields[1]               */
    switch (inFields[1])
    {
    case CPSS_TUNNEL_X_OVER_IPV4_E:
    case CPSS_TUNNEL_X_OVER_GRE_IPV4_E:
        return wrCpssExMxPmTunnelStartEntrySet_IPV4_CONFIG(
                                                          inArgs, inFields ,numFields ,outArgs);
    case CPSS_TUNNEL_X_OVER_MPLS_E:
        return wrCpssExMxPmTunnelStartEntrySet__MPLS_CONFIG(
                                                           inArgs, inFields ,numFields ,outArgs);
    case CPSS_TUNNEL_MAC_IN_MAC_E:
        return wrCpssExMxPmTunnelStartEntrySet__MIM_CONFIG(
                                                           inArgs, inFields ,numFields ,outArgs);
    default:
        galtisOutput(outArgs, GT_BAD_PARAM, "");
        return CMD_AGENT_ERROR;
    }
}
/*******************************puma 2 updated table******************************/
static CMD_STATUS wrCpssExMxPm2TunnelStartEntrySet
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
  isExtended=GT_TRUE;/*enter extended mode*/
  return wrCpssExMxPmTunnelStartEntrySet(inArgs,inFields,numFields,outArgs);
  
}


static GT_U32 gtIndex;
static GT_U32 maxIndex;
/*******************************************************************************
* cpssExMxPmTunnelStartEntryGet
*
* DESCRIPTION:
*       Get a tunnel start entry.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
*   devNum                        - device number
*   routerArpTunnelStartLineIndex - index of the Tunnel start entry.
*                                   in FDB split and shared mode range (0-4095),
*                                   in FDB external mode range (0-16383).
*   tunnelType                    - points to the type of the tunnel,
*                                   valid values:
*                                   CPSS_TUNNEL_X_OVER_IPV4_E,
*                                   CPSS_TUNNEL_X_OVER_GRE_IPV4_E,
*                                   CPSS_TUNNEL_X_OVER_MPLS_E,
*                                   CPSS_TUNNEL_MAC_IN_MAC_E
*
* OUTPUTS:
*   entryPtr                        - points to tunnel start entry configuration
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - on failure
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE             - on invalid tunnel type
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       see cpssExMxPmTunnelStartEntrySet function.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTunnelStartEntryGetEntry
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 routerArpTunnelStartLineIndex;
    CPSS_TUNNEL_TYPE_ENT tunnelType;
    CPSS_EXMXPM_TUNNEL_START_ENTRY_UNT entry;
    GT_U8 type;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if (gtIndex>maxIndex)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    tunnelType = (CPSS_TUNNEL_TYPE_ENT)inFields[1];

    if(isExtended)
        tunnelType=(CPSS_TUNNEL_TYPE_ENT)inArgs[3];

    routerArpTunnelStartLineIndex=gtIndex;

    /* call cpss api function */
    result = cpssExMxPmTunnelStartEntryGet(devNum, routerArpTunnelStartLineIndex, tunnelType, &entry);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    switch (tunnelType)
    {
    case CPSS_TUNNEL_X_OVER_IPV4_E:
    case CPSS_TUNNEL_X_OVER_GRE_IPV4_E:
        type=0;
        break;
    case CPSS_TUNNEL_X_OVER_MPLS_E:
        type=1;
        break;
    case CPSS_TUNNEL_MAC_IN_MAC_E:
        type=2;
        break;
    default:
        galtisOutput(outArgs, GT_BAD_PARAM, "");
        return CMD_AGENT_ERROR;
    }

    inFields[0]=gtIndex;

    switch (tunnelType)
    {
    case CPSS_TUNNEL_X_OVER_IPV4_E:
    case CPSS_TUNNEL_X_OVER_GRE_IPV4_E:
        inFields[1]=tunnelType;
        inFields[2]=entry.ipv4Cfg.tagEnable;
        inFields[3]=entry.ipv4Cfg.vlanId;
        inFields[4]=entry.ipv4Cfg.upMarkMode;
        inFields[5]=entry.ipv4Cfg.up;
        inFields[6]=entry.ipv4Cfg.dscpMarkMode;
        inFields[7]=entry.ipv4Cfg.dscp;
        inFields[9]=entry.ipv4Cfg.dontFragmentFlag;
        inFields[10]=entry.ipv4Cfg.ttl;
        inFields[11]=entry.ipv4Cfg.autoTunnel;
        inFields[12]=entry.ipv4Cfg.autoTunnelOffset;
        inFields[13]=entry.ipv4Cfg.ethType;
        inFields[16]=entry.ipv4Cfg.retainCrc;
        break;


    case CPSS_TUNNEL_X_OVER_MPLS_E:
        inFields[1]=tunnelType;
        inFields[2]=entry.mplsCfg.tagEnable;
        inFields[3]=entry.mplsCfg.vlanId;
        inFields[4]=entry.mplsCfg.upMarkMode;
        inFields[5]=entry.mplsCfg.up;
        inFields[7]=entry.mplsCfg.numLabels;
        inFields[8]=entry.mplsCfg.ttl;
        inFields[9]=entry.mplsCfg.label1;
        inFields[10]=entry.mplsCfg.exp1MarkMode;
        inFields[11]=entry.mplsCfg.exp1;
        inFields[12]=entry.mplsCfg.label2;
        inFields[13]=entry.mplsCfg.exp2MarkMode;
        inFields[14]=entry.mplsCfg.exp2;
        inFields[15]=entry.mplsCfg.retainCrc;
        if(isExtended)
            inFields[16]=entry.mplsCfg.ethType;
        break;
    case CPSS_TUNNEL_MAC_IN_MAC_E:
        inFields[1]=tunnelType;
        inFields[2]=entry.mimCfg.tagEnable;
        inFields[3]=entry.mimCfg.vlanId;
        inFields[4]=entry.mimCfg.upMarkMode;
        inFields[5]=entry.mimCfg.up;
        inFields[7]=entry.mimCfg.iSid;
        inFields[8]=entry.mimCfg.ttl;
        inFields[9]=entry.mimCfg.iSidAssignMode; 
        inFields[10]=entry.mimCfg.iUp;
        inFields[11]=entry.mimCfg.iUpMarkMode;
        inFields[12]=entry.mimCfg.iDp;
        inFields[13]=entry.mimCfg.iDpMarkMode;
        inFields[14]=entry.mimCfg.iTagReserved;
        inFields[15]=entry.mimCfg.retainCrc;
        break;

    default:
        return GT_BAD_PARAM;

    }



/*  output table fields */
    switch (tunnelType)
    {

    case CPSS_TUNNEL_X_OVER_IPV4_E:
    case CPSS_TUNNEL_X_OVER_GRE_IPV4_E:

        fieldOutput("%d%d%d%d%d%d%d%d%6b%d%d%d%d%d%4b%4b%d", inFields[0],
                    inFields[1],  inFields[2],inFields[3],  inFields[4],inFields[5],  inFields[6],
                    inFields[7], entry.ipv4Cfg.macDa.arEther ,inFields[9],  inFields[10],inFields[11],  inFields[12],
                    inFields[13], entry.ipv4Cfg.destIp.arIP,entry.ipv4Cfg.srcIp.arIP,inFields[16]);

        break;


    case CPSS_TUNNEL_X_OVER_MPLS_E:
       if(!isExtended)
        fieldOutput("%d%d%d%d%d%d%6b%d%d%d%d%d%d%d%d%d", inFields[0],
                    inFields[1],  inFields[2],inFields[3],  inFields[4],  inFields[5],entry.mplsCfg.macDa.arEther,
                    inFields[7],  inFields[8],inFields[9],  inFields[10],inFields[11],  inFields[12],
                    inFields[13],  inFields[14],inFields[15]);
         else
            fieldOutput("%d%d%d%d%d%d%6b%d%d%d%d%d%d%d%d%d%d", inFields[0],
                    inFields[1],  inFields[2],inFields[3],  inFields[4],  inFields[5],entry.mplsCfg.macDa.arEther,
                    inFields[7],  inFields[8],inFields[9],  inFields[10],inFields[11],  inFields[12],
                    inFields[13],  inFields[14],inFields[15],inFields[16]);

        break;
    case CPSS_TUNNEL_MAC_IN_MAC_E:

        fieldOutput("%d%d%d%d%d%d%6b%d%d%d%d%d%d%d%d%d", inFields[0],
                    inFields[1],  inFields[2],inFields[3],  inFields[4],  inFields[5],entry.mimCfg.macDa.arEther,
                    inFields[7],  inFields[8],inFields[9],  inFields[10],inFields[11],  inFields[12],
                    inFields[13],  inFields[14],inFields[15]);

        break;

    CPSS_COVERITY_NON_ISSUE_BOOKMARK
    /* coverity[dead_error_begin] */
    default:
        return GT_BAD_PARAM;
    }

    isExtended=GT_FALSE;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%f", type);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmTunnelStartEntryGet
*
* DESCRIPTION:
*       Get a tunnel start entry.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
*   devNum                        - device number
*   routerArpTunnelStartLineIndex - index of the Tunnel start entry.
*                                   in FDB split and shared mode range (0-4095),
*                                   in FDB external mode range (0-16383).
*   tunnelType                    - points to the type of the tunnel,
*                                   valid values:
*                                   CPSS_TUNNEL_X_OVER_IPV4_E,
*                                   CPSS_TUNNEL_X_OVER_GRE_IPV4_E,
*                                   CPSS_TUNNEL_X_OVER_MPLS_E,
*                                   CPSS_TUNNEL_MAC_IN_MAC_E
*
* OUTPUTS:
*   entryPtr                        - points to tunnel start entry configuration
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - on failure
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE             - on invalid tunnel type
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       see cpssExMxPmTunnelStartEntrySet function.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTunnelStartEntryGetFirst
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    gtIndex=(GT_U32)inArgs[1];
    maxIndex=(GT_U32)inArgs[2];
    return wrCpssExMxPmTunnelStartEntryGetEntry(inArgs,inFields,numFields,outArgs);
}

/*******************************************************************************
* cpssExMxPmTunnelStartEntryGet
*
* DESCRIPTION:
*       Get a tunnel start entry.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum            - device number
*       index             - index of the Tunnel start entry (0..1023)
*
* OUTPUTS:
*       tunnelTypePtr     - points to the type of the tunnel, valid values:
*                           CPSS_TUNNEL_X_OVER_IPV4_E,
*                           CPSS_TUNNEL_X_OVER_GRE_IPV4_E,
*                           CPSS_TUNNEL_X_OVER_MPLS_E,
*                           CPSS_TUNNEL_INVALID_E
*       entryPtr         - points to tunnel start configuration
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - wrong value in any of the parameters
*       GT_BAD_PTR        - on NULL pointer
*       GT_FAIL           - on failure
*       GT_HW_ERROR       - on hardware error
*       GT_BAD_STATE      - on invalid tunnel type
*
* COMMENTS:
*       Bridge Forwarding Database (FDB table) has few memory modes that
*       determines the memory placement of the FDB table, router ARP addresses
*       and tunnel start entries:
*         - split mode
*           In this mode the FDB resides in internal FDB table and ARP and
*           tunnel start entries share the ARP/tunnel table
*         - shared mode
*           In this mode the FDB and ARP share the FDB table and ARP/tunnel
*           table is occupied only by tunnel start entries
*         - external mode
*           In this mode the FDB resides in external memory, the ARP resides
*           in internal FDB table and ARP/tunnel table is occupied only by
*           tunnel start entries.
*       The ARP/tunnel start table contains 1K lines.
*       Each line can hold:
*       - 1 tunnel start entry
*       - 4 router ARP addresses entries
*       Indexes for tunnel start range is (0..1023); Indexes for router ARP
*       addresses range is (0..4095).
*       When ARP and tunnel start entries share the same memory, then tunnel
*       start entry at index n and router ARP addresses at indexes
*       4*n..4*n+3 share the same memory. For example tunnel start entry at
*       index 100 and router ARP addresses at indexes 400..403 share the same
*       physical memory.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTunnelStartEntryGetNext
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    gtIndex++;

    return wrCpssExMxPmTunnelStartEntryGetEntry(inArgs,inFields,numFields,outArgs);
}
/*******************************puma 2 updated table******************************/
static CMD_STATUS wrCpssExMxPm2TunnelStartEntryGetFirst
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
  isExtended=GT_TRUE;/*enter extended mode*/
  return wrCpssExMxPmTunnelStartEntryGetFirst(inArgs,inFields,numFields,outArgs);
  
}

/*********************************************************************************/
static CMD_STATUS wrCpssExMxPm2TunnelStartEntryGetNext
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
  isExtended=GT_TRUE;/*enter extended mode*/
  return wrCpssExMxPmTunnelStartEntryGetNext(inArgs,inFields,numFields,outArgs);
  
}
/*******************************************************************************
* cpssExMxPmTunnelStartGreEtherTypeSet
*
* DESCRIPTION:
*       Sets GRE (Generic Routing Encapsulation) 0 (control) or 1 (data) traffic ethertype.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum          - device number.
*       greType         - GRE tunnel type.
*       ethType         - GRE ethertype (16 bit).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTunnelStartGreEtherTypeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_TUNNEL_GRE_ETHER_TYPE_ENT greType;
    GT_U32 ethType;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    greType = (CPSS_TUNNEL_GRE_ETHER_TYPE_ENT)inArgs[1];
    ethType = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmTunnelStartGreEtherTypeSet(devNum, greType, ethType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmTunnelStartGreEtherTypeGet
*
* DESCRIPTION:
*       Gets GRE (Generic Routing Encapsulation) 0 (control) or 1 (data) traffic ethertype.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum          - device number.
*       greType         - GRE tunnel type.
*
* OUTPUTS:
*       ethTypePtr      - GRE ethertype (16 bit).
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTunnelStartGreEtherTypeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_TUNNEL_GRE_ETHER_TYPE_ENT greType;
    GT_U32 ethType;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    greType = (CPSS_TUNNEL_GRE_ETHER_TYPE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmTunnelStartGreEtherTypeGet(devNum, greType, &ethType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", ethType);

    return CMD_OK;
}
/*******************************************************************************
* cpssExMxPmTunnelStartMimITagEtherTypeSet
*
* DESCRIPTION:
*       Sets globally the MAC in MAC I-Tag Ethertype (802.1ah).
*       This is the Backbone Service Instance TAG identified by
*       a 802.1ah Ethertype
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum            - device number.
*       iTagEthType       - i-tag (instance tag) ethertype (16 bit).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTunnelStartMimITagEtherTypeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 iTagEthType;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    iTagEthType = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmTunnelStartMimITagEtherTypeSet(devNum, iTagEthType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmTunnelStartMimITagEtherTypeGet
*
* DESCRIPTION:
*       Gets the MAC in MAC I-Tag Ethertype (802.1ah).
*       This is the Backbone Service Instance TAG identified by
*       a 802.1ah Ethertype.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum              - device number
*
* OUTPUTS:
*       iTagEthTypePtr      - i-tag (instance tag) ethertype
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device number
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTunnelStartMimITagEtherTypeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 iTagEthType;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmTunnelStartMimITagEtherTypeGet(devNum, &iTagEthType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", iTagEthType);

    return CMD_OK;
}

/**** database initialization **************************************/




static CMD_COMMAND dbCommands[] =
{
    {"cpssExMxPmTunnelStartEgressFilteringEnableSet",
        &wrCpssExMxPmTunnelStartEgressFilteringEnableSet,
        2, 0},
    {"cpssExMxPmTunnelStartEgressFilteringEnableGet",
        &wrCpssExMxPmTunnelStartEgressFilteringEnableGet,
        1, 0},
    {"cpssExMxPmTunnelStartEntrySet",
        &wrCpssExMxPmTunnelStartEntrySet,
        4, 17},
    {"cpssExMxPmTunnelStartEntryGetFirst",
        &wrCpssExMxPmTunnelStartEntryGetFirst,
        4, 0},
    {"cpssExMxPmTunnelStartEntryGetNext",
        &wrCpssExMxPmTunnelStartEntryGetNext,
        4, 0},
    /**********puma 2 table********************/
    {"cpssExMxPm2TunnelStartEntrySet",
        &wrCpssExMxPm2TunnelStartEntrySet,
        4, 17},
    {"cpssExMxPm2TunnelStartEntryGetFirst",
        &wrCpssExMxPm2TunnelStartEntryGetFirst,
        5, 0},
    {"cpssExMxPm2TunnelStartEntryGetNext",
        &wrCpssExMxPm2TunnelStartEntryGetNext,
        5, 0},

    {"cpssExMxPm2TunnelStartGreEtherTypeSet",
         &wrCpssExMxPmTunnelStartGreEtherTypeSet,
         3, 0},
    {"cpssExMxPm2TunnelStartGreEtherTypeGet",
         &wrCpssExMxPmTunnelStartGreEtherTypeGet,
         2, 0},
    {"cpssExMxPm2TunnelStartMimITagEtherTypeSet",
         &wrCpssExMxPmTunnelStartMimITagEtherTypeSet,
         2, 0},
    {"cpssExMxPm2TunnelStartMimITagEtherTypeGet",
         &wrCpssExMxPmTunnelStartMimITagEtherTypeGet,
         1, 0}
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxPmTunnelStart
*
* DESCRIPTION:
*     Library database initialization function.
*
* INPUTS:
*     none
*
* OUTPUTS:
*     none
*
* RETURNS:
*     GT_OK   - on success.
*     GT_FAIL - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
GT_STATUS cmdLibInitCpssExMxPmTunnelStart
(
GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

