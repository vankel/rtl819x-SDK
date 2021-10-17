/*
 * Copyright (C) 2012 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 14000 $
 * $Date: 2010-11-08 17:47:25 +0800 (?üÊ?‰∏Ä, 08 ?Å‰???2010) $
 *
 * Purpose : Definition of VLAN API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) Vlan table configure and modification
 *           (2) Accept frame type
 *           (3) Vlan ingress/egress filter
 *           (4) Port based and protocol based vlan
 *           (5) TPID configuration
 *           (6) Ingress tag handling
 *           (7) Tag format handling
 *
 */



/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/port.h>
#include <dal/apollomp/dal_apollomp.h>
#include <rtk/vlan.h>
#include <dal/apollomp/dal_apollomp_vlan.h>
#include <dal/apollomp/raw/apollomp_raw_vlan.h>
#include <dal/apollomp/raw/apollomp_raw_trap.h>
#include <hal/mac/drv.h>

/*
 * Symbol Definition
 */
static uint32   vlan_init = INIT_NOT_COMPLETED;

static uint32       vlan_valid[(RTK_EXT_VLAN_ID_MAX + 1) / 32];
static rtk_vlan_t   vlan_mbrCfgVid[DAL_APOLLOMP_MAX_NUM_MBR_CFG];

/*
 * Macro Declaration
 */
#define DAL_APOLLOMP_VLAN_SET(vid) \
do {\
    if ((vid) <= RTK_EXT_VLAN_ID_MAX) {vlan_valid[vid >> 5] |= (1 << (vid&31));}\
} while (0);\

#define DAL_APOLLOMP_VLAN_CLEAR(vid) \
do {\
    if ((vid) <= RTK_EXT_VLAN_ID_MAX) {vlan_valid[vid >> 5] &= (~(1 << (vid&31)));}\
} while (0);\

#define DAL_APOLLOMP_VLAN_IS_SET(vid) \
    (((vid) <= RTK_EXT_VLAN_ID_MAX)?((vlan_valid[vid >> 5] >> (vid&31)) & 1): 0)

#define DAL_APOLLOMP_VLAN_MBECFG_SET(index, vid) \
do {\
    vlan_mbrCfgVid[index] = vid;\
} while (0);\

#define DAL_APOLLOMP_VLAN_MBECFG_VID(index)       (vlan_mbrCfgVid[index])

/*
 * Function Declaration
 */
/* Function Name:
 *      _dal_apollomp_vlan_checkAndUpdate
 * Description:
 *      Check VLAN ID update
 * Input:
 *      vid     - VLAN ID
 * Output:
*       None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_ENTRY_FULL
 * Note:
 *      None.
 */
static int32
_dal_apollomp_vlan_checkAndUpdate(rtk_vlan_t vid)
{
    int32 ret;
    uint32 vidx;
    apollomp_raw_vlan4kentry_t vlan;
    apollomp_raw_vlanconfig_t mbrCfg;

    RT_PARAM_CHK((vid > RTK_EXT_VLAN_ID_MAX), RT_ERR_VLAN_VID);

    if(vid <= RTK_VLAN_ID_MAX) /* for the VID larger than 4K, don't need to serach mbrCfg. */
    {
        for(vidx = 0; vidx <= HAL_MAX_VLAN_MBRCFG_IDX(); vidx++)
        {
            if(DAL_APOLLOMP_VLAN_MBECFG_VID(vidx) == vid)
            {
                /* Found, update it */
                osal_memset(&vlan, 0x00, sizeof(apollomp_raw_vlan4kentry_t));
                vlan.vid = vid;
                if((ret = apollomp_raw_vlan_4kEntry_get(&vlan)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                    return ret;
                }

                osal_memset(&mbrCfg, 0x00, sizeof(apollomp_raw_vlanconfig_t));
                mbrCfg.index    = vidx;
                mbrCfg.evid     = vlan.vid;
                RTK_PORTMASK_ASSIGN(mbrCfg.mbr, vlan.mbr);
                RTK_PORTMASK_ASSIGN(mbrCfg.exMbr, vlan.exMbr);
                mbrCfg.fid_msti = vlan.fid_msti;
                mbrCfg.envlanpol= vlan.envlanpol;
                mbrCfg.meteridx = vlan.meteridx;
                mbrCfg.vbpen    = vlan.vbpen;
                mbrCfg.vbpri    = vlan.vbpri;
                if((ret = apollomp_raw_vlan_memberConfig_set(&mbrCfg)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                    return ret;
                }

                return RT_ERR_OK;
            }
        }
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_apollomp_vlan_checkAndCreate
 * Description:
 *      Check VLAN ID and return VIDX
 * Input:
 *      vid     - VLAN ID
 * Output:
 *      pVidx   - VLAN Member Config index
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_ENTRY_FULL
 * Note:
 *      None.
 */
int32
dal_apollomp_vlan_checkAndCreate(rtk_vlan_t vid, uint32 *pVidx)
{
    int32 ret;
    uint32 vidx;
    uint32 empty_vidx = 0xFFFF;
    apollomp_raw_vlan4kentry_t vlan;
    apollomp_raw_vlanconfig_t mbrCfg;

    RT_PARAM_CHK((vid > RTK_EXT_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((NULL == pVidx), RT_ERR_NULL_POINTER);

    if(DAL_APOLLOMP_VLAN_IS_SET(vid))
    {
        for(vidx = 0; vidx <= HAL_MAX_VLAN_MBRCFG_IDX(); vidx++)
        {
            if(DAL_APOLLOMP_VLAN_MBECFG_VID(vidx) == vid)
            {
                *pVidx = vidx;
                return RT_ERR_OK;
            }
            else if(DAL_APOLLOMP_VLAN_MBECFG_VID(vidx) == 0) /* Unused */
            {
                if(empty_vidx == 0xFFFF)
                    empty_vidx = vidx;
            }
        }
    }
    else
        return RT_ERR_VLAN_VID;

    if(empty_vidx == 0xFFFF)
    {
        /* No empty index */
        return RT_ERR_ENTRY_FULL;
    }
    else
    {
        if(vid <= RTK_VLAN_ID_MAX) /* sync when VID is less than 4K */
        {
            /* found space, move 4k entry to mbrcfg */
            osal_memset(&vlan, 0x00, sizeof(apollomp_raw_vlan4kentry_t));
            vlan.vid = vid;
            if((ret = apollomp_raw_vlan_4kEntry_get(&vlan)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                return ret;
            }

            osal_memset(&mbrCfg, 0x00, sizeof(apollomp_raw_vlanconfig_t));
            mbrCfg.index    = empty_vidx;
            mbrCfg.evid     = vlan.vid;
            RTK_PORTMASK_ASSIGN(mbrCfg.mbr, vlan.mbr);
            RTK_PORTMASK_ASSIGN(mbrCfg.exMbr, vlan.exMbr);
            mbrCfg.fid_msti = vlan.fid_msti;
            mbrCfg.envlanpol= vlan.envlanpol;
            mbrCfg.meteridx = vlan.meteridx;
            mbrCfg.vbpen    = vlan.vbpen;
            mbrCfg.vbpri    = vlan.vbpri;
            if((ret = apollomp_raw_vlan_memberConfig_set(&mbrCfg)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                return ret;
            }

            *pVidx = empty_vidx;
            DAL_APOLLOMP_VLAN_MBECFG_SET(empty_vidx, vid);
        }
        else
            return RT_ERR_ENTRY_NOTFOUND;
    }

    return RT_ERR_OK;

}

/* Function Name:
 *      dal_apollomp_vlan_init
 * Description:
 *      Initialize vlan module.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize vlan module before calling any vlan APIs.
 */
int32
dal_apollomp_vlan_init(void)
{
    int32   ret;
    rtk_port_t port;
    rtk_portmask_t allPortMask;
    rtk_portmask_t allExtPortMask;
    rtk_portmask_t empty_portmask;
    apollomp_raw_vlan4kentry_t vlan;
    apollomp_raw_vlanconfig_t mbrCfg;
    rtk_leaky_type_t leaky_type;
    uint32 idx;
    rtk_vlan_protoGroup_t protoGroup;
    rtk_vlan_protoVlanCfg_t protoVlan;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    osal_memset(vlan_valid, 0x00, sizeof(uint32) * ((RTK_EXT_VLAN_ID_MAX + 1) / 32));
    osal_memset(vlan_mbrCfgVid, 0x00, sizeof(rtk_vlan_t) * DAL_APOLLOMP_MAX_NUM_MBR_CFG);
    vlan_init = INIT_COMPLETED;

    /* Reset VLAN Table */
    if((ret = dal_apollomp_vlan_destroyAll(DISABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        vlan_init = INIT_NOT_COMPLETED;
        return ret;
    }

    /* VLAN filter */
    if((ret = apollomp_raw_vlan_egrIgrFilterEnable_set(ENABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        vlan_init = INIT_NOT_COMPLETED;
        return ret;
    }

    HAL_SCAN_ALL_PORT(port)
    {
        /* VLAN Ingress filter */
        if((ret = apollomp_raw_vlan_igrFlterEnable_set(port, ENABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
            vlan_init = INIT_NOT_COMPLETED;
            return ret;
        }

        /* Egress Tag Mode */
        if((ret = apollomp_raw_vlan_egrTagMode_set(port, APOLLOMP_VLAN_EGR_TAG_MODE_ORI)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
            vlan_init = INIT_NOT_COMPLETED;
            return ret;
        }
    }

    /* Create Default VLAN */
    osal_memset(&vlan, 0x00, sizeof(apollomp_raw_vlan4kentry_t));
    vlan.vid = DAL_APOLLOMP_DEFAULT_VLAN_ID;

    HAL_GET_ALL_PORTMASK(allPortMask);
    RTK_PORTMASK_ASSIGN(vlan.mbr, allPortMask);
    RTK_PORTMASK_ASSIGN(vlan.untag, allPortMask);
    HAL_GET_ALL_EXT_PORTMASK(allExtPortMask);
    RTK_PORTMASK_ASSIGN(vlan.exMbr, allExtPortMask);
    if((ret = apollomp_raw_vlan_4kEntry_set(&vlan)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        vlan_init = INIT_NOT_COMPLETED;
        return ret;
    }

    /* Set VLAN Member Config */
    osal_memset(&mbrCfg, 0x00, sizeof(apollomp_raw_vlanconfig_t));
    mbrCfg.index = 0;
    mbrCfg.evid = DAL_APOLLOMP_DEFAULT_VLAN_ID;
    RTK_PORTMASK_ASSIGN(mbrCfg.mbr, allPortMask);
    if((ret = apollomp_raw_vlan_memberConfig_set(&mbrCfg)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        vlan_init = INIT_NOT_COMPLETED;
        return ret;
    }
#if 0
    /* Set VLAN Member Config 31 for port Isolation */
    osal_memset(&mbrCfg, 0x00, sizeof(apollomp_raw_vlanconfig_t));
    mbrCfg.index = 31;
    mbrCfg.evid = 0;
    RTK_PORTMASK_ASSIGN(mbrCfg.mbr, allPortMask);
    if((ret = apollomp_raw_vlan_memberConfig_set(&mbrCfg)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        vlan_init = INIT_NOT_COMPLETED;
        return ret;
    }
#endif
    /* Set PVID */
    HAL_SCAN_ALL_PORT(port)
    {
        if((ret = apollomp_raw_vlan_portBasedVID_set(port, 0, 0)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
            vlan_init = INIT_NOT_COMPLETED;
            return ret;
        }
    }

    /* Database update */
    DAL_APOLLOMP_VLAN_SET(DAL_APOLLOMP_DEFAULT_VLAN_ID)
    DAL_APOLLOMP_VLAN_MBECFG_SET(0, DAL_APOLLOMP_DEFAULT_VLAN_ID);
#if 0
    DAL_APOLLOMP_VLAN_MBECFG_SET(31, 0);
#endif

    HAL_SCAN_ALL_PORT(port)
    {
        if((ret = dal_apollomp_vlan_portAcceptFrameType_set(port, ACCEPT_FRAME_TYPE_ALL )) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
            vlan_init = INIT_NOT_COMPLETED;
            return ret;
        }

        if((ret = dal_apollomp_vlan_portIgrFilterEnable_set(port, ENABLED )) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
            vlan_init = INIT_NOT_COMPLETED;
            return ret;
        }
    }

    if((ret = dal_apollomp_vlan_vlanFunctionEnable_set(ENABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        vlan_init = INIT_NOT_COMPLETED;
        return ret;
    }

    for(leaky_type = LEAKY_BRG_GROUP; leaky_type < LEAKY_END; leaky_type++)
    {
        ret = dal_apollomp_vlan_leaky_set(leaky_type, DISABLED);
        if( ret == RT_ERR_CHIP_NOT_SUPPORTED)
            continue;

        if( ret != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
            vlan_init = INIT_NOT_COMPLETED;
            return ret;
        }
    }

    osal_memset(&protoGroup, 0x00, sizeof(rtk_vlan_protoGroup_t));
    for(idx = 0; idx <= HAL_PROTOCOL_VLAN_IDX_MAX(); idx++)
    {
        if((ret = dal_apollomp_vlan_protoGroup_set(idx, &protoGroup)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
            vlan_init = INIT_NOT_COMPLETED;
            return ret;
        }
    }

    osal_memset(&protoVlan, 0x00, sizeof(rtk_vlan_protoVlanCfg_t));
    protoVlan.vid = DAL_APOLLOMP_DEFAULT_VLAN_ID;
    HAL_SCAN_ALL_PORT(port)
    {
        if((ret = dal_apollomp_vlan_portProtoVlan_set(port, 0, &protoVlan)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
            vlan_init = INIT_NOT_COMPLETED;

            return ret;
        }
    }

    HAL_SCAN_ALL_PORT(port)
    {
        if((ret = dal_apollomp_vlan_portFid_set(port, 0, DISABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
            vlan_init = INIT_NOT_COMPLETED;
            return ret;
        }
    }

    RTK_PORTMASK_RESET(empty_portmask);
    HAL_SCAN_ALL_PORT(port)
    {
        if((ret = dal_apollomp_vlan_portEgrTagKeepType_set(port, &empty_portmask, TAG_KEEP_TYPE_CONTENT)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
            vlan_init = INIT_NOT_COMPLETED;
            return ret;
        }
    }

    if((ret = dal_apollomp_vlan_transparentEnable_set(DISABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        vlan_init = INIT_NOT_COMPLETED;
        return ret;
    }

    if((ret = dal_apollomp_vlan_cfiKeepEnable_set(DISABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        vlan_init = INIT_NOT_COMPLETED;
        return ret;
    }

    if((ret = dal_apollomp_vlan_reservedVidAction_set(RESVID_ACTION_UNTAG, RESVID_ACTION_UNTAG)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        vlan_init = INIT_NOT_COMPLETED;
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_vlan_init */

/* Module Name    : Vlan                                  */
/* Sub-module Name: Vlan table configure and modification */

/* Function Name:
 *      dal_apollomp_vlan_create
 * Description:
 *      Create the vlan in the specified device.
 * Input:
 *      vid  - vlan id to be created
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_VLAN_VID   - invalid vid
 *      RT_ERR_VLAN_EXIST - vlan is exist
 * Note:
 */
int32
dal_apollomp_vlan_create(rtk_vlan_t vid)
{
    int32 ret;
    apollomp_raw_vlanconfig_t mbrCfg;
    uint32 vidx;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_EXT_VLAN_ID_MAX < vid), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((DAL_APOLLOMP_VLAN_IS_SET(vid)), RT_ERR_VLAN_EXIST);

    if(vid > RTK_VLAN_ID_MAX)
    {
        for(vidx = 0; vidx <= HAL_MAX_VLAN_MBRCFG_IDX(); vidx++)
        {
            if(DAL_APOLLOMP_VLAN_MBECFG_VID(vidx) == 0)
            {
                osal_memset(&mbrCfg, 0x00, sizeof(apollomp_raw_vlanconfig_t));
                mbrCfg.index = vidx;
                mbrCfg.evid  = vid;
                if((ret = apollomp_raw_vlan_memberConfig_set(&mbrCfg)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                    return ret;
                }

                DAL_APOLLOMP_VLAN_MBECFG_SET(vidx, vid)
                break;
            }
        }
    }

    DAL_APOLLOMP_VLAN_SET(vid);
    return RT_ERR_OK;
} /* end of dal_apollomp_vlan_create */

/* Function Name:
 *      dal_apollomp_vlan_destroy
 * Description:
 *      Destroy the vlan.
 * Input:
 *      vid  - vlan id to be destroyed
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 * Note:
 *      None
 */
int32
dal_apollomp_vlan_destroy(rtk_vlan_t vid)
{
    int32 ret;
    apollomp_raw_vlan4kentry_t vlan;
    apollomp_raw_vlanconfig_t mbrCfg;
    uint32 vidx;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_EXT_VLAN_ID_MAX < vid), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((!DAL_APOLLOMP_VLAN_IS_SET(vid)), RT_ERR_VLAN_ENTRY_NOT_FOUND);

    /* Clear 4K Entry */
    if(vid <= RTK_VLAN_ID_MAX)
    {
        osal_memset(&vlan, 0x00, sizeof(apollomp_raw_vlan4kentry_t));
        vlan.vid = vid;
        if((ret = apollomp_raw_vlan_4kEntry_get(&vlan)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
            return ret;
        }
    }

    /* Clear mbrcfg */
    for(vidx = 0; vidx <= HAL_MAX_VLAN_MBRCFG_IDX(); vidx++)
    {
        if(DAL_APOLLOMP_VLAN_MBECFG_VID(vidx) == vid)
        {
            osal_memset(&mbrCfg, 0x00, sizeof(apollomp_raw_vlanconfig_t));
            mbrCfg.index = vidx;
            if((ret = apollomp_raw_vlan_memberConfig_set(&mbrCfg)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                return ret;
            }

            DAL_APOLLOMP_VLAN_MBECFG_SET(vidx, 0)
            break;
        }
    }

    /* Clear Database */
    DAL_APOLLOMP_VLAN_CLEAR(vid);

    return RT_ERR_OK;
} /* end of dal_apollomp_vlan_destroy */


/* Function Name:
 *      dal_apollomp_vlan_destroyAll
 * Description:
 *      Destroy all vlans except default vlan.
 * Input:
 *      restore_default_vlan - keep and restore default vlan id or not?
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 * Note:
 *      The restore argument is permit following value:
 *      - 0: remove default vlan
 *      - 1: restore default vlan
 */
int32
dal_apollomp_vlan_destroyAll(uint32 restoreDefaultVlan)
{
    int32   ret;
    apollomp_raw_vlanconfig_t mbrCfg;
    apollomp_raw_vlan4kentry_t vlan;
    uint32 index;
    rtk_port_t port;
    rtk_portmask_t allPortMask;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= restoreDefaultVlan), RT_ERR_INPUT);

    /* Clear 4K VLAN */
    if((ret = apollomp_l2_table_clear(APOLLOMP_VLANt, RTK_VLAN_ID_MIN, RTK_VLAN_ID_MAX)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
		return ret;
    }

    /* Clear Member Config */
    osal_memset(&mbrCfg, 0x00, sizeof(apollomp_raw_vlanconfig_t));
    for(index = 0; index <= HAL_MAX_VLAN_MBRCFG_IDX(); index++)
    {
        mbrCfg.index = index;
        if((ret = apollomp_raw_vlan_memberConfig_set(&mbrCfg)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
            return ret;
        }
    }

    /* Update database */
    osal_memset(vlan_valid, 0x00, sizeof(uint32) * (RTK_EXT_VLAN_ID_MAX + 1) / 32);
    osal_memset(vlan_mbrCfgVid, 0x00, sizeof(rtk_vlan_t) * DAL_APOLLOMP_MAX_NUM_MBR_CFG);

    if(ENABLED == restoreDefaultVlan)
    {
        /* Create Default VLAN */
        osal_memset(&vlan, 0x00, sizeof(apollomp_raw_vlan4kentry_t));
        vlan.vid = DAL_APOLLOMP_DEFAULT_VLAN_ID;

        HAL_GET_ALL_PORTMASK(allPortMask);
        RTK_PORTMASK_ASSIGN(vlan.mbr, allPortMask);
        RTK_PORTMASK_ASSIGN(vlan.untag, allPortMask);
        if((ret = apollomp_raw_vlan_4kEntry_set(&vlan)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
            return ret;
        }

        /* Set VLAN Member Config */
        osal_memset(&mbrCfg, 0x00, sizeof(apollomp_raw_vlanconfig_t));
        mbrCfg.index = 0;
        mbrCfg.evid = DAL_APOLLOMP_DEFAULT_VLAN_ID;
        RTK_PORTMASK_ASSIGN(mbrCfg.mbr, allPortMask);
        if((ret = apollomp_raw_vlan_memberConfig_set(&mbrCfg)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
            return ret;
        }

        /* Set PVID */
        HAL_SCAN_ALL_PORT(port)
        {
            if((ret = apollomp_raw_vlan_portBasedVID_set(port, 0, 0)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                return ret;
            }
        }

        /* Database update */
        DAL_APOLLOMP_VLAN_SET(DAL_APOLLOMP_DEFAULT_VLAN_ID)
        DAL_APOLLOMP_VLAN_MBECFG_SET(0, DAL_APOLLOMP_DEFAULT_VLAN_ID);
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_vlan_destroyAll */

/* Function Name:
 *      dal_apollomp_vlan_fid_get
 * Description:
 *      Get the filter id of the vlan.
 * Input:
 *      vid  - vlan id
 * Output:
 *      pFid - pointer buffer of filter id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      (1) In IVL mode, vid is equal with fid after vlan create.
 *      (2) You don't need to care fid when you use the IVL mode.
 *      (3) The API should be used for SVL mode.
 */
int32
dal_apollomp_vlan_fid_get(rtk_vlan_t vid, rtk_fid_t *pFid)
{
    int32   ret;
    apollomp_raw_vlan4kentry_t vlan;
    apollomp_raw_vlanconfig_t mbrCfg;
    uint32 vidx;
    uint32 found = 0;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_EXT_VLAN_ID_MAX < vid), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((NULL == pFid), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((!DAL_APOLLOMP_VLAN_IS_SET(vid)), RT_ERR_VLAN_ENTRY_NOT_FOUND);

    if(vid <= RTK_VLAN_ID_MAX)
    {
        osal_memset(&vlan, 0x00, sizeof(apollomp_raw_vlan4kentry_t));
        vlan.vid = vid;
        if ((ret = apollomp_raw_vlan_4kEntry_get(&vlan)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
            return ret;
        }

        *pFid = (rtk_fid_t)vlan.fid_msti;
    }
    else
    {
        for(vidx = 0; vidx <= HAL_MAX_VLAN_MBRCFG_IDX(); vidx++)
        {
            if(DAL_APOLLOMP_VLAN_MBECFG_VID(vidx) == vid)
            {
                osal_memset(&mbrCfg, 0x00, sizeof(apollomp_raw_vlanconfig_t));
                mbrCfg.index = vidx;
                if((ret = apollomp_raw_vlan_memberConfig_get(&mbrCfg)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                    return ret;
                }

                found = 1;
                break;
            }
        }

        if(1 == found)
            *pFid = (rtk_fid_t)mbrCfg.fid_msti;
        else
            return RT_ERR_ENTRY_NOTFOUND;

    }

    return RT_ERR_OK;
} /* end of dal_apollomp_vlan_fid_get */


/* Function Name:
 *      dal_apollomp_vlan_fid_set
 * Description:
 *      Set the filter id of the vlan.
 * Input:
 *      vid  - vlan id
 *      fid  - filter id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_OUT_OF_RANGE         - input parameter out of range
 * Note:
 */
int32
dal_apollomp_vlan_fid_set(rtk_vlan_t vid, rtk_fid_t fid)
{
    int32   ret;
    apollomp_raw_vlan4kentry_t vlan;
    apollomp_raw_vlanconfig_t mbrCfg;
    uint32 vidx;
    uint32 found = 0;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_EXT_VLAN_ID_MAX < vid), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((HAL_VLAN_FID_MAX() < fid), RT_ERR_FID);
    RT_PARAM_CHK((!DAL_APOLLOMP_VLAN_IS_SET(vid)), RT_ERR_VLAN_ENTRY_NOT_FOUND);

    if(vid <= RTK_VLAN_ID_MAX)
    {
        osal_memset(&vlan, 0x00, sizeof(apollomp_raw_vlan4kentry_t));
        vlan.vid = vid;
        if ((ret = apollomp_raw_vlan_4kEntry_get(&vlan)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
            return ret;
        }

        vlan.fid_msti = (uint32)fid;

        if ((ret = apollomp_raw_vlan_4kEntry_set(&vlan)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
            return ret;
        }

        if ((ret = _dal_apollomp_vlan_checkAndUpdate(vid)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
            return ret;
        }
    }
    else
    {

        for(vidx = 0; vidx <= HAL_MAX_VLAN_MBRCFG_IDX(); vidx++)
        {
            if(DAL_APOLLOMP_VLAN_MBECFG_VID(vidx) == vid)
            {
                osal_memset(&mbrCfg, 0x00, sizeof(apollomp_raw_vlanconfig_t));
                mbrCfg.index = vidx;
                if((ret = apollomp_raw_vlan_memberConfig_get(&mbrCfg)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                    return ret;
                }

                mbrCfg.fid_msti = fid;

                if((ret = apollomp_raw_vlan_memberConfig_set(&mbrCfg)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                    return ret;
                }

                found = 1;
                break;
            }
        }

        if(0 == found)
            return RT_ERR_ENTRY_NOTFOUND;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_vlan_fid_set */

/* Function Name:
 *      dal_apollomp_vlan_fidMode_get
 * Description:
 *      Get the filter id mode of the vlan.
 * Input:
 *      vid  - vlan id
 * Output:
 *      pMode - pointer buffer of filter id mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      mode can be: -VLAN__FID_IVL
 *                   -VLAN__FID_SVL
 */
int32
dal_apollomp_vlan_fidMode_get(rtk_vlan_t vid, rtk_fidMode_t *pMode)
{
    int32   ret;
    apollomp_raw_vlan4kentry_t vlan;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_EXT_VLAN_ID_MAX < vid), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((NULL == pMode), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((!DAL_APOLLOMP_VLAN_IS_SET(vid)), RT_ERR_VLAN_ENTRY_NOT_FOUND);

    if(vid <= RTK_VLAN_ID_MAX)
    {
        osal_memset(&vlan, 0x00, sizeof(apollomp_raw_vlan4kentry_t));
        vlan.vid = vid;
        if ((ret = apollomp_raw_vlan_4kEntry_get(&vlan)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
            return ret;
        }

        *pMode = (rtk_fidMode_t)((APOLLOMP_VLAN_HASH_IVL == vlan.ivl_svl) ? VLAN_FID_IVL : VLAN_FID_SVL);
    }
    else
    {
        *pMode = VLAN_FID_SVL;
    }


    return RT_ERR_OK;
} /* end of dal_apollomp_vlan_fidMode_get */

/* Function Name:
 *      dal_apollomp_vlan_fidMode_set
 * Description:
 *      Set the filter id mode of the vlan.
 * Input:
 *      vid   - vlan id
 *      mode  - filter id mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_OUT_OF_RANGE         - input parameter out of range
 * Note:
 *      mode can be: -VLAN__FID_IVL
 *                   -VLAN__FID_SVL
 */
int32
dal_apollomp_vlan_fidMode_set(rtk_vlan_t vid, rtk_fidMode_t mode)
{
    int32   ret;
    apollomp_raw_vlan4kentry_t vlan;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_EXT_VLAN_ID_MAX < vid), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((VLAN_FID_MODE_END <= mode), RT_ERR_INPUT);
    RT_PARAM_CHK((!DAL_APOLLOMP_VLAN_IS_SET(vid)), RT_ERR_VLAN_ENTRY_NOT_FOUND);


    if(vid <= RTK_VLAN_ID_MAX)
    {
        osal_memset(&vlan, 0x00, sizeof(apollomp_raw_vlan4kentry_t));
        vlan.vid = vid;
        if ((ret = apollomp_raw_vlan_4kEntry_get(&vlan)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
            return ret;
        }

        vlan.ivl_svl = (apollomp_raw_vlan_l2HashType_t)((VLAN_FID_IVL == mode) ? APOLLOMP_VLAN_HASH_IVL : APOLLOMP_VLAN_HASH_SVL);

        if ((ret = apollomp_raw_vlan_4kEntry_set(&vlan)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
            return ret;
        }

        if ((ret = _dal_apollomp_vlan_checkAndUpdate(vid)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
            return ret;
        }
    }
    else
    {
        if(mode == VLAN_FID_IVL)
        {
            return RT_ERR_INPUT;
        }
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_vlan_fidMode_set */

/* Function Name:
 *      dal_apollomp_vlan_port_get
 * Description:
 *      Get the vlan members.
 * Input:
 *      vid              - vlan id
 * Output:
 *      pMember_portmask - pointer buffer of member ports
 *      pUntag_portmask  - pointer buffer of untagged member ports
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_vlan_port_get(
    rtk_vlan_t     vid,
    rtk_portmask_t *pMemberPortmask,
    rtk_portmask_t *pUntagPortmask)
{
    int32   ret;
    apollomp_raw_vlan4kentry_t vlan;
    apollomp_raw_vlanconfig_t mbrCfg;
    uint32 vidx;
    uint32 found = 0;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_EXT_VLAN_ID_MAX < vid), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((NULL == pMemberPortmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pUntagPortmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((!DAL_APOLLOMP_VLAN_IS_SET(vid)), RT_ERR_VLAN_ENTRY_NOT_FOUND);

    if(vid <= RTK_VLAN_ID_MAX)
    {
        osal_memset(&vlan, 0x00, sizeof(apollomp_raw_vlan4kentry_t));
        vlan.vid = vid;
        if ((ret = apollomp_raw_vlan_4kEntry_get(&vlan)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
            return ret;
        }

        RTK_PORTMASK_ASSIGN((*pMemberPortmask), vlan.mbr);
        RTK_PORTMASK_ASSIGN((*pUntagPortmask), vlan.untag);
    }
    else
    {
        for(vidx = 0; vidx <= HAL_MAX_VLAN_MBRCFG_IDX(); vidx++)
        {
            if(DAL_APOLLOMP_VLAN_MBECFG_VID(vidx) == vid)
            {
                osal_memset(&mbrCfg, 0x00, sizeof(apollomp_raw_vlanconfig_t));
                mbrCfg.index = vidx;
                if((ret = apollomp_raw_vlan_memberConfig_get(&mbrCfg)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                    return ret;
                }

                RTK_PORTMASK_ASSIGN((*pMemberPortmask), mbrCfg.mbr);
                RTK_PORTMASK_RESET((*pUntagPortmask));
                found = 1;
                break;
            }
        }

        if(0 == found)
            return RT_ERR_ENTRY_NOTFOUND;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_vlan_port_get */


/* Function Name:
 *      dal_apollomp_vlan_port_set
 * Description:
 *      Replace the vlan members.
 * Input:
 *      vid              - vlan id
 *      pMember_portmask - member ports
 *      pUntag_portmask  - untagged member ports
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      Don't care the original vlan members and replace with new configure
 *      directly.
 */
int32
dal_apollomp_vlan_port_set(
    rtk_vlan_t     vid,
    rtk_portmask_t *pMember_portmask,
    rtk_portmask_t *pUntag_portmask)
{
    int32   ret;
    apollomp_raw_vlan4kentry_t vlan;
    apollomp_raw_vlanconfig_t mbrCfg;
    uint32 vidx;
    uint32 found = 0;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_EXT_VLAN_ID_MAX < vid), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((NULL == pMember_portmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pUntag_portmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HAL_IS_PORTMASK_VALID((*pMember_portmask)), RT_ERR_PORT_MASK);
    RT_PARAM_CHK(!HAL_IS_PORTMASK_VALID((*pUntag_portmask)), RT_ERR_PORT_MASK);
    RT_PARAM_CHK((!DAL_APOLLOMP_VLAN_IS_SET(vid)), RT_ERR_VLAN_ENTRY_NOT_FOUND);

    if(vid <= RTK_VLAN_ID_MAX)
    {
        osal_memset(&vlan, 0x00, sizeof(apollomp_raw_vlan4kentry_t));
        vlan.vid = vid;
        if ((ret = apollomp_raw_vlan_4kEntry_get(&vlan)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
            return ret;
        }

        RTK_PORTMASK_ASSIGN(vlan.mbr, (*pMember_portmask));
        RTK_PORTMASK_ASSIGN(vlan.untag, (*pUntag_portmask));

        if ((ret = apollomp_raw_vlan_4kEntry_set(&vlan)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
            return ret;
        }

        if ((ret = _dal_apollomp_vlan_checkAndUpdate(vid)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
            return ret;
        }
    }
    else
    {
        for(vidx = 0; vidx <= HAL_MAX_VLAN_MBRCFG_IDX(); vidx++)
        {
            if(DAL_APOLLOMP_VLAN_MBECFG_VID(vidx) == vid)
            {
                osal_memset(&mbrCfg, 0x00, sizeof(apollomp_raw_vlanconfig_t));
                mbrCfg.index = vidx;
                if((ret = apollomp_raw_vlan_memberConfig_get(&mbrCfg)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                    return ret;
                }

                RTK_PORTMASK_ASSIGN(mbrCfg.mbr, (*pMember_portmask));

                if((ret = apollomp_raw_vlan_memberConfig_set(&mbrCfg)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                    return ret;
                }

                found = 1;
                break;
            }
        }

        if(0 == found)
            return RT_ERR_ENTRY_NOTFOUND;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_vlan_port_set */

/* Function Name:
 *      dal_apollomp_vlan_extPort_get
 * Description:
 *      Get the vlan extension members.
 * Input:
 *      vid              - vlan id
 * Output:
 *      pExt_portmask    - pointer buffer of extension member ports
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_vlan_extPort_get(
    rtk_vlan_t     vid,
    rtk_portmask_t *pExt_portmask)
{
    int32   ret;
    apollomp_raw_vlan4kentry_t vlan;
    apollomp_raw_vlanconfig_t mbrCfg;
    uint32 vidx;
    uint32 found = 0;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_EXT_VLAN_ID_MAX <= vid), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pExt_portmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((!DAL_APOLLOMP_VLAN_IS_SET(vid)), RT_ERR_VLAN_ENTRY_NOT_FOUND);

    if(vid <= RTK_VLAN_ID_MAX)
    {
        osal_memset(&vlan, 0x00, sizeof(apollomp_raw_vlan4kentry_t));
        vlan.vid = vid;
        if ((ret = apollomp_raw_vlan_4kEntry_get(&vlan)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
            return ret;
        }

        RTK_PORTMASK_ASSIGN((*pExt_portmask), vlan.exMbr);
    }
    else
    {
        for(vidx = 0; vidx <= HAL_MAX_VLAN_MBRCFG_IDX(); vidx++)
        {
            if(DAL_APOLLOMP_VLAN_MBECFG_VID(vidx) == vid)
            {
                osal_memset(&mbrCfg, 0x00, sizeof(apollomp_raw_vlanconfig_t));
                mbrCfg.index = vidx;
                if((ret = apollomp_raw_vlan_memberConfig_get(&mbrCfg)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                    return ret;
                }

                RTK_PORTMASK_ASSIGN((*pExt_portmask), mbrCfg.exMbr);
                found = 1;
                break;
            }
        }

        if(0 == found)
            return RT_ERR_ENTRY_NOTFOUND;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_vlan_extPort_get */


/* Function Name:
 *      dal_apollomp_vlan_extPort_set
 * Description:
 *      Replace the vlan extension members.
 * Input:
 *      vid              - vlan id
 *      pExt_portmask    - extension member ports
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      Don't care the original vlan members and replace with new configure
 *      directly.
 */
int32
dal_apollomp_vlan_extPort_set(
    rtk_vlan_t     vid,
    rtk_portmask_t *pExt_portmask)
{
    int32   ret;
    apollomp_raw_vlan4kentry_t vlan;
    apollomp_raw_vlanconfig_t mbrCfg;
    uint32 vidx;
    uint32 found = 0;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_EXT_VLAN_ID_MAX <= vid), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pExt_portmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((!DAL_APOLLOMP_VLAN_IS_SET(vid)), RT_ERR_VLAN_ENTRY_NOT_FOUND);
    RT_PARAM_CHK(!HAL_IS_EXTPORTMASK_VALID((*pExt_portmask)), RT_ERR_PORT_MASK);

    if(vid <= RTK_VLAN_ID_MAX)
    {
        osal_memset(&vlan, 0x00, sizeof(apollomp_raw_vlan4kentry_t));
        vlan.vid = vid;
        if ((ret = apollomp_raw_vlan_4kEntry_get(&vlan)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
            return ret;
        }

        RTK_PORTMASK_ASSIGN(vlan.exMbr, (*pExt_portmask));

        if ((ret = apollomp_raw_vlan_4kEntry_set(&vlan)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
            return ret;
        }

        if ((ret = _dal_apollomp_vlan_checkAndUpdate(vid)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
            return ret;
        }
    }
    else
    {
        for(vidx = 0; vidx <= HAL_MAX_VLAN_MBRCFG_IDX(); vidx++)
        {
            if(DAL_APOLLOMP_VLAN_MBECFG_VID(vidx) == vid)
            {
                osal_memset(&mbrCfg, 0x00, sizeof(apollomp_raw_vlanconfig_t));
                mbrCfg.index = vidx;
                if((ret = apollomp_raw_vlan_memberConfig_get(&mbrCfg)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                    return ret;
                }

                RTK_PORTMASK_ASSIGN(mbrCfg.exMbr, (*pExt_portmask));

                if((ret = apollomp_raw_vlan_memberConfig_set(&mbrCfg)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                    return ret;
                }

                found = 1;
                break;
            }
        }

        if(0 == found)
            return RT_ERR_ENTRY_NOTFOUND;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_vlan_extPort_set */

/* Function Name:
 *      dal_apollomp_vlan_stg_get
 * Description:
 *      Get spanning tree group instance of the vlan from the specified device.
 * Input:
 *      vid  - vlan id
 * Output:
 *      pStg - pointer buffer of spanning tree group instance
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_vlan_stg_get(rtk_vlan_t vid, rtk_stg_t *pStg)
{
    int32   ret;
    apollomp_raw_vlan4kentry_t vlan;
    apollomp_raw_vlanconfig_t mbrCfg;
    uint32 vidx;
    uint32 found = 0;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_EXT_VLAN_ID_MAX < vid), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((NULL == pStg), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((!DAL_APOLLOMP_VLAN_IS_SET(vid)), RT_ERR_VLAN_ENTRY_NOT_FOUND);

    if(vid <= RTK_VLAN_ID_MAX)
    {
        osal_memset(&vlan, 0x00, sizeof(apollomp_raw_vlan4kentry_t));
        vlan.vid = vid;
        if ((ret = apollomp_raw_vlan_4kEntry_get(&vlan)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
            return ret;
        }

        *pStg = (rtk_stg_t)vlan.fid_msti;
    }
    else
    {
        for(vidx = 0; vidx <= HAL_MAX_VLAN_MBRCFG_IDX(); vidx++)
        {
            if(DAL_APOLLOMP_VLAN_MBECFG_VID(vidx) == vid)
            {
                osal_memset(&mbrCfg, 0x00, sizeof(apollomp_raw_vlanconfig_t));
                mbrCfg.index = vidx;
                if((ret = apollomp_raw_vlan_memberConfig_get(&mbrCfg)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                    return ret;
                }

                *pStg = (rtk_stg_t)mbrCfg.fid_msti;
                found = 1;
                break;
            }
        }

        if(0 == found)
            return RT_ERR_ENTRY_NOTFOUND;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_vlan_stg_get */


/* Function Name:
 *      dal_apollomp_vlan_stg_set
 * Description:
 *      Set spanning tree group instance of the vlan.
 * Input:
 *      vid  - vlan id
 *      stg  - spanning tree group instance
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_MSTI                 - invalid msti
 * Note:
 *      None
 */
int32
dal_apollomp_vlan_stg_set(rtk_vlan_t vid, rtk_stg_t stg)
{
    int32   ret;
    apollomp_raw_vlan4kentry_t vlan;
    apollomp_raw_vlanconfig_t mbrCfg;
    uint32 vidx;
    uint32 found = 0;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_EXT_VLAN_ID_MAX < vid), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((HAL_MAX_NUM_OF_MSTI() <= stg), RT_ERR_MSTI);
    RT_PARAM_CHK((!DAL_APOLLOMP_VLAN_IS_SET(vid)), RT_ERR_VLAN_ENTRY_NOT_FOUND);

    if(vid <= RTK_VLAN_ID_MAX)
    {
        osal_memset(&vlan, 0x00, sizeof(apollomp_raw_vlan4kentry_t));
        vlan.vid = vid;
        if ((ret = apollomp_raw_vlan_4kEntry_get(&vlan)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
            return ret;
        }

        vlan.fid_msti = (uint32)stg;

        if ((ret = apollomp_raw_vlan_4kEntry_set(&vlan)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
            return ret;
        }

        if ((ret = _dal_apollomp_vlan_checkAndUpdate(vid)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
            return ret;
        }
    }
    else
    {
        for(vidx = 0; vidx <= HAL_MAX_VLAN_MBRCFG_IDX(); vidx++)
        {
            if(DAL_APOLLOMP_VLAN_MBECFG_VID(vidx) == vid)
            {
                osal_memset(&mbrCfg, 0x00, sizeof(apollomp_raw_vlanconfig_t));
                mbrCfg.index = vidx;
                if((ret = apollomp_raw_vlan_memberConfig_get(&mbrCfg)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                    return ret;
                }

                mbrCfg.fid_msti = (uint32)stg;

                if((ret = apollomp_raw_vlan_memberConfig_set(&mbrCfg)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                    return ret;
                }

                found = 1;
                break;
            }
        }

        if(0 == found)
            return RT_ERR_ENTRY_NOTFOUND;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_vlan_stg_set */

/* Function Name:
 *      dal_apollomp_vlan_priority_get
 * Description:
 *      Get VLAN priority for each CVLAN.
 * Input:
 *      vid  - vlan id
 * Output:
 *      pPriority - 802.1p priority for the PVID.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_VLAN_VID     - Invalid VID parameter.
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *     This API is used to set priority per VLAN.
 */
int32
dal_apollomp_vlan_priority_get(rtk_vlan_t vid, rtk_pri_t *pPriority)
{
    int32   ret;
    apollomp_raw_vlan4kentry_t vlan;
    apollomp_raw_vlanconfig_t mbrCfg;
    uint32 vidx;
    uint32 found = 0;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_EXT_VLAN_ID_MAX < vid), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((NULL == pPriority), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((!DAL_APOLLOMP_VLAN_IS_SET(vid)), RT_ERR_VLAN_ENTRY_NOT_FOUND);

    if(vid <= RTK_VLAN_ID_MAX)
    {
        osal_memset(&vlan, 0x00, sizeof(apollomp_raw_vlan4kentry_t));
        vlan.vid = vid;
        if ((ret = apollomp_raw_vlan_4kEntry_get(&vlan)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
            return ret;
        }

        *pPriority = (rtk_pri_t)vlan.vbpri;
    }
    else
    {
        for(vidx = 0; vidx <= HAL_MAX_VLAN_MBRCFG_IDX(); vidx++)
        {
            if(DAL_APOLLOMP_VLAN_MBECFG_VID(vidx) == vid)
            {
                osal_memset(&mbrCfg, 0x00, sizeof(apollomp_raw_vlanconfig_t));
                mbrCfg.index = vidx;
                if((ret = apollomp_raw_vlan_memberConfig_get(&mbrCfg)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                    return ret;
                }

                *pPriority = (rtk_pri_t)mbrCfg.vbpri;
                found = 1;
                break;
            }
        }

        if(0 == found)
            return RT_ERR_ENTRY_NOTFOUND;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_vlan_priority_get */

/* Function Name:
 *      dal_apollomp_vlan_priority_set
 * Description:
 *      Set VLAN priority for each CVLAN.
 * Input:
 *      vid  - vlan id
 *      priority - 802.1p priority for the PVID.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_VLAN_VID         - Invalid VID parameter.
 *      RT_ERR_VLAN_PRIORITY    - Invalid priority.
 * Note:
 *      This API is used to set priority per VLAN.
 */
int32
dal_apollomp_vlan_priority_set(rtk_vlan_t vid, rtk_pri_t priority)
{
    int32   ret;
    apollomp_raw_vlan4kentry_t vlan;
    apollomp_raw_vlanconfig_t mbrCfg;
    uint32 vidx;
    uint32 found = 0;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_EXT_VLAN_ID_MAX < vid), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((HAL_INTERNAL_PRIORITY_MAX() < priority), RT_ERR_PRIORITY);
    RT_PARAM_CHK((!DAL_APOLLOMP_VLAN_IS_SET(vid)), RT_ERR_VLAN_ENTRY_NOT_FOUND);

    if(vid <= RTK_VLAN_ID_MAX)
    {
        osal_memset(&vlan, 0x00, sizeof(apollomp_raw_vlan4kentry_t));
        vlan.vid = vid;
        if ((ret = apollomp_raw_vlan_4kEntry_get(&vlan)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
            return ret;
        }

        vlan.vbpri = (uint32)priority;

        if ((ret = apollomp_raw_vlan_4kEntry_set(&vlan)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
            return ret;
        }

        if ((ret = _dal_apollomp_vlan_checkAndUpdate(vid)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
            return ret;
        }
    }
    else
    {
        for(vidx = 0; vidx <= HAL_MAX_VLAN_MBRCFG_IDX(); vidx++)
        {
            if(DAL_APOLLOMP_VLAN_MBECFG_VID(vidx) == vid)
            {
                osal_memset(&mbrCfg, 0x00, sizeof(apollomp_raw_vlanconfig_t));
                mbrCfg.index = vidx;
                if((ret = apollomp_raw_vlan_memberConfig_get(&mbrCfg)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                    return ret;
                }

                mbrCfg.vbpri = (uint32)priority;

                if((ret = apollomp_raw_vlan_memberConfig_set(&mbrCfg)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                    return ret;
                }

                found = 1;
                break;
            }
        }

        if(0 == found)
            return RT_ERR_ENTRY_NOTFOUND;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_vlan_priority_set */

/* Function Name:
 *      dal_apollomp_vlan_priorityEnable_get
 * Description:
 *      Get vlan based priority assignment status.
 * Input:
 *      vid  - vlan id
 * Output:
 *      pEnable - pointer to vlan based priority assignment status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_vlan_priorityEnable_get(rtk_vlan_t vid, rtk_enable_t *pEnable)
{
    int32   ret;
    apollomp_raw_vlan4kentry_t vlan;
    apollomp_raw_vlanconfig_t mbrCfg;
    uint32 vidx;
    uint32 found = 0;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_EXT_VLAN_ID_MAX < vid), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((!DAL_APOLLOMP_VLAN_IS_SET(vid)), RT_ERR_VLAN_ENTRY_NOT_FOUND);

    if(vid <= RTK_VLAN_ID_MAX)
    {
        osal_memset(&vlan, 0x00, sizeof(apollomp_raw_vlan4kentry_t));
        vlan.vid = vid;
        if ((ret = apollomp_raw_vlan_4kEntry_get(&vlan)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
            return ret;
        }

        *pEnable = (rtk_enable_t)((ENABLED == vlan.vbpen) ? ENABLED : DISABLED);
    }
    else
    {
        for(vidx = 0; vidx <= HAL_MAX_VLAN_MBRCFG_IDX(); vidx++)
        {
            if(DAL_APOLLOMP_VLAN_MBECFG_VID(vidx) == vid)
            {
                osal_memset(&mbrCfg, 0x00, sizeof(apollomp_raw_vlanconfig_t));
                mbrCfg.index = vidx;
                if((ret = apollomp_raw_vlan_memberConfig_get(&mbrCfg)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                    return ret;
                }

                *pEnable = (rtk_enable_t)((ENABLED == mbrCfg.vbpen) ? ENABLED : DISABLED);

                found = 1;
                break;
            }
        }

        if(0 == found)
            return RT_ERR_ENTRY_NOTFOUND;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_vlan_priorityEnable_get */


/* Function Name:
 *      dal_apollomp_vlan_priorityEnable_set
 * Description:
 *      Set vlan based priority assignment status.
 * Input:
 *      vid    - vlan id
 *      enable - vlan based priority assignment status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 *      None
 */
int32
dal_apollomp_vlan_priorityEnable_set(rtk_vlan_t vid, rtk_enable_t enable)
{
    int32   ret;
    apollomp_raw_vlan4kentry_t vlan;
    apollomp_raw_vlanconfig_t mbrCfg;
    uint32 vidx;
    uint32 found = 0;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_EXT_VLAN_ID_MAX < vid), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);
    RT_PARAM_CHK((!DAL_APOLLOMP_VLAN_IS_SET(vid)), RT_ERR_VLAN_ENTRY_NOT_FOUND);

    if(vid <= RTK_VLAN_ID_MAX)
    {
        osal_memset(&vlan, 0x00, sizeof(apollomp_raw_vlan4kentry_t));
        vlan.vid = vid;
        if ((ret = apollomp_raw_vlan_4kEntry_get(&vlan)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
            return ret;
        }

        vlan.vbpen = (uint32)enable;

        if ((ret = apollomp_raw_vlan_4kEntry_set(&vlan)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
            return ret;
        }

        if ((ret = _dal_apollomp_vlan_checkAndUpdate(vid)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
            return ret;
        }
    }
    else
    {
        for(vidx = 0; vidx <= HAL_MAX_VLAN_MBRCFG_IDX(); vidx++)
        {
            if(DAL_APOLLOMP_VLAN_MBECFG_VID(vidx) == vid)
            {
                osal_memset(&mbrCfg, 0x00, sizeof(apollomp_raw_vlanconfig_t));
                mbrCfg.index = vidx;
                if((ret = apollomp_raw_vlan_memberConfig_get(&mbrCfg)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                    return ret;
                }

                mbrCfg.vbpen = (uint32)enable;

                if((ret = apollomp_raw_vlan_memberConfig_set(&mbrCfg)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                    return ret;
                }

                found = 1;
                break;
            }
        }

        if(0 == found)
            return RT_ERR_ENTRY_NOTFOUND;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_vlan_priorityEnable_set */


/* Module Name     : vlan                */
/* Sub-module Name : vlan port attribute */

/* Function Name:
 *      dal_apollomp_vlan_portAcceptFrameType_get
 * Description:
 *      Get vlan accept frame type of the port.
 * Input:
 *      port               - port id
 * Output:
 *      pAcceptFrameType   - pointer buffer of accept frame type
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The accept frame type as following:
 *          - ACCEPT_FRAME_TYPE_ALL
 *          - ACCEPT_FRAME_TYPE_TAG_ONLY
 *          - ACCEPT_FRAME_TYPE_UNTAG_ONLY
 *          - ACCPET_FRAME_TYPE_1P_1Q_TAG_ONLY
 *
 */
int32
dal_apollomp_vlan_portAcceptFrameType_get(
    rtk_port_t                 port,
    rtk_vlan_acceptFrameType_t *pAcceptFrameType)
{
    int32   ret;
    apollomp_raw_vlanAcpFrameType_t type;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pAcceptFrameType), RT_ERR_NULL_POINTER);

    if ((ret = apollomp_raw_vlan_acceptFrameType_get(port, &type)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }

    switch(type)
    {
        case APOLLOMP_VLAN_ACCEPT_ALL:
            *pAcceptFrameType = ACCEPT_FRAME_TYPE_ALL;
            break;
        case APOLLOMP_VLAN_ACCEPT_TAGGED_ONLY:
            *pAcceptFrameType = ACCEPT_FRAME_TYPE_TAG_ONLY;
            break;
        case APOLLOMP_VLAN_ACCEPT_UNTAGGED_ONLY:
            *pAcceptFrameType = ACCEPT_FRAME_TYPE_UNTAG_ONLY;
            break;
        case APOLLOMP_VLAN_ACCEPT_1Q_1P_TAGGED_ONLY:
            *pAcceptFrameType = ACCPET_FRAME_TYPE_1P_1Q_TAG_ONLY;
            break;
        default:
            return RT_ERR_FAILED;
            break;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_vlan_portAcceptFrameType_get */


/* Function Name:
 *      dal_apollomp_vlan_portAcceptFrameType_set
 * Description:
 *      Set vlan accept frame type of the port.
 * Input:
 *      port              - port id
 *      acceptFrameType   - accept frame type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT               - The module is not initial
 *      RT_ERR_PORT_ID                - invalid port id
 *      RT_ERR_VLAN_ACCEPT_FRAME_TYPE - invalid accept frame type
 *      RT_ERR_CHIP_NOT_SUPPORTED     - functions not supported by this chip model
 * Note:
 *      The accept frame type as following:
 *          - ACCEPT_FRAME_TYPE_ALL
 *          - ACCEPT_FRAME_TYPE_TAG_ONLY
 *          - ACCEPT_FRAME_TYPE_UNTAG_ONLY
 */
int32
dal_apollomp_vlan_portAcceptFrameType_set(
    rtk_port_t                 port,
    rtk_vlan_acceptFrameType_t acceptFrameType)
{
    int32   ret;
    apollomp_raw_vlanAcpFrameType_t type;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((ACCEPT_FRAME_TYPE_END <= acceptFrameType), RT_ERR_INPUT);

    switch(acceptFrameType)
    {
        case ACCEPT_FRAME_TYPE_ALL:
            type = APOLLOMP_VLAN_ACCEPT_ALL;
            break;
        case ACCEPT_FRAME_TYPE_TAG_ONLY:
            type = APOLLOMP_VLAN_ACCEPT_TAGGED_ONLY;
            break;
        case ACCEPT_FRAME_TYPE_UNTAG_ONLY:
            type = APOLLOMP_VLAN_ACCEPT_UNTAGGED_ONLY;
            break;
        case ACCPET_FRAME_TYPE_1P_1Q_TAG_ONLY:
            type = APOLLOMP_VLAN_ACCEPT_1Q_1P_TAGGED_ONLY;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
            break;
    }

    if ((ret = apollomp_raw_vlan_acceptFrameType_set(port, type)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_vlan_portAcceptFrameType_set */

/* Function Name:
 *      dal_apollomp_vlan_vlanFunctionEnable_get
 * Description:
 *      Get the VLAN enable status.
 * Input:
 *      enable - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 *      The status of vlan function is as following:
 *      - DISABLED
 *      - ENABLED
 */
int32
dal_apollomp_vlan_vlanFunctionEnable_get(rtk_enable_t *pEnable)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if ((ret = apollomp_raw_vlan_egrIgrFilterEnable_get(pEnable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_vlan_vlanFunctionEnable_get */


/* Function Name:
 *      dal_apollomp_vlan_vlanFunctionEnable_set
 * Description:
 *      Set the VLAN enable status.
 * Input:
 *      enable - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 *      The status of vlan function is as following:
 *      - DISABLED
 *      - ENABLED
 */
int32
dal_apollomp_vlan_vlanFunctionEnable_set(rtk_enable_t enable)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    if ((ret = apollomp_raw_vlan_egrIgrFilterEnable_set(enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_vlan_vlanFunctionEnable_set */

/* Module Name    : Vlan                       */
/* Sub-module Name: Vlan ingress/egress filter */

/* Function Name:
 *      dal_apollomp_vlan_portIgrFilterEnable_get
 * Description:
 *      Get vlan ingress filter status of the port.
 * Input:
 *      port        - port id
 * Output:
 *      pEnable     - pointer buffer of ingress filter status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The status of vlan function is as following:
 *      - DISABLED
 *      - ENABLED
 */
int32
dal_apollomp_vlan_portIgrFilterEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if ((ret = apollomp_raw_vlan_igrFlterEnable_get(port, pEnable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_vlan_portIgrFilterEnable_get */


/* Function Name:
 *      dal_apollomp_vlan_portIgrFilterEnable_set
 * Description:
 *      Set vlan ingress filter status of the port to the specified device.
 * Input:
 *      port       - port id
 *      enable     - ingress filter configure
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      The status of vlan ingress filter is as following:
 *          - DISABLED
 *          - ENABLED
 */
int32
dal_apollomp_vlan_portIgrFilterEnable_set(rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    if ((ret = apollomp_raw_vlan_igrFlterEnable_set(port, enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_vlan_portIgrFilterEnable_set */

/* Function Name:
 *      dal_apollomp_vlan_leaky_get
 * Description:
 *      Get VLAN leaky.
 * Input:
 *      type - Packet type for VLAN leaky.
 * Output:
 *      pEnable - Leaky status.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT 		- Invalid input parameters.
 * Note:
 *      This API can get VLAN leaky status for RMA and IGMP/MLD packets.
 *      The leaky frame types are as following:
 *      - LEAKY_BRG_GROUP,
 *      - LEAKY_FD_PAUSE,
 *      - LEAKY_SP_MCAST,
 *      - LEAKY_1X_PAE,
 *      - LEAKY_UNDEF_BRG_04,
 *      - LEAKY_UNDEF_BRG_05,
 *      - LEAKY_UNDEF_BRG_06,
 *      - LEAKY_UNDEF_BRG_07,
 *      - LEAKY_PROVIDER_BRIDGE_GROUP_ADDRESS,
 *      - LEAKY_UNDEF_BRG_09,
 *      - LEAKY_UNDEF_BRG_0A,
 *      - LEAKY_UNDEF_BRG_0B,
 *      - LEAKY_UNDEF_BRG_0C,
 *      - LEAKY_PROVIDER_BRIDGE_GVRP_ADDRESS,
 *      - LEAKY_8021AB,
 *      - LEAKY_UNDEF_BRG_0F,
 *      - LEAKY_BRG_MNGEMENT,
 *      - LEAKY_UNDEFINED_11,
 *      - LEAKY_UNDEFINED_12,
 *      - LEAKY_UNDEFINED_13,
 *      - LEAKY_UNDEFINED_14,
 *      - LEAKY_UNDEFINED_15,
 *      - LEAKY_UNDEFINED_16,
 *      - LEAKY_UNDEFINED_17,
 *      - LEAKY_UNDEFINED_18,
 *      - LEAKY_UNDEFINED_19,
 *      - LEAKY_UNDEFINED_1A,
 *      - LEAKY_UNDEFINED_1B,
 *      - LEAKY_UNDEFINED_1C,
 *      - LEAKY_UNDEFINED_1D,
 *      - LEAKY_UNDEFINED_1E,
 *      - LEAKY_UNDEFINED_1F,
 *      - LEAKY_GMRP,
 *      - LEAKY_GVRP,
 *      - LEAKY_UNDEF_GARP_22,
 *      - LEAKY_UNDEF_GARP_23,
 *      - LEAKY_UNDEF_GARP_24,
 *      - LEAKY_UNDEF_GARP_25,
 *      - LEAKY_UNDEF_GARP_26,
 *      - LEAKY_UNDEF_GARP_27,
 *      - LEAKY_UNDEF_GARP_28,
 *      - LEAKY_UNDEF_GARP_29,
 *      - LEAKY_UNDEF_GARP_2A,
 *      - LEAKY_UNDEF_GARP_2B,
 *      - LEAKY_UNDEF_GARP_2C,
 *      - LEAKY_UNDEF_GARP_2D,
 *      - LEAKY_UNDEF_GARP_2E,
 *      - LEAKY_UNDEF_GARP_2F,
 *      - LEAKY_IGMP,
 *      - LEAKY_IPMULTICAST.
 */
int32
dal_apollomp_vlan_leaky_get(rtk_leaky_type_t type, rtk_enable_t *pEnable)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((LEAKY_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    switch(type)
    {
        case LEAKY_BRG_GROUP:
        case LEAKY_FD_PAUSE:
        case LEAKY_SP_MCAST:
        case LEAKY_1X_PAE:
        case LEAKY_UNDEF_BRG_04:
        case LEAKY_UNDEF_BRG_05:
        case LEAKY_UNDEF_BRG_06:
        case LEAKY_UNDEF_BRG_07:
        case LEAKY_PROVIDER_BRIDGE_GROUP_ADDRESS:
        case LEAKY_UNDEF_BRG_09:
        case LEAKY_UNDEF_BRG_0A:
        case LEAKY_UNDEF_BRG_0B:
        case LEAKY_UNDEF_BRG_0C:
        case LEAKY_PROVIDER_BRIDGE_GVRP_ADDRESS:
        case LEAKY_8021AB:
        case LEAKY_UNDEF_BRG_0F:
        case LEAKY_BRG_MNGEMENT:
        case LEAKY_UNDEFINED_11:
        case LEAKY_UNDEFINED_12:
        case LEAKY_UNDEFINED_13:
        case LEAKY_UNDEFINED_14:
        case LEAKY_UNDEFINED_15:
        case LEAKY_UNDEFINED_16:
        case LEAKY_UNDEFINED_17:
        case LEAKY_UNDEFINED_18:
        case LEAKY_UNDEFINED_19:
        case LEAKY_UNDEFINED_1A:
        case LEAKY_UNDEFINED_1B:
        case LEAKY_UNDEFINED_1C:
        case LEAKY_UNDEFINED_1D:
        case LEAKY_UNDEFINED_1E:
        case LEAKY_UNDEFINED_1F:
        case LEAKY_GMRP:
        case LEAKY_GVRP:
        case LEAKY_UNDEF_GARP_22:
        case LEAKY_UNDEF_GARP_23:
        case LEAKY_UNDEF_GARP_24:
        case LEAKY_UNDEF_GARP_25:
        case LEAKY_UNDEF_GARP_26:
        case LEAKY_UNDEF_GARP_27:
        case LEAKY_UNDEF_GARP_28:
        case LEAKY_UNDEF_GARP_29:
        case LEAKY_UNDEF_GARP_2A:
        case LEAKY_UNDEF_GARP_2B:
        case LEAKY_UNDEF_GARP_2C:
        case LEAKY_UNDEF_GARP_2D:
        case LEAKY_UNDEF_GARP_2E:
        case LEAKY_UNDEF_GARP_2F:
            if ((ret = apollomp_raw_trap_rmaFeature_get((uint32)type, APOLLOMP_RMA_FUN_VLAN_LEAKY, pEnable)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                return ret;
            }
            break;
        case LEAKY_IGMP:
            if ((ret = apollomp_raw_trap_igmpVLANLeaky_get(pEnable)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                return ret;
            }
            break;
        case LEAKY_IPMULTICAST:
            if ((ret = apollomp_raw_vlan_ipmcastLeaky_get(0, pEnable)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                return ret;
            }
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_vlan_leaky_get */

/* Function Name:
 *      dal_apollomp_vlan_leaky_set
 * Description:
 *      Set VLAN leaky.
 * Input:
 *      type - Packet type for VLAN leaky.
 *      enable - Leaky status.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT 		- Invalid input parameters.
 *      RT_ERR_ENABLE       - Invalid enable input
 * Note:
 *      This API can set VLAN leaky for RMA and IGMP/MLD packets.
 *      The leaky frame types are as following:
 *      - LEAKY_BRG_GROUP,
 *      - LEAKY_FD_PAUSE,
 *      - LEAKY_SP_MCAST,
 *      - LEAKY_1X_PAE,
 *      - LEAKY_UNDEF_BRG_04,
 *      - LEAKY_UNDEF_BRG_05,
 *      - LEAKY_UNDEF_BRG_06,
 *      - LEAKY_UNDEF_BRG_07,
 *      - LEAKY_PROVIDER_BRIDGE_GROUP_ADDRESS,
 *      - LEAKY_UNDEF_BRG_09,
 *      - LEAKY_UNDEF_BRG_0A,
 *      - LEAKY_UNDEF_BRG_0B,
 *      - LEAKY_UNDEF_BRG_0C,
 *      - LEAKY_PROVIDER_BRIDGE_GVRP_ADDRESS,
 *      - LEAKY_8021AB,
 *      - LEAKY_UNDEF_BRG_0F,
 *      - LEAKY_BRG_MNGEMENT,
 *      - LEAKY_UNDEFINED_11,
 *      - LEAKY_UNDEFINED_12,
 *      - LEAKY_UNDEFINED_13,
 *      - LEAKY_UNDEFINED_14,
 *      - LEAKY_UNDEFINED_15,
 *      - LEAKY_UNDEFINED_16,
 *      - LEAKY_UNDEFINED_17,
 *      - LEAKY_UNDEFINED_18,
 *      - LEAKY_UNDEFINED_19,
 *      - LEAKY_UNDEFINED_1A,
 *      - LEAKY_UNDEFINED_1B,
 *      - LEAKY_UNDEFINED_1C,
 *      - LEAKY_UNDEFINED_1D,
 *      - LEAKY_UNDEFINED_1E,
 *      - LEAKY_UNDEFINED_1F,
 *      - LEAKY_GMRP,
 *      - LEAKY_GVRP,
 *      - LEAKY_UNDEF_GARP_22,
 *      - LEAKY_UNDEF_GARP_23,
 *      - LEAKY_UNDEF_GARP_24,
 *      - LEAKY_UNDEF_GARP_25,
 *      - LEAKY_UNDEF_GARP_26,
 *      - LEAKY_UNDEF_GARP_27,
 *      - LEAKY_UNDEF_GARP_28,
 *      - LEAKY_UNDEF_GARP_29,
 *      - LEAKY_UNDEF_GARP_2A,
 *      - LEAKY_UNDEF_GARP_2B,
 *      - LEAKY_UNDEF_GARP_2C,
 *      - LEAKY_UNDEF_GARP_2D,
 *      - LEAKY_UNDEF_GARP_2E,
 *      - LEAKY_UNDEF_GARP_2F,
 *      - LEAKY_IGMP,
 *      - LEAKY_IPMULTICAST.
 */
int32
dal_apollomp_vlan_leaky_set(rtk_leaky_type_t type, rtk_enable_t enable)
{
    int32   ret;
    rtk_port_t port;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((LEAKY_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    switch(type)
    {
        case LEAKY_BRG_GROUP:
        case LEAKY_FD_PAUSE:
        case LEAKY_SP_MCAST:
        case LEAKY_1X_PAE:
        case LEAKY_UNDEF_BRG_04:
        case LEAKY_UNDEF_BRG_05:
        case LEAKY_UNDEF_BRG_06:
        case LEAKY_UNDEF_BRG_07:
        case LEAKY_PROVIDER_BRIDGE_GROUP_ADDRESS:
        case LEAKY_UNDEF_BRG_09:
        case LEAKY_UNDEF_BRG_0A:
        case LEAKY_UNDEF_BRG_0B:
        case LEAKY_UNDEF_BRG_0C:
        case LEAKY_PROVIDER_BRIDGE_GVRP_ADDRESS:
        case LEAKY_8021AB:
        case LEAKY_UNDEF_BRG_0F:
        case LEAKY_BRG_MNGEMENT:
        case LEAKY_UNDEFINED_11:
        case LEAKY_UNDEFINED_12:
        case LEAKY_UNDEFINED_13:
        case LEAKY_UNDEFINED_14:
        case LEAKY_UNDEFINED_15:
        case LEAKY_UNDEFINED_16:
        case LEAKY_UNDEFINED_17:
        case LEAKY_UNDEFINED_18:
        case LEAKY_UNDEFINED_19:
        case LEAKY_UNDEFINED_1A:
        case LEAKY_UNDEFINED_1B:
        case LEAKY_UNDEFINED_1C:
        case LEAKY_UNDEFINED_1D:
        case LEAKY_UNDEFINED_1E:
        case LEAKY_UNDEFINED_1F:
        case LEAKY_GMRP:
        case LEAKY_GVRP:
        case LEAKY_UNDEF_GARP_22:
        case LEAKY_UNDEF_GARP_23:
        case LEAKY_UNDEF_GARP_24:
        case LEAKY_UNDEF_GARP_25:
        case LEAKY_UNDEF_GARP_26:
        case LEAKY_UNDEF_GARP_27:
        case LEAKY_UNDEF_GARP_28:
        case LEAKY_UNDEF_GARP_29:
        case LEAKY_UNDEF_GARP_2A:
        case LEAKY_UNDEF_GARP_2B:
        case LEAKY_UNDEF_GARP_2C:
        case LEAKY_UNDEF_GARP_2D:
        case LEAKY_UNDEF_GARP_2E:
        case LEAKY_UNDEF_GARP_2F:
            if ((ret = apollomp_raw_trap_rmaFeature_set((uint32)type, APOLLOMP_RMA_FUN_VLAN_LEAKY, enable)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                return ret;
            }
            break;
        case LEAKY_IGMP:
            if ((ret = apollomp_raw_trap_igmpVLANLeaky_set(enable)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                return ret;
            }
            break;
        case LEAKY_IPMULTICAST:
            HAL_SCAN_ALL_PORT(port)
            {
                if ((ret = apollomp_raw_vlan_ipmcastLeaky_set(port, enable)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                    return ret;
                }
            }
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_vlan_leaky_set */


/* Module Name    : Vlan                               */
/* Sub-module Name: Port based and protocol based vlan */

/* Function Name:
 *      dal_apollomp_vlan_portPvid_get
 * Description:
 *      Get port default vlan id.
 * Input:
 *      port  - port id
 * Output:
 *      pPvid - pointer buffer of port default vlan id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_vlan_portPvid_get(rtk_port_t port, uint32 *pPvid)
{
    int32  ret;
    uint32 vidx;
    uint32 pri;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pPvid), RT_ERR_NULL_POINTER);

    if((ret = apollomp_raw_vlan_portBasedVID_get(port, &vidx, &pri)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }

    *pPvid = (uint32)DAL_APOLLOMP_VLAN_MBECFG_VID(vidx);
    return RT_ERR_OK;
} /* end of dal_apollomp_vlan_portPvid_get */


/* Function Name:
 *      dal_apollomp_vlan_portPvid_set
 * Description:
 *      Set port default vlan id.
 * Input:
 *      port - port id
 *      pvid - port default vlan id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_VLAN_VID - invalid vid
 * Note:
 *      None
 */
int32
dal_apollomp_vlan_portPvid_set(rtk_port_t port, uint32 pvid)
{
    int32  ret;
    uint32 vidx;
    uint32 mbr_idx;
    uint32 pri;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_EXT_VLAN_ID_MAX < pvid), RT_ERR_INPUT);
    RT_PARAM_CHK((!DAL_APOLLOMP_VLAN_IS_SET(pvid)), RT_ERR_VLAN_ENTRY_NOT_FOUND);

    if((ret = dal_apollomp_vlan_checkAndCreate(pvid, &mbr_idx)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }

    if((ret = apollomp_raw_vlan_portBasedVID_get(port, &vidx, &pri)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }

    vidx = mbr_idx;
    if((ret = apollomp_raw_vlan_portBasedVID_set(port, vidx, pri)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_vlan_portPvid_set */


/* Function Name:
 *      dal_apollomp_vlan_extPortPvid_get
 * Description:
 *      Get extension port default vlan id.
 * Input:
 *      extPort - Extension port id
 * Output:
 *      pPvid - pointer buffer of port default vlan id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_vlan_extPortPvid_get(uint32 extPort, uint32 *pPvid)
{
    int32   ret;
    uint32 vidx;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_EXT_PORT(extPort), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pPvid), RT_ERR_NULL_POINTER);

    if((ret = apollomp_raw_vlan_extPortToMbrCfgIdx_get(extPort, &vidx)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }

    *pPvid = (uint32)DAL_APOLLOMP_VLAN_MBECFG_VID(vidx);
    return RT_ERR_OK;
} /* end of dal_apollomp_vlan_extPortPvid_get */


/* Function Name:
 *      dal_apollomp_vlan_extPortPvid_set
 * Description:
 *      Set extension port default vlan id.
 * Input:
 *      extPort - extension port id
 *      pvid    - extension port default vlan id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_VLAN_VID - invalid vid
 * Note:
 *      None
 */
int32
dal_apollomp_vlan_extPortPvid_set(uint32 extPort, uint32 pvid)
{
    int32   ret;
    uint32 vidx;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_EXT_PORT(extPort), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_EXT_VLAN_ID_MAX <= pvid), RT_ERR_INPUT);

    if((ret = dal_apollomp_vlan_checkAndCreate(pvid, &vidx)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }

    if((ret = apollomp_raw_vlan_extPortToMbrCfgIdx_set(extPort, vidx)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_vlan_extPortPvid_set */

/* Function Name:
 *      dal_apollomp_vlan_protoGroup_get
 * Description:
 *      Get protocol group for protocol based vlan.
 * Input:
 *      protoGroupIdx  - protocol group index
 * Output:
 *      pProtoGroup    - pointer to protocol group
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - protocol group index is out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_vlan_protoGroup_get(
    uint32                  protoGroupIdx,
    rtk_vlan_protoGroup_t   *pProtoGroup)
{
    int32   ret;
    apollomp_raw_protoValnEntryCfg_t entry;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_PROTOCOL_VLAN_IDX_MAX() < protoGroupIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pProtoGroup), RT_ERR_NULL_POINTER);

    entry.index = protoGroupIdx;
    if((ret = apollomp_raw_vlanProtoAndPortBasedEntry_get(&entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }

    switch(entry.frameType)
    {
        case APOLLOMP_PPVLAN_FRAME_TYPE_ETHERNET:
            pProtoGroup->frametype = FRAME_TYPE_ETHERNET;
            break;
        case APOLLOMP_PPVLAN_FRAME_TYPE_LLC:
            pProtoGroup->frametype = FRAME_TYPE_LLCOTHER;
            break;
        case APOLLOMP_PPVLAN_FRAME_TYPE_RFC1042:
            pProtoGroup->frametype = FRAME_TYPE_RFC1042;
            break;
        default:
            return RT_ERR_FAILED;
            break;
    }

    pProtoGroup->framevalue = entry.etherType;
    return RT_ERR_OK;
} /* end of dal_apollomp_vlan_protoGroup_get */

/* Function Name:
 *      dal_apollomp_vlan_protoGroup_set
 * Description:
 *      Set protocol group for protocol based vlan.
 * Input:
 *      protoGroupIdx  - protocol group index
 *      protoGroup     - protocol group
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_VLAN_FRAME_TYPE - invalid frame type
 *      RT_ERR_OUT_OF_RANGE    - protocol group index is out of range
 *      RT_ERR_INPUT           - invalid input parameter
 * Note:
 *      Frame type is as following:
 *      - FRAME_TYPE_ETHERNET
 *      - FRAME_TYPE_RFC1042 (SNAP)
 *      - FRAME_TYPE_LLCOTHER
 */
int32
dal_apollomp_vlan_protoGroup_set(
    uint32                  protoGroupIdx,
    rtk_vlan_protoGroup_t   *pProtoGroup)
{
    int32   ret;
    apollomp_raw_protoValnEntryCfg_t entry;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_PROTOCOL_VLAN_IDX_MAX() < protoGroupIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pProtoGroup), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pProtoGroup->frametype >= FRAME_TYPE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((pProtoGroup->frametype == FRAME_TYPE_SNAP8021H), RT_ERR_INPUT);
    RT_PARAM_CHK((pProtoGroup->frametype == FRAME_TYPE_SNAPOTHER), RT_ERR_INPUT);
    RT_PARAM_CHK((pProtoGroup->framevalue > 0x0000FFFF), RT_ERR_INPUT);

    entry.index = protoGroupIdx;
    entry.etherType = pProtoGroup->framevalue;

    switch(pProtoGroup->frametype)
    {
        case FRAME_TYPE_ETHERNET:
            entry.frameType = APOLLOMP_PPVLAN_FRAME_TYPE_ETHERNET;
            break;
        case FRAME_TYPE_LLCOTHER:
            entry.frameType = APOLLOMP_PPVLAN_FRAME_TYPE_LLC;
            break;
        case FRAME_TYPE_RFC1042:
            entry.frameType = APOLLOMP_PPVLAN_FRAME_TYPE_RFC1042;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
            break;
    }

    if((ret = apollomp_raw_vlanProtoAndPortBasedEntry_set(&entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_vlan_protoGroup_set */

/* Function Name:
 *      dal_apollomp_vlan_portProtoVlan_get
 * Description:
 *      Get vlan of specificed protocol group on specified port.
 * Input:
 *      port           - port id
 *      protoGroup_idx - protocol group index
 * Output:
 *      pVlan_cfg      - pointer to vlan configuration of protocol group
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_OUT_OF_RANGE - protocol group index is out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_vlan_portProtoVlan_get(
    rtk_port_t              port,
    uint32                  protoGroupIdx,
    rtk_vlan_protoVlanCfg_t *pVlanCfg)
{
    int32   ret;
    apollomp_raw_protoVlanCfg_t cfg;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((HAL_PROTOCOL_VLAN_IDX_MAX() < protoGroupIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pVlanCfg), RT_ERR_NULL_POINTER);

    cfg.index = protoGroupIdx;
    if((ret = apollomp_raw_vlan_portAndProtoBasedEntry_get(port, &cfg)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }

    pVlanCfg->valid = cfg.valid;
    pVlanCfg->dei   = 0;
    pVlanCfg->vid   = DAL_APOLLOMP_VLAN_MBECFG_VID(cfg.vlan_idx);
    pVlanCfg->pri   = cfg.priority;

    return RT_ERR_OK;
} /* end of dal_apollomp_vlan_portProtoVlan_get */

/* Function Name:
 *      dal_apollomp_vlan_portProtoVlan_set
 * Description:
 *      Set vlan of specificed protocol group on specified port.
 * Input:
 *      port           - port id
 *      protoGroupIdx  - protocol group index
 *      pVlanCfg       - vlan configuration of protocol group
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_OUT_OF_RANGE - protocol group index is out of range
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_vlan_portProtoVlan_set(
    rtk_port_t              port,
    uint32                  protoGroupIdx,
    rtk_vlan_protoVlanCfg_t *pVlanCfg)
{
    int32   ret;
    apollomp_raw_protoVlanCfg_t cfg;
    uint32 vidx;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((HAL_PROTOCOL_VLAN_IDX_MAX() < protoGroupIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pVlanCfg), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((0 != pVlanCfg->dei), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= pVlanCfg->valid), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_EXT_VLAN_ID_MAX < pVlanCfg->vid), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((HAL_INTERNAL_PRIORITY_MAX() < pVlanCfg->pri), RT_ERR_INPUT);

    if((ret = dal_apollomp_vlan_checkAndCreate(pVlanCfg->vid, &vidx)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }

    cfg.index = protoGroupIdx;
    cfg.valid = pVlanCfg->valid;
    cfg.vlan_idx = vidx;
    cfg.priority = pVlanCfg->pri;

    if((ret = apollomp_raw_vlan_protoAndPortBasedEntry_set(port, &cfg)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_vlan_portProtoVlan_set */

/* Module Name    : Vlan                */
/* Sub-module Name: Tag format handling */

/* Function Name:
 *      dal_apollomp_vlan_tagMode_get
 * Description:
 *      Get vlan tagged mode of the port.
 * Input:
 *      port      - port id
 * Output:
 *      pTagMode  - pointer buffer of vlan tagged mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The vlan tagged mode as following:
 *      - VLAN_TAG_MODE_ORIGINAL        (depend on chip normal decision)
 *      - VLAN_TAG_MODE_KEEP_FORMAT     (keep ingress format to egress)
 *      - VLAN_TAG_MODE_PRI             (always priority tag out)
 */
int32
dal_apollomp_vlan_tagMode_get(rtk_port_t port, rtk_vlan_tagMode_t *pTagMode)
{
    int32   ret;
    apollomp_raw_vlan_egrTagMode_t mode;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pTagMode), RT_ERR_NULL_POINTER);

    if((ret = apollomp_raw_vlan_egrTagMode_get(port, &mode)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }

    switch (mode)
    {
        case APOLLOMP_VLAN_EGR_TAG_MODE_ORI:
            *pTagMode = VLAN_TAG_MODE_ORIGINAL;
            break;
        case APOLLOMP_VLAN_EGR_TAG_MODE_KEEP:
            *pTagMode = VLAN_TAG_MODE_KEEP_FORMAT;
            break;
        case APOLLOMP_VLAN_EGR_TAG_MODE_PRI_TAG:
            *pTagMode = VLAN_TAG_MODE_PRI;
            break;
        default:
            return RT_ERR_FAILED;
            break;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_vlan_tagMode_get */


/* Function Name:
 *      dal_apollomp_vlan_tagMode_set
 * Description:
 *      Set vlan tagged mode of the port.
 * Input:
 *      port     - port id
 *      tagMode  - vlan tagged mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      The vlan tagged mode as following:
 *      - VLAN_TAG_MODE_ORIGINAL        (depend on chip normal decision)
 *      - VLAN_TAG_MODE_KEEP_FORMAT     (keep ingress format to egress)
 *      - VLAN_TAG_MODE_PRI             (always priority tag out)
 */
int32
dal_apollomp_vlan_tagMode_set(rtk_port_t port, rtk_vlan_tagMode_t tagMode)
{
    int32   ret;
    apollomp_raw_vlan_egrTagMode_t mode;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((VLAN_TAG_MODE_END <= tagMode), RT_ERR_INPUT);

    switch (tagMode)
    {
        case VLAN_TAG_MODE_ORIGINAL:
            mode = APOLLOMP_VLAN_EGR_TAG_MODE_ORI;
            break;
        case VLAN_TAG_MODE_KEEP_FORMAT:
            mode = APOLLOMP_VLAN_EGR_TAG_MODE_KEEP;
            break;
        case VLAN_TAG_MODE_PRI:
            mode = APOLLOMP_VLAN_EGR_TAG_MODE_PRI_TAG;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
            break;
    }

    if((ret = apollomp_raw_vlan_egrTagMode_set(port, mode)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_vlan_tagMode_set */




/* Function Name:
 *      dal_apollomp_vlan_portFid_get
 * Description:
 *      Get port-based filtering database
 * Input:
 *      port - Port id.
 * Output:
 *      pEnable - ebable port-based FID
 *      pFid - Specified filtering database.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_INPUT - Invalid input parameters.
 *      RT_ERR_PORT_ID - Invalid port ID.
 * Note:
 *      The API can get port-based filtering database status. If the function is enabled, all input
 *      packets will be assigned to the port-based fid regardless vlan tag.
 */
int32
dal_apollomp_vlan_portFid_get(rtk_port_t port, rtk_enable_t *pEnable, rtk_fid_t *pFid)
{
    int32   ret;
    uint32 fid;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pFid), RT_ERR_NULL_POINTER);

    if((ret = apollomp_raw_vlan_portBasedFidEnable_get(port, pEnable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }

    if((ret = apollomp_raw_vlan_portBasedFid_get(port, &fid)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }

    *pFid = (rtk_fid_t)fid;

    return RT_ERR_OK;
} /* end of dal_apollomp_vlan_portFid_get */


/* Function Name:
 *      dal_apollomp_vlan_portFid_set
 * Description:
 *      Set port-based filtering database
 * Input:
 *      port - Port id.
 *      enable - ebable port-based FID
 *      fid - Specified filtering database.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_L2_FID - Invalid fid.
 *      RT_ERR_INPUT - Invalid input parameter.
 *      RT_ERR_PORT_ID - Invalid port ID.
 * Note:
 *      The API can set port-based filtering database. If the function is enabled, all input
 *      packets will be assigned to the port-based fid regardless vlan tag.
 */
int32
dal_apollomp_vlan_portFid_set(rtk_port_t port, rtk_enable_t enable, rtk_fid_t fid)
{
    int32   ret;
    uint32 raw_fid;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_VLAN_FID_MAX() < fid), RT_ERR_FID);

    if((ret = apollomp_raw_vlan_portBasedFidEnable_set(port, enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }

    raw_fid = (uint32)fid;
    if((ret = apollomp_raw_vlan_portBasedFid_set(port, raw_fid)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_vlan_portFid_set */

/* Function Name:
 *      dal_apollomp_vlan_portPriority_get
 * Description:
 *      Get port-based priority
 * Input:
 *      port - Port id.
 * Output:
 *      pPriority - port-based priority
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_INPUT - Invalid input parameters.
 *      RT_ERR_PORT_ID - Invalid port ID.
 * Note:
 *
 */
int32
dal_apollomp_vlan_portPriority_get(rtk_port_t port, rtk_pri_t *pPriority)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pPriority), RT_ERR_NULL_POINTER);

    if((ret = apollomp_raw_vlan_portBasedPri_get(port, (uint32 *)pPriority)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_apollomp_vlan_portPriority_set
 * Description:
 *      Set port-based priority
 * Input:
 *      port - Port id.
 *      priority - VLAN port-based priority
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_INPUT - Invalid input parameter.
 *      RT_ERR_PORT_ID - Invalid port ID.
 * Note:
 */
int32
dal_apollomp_vlan_portPriority_set(rtk_port_t port, rtk_pri_t priority)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((priority > HAL_INTERNAL_PRIORITY_MAX()), RT_ERR_NULL_POINTER);

    if((ret = apollomp_raw_vlan_portBasedPri_set(port, (uint32)priority)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_apollomp_vlan_portEgrTagKeepType_get
 * Description:
 *      Get egress tag keep type
 * Input:
 *      egr_port     - Egress port id.
 * Output:
 *      pIgr_portmask - Pointer of Ingress portmask
 *      pType         - Pointer of tag keep type
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - Invalid input parameter.
 *      RT_ERR_PORT_ID      - Invalid port ID.
 *      RT_ERR_NULL_POINTER - Null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_vlan_portEgrTagKeepType_get(rtk_port_t egr_port, rtk_portmask_t *pIgr_portmask, rtk_vlan_tagKeepType_t *pType)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(egr_port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pIgr_portmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pType), RT_ERR_NULL_POINTER);

    if((ret = apollomp_raw_vlan_egrKeepPmsk_get(egr_port, pIgr_portmask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }

    *pType = TAG_KEEP_TYPE_CONTENT;

    return RT_ERR_OK;
} /* end of dal_apollomp_vlan_portEgrTagKeepType_get */

/* Function Name:
 *      dal_apollomp_vlan_portEgrTagKeepType_set
 * Description:
 *      Set egress tag keep type
 * Input:
 *      egr_port      - Egress port id.
 *      pIgr_portmask - Pointer of Ingress portmask
 *      Type          - Tag keep type
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - Invalid input parameter.
 *      RT_ERR_PORT_ID      - Invalid port ID.
 *      RT_ERR_NULL_POINTER - Null pointer
 *      RT_ERR_PORT_MASK    - Invalid port mask.
 * Note:
 *      None
 */
int32
dal_apollomp_vlan_portEgrTagKeepType_set(rtk_port_t egr_port, rtk_portmask_t *pIgr_portmask, rtk_vlan_tagKeepType_t type)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(egr_port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pIgr_portmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((type != TAG_KEEP_TYPE_CONTENT), RT_ERR_INPUT);

    if((ret = apollomp_raw_vlan_egrKeepPmsk_set(egr_port, *pIgr_portmask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_vlan_portEgrTagKeepType_set */


/* Function Name:
 *      dal_apollomp_vlan_transparentEnable_get
 * Description:
 *      Get state of VLAN transparent
 * Input:
 *      None
 * Output:
 *      pEnable - Pointer of VLAN transparent function
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_NULL_POINTER - Null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_vlan_transparentEnable_get(rtk_enable_t *pEnable)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if((ret = apollomp_raw_vlan_transparentEnable_get(pEnable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_vlan_transparentEnable_get */

/* Function Name:
 *      dal_apollomp_vlan_transparentEnable_set
 * Description:
 *      Set state of VLAN transparent
 * Input:
 *      enable  - VLAN transparent function.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - Invalid input parameter.
 * Note:
 *      None
 */
int32
dal_apollomp_vlan_transparentEnable_set(rtk_enable_t enable)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    if((ret = apollomp_raw_vlan_transparentEnable_set(enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_vlan_transparentEnable_set */

/* Function Name:
 *      dal_apollomp_vlan_cfiKeepEnable_get
 * Description:
 *      Get state of CFI keep
 * Input:
 *      None
 * Output:
 *      pEnable - Pointer of CFI Keep
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_NULL_POINTER - Null pointer
 * Note:
 *      ENABLED: Keep original CFI value
 *      DISABLED: Always output VLAN tag with CFI = 0
 */
int32
dal_apollomp_vlan_cfiKeepEnable_get(rtk_enable_t *pEnable)
{
    int32   ret;
    apollomp_raw_vlan_cfiKeepMode_t mode;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if((ret = apollomp_raw_vlan_cfiKeepMode_get(&mode)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }

    *pEnable = ((APOLLOMP_CFI_KEEP_INGRESS == mode) ? ENABLED : DISABLED);

    return RT_ERR_OK;
} /* end of dal_apollomp_vlan_cfiKeepEnable_get */

/* Function Name:
 *      dal_apollomp_vlan_cfiKeepEnable_set
 * Description:
 *      Set state of CFI keep
 * Input:
 *      enable  - state of CFI KEEP
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - Invalid input parameter.
 * Note:
 *      ENABLED: Keep original CFI value
 *      DISABLED: Always output VLAN tag with CFI = 0
 */
int32
dal_apollomp_vlan_cfiKeepEnable_set(rtk_enable_t enable)
{
    int32   ret;
    apollomp_raw_vlan_cfiKeepMode_t mode;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    mode = ((ENABLED == enable) ? APOLLOMP_CFI_KEEP_INGRESS : APOLLOMP_CFI_ALWAYS_0);
    if((ret = apollomp_raw_vlan_cfiKeepMode_set(mode)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_vlan_cfiKeepEnable_set */

/* Function Name:
 *      dal_apollomp_vlan_reservedVidAction_get
 * Description:
 *      Get the action of VID 0 and VID 4095 packet
 * Input:
 *      None
 * Output:
 *      pAction_vid0    - Pointer of VID 0 action
 *      pAction_vid4095 - Pointer of VID 4095 action
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_NULL_POINTER - Null pointer
 * Note:
 *      RESVID_ACTION_UNTAG: VID 0 or VID 4095 tagged packets will be treated as untagged packets
 *      RESVID_ACTION_TAG: VID 0 or VID 4095 tagged packets will be treated as tagged packets
 */
int32
dal_apollomp_vlan_reservedVidAction_get(rtk_vlan_resVidAction_t *pAction_vid0, rtk_vlan_resVidAction_t *pAction_vid4095)
{
    int32   ret;
    apollomp_raw_vlanTag_t vid0_type;
    apollomp_raw_vlanTag_t vid4095_type;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pAction_vid0), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pAction_vid4095), RT_ERR_NULL_POINTER);

    if((ret = apollomp_raw_vlan_vid0TagType_get(&vid0_type)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }

    if((ret = apollomp_raw_vlan_vid4095TagType_get(&vid4095_type)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }

    *pAction_vid0 = ((APOLLOMP_VLAN_UNTAG == vid0_type) ? RESVID_ACTION_UNTAG : RESVID_ACTION_TAG);
    *pAction_vid4095 = ((APOLLOMP_VLAN_UNTAG == vid4095_type) ? RESVID_ACTION_UNTAG : RESVID_ACTION_TAG);

    return RT_ERR_OK;
} /* end of dal_apollomp_vlan_reservedVidAction_get */

/* Function Name:
 *      dal_apollomp_vlan_reservedVidAction_set
 * Description:
 *      Set the action of VID 0 and VID 4095 packet
 * Input:
 *      None
 * Output:
 *      action_vid0     - Pointer of VID 0 action
 *      action_vid4095  - Pointer of VID 4095 action
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - Error Input
 * Note:
 *      RESVID_ACTION_UNTAG: VID 0 or VID 4095 tagged packets will be treated as untagged packets
 *      RESVID_ACTION_TAG: VID 0 or VID 4095 tagged packets will be treated as tagged packets
 */
int32
dal_apollomp_vlan_reservedVidAction_set(rtk_vlan_resVidAction_t action_vid0, rtk_vlan_resVidAction_t action_vid4095)
{
    int32   ret;
    apollomp_raw_vlanTag_t vid0_type;
    apollomp_raw_vlanTag_t vid4095_type;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((action_vid0 >= RESVID_ACTION_END), RT_ERR_INPUT);
    RT_PARAM_CHK((action_vid4095 >= RESVID_ACTION_END), RT_ERR_INPUT);

    vid0_type = ((RESVID_ACTION_UNTAG == action_vid0) ? APOLLOMP_VLAN_UNTAG : APOLLOMP_VLAN_TAG);
    vid4095_type = ((RESVID_ACTION_UNTAG == action_vid4095) ? APOLLOMP_VLAN_UNTAG : APOLLOMP_VLAN_TAG);

    if((ret = apollomp_raw_vlan_vid0TagType_set(vid0_type)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }

    if((ret = apollomp_raw_vlan_vid4095TagType_set(vid4095_type)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_vlan_reservedVidAction_set */


