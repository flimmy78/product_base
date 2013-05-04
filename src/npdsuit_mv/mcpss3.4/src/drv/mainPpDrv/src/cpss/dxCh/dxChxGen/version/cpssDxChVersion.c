/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChVersion.c
*
* DESCRIPTION:
*       Implements software CPSS DxCh version information.
*
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
/* get the OS , extDrv functions*/
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/dxCh/dxChxGen/version/cpssDxChVersion.h>

/* string to define the CPSS DxCh version used */
#define DXCH_VERSION_CNS                    "cpss3.4 release"

/*******************************************************************************
* cpssDxChVersionGet
*
* DESCRIPTION:
*       This function returns CPSS DxCh version.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       versionPtr     - (pointer to)CPSS DxCh version info.
*
* RETURNS:
*       GT_OK       - on success
*       GT_BAD_PTR  - one parameter is NULL pointer
*       GT_BAD_SIZE - the version name is too long
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChVersionGet
(
    OUT CPSS_VERSION_INFO_STC   *versionPtr
)
{
    GT_U32  versionLen = cpssOsStrlen(DXCH_VERSION_CNS);

    if(versionPtr == NULL)
    {
        return GT_BAD_PTR;
    }

    if(versionLen > CPSS_VERSION_MAX_LEN_CNS)
    {
        return GT_BAD_SIZE;
    }

    cpssOsMemCpy(versionPtr->version, DXCH_VERSION_CNS, versionLen + 1);

    cpssOsPrintf("CPSS DxCh Version: %s\n",DXCH_VERSION_CNS);

    return GT_OK;
}

