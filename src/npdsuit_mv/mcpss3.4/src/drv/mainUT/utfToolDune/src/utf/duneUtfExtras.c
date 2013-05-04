
#include <FX950/include/fap20m_api_general.h>

#include <utf/duneUtfMain.h>
#include <utf/private/prvUtfHelpers.h>
#include <utf/private/prvDuneUtfExtras.h>

#include <utf/utfMain.h>


/******************************************************************************\
 *                                  DUNE Defifnitions                         *
\******************************************************************************/

/*
 * Typedef: struct EXTRAS_DUNE_CTX_STC
 *
 * Description: DUNE extras context storage.
 *              There is can be only one statically allocated
 *              instance of this storage. Used for saving
 *              devices, ports, and iterators run-time info.
 *
 * Fields:
 *          devArray    - array of active/non active devices
 *                        (for non-active device = 0, for active device = 0xFF)
 *          portsArray  - contains number of physical ports for each device
 *          devForPorts - current active device id for iteration thru ports
 */
typedef struct
{
    SAND_UINT  devArray[SAND_MAX_DEVICE];
    SAND_U8    portsArray[FAP20M_NOF_DATA_PORTS];
    SAND_U8    devForPorts;
} EXTRAS_DUNE_CTX_STC;

/* single instance of extras context, used for iteration */
static EXTRAS_DUNE_CTX_STC extDuneCtx;


/*******************************************************************************
* prvDuneUtfExtrasInit
*
* DESCRIPTION:
*       Initialize extras for DUNE unit testing.
*       This function inits available devices and ports per device.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvDuneUtfExtrasInit
(
    void
)
{
    GT_STATUS    st          = GT_OK;
    SAND_UINT    device_id   = 0;
    SAND_BOOL    devWasFound = FALSE;
    SAND_U32     ex          = 0;
    SAND_RET     rc          = SAND_OK;


    for (device_id = 0; device_id < SAND_MAX_DEVICE; device_id++)
    {
        sand_check_driver_and_device(device_id, &ex);
        rc = sand_get_error_code_from_error_word(ex);

        if (SAND_OK == rc)
        {
            extDuneCtx.devArray[device_id] = 0xFF;
            devWasFound = TRUE;

            PRV_UTF_LOG1_MAC("[UTF]: prvDuneUtfExtrasInit: PP dev [%d]\n", device_id);
        }
        else
        {
            extDuneCtx.devArray[device_id] = 0;
        }
    }

    if (FALSE == devWasFound)
    {
        PRV_UTF_LOG0_MAC("[UTF]: prvDuneUtfExtrasInit: no active device was found\n");
        st = GT_FAIL;
    }

    return st;
}


/******************************************************************************\
 *                               DUNE Implementation                          *
\******************************************************************************/

/*******************************************************************************
* prvDuneUtfNextDeviceReset
*
* DESCRIPTION:
*       Reset device iterator must be called before go over all devices
*
* INPUTS:
*       devPtr   -   iterator through devices
*
* OUTPUTS:
*       None
*
* RETURNS:
*       SAND_OK  -   iterator initialization OK
*       SAND_ERR -   general failure error
*
* COMMENTS:
*       None.
*******************************************************************************/
SAND_RET prvDuneUtfNextDeviceReset
(
    SAND_INOUT   SAND_UINT   *devPtr
)
{
    if (NULL == devPtr)
    {
        return SAND_ERR;
    }

    *devPtr = (SAND_UINT) -1;

    return SAND_OK;
}

/*******************************************************************************
* prvDuneUtfNextDeviceGet
*
* DESCRIPTION:
*       This routine returns next active/non active device id.
*       Call the function until non SAND_OK error. Note that
*       iterator must be initialized before.
*
* INPUTS:
*       devPtr   -   device id
*       activeIs -   set to TRUE for getting active devices
*
* OUTPUTS:
*       None.
*
*       SAND_OK  -   Get next device was OK
*       SAND_ERR -   There is no active devices
*
* COMMENTS:
*       None.
*******************************************************************************/
SAND_RET prvDuneUtfNextDeviceGet
(
    SAND_INOUT  SAND_UINT  *devPtr,
    SAND_IN     SAND_BOOL   activeIs
)
{
    SAND_RET st = SAND_OK;

    if (NULL == devPtr)
    {
        return SAND_ERR;
    }

    st = SAND_ERR;
    (*devPtr)++;

    while (*devPtr < SAND_MAX_DEVICE)
    {
        if (((extDuneCtx.devArray[*devPtr] == 0) ? FALSE : TRUE) == activeIs)
        {
            st = SAND_OK;
            break;
        }
        (*devPtr)++;
    }

    return st;
}

