/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* tgfIpfixGen.h
*
* DESCRIPTION:
*       Generic API for IPFIX
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __tgfIpfixGenh
#define __tgfIpfixGenh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <common/tgfPolicerGen.h>

#ifdef CHX_FAMILY
    #include <cpss/dxCh/dxChxGen/ipfix/cpssDxChIpfix.h>
#endif /* CHX_FAMILY */

/* length of array to retrieve alarm events */
#define PRV_TGF_IPFIX_ALARM_EVENTS_ARRAY_LENGTH_CNS 16
        
/*
 * typedef: enum PRV_TGF_IPFIX_SAMPLING_ACTION_ENT
 *
 * Description: Enumeration for IPFIX sampling action.
 *
 * Enumerations:
 *  PRV_TGF_IPFIX_SAMPLING_ACTION_ALARM_E   - Alarm, each sampled packet will 
 *                                            trigger an interrupt to cpu.
 *  PRV_TGF_IPFIX_SAMPLING_ACTION_MIRROR_E  - Mirror, each sampled packet will
 *                                            change packet command to Mirror.
 */
typedef enum{
    PRV_TGF_IPFIX_SAMPLING_ACTION_ALARM_E,
    PRV_TGF_IPFIX_SAMPLING_ACTION_MIRROR_E
} PRV_TGF_IPFIX_SAMPLING_ACTION_ENT;

/*
 * typedef: enum PRV_TGF_IPFIX_SAMPLING_DIST_ENT
 *
 * Description: Enumeration for IPFIX sampling distribution.
 *
 * Enumerations:
 *  PRV_TGF_IPFIX_SAMPLING_DIST_DETERMINISTIC_E - Deterministic, the random 
 *                                                offset is deterministic and
 *                                                equals zero. 
 *  PRV_TGF_IPFIX_SAMPLING_DIST_RANDOM_E        - Random, the random_offset 
 *                                                is a unified distributed 
 *                                                random number which is 
 *                                                picked for each packet.
 */
typedef enum{
    PRV_TGF_IPFIX_SAMPLING_DIST_DETERMINISTIC_E,
    PRV_TGF_IPFIX_SAMPLING_DIST_RANDOM_E
} PRV_TGF_IPFIX_SAMPLING_DIST_ENT;

/*
 * typedef: enum PRV_TGF_IPFIX_SAMPLING_MODE_ENT
 *
 * Description: Enumeration for IPFIX sampling mode.
 *
 * Enumerations:
 *  PRV_TGF_IPFIX_SAMPLING_MODE_DISABLE_E   - Disable sampling.
 *  PRV_TGF_IPFIX_SAMPLING_MODE_PACKET_E    - Packet count based sampling.
 *  PRV_TGF_IPFIX_SAMPLING_MODE_BYTE_E      - Byte count based sampling.
 *  PRV_TGF_IPFIX_SAMPLING_MODE_TIME_E      - Time based sampling.
 */
typedef enum{
    PRV_TGF_IPFIX_SAMPLING_MODE_DISABLE_E,
    PRV_TGF_IPFIX_SAMPLING_MODE_PACKET_E,
    PRV_TGF_IPFIX_SAMPLING_MODE_BYTE_E,
    PRV_TGF_IPFIX_SAMPLING_MODE_TIME_E
} PRV_TGF_IPFIX_SAMPLING_MODE_ENT;

/*
 * typedef: enum PRV_TGF_IPFIX_DROP_COUNT_MODE_ENT
 *
 * Description: Enumeration for IPFIX drop counter selected mode.
 *
 * Enumerations:
 *  PRV_TGF_IPFIX_DROP_COUNT_MODE_METER_ONLY_E - The drop counter is updated
 *                                               only for packets dropped due
 *                                               to the Policer engine. 
 *  PRV_TGF_IPFIX_DROP_COUNT_MODE_ALL_E        - The drop counter is updated
 *                                               for drops up to and 
 *                                               including the Policer engine.
 */
typedef enum{
    PRV_TGF_IPFIX_DROP_COUNT_MODE_METER_ONLY_E,
    PRV_TGF_IPFIX_DROP_COUNT_MODE_ALL_E
} PRV_TGF_IPFIX_DROP_COUNT_MODE_ENT;

/*
 * typedef: enum PRV_TGF_IPFIX_WRAPAROUND_ACTION_ENT
 *
 * Description: Enumeration for IPFIX counter action when wraparound occurs.
 *
 * Enumerations:
 *  PRV_TGF_IPFIX_WRAPAROUND_ACTION_NONE_E   - Freeze the counter at the 
 *                                             threshold value and stop 
 *                                             performing any updates. 
 *  PRV_TGF_IPFIX_WRAPAROUND_ACTION_CLEAR_E  - Subtract the threshold value
 *                                             from the current value of the
 *                                             counter and continue updates.
 */
typedef enum{
    PRV_TGF_IPFIX_WRAPAROUND_ACTION_NONE_E,
    PRV_TGF_IPFIX_WRAPAROUND_ACTION_CLEAR_E
} PRV_TGF_IPFIX_WRAPAROUND_ACTION_ENT;

/*
 * typedef: enum PRV_TGF_IPFIX_TIMESTAMP_UPLOAD_MODE_ENT
 *
 * Description: Enumeration for loading of IPFIX time stamp method.
 *
 * Enumerations:
 *  PRV_TGF_IPFIX_TIMESTAMP_UPLOAD_MODE_ABSOLUTE_E    - IPfix timer is 
 *                                                      uploaded to the exact 
 *                                                      value of time stamp 
 *                                                      upload registers. 
 *  PRV_TGF_IPFIX_TIMESTAMP_UPLOAD_MODE_INCREMENTAL_E - IPfix timer is 
 *                                                      incremented by the 
 *                                                      value of time stamp
 *                                                      upload registers.
 */
typedef enum{
    PRV_TGF_IPFIX_TIMESTAMP_UPLOAD_MODE_ABSOLUTE_E,
    PRV_TGF_IPFIX_TIMESTAMP_UPLOAD_MODE_INCREMENTAL_E
} PRV_TGF_IPFIX_TIMESTAMP_UPLOAD_MODE_ENT;

/*
 * typedef: struct PRV_TGF_IPFIX_ENTRY_STC
 *
 * Description:
 *      The IPFIX entry.
 *
 * Fields:
 *      timeStamp           - Time stamp from RTC counter. 16 bits.
 *      packetCounter       - packets counter. 30 bits.
 *      byteCount           - bytes counter. 36 bits.
 *      dropCounter         - Drop packet counter. 30 bits.
 *      randomOffset        - Picked random value after each sampling. 32 bits.
 *      lastSampledValue    - Once the packet is sampled, it contains the 
 *                            summary of the previous lastSampledValue + 
 *                            samplingWindow. 36 bits.
 *      samplingWindow      - Window used for Reference value calculation. The
 *                            units of this window are according to 
 *                            samplingMode. 36 bits.
 *      samplingAction      - Command done on sampled packets.
 *      logSamplingRange    - Defines the distribution range of the 
 *                            randomOffset. Relevant only when randomFlag is 
 *                            random. 6 bits.
 *      randomFlag          - Determines the random offset used for the 
 *                            reference value calculation.
 *      samplingMode        - Sampling base method.
 *      cpuSubCode          - 2 lsb bits of the cpu code for sampled packets.
 *                            Relevant only when samplingAction is Mirror.
 * Comment:
 */
typedef struct
{
    GT_U32                            timeStamp;
    GT_U32                            packetCount;
    GT_U64                            byteCount;
    GT_U32                            dropCounter;
    GT_U32                            randomOffset;
    GT_U64                            lastSampledValue;
    GT_U64                            samplingWindow;
    PRV_TGF_IPFIX_SAMPLING_ACTION_ENT samplingAction;
    GT_U32                            logSamplingRange;
    PRV_TGF_IPFIX_SAMPLING_DIST_ENT   randomFlag;
    PRV_TGF_IPFIX_SAMPLING_MODE_ENT   samplingMode;
    GT_U32                            cpuSubCode;
} PRV_TGF_IPFIX_ENTRY_STC;

/*
 * typedef: struct PRV_TGF_IPFIX_WRAPAROUND_CFG_STC
 *
 * Description:
 *      IPFIX wraparound configuration.
 *
 * Fields:
 *      action          - The action when a counter reaches its 
 *                        threshold value.
 *      dropThreshold   - Drop packet counter threshold. 30 bits.
 *      packetThreshold - Packets counter threshold. 30 bits.
 *      byteThreshold   - Bytes counter threshold. 36 bits.
 *
 * Comment:
 */
typedef struct
{
    PRV_TGF_IPFIX_WRAPAROUND_ACTION_ENT   action;
    GT_U32                                dropThreshold;
    GT_U32                                packetThreshold;
    GT_U64                                byteThreshold;
} PRV_TGF_IPFIX_WRAPAROUND_CFG_STC;

/*
 * typedef: struct PRV_TGF_IPFIX_TIMER_STC
 *
 * Description:
 *      IPFIX timer.
 *
 * Fields:
 *      nanoSecondTimer - IPFIX nano second timer. 30 bits.
 *      secondTimer     - IPFIX second timer. 64 bits.
 *
 * Comment:
 */
typedef struct
{
    GT_U32  nanoSecondTimer;
    GT_U64  secondTimer;
} PRV_TGF_IPFIX_TIMER_STC;

/*
 * typedef: struct PRV_TGF_IPFIX_TIMESTAMP_UPLOAD_CONF_STC
 *
 * Description:
 *      IPFIX timestamp upload configuration.
 *
 * Fields:
 *      uploadMode  - timestamp upload mode.
 *      ipfixTimer  - IPFIX timer structure.
 *
 * Comment:
 */
typedef struct
{
    PRV_TGF_IPFIX_TIMESTAMP_UPLOAD_MODE_ENT  uploadMode;
    PRV_TGF_IPFIX_TIMER_STC                  timer;
} PRV_TGF_IPFIX_TIMESTAMP_UPLOAD_CONF_STC;

/*******************************************************************************
* prvTgfIpfixEntrySet
*
* DESCRIPTION:
*       Sets IPFIX entry.
*
* INPUTS:
*       devNum          - device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       entryIndex      - index of IPFIX Entry.
*       ipfixEntryPtr   - pointer to the IPFIX Entry.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or entryIndex.
*       GT_OUT_OF_RANGE             - on value out of range.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfIpfixEntrySet
(
    IN  GT_U8                             devNum,
    IN  PRV_TGF_POLICER_STAGE_TYPE_ENT    stage,
    IN  GT_U32                            entryIndex,
    IN  PRV_TGF_IPFIX_ENTRY_STC           *ipfixEntryPtr
);

/*******************************************************************************
* prvTgfIpfixEntryGet
*
* DESCRIPTION:
*       Gets IPFIX entry.
*
*  INPUTS:
*       devNum      - device number.
*       stage       - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       entryIndex  - index of IPFIX Entry.
*       reset       - reset flag:
*                     GT_TRUE  - performing read and reset atomic operation.
*                     GT_FALSE - performing read entry operation only.
*
* OUTPUTS:
*       ipfixEntryPtr   - pointer to the IPFIX Entry.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_TIMEOUT                  - on time out of IPLR Table indirect access.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or entryIndex.
*       GT_BAD_STATE                - on counter entry type mismatch.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfIpfixEntryGet
(
    IN  GT_U8                             devNum,
    IN  PRV_TGF_POLICER_STAGE_TYPE_ENT    stage,
    IN  GT_U32                            entryIndex,
    IN  GT_BOOL                           reset,
    OUT PRV_TGF_IPFIX_ENTRY_STC           *ipfixEntryPtr
);

/*******************************************************************************
* prvTgfIpfixDropCountModeSet
*
* DESCRIPTION:
*       Configures IPFIX drop counting mode.
*
* INPUTS:
*       devNum  - physical device number.
*       stage   - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       mode    - drop counting mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or mode.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfIpfixDropCountModeSet
(
    IN GT_U8                                devNum,
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT       stage,
    IN PRV_TGF_IPFIX_DROP_COUNT_MODE_ENT    mode
);

/*******************************************************************************
* prvTgfIpfixDropCountModeGet
*
* DESCRIPTION:
*       Gets IPFIX drop counting mode.
*
* INPUTS:
*       devNum  - physical device number.
*       stage   - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*
* OUTPUTS:
*       modePtr - pointer to drop counting mode.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfIpfixDropCountModeGet
(
    IN GT_U8                                  devNum,
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT         stage,
    OUT PRV_TGF_IPFIX_DROP_COUNT_MODE_ENT     *modePtr
);

/*******************************************************************************
* prvTgfIpfixCpuCodeSet
*
* DESCRIPTION:
*       Sets the 6 most significant bits of the CPU Code in case of mirroring. 
*       The two least significant bits are taken from the IPfix entry.
*
* INPUTS:
*       devNum  - physical device number.
*       stage   - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       cpuCode - A cpu code to set the 6 most significant bits.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or cpu code.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfIpfixCpuCodeSet
(
    IN GT_U8                          devNum,
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT stage,
    IN CPSS_NET_RX_CPU_CODE_ENT       cpuCode
);

/*******************************************************************************
* prvTgfIpfixCpuCodeGet
*
* DESCRIPTION:
*       Gets the 6 most significant bits of the CPU Code in case of mirroring. 
*       The two least significant bits are taken from the IPfix entry.
*
* INPUTS:
*       devNum      - physical device number.
*       stage       - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*
* OUTPUTS:
*       cpuCodePtr  - pointer to a cpu code to set the 6 most significant bits.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfIpfixCpuCodeGet
(
    IN GT_U8                          devNum,
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT stage,
    OUT CPSS_NET_RX_CPU_CODE_ENT      *cpuCodePtr
);

/*******************************************************************************
* prvTgfIpfixAgingEnableSet
*
* DESCRIPTION:
*       Enables or disabled the activation of aging for IPfix.
*
* INPUTS:
*       devNum  - physical device number.
*       stage   - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       enable  - GT_TRUE for enable, GT_FALSE for disable.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfIpfixAgingEnableSet
(
    IN GT_U8                          devNum,
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT stage,
    IN GT_BOOL                        enable
);

/*******************************************************************************
* prvTgfIpfixAgingEnableGet
*
* DESCRIPTION:
*       Gets enabling status of aging for IPfix.
*
* INPUTS:
*       devNum      - physical device number.
*       stage       - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*
* OUTPUTS:
*       enablePtr   - pointer to: GT_TRUE for enable, GT_FALSE for disable.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfIpfixAgingEnableGet
(
    IN GT_U8                          devNum,
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT stage,
    OUT GT_BOOL                       *enablePtr
);

/*******************************************************************************
* prvTgfIpfixWraparoundConfSet
*
* DESCRIPTION:
*       Configures IPFIX wraparound parameters.
*
* INPUTS:
*       devNum      - physical device number.
*       stage       - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       confPtr   - pointer to wraparound configuration structure.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage, action or threshold.
*       GT_OUT_OF_RANGE             - on value out of range.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfIpfixWraparoundConfSet
(
    IN GT_U8                                  devNum,
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT         stage,
    IN PRV_TGF_IPFIX_WRAPAROUND_CFG_STC       *confPtr
);

/*******************************************************************************
* prvTgfIpfixWraparoundConfGet
*
* DESCRIPTION:
*       Get IPFIX wraparound configuration.
*
* INPUTS:
*       devNum      - physical device number.
*       stage       - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*
* OUTPUTS:
*       confPtr   - pointer to wraparound configuration structure.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfIpfixWraparoundConfGet
(
    IN GT_U8                                  devNum,
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT         stage,
    OUT PRV_TGF_IPFIX_WRAPAROUND_CFG_STC      *confPtr
);

/*******************************************************************************
* prvTgfIpfixWraparoundStatusGet
*
* DESCRIPTION:
*       Retrieves a bitmap of wraparound entries indexes.
*
* INPUTS:
*       devNum      - physical device number.
*       stage       - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       startIndex  - the first entry index to get wraparound indication on.
*       endIndex    - the last entry index to get wraparound indication on.
*       reset       - clear the bits after read: 
*                     GT_TRUE - clear,
*                     GT_FALSE - don't clear.
*
*
* OUTPUTS:
*       bmpPtr      - pointer to a bitmap indication wraparound.
*                     each bit indicate: 0 - no WA occured, 1 - WA occured.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       It is application responsibility to allocate the buffer size for the
*       bitmap(bmpPtr). Since the buffer is made from GT_U32 elements, each
*       element contains 32 indications.
*
*******************************************************************************/
GT_STATUS prvTgfIpfixWraparoundStatusGet
(
    IN GT_U8                          devNum,
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT stage,
    IN GT_U32                         startIndex,
    IN GT_U32                         endIndex,
    IN GT_BOOL                        reset,
    OUT GT_U32                        *bmpPtr
);

/*******************************************************************************
* prvTgfIpfixAgingStatusGet
*
* DESCRIPTION:
*       Retrieves a bitmap of aged entries indexes.
*
* INPUTS:
*       devNum      - physical device number.
*       stage       - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       startIndex  - the first entry index to get aging indication on.
*       endIndex    - the last entry index to get aging indication on.
*       reset       - clear the bits after read: 
*                     GT_TRUE - clear,
*                     GT_FALSE - don't clear.
*
*
* OUTPUTS:
*       bmpPtr      - pointer to a bitmap indication aging.
*                     each bit indicate: 0 - no packet recieved on flow (aged).
*                                        1 - packet recieved on flow.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       It is application responsibility to allocate the buffer size for the
*       bitmap(bmpPtr). Since the buffer is made from GT_U32 elements, each
*       element contains 32 indications.
*
*******************************************************************************/
GT_STATUS prvTgfIpfixAgingStatusGet
(
    IN GT_U8                          devNum,
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT stage,
    IN GT_U32                         startIndex,
    IN GT_U32                         endIndex,
    IN GT_BOOL                        reset,
    OUT GT_U32                        *bmpPtr
);

/*******************************************************************************
* prvTgfIpfixTimestampUploadSet
*
* DESCRIPTION:
*       Configures IPFIX timestamp upload per stages.
*
* INPUTS:
*       devNum      - physical device number.
*       stage       - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       uploadPtr   - pointer to timestamp upload configuration of a stage.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or upload mode.
*       GT_OUT_OF_RANGE             - on value out of range.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       Actual upload operation is triggered by 
*       "prvTgfIpfixTimestampUploadTrigger".
*
*******************************************************************************/
GT_STATUS prvTgfIpfixTimestampUploadSet
(
    IN GT_U8                                      devNum,
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT             stage,
    IN PRV_TGF_IPFIX_TIMESTAMP_UPLOAD_CONF_STC    *uploadPtr
);

/*******************************************************************************
* prvTgfIpfixTimestampUploadGet
*
* DESCRIPTION:
*       Gets IPFIX timestamp upload configuration for a stage.
*
* INPUTS:
*       devNum      - physical device number.
*       stage       - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*
* OUTPUTS:
*       uploadPtr   - pointer to timestamp upload configuration of a stage.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfIpfixTimestampUploadGet
(
    IN GT_U8                                      devNum,
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT             stage,
    OUT PRV_TGF_IPFIX_TIMESTAMP_UPLOAD_CONF_STC   *uploadPtr
);

/*******************************************************************************
* prvTgfIpfixTimestampUploadTrigger
*
* DESCRIPTION:
*       Triggers the IPFIX timestamp upload operation.
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum.
*       GT_BAD_STATE                - if previous upload operation is not 
*                                     finished yet.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfIpfixTimestampUploadTrigger
(
    IN GT_U8    devNum
);

/*******************************************************************************
* prvTgfIpfixTimestampUploadStatusGet
*
* DESCRIPTION:
*       Gets IPFIX timestamp upload operation status.
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*       uploadStatusPtr - pointer to indication whether upload was done.
*                         GT_TRUE: upload is finished.
*                         GT_FALSE: upload still in progess.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfIpfixTimestampUploadStatusGet
(
    IN GT_U8    devNum,
    OUT GT_BOOL *uploadStatusPtr
);

/*******************************************************************************
* prvTgfIpfixTimerGet
*
* DESCRIPTION:
*       Gets IPFIX timer
*
* INPUTS:
*       devNum      - physical device number.
*       stage       - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*
* OUTPUTS:
*       timerPtr    - pointer to IPFIX timer.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfIpfixTimerGet
(
    IN GT_U8                          devNum,
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT stage,
    OUT PRV_TGF_IPFIX_TIMER_STC       *timerPtr
);

/*******************************************************************************
* prvTgfIpfixAlarmEventsGet
*
* DESCRIPTION:
*       Retrieves up to 16 IPFIX entries indexes where alarm events (due to 
*       sampling) occured.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*       devNum       - physical device number.
*       stage        - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*
*
* OUTPUTS:
*       eventsArr    - array of alarm events, each valid element contains
*                      the index of IPFIX entry which caused the alarm.
*       eventsNumPtr - The number of valid entries in eventsPtr list.
*                      Range: 0..15
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfIpfixAlarmEventsGet
(
    IN GT_U8                          devNum,
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT stage,
    OUT GT_U32                        eventsArr[PRV_TGF_IPFIX_ALARM_EVENTS_ARRAY_LENGTH_CNS],
    OUT GT_U32                        *eventsNumPtr
);

/*******************************************************************************
* prvTgfIpfixTimestampToCpuEnableSet
*
* DESCRIPTION:
*       Configures all TO_CPU DSA tags to include a Timestamp.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*       devNum  - physical device number.
*       enable  - GT_TRUE: TO_CPU DSA tag contains a Timestamp.
*                 GT_FALSE: TO_CPU DSA tag does not contain a Timestamp.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfIpfixTimestampToCpuEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
);

/*******************************************************************************
* prvTgfIpfixTimestampToCpuEnableGet
*
* DESCRIPTION:
*       Gets enabling status of TO_CPU DSA tags including a timestamp.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*       devNum  - physical device number.
*
* OUTPUTS:
*       enablePtr  - (pointer to)
*                    GT_TRUE: TO_CPU DSA tag contains a Timestamp.
*                    GT_FALSE: TO_CPU DSA tag does not contain a Timestamp.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfIpfixTimestampToCpuEnableGet
(
    IN GT_U8     devNum,
    OUT GT_BOOL *enablePtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __tgfIpfixGenh */

