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
 * $Revision: 16745 $
 * $Date: 2011-04-12 11:46:26 +0800 (Tue, 12 Apr 2011) $
 *
 * Purpose : Definition those XXX command and APIs in the SDK diagnostic shell.
 *
 * Feature : The file have include the following module and sub-modules
 *
 */

/*
 * Include Files
 */
#include <stdio.h>
#include <string.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <diag_util.h>
#include <parser/cparser_priv.h>
#include <diag_str.h>
#include <rtk/classify.h>
#include <hal/common/halctrl.h>


/* variable */
static rtk_classify_cfg_t diag_classfRule;

/* local function */
static uint32 _diag_classf_show_field(rtk_classify_field_t *pField)
{
    switch(pField->fieldType)
    {
        case CLASSIFY_FIELD_ETHERTYPE:
            diag_util_printf("ether type data: 0x%04x\n", pField->classify_pattern.etherType.value);
            diag_util_printf("           mask: 0x%x\n", pField->classify_pattern.etherType.mask);
        break;
        case CLASSIFY_FIELD_TOS_DSIDX:
            diag_util_printf("tos/sid data: 0x%x\n", pField->classify_pattern.tosDsidx.value);
            diag_util_printf("        mask: 0x%x\n", pField->classify_pattern.tosDsidx.mask);
        break;
        case CLASSIFY_FIELD_TAG_VID:
            diag_util_printf("tag vid data: %d\n", pField->classify_pattern.tagVid.value);
            diag_util_printf("        mask: 0x%x\n", pField->classify_pattern.tagVid.mask);
        break;
        case CLASSIFY_FIELD_TAG_PRI:
            diag_util_printf("tag priority data: %d\n", pField->classify_pattern.tagPri.value);
            diag_util_printf("             mask: 0x%x\n", pField->classify_pattern.tagPri.mask);
        break;
        case CLASSIFY_FIELD_INTER_PRI:
            diag_util_printf("internal priority data: %d\n", pField->classify_pattern.interPri.value);
            diag_util_printf("                  mask: 0x%x\n", pField->classify_pattern.interPri.mask);
        break;
        case CLASSIFY_FIELD_IS_CTAG:
            diag_util_printf("s-bit data: %d\n", pField->classify_pattern.isCtag.value);
            diag_util_printf("      mask: 0x%x\n", pField->classify_pattern.isCtag.mask);
        break;
        case CLASSIFY_FIELD_IS_STAG:
            diag_util_printf("c-bit data: %d\n", pField->classify_pattern.isStag.value);
            diag_util_printf("      mask: 0x%x\n", pField->classify_pattern.isStag.mask);
        break;
        case CLASSIFY_FIELD_UNI:
            diag_util_printf("UNI data: %d\n", pField->classify_pattern.uni.value);
            diag_util_printf("    mask: 0x%x\n", pField->classify_pattern.uni.mask);
        break;

        default:
        break;
    }

#ifdef CONFIG_SDK_APOLLOMP
    if(APOLLOMP_CHIP_ID == DIAG_UTIL_CHIP_TYPE)
    {
        switch(pField->fieldType)
        {
            case CLASSIFY_FIELD_PORT_RANGE:
                diag_util_printf("L4-port range data: %d\n", pField->classify_pattern.portRange.value);
                diag_util_printf("              mask: 0x%x\n", pField->classify_pattern.portRange.mask);
            break;
            case CLASSIFY_FIELD_IP_RANGE:
                diag_util_printf("IP range data: %d\n", pField->classify_pattern.ipRange.value);
                diag_util_printf("         mask: 0x%x\n", pField->classify_pattern.ipRange.mask);
            break;
            case CLASSIFY_FIELD_ACL_HIT:
                diag_util_printf("ACL hit data: %d\n", pField->classify_pattern.aclHit.value);
                diag_util_printf("        mask: 0x%x\n", pField->classify_pattern.aclHit.mask);
            break;
            case CLASSIFY_FIELD_WAN_IF:
                diag_util_printf("WAN interface data: %d\n", pField->classify_pattern.wanIf.value);
                diag_util_printf("              mask: 0x%x\n", pField->classify_pattern.wanIf.mask);
            break;
            case CLASSIFY_FIELD_IP6_MC:
                diag_util_printf("IPv6 multicast data: %d\n", pField->classify_pattern.ip6Mc.value);
                diag_util_printf("               mask: 0x%x\n", pField->classify_pattern.ip6Mc.mask);
            break;
            case CLASSIFY_FIELD_IP4_MC:
                diag_util_printf("IPv4 multicast data: %d\n", pField->classify_pattern.ip4Mc.value);
                diag_util_printf("               mask: 0x%x\n", pField->classify_pattern.ip4Mc.mask);
            break;
            case CLASSIFY_FIELD_MLD:
                diag_util_printf("MLD data: %d\n", pField->classify_pattern.mld.value);
                diag_util_printf("    mask: 0x%x\n", pField->classify_pattern.mld.mask);
            break;
            case CLASSIFY_FIELD_IGMP:
                diag_util_printf("IGMP data: %d\n", pField->classify_pattern.igmp.value);
                diag_util_printf("     mask: 0x%x\n", pField->classify_pattern.igmp.mask);
            break;
            case CLASSIFY_FIELD_DEI:
                diag_util_printf("DEI  data: %d\n", pField->classify_pattern.dei.value);
                diag_util_printf("     mask: 0x%x\n", pField->classify_pattern.dei.mask);
            break;

            default:
            break;
        }
    }
#endif
    return RT_ERR_OK;
}

static uint32 _diag_classf_show_dsAction(rtk_classify_ds_act_t *pAct)
{
    rtk_portmask_t portlist;
    uint8   buf[UTIL_PORT_MASK_BUFFER_LENGTH];

    diag_util_printf("Stag action: %s\n", diagStr_dsCStagAction[pAct->csAct]);
    if(CLASSIFY_DS_CSACT_ADD_TAG_VS_TPID == pAct->csAct ||
       CLASSIFY_DS_CSACT_ADD_TAG_8100 == pAct->csAct)
    {
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#ifdef CONFIG_SDK_APOLLO
            case APOLLO_CHIP_ID:
                diag_util_printf("Tag VID: %d\n", pAct->cTagVid);
                diag_util_printf("Tag RPI: %d\n", pAct->cTagPri);
            break;
#endif
#ifdef CONFIG_SDK_APOLLOMP
            case APOLLOMP_CHIP_ID:
                diag_util_printf("Stag VID action: %s\n", diagStr_dsSvidAction[pAct->csVidAct]);
                if(CLASSIFY_DS_VID_ACT_ASSIGN == pAct->csVidAct)
                    diag_util_printf("Stag VID: %d\n", pAct->sTagVid);

                diag_util_printf("Stag PRI action: %s\n", diagStr_dsSpriAction[pAct->csPriAct]);
                if(CLASSIFY_DS_PRI_ACT_ASSIGN == pAct->csPriAct)
                    diag_util_printf("Stag PRI: %d\n", pAct->sTagPri);
            break;
#endif
        }

    }

    diag_util_printf("Ctag action: %s\n", diagStr_dsCtagAction[pAct->cAct]);
    if(CLASSIFY_DS_CACT_ADD_CTAG_8100 == pAct->cAct)
    {
        diag_util_printf("Ctag VID action: %s\n", diagStr_dsCvidAction[pAct->cVidAct]);
        if(CLASSIFY_DS_VID_ACT_ASSIGN == pAct->cVidAct)
            diag_util_printf("Ctag VID: %d\n", pAct->cTagVid);

        diag_util_printf("Ctag PRI action: %s\n", diagStr_dsCpriAction[pAct->cPriAct]);
        if(CLASSIFY_DS_PRI_ACT_ASSIGN == pAct->csPriAct)
            diag_util_printf("Stag PRI: %d\n", pAct->sTagPri);
    }

    diag_util_printf("Classf PRI action: %s\n", diagStr_cfpriAction[pAct->interPriAct]);
    if(CLASSIFY_CF_PRI_ACT_ASSIGN == pAct->interPriAct)
        diag_util_printf("CF PRI: %d\n", pAct->cfPri);

    diag_util_printf("UNI action: %s\n", diagStr_dsUniAction[pAct->uniAct]);
    diag_util_lPortMask2str(buf,&pAct->uniMask);
    diag_util_printf("UNI ports: %s\n", buf);

#ifdef CONFIG_SDK_APOLLOMP
    if(APOLLOMP_CHIP_ID == DIAG_UTIL_CHIP_TYPE)
    {
        diag_util_printf("DSCP remarking action: %s\n", diagStr_enable[pAct->dscp]);
    }
#endif
    return RT_ERR_OK;
}

uint32 _diag_classf_show_usAction(rtk_classify_us_act_t *pAct)
{
    rtk_portmask_t portlist;
    uint8   buf[UTIL_PORT_MASK_BUFFER_LENGTH];

    diag_util_printf("Stag action: %s\n", diagStr_usCStagAction[pAct->csAct]);
    if(CLASSIFY_US_CSACT_ADD_TAG_VS_TPID == pAct->csAct ||
       CLASSIFY_US_CSACT_ADD_TAG_8100 == pAct->csAct)
    {
        diag_util_printf("Stag VID action: %s\n", diagStr_usSvidAction[pAct->csVidAct]);
        if(CLASSIFY_US_VID_ACT_ASSIGN == pAct->csVidAct)
            diag_util_printf("Stag VID: %d\n", pAct->sTagVid);

        diag_util_printf("Stag PRI action: %s\n", diagStr_usSpriAction[pAct->csPriAct]);
        if(CLASSIFY_US_PRI_ACT_ASSIGN == pAct->csPriAct)
            diag_util_printf("Stag PRI: %d\n", pAct->sTagPri);
    }

    diag_util_printf("Ctag action: %s\n", diagStr_usCtagAction[pAct->cAct]);

#ifdef CONFIG_SDK_APOLLOMP
    if(APOLLOMP_CHIP_ID == DIAG_UTIL_CHIP_TYPE)
    {
        if(CLASSIFY_US_CACT_ADD_CTAG_8100 == pAct->cAct)
        {
            diag_util_printf("Ctag VID action: %s\n", diagStr_usCvidAction[pAct->cVidAct]);
            if(CLASSIFY_US_VID_ACT_ASSIGN == pAct->cVidAct)
                diag_util_printf("Ctag VID: %d\n", pAct->cTagVid);

            diag_util_printf("Ctag PRI action: %s\n", diagStr_usCpriAction[pAct->cPriAct]);
            if(CLASSIFY_US_PRI_ACT_ASSIGN == pAct->csPriAct)
                diag_util_printf("Ctag PRI: %d\n", pAct->cTagPri);
        }
    }
#endif

    diag_util_printf("SID action: %s\n", diagStr_usSidAction[pAct->sidQidAct]);
    diag_util_printf("Assign ID: %d\n", pAct->sidQid);

#ifdef CONFIG_SDK_APOLLOMP
    if(APOLLOMP_CHIP_ID == DIAG_UTIL_CHIP_TYPE)
    {
        diag_util_printf("Classf PRI action: %s\n", diagStr_cfpriAction[pAct->interPriAct]);
        if(CLASSIFY_CF_PRI_ACT_ASSIGN == pAct->interPriAct)
            diag_util_printf("CF PRI: %d\n", pAct->cfPri);

        diag_util_printf("DSCP remarking action: %s\n", diagStr_enable[pAct->dscp]);
        diag_util_printf("Drop action: %s\n", diagStr_enable[pAct->drop]);
        diag_util_printf("logging action: %s\n", diagStr_enable[pAct->log]);
        if(CLASSIFY_US_LOG_ACT_ENABLE == pAct->log)
            diag_util_printf("logging index: %d\n", pAct->logCntIdx);
    }
#endif
    return RT_ERR_OK;
}

/*
 * classf show rule
 */
cparser_result_t
cparser_cmd_classf_show_rule(
    cparser_context_t *context)
{
    rtk_classify_field_t *cf_field;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_printf("Not: %s\n", diagStr_enable[diag_classfRule.invert]);
    diag_util_printf("direction: %s\n",diagStr_direction[diag_classfRule.direction]);

    cf_field = diag_classfRule.field.pFieldHead;
    while(cf_field != NULL)
    {
        diag_util_printf("Rule: \n");
        _diag_classf_show_field(cf_field);
        cf_field = cf_field->next;
    }

    if(CLASSIFY_DIRECTION_DS == diag_classfRule.direction)
    {
        diag_util_printf("Downstream action: \n");
        _diag_classf_show_dsAction(&diag_classfRule.act.dsAct);
    }
    else
    {
        diag_util_printf("Upstream action: \n");
        _diag_classf_show_usAction(&diag_classfRule.act.usAct);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_show_rule */

/*
 * classf clear
 */
cparser_result_t
cparser_cmd_classf_clear(
    cparser_context_t *context)
{
    rtk_classify_field_t *fieldNext, *fieldThis;

    DIAG_UTIL_PARAM_CHK();

    fieldThis = diag_classfRule.field.pFieldHead;
    while(fieldThis != NULL)
    {
        fieldNext = fieldThis->next;
        osal_free(fieldThis);
        fieldThis = fieldNext;
    }

    diag_classfRule.field.pFieldHead = NULL;

    osal_memset(&diag_classfRule, 0x0, sizeof(rtk_classify_cfg_t));

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_clear */

/*
 * classf init
 */
cparser_result_t
cparser_cmd_classf_init(
    cparser_context_t *context)
{
    int32 ret;

    DIAG_UTIL_PARAM_CHK();


    DIAG_UTIL_ERR_CHK(rtk_classify_init(), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_clear */

/*
 * classf add entry <UINT:index>
 */
cparser_result_t
cparser_cmd_classf_add_entry_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    rtk_classify_field_t *fieldNext, *fieldThis;
    int32 ret;
    DIAG_UTIL_PARAM_CHK();

    diag_classfRule.index = *index_ptr;
    diag_classfRule.valid = 1;

    if((ret = rtk_classify_cfgEntry_add(&diag_classfRule)) != RT_ERR_OK)
        DIAG_ERR_PRINT(ret);\

    fieldThis = diag_classfRule.field.pFieldHead;
    while(fieldThis != NULL)
    {
        fieldNext = fieldThis->next;
        osal_free(fieldThis);
        fieldThis = fieldNext;
    }

    diag_classfRule.field.pFieldHead = NULL;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_add_entry_index */

/*
 * classf del entry <UINT:index>
 */
cparser_result_t
cparser_cmd_classf_del_entry_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_classify_cfgEntry_del(*index_ptr), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_del_entry_index */

/*
 * classf del entry all
 */
cparser_result_t
cparser_cmd_classf_del_entry_all(
    cparser_context_t *context)
{
    uint32 start_entry, total_entry;
    uint32 idx;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    start_entry = 0;
    total_entry = HAL_CLASSIFY_ENTRY_MAX();

    for(idx=start_entry; idx < (start_entry + total_entry); idx++)
    {
        DIAG_UTIL_ERR_CHK(rtk_classify_cfgEntry_del(idx), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_del_entry_all */

/*
 * classf get entry <UINT:index>
 */
cparser_result_t
cparser_cmd_classf_get_entry_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    rtk_classify_cfg_t entry;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    entry.index = *index_ptr;
    DIAG_UTIL_ERR_CHK(rtk_classify_cfgEntry_get(&entry), ret);

    diag_util_printf("Valid: %s\n", diagStr_valid[entry.valid]);
    diag_util_printf("Not: %s\n", diagStr_enable[entry.invert]);
    diag_util_printf("direction: %s\n",diagStr_direction[entry.direction]);

    diag_util_printf("databit: 0x%04x-%04x-%04x\n", entry.field.readField.dataFieldRaw[0],
                                                    entry.field.readField.dataFieldRaw[1],
                                                    entry.field.readField.dataFieldRaw[2]);

    diag_util_printf("carebit: 0x%04x-%04x-%04x\n", entry.field.readField.careFieldRaw[0],
                                                    entry.field.readField.careFieldRaw[1],
                                                    entry.field.readField.careFieldRaw[2]);

    if(CLASSIFY_DIRECTION_DS == entry.direction)
    {
        diag_util_printf("Downstream action: \n");
        _diag_classf_show_dsAction(&entry.act.dsAct);
    }
    else
    {
        diag_util_printf("Upstream action: \n");
        _diag_classf_show_usAction(&entry.act.usAct);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_get_entry_index */

/*
 * classf set rule direction ( upstream | downstream )
 */
cparser_result_t
cparser_cmd_classf_set_rule_direction_upstream_downstream(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    if ('u' == TOKEN_CHAR(4,0))
        diag_classfRule.direction = CLASSIFY_DIRECTION_US;
    else
        diag_classfRule.direction = CLASSIFY_DIRECTION_DS;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_rule_direction_upstream_downstream */

/*
 * classf set rule tos-sid data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_classf_set_rule_tos_sid_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    rtk_classify_field_t *fieldHead;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    if(CLASSIFY_DIRECTION_DS == diag_classfRule.direction)
    {
        DIAG_UTIL_PARAM_RANGE_CHK((*data_ptr >= HAL_CLASSIFY_SID_NUM()), CPARSER_ERR_INVALID_PARAMS);
        DIAG_UTIL_PARAM_RANGE_CHK((*mask_ptr >= HAL_CLASSIFY_SID_NUM()), CPARSER_ERR_INVALID_PARAMS);
    }
    else
    {
        DIAG_UTIL_PARAM_RANGE_CHK((*data_ptr > 0xFF), CPARSER_ERR_INVALID_PARAMS);
        DIAG_UTIL_PARAM_RANGE_CHK((*mask_ptr > 0xFF), CPARSER_ERR_INVALID_PARAMS);
    }

    fieldHead = (rtk_classify_field_t*)osal_alloc(sizeof(rtk_classify_field_t));
    fieldHead->fieldType = CLASSIFY_FIELD_TOS_DSIDX;
    fieldHead->classify_pattern.tosDsidx.value = (uint16)*data_ptr;
    fieldHead->classify_pattern.tosDsidx.mask = (uint16)*mask_ptr;
    fieldHead->next = NULL;

    DIAG_UTIL_ERR_CHK(rtk_classify_field_add(&diag_classfRule, fieldHead), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_rule_tos_sid_data_data_mask_mask */

/*
 * classf set rule tag-vid data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_classf_set_rule_tag_vid_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    rtk_classify_field_t *fieldHead;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_PARAM_RANGE_CHK((*data_ptr > RTK_VLAN_ID_MAX), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*mask_ptr > RTK_VLAN_ID_MAX), CPARSER_ERR_INVALID_PARAMS);

    fieldHead = (rtk_classify_field_t*)osal_alloc(sizeof(rtk_classify_field_t));
    fieldHead->fieldType = CLASSIFY_FIELD_TAG_VID;
    fieldHead->classify_pattern.tagVid.value = (uint16)*data_ptr;
    fieldHead->classify_pattern.tagVid.mask = (uint16)*mask_ptr;
    fieldHead->next = NULL;

    DIAG_UTIL_ERR_CHK(rtk_classify_field_add(&diag_classfRule, fieldHead), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_rule_tag_vid_data_data_mask_mask */

/*
 * classf set rule tag-priority data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_classf_set_rule_tag_priority_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    rtk_classify_field_t *fieldHead;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_PARAM_RANGE_CHK((*data_ptr > RTK_DOT1P_PRIORITY_MAX), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*mask_ptr > RTK_DOT1P_PRIORITY_MAX), CPARSER_ERR_INVALID_PARAMS);

    fieldHead = (rtk_classify_field_t*)osal_alloc(sizeof(rtk_classify_field_t));
    fieldHead->fieldType = CLASSIFY_FIELD_TAG_PRI;
    fieldHead->classify_pattern.tagPri.value = (uint16)*data_ptr;
    fieldHead->classify_pattern.tagPri.mask = (uint16)*mask_ptr;
    fieldHead->next = NULL;

    DIAG_UTIL_ERR_CHK(rtk_classify_field_add(&diag_classfRule, fieldHead), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_rule_tag_priority_data_data_mask_mask */

/*
 * classf set rule internal-priority data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_classf_set_rule_internal_priority_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    rtk_classify_field_t *fieldHead;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_PARAM_RANGE_CHK((*data_ptr > RTK_DOT1P_PRIORITY_MAX), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*mask_ptr > RTK_DOT1P_PRIORITY_MAX), CPARSER_ERR_INVALID_PARAMS);

    fieldHead = (rtk_classify_field_t*)osal_alloc(sizeof(rtk_classify_field_t));
    fieldHead->fieldType = CLASSIFY_FIELD_INTER_PRI;
    fieldHead->classify_pattern.interPri.value = (uint16)*data_ptr;
    fieldHead->classify_pattern.interPri.mask = (uint16)*mask_ptr;
    fieldHead->next = NULL;

    DIAG_UTIL_ERR_CHK(rtk_classify_field_add(&diag_classfRule, fieldHead), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_rule_internal_priority_data_data_mask_mask */

/*
 * classf set rule svlan-bit data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_classf_set_rule_svlan_bit_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    rtk_classify_field_t *fieldHead;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_PARAM_RANGE_CHK((*data_ptr > 1), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*mask_ptr > 1), CPARSER_ERR_INVALID_PARAMS);

    fieldHead = (rtk_classify_field_t*)osal_alloc(sizeof(rtk_classify_field_t));
    fieldHead->fieldType = CLASSIFY_FIELD_IS_STAG;
    fieldHead->classify_pattern.isStag.value = (uint16)*data_ptr;
    fieldHead->classify_pattern.isStag.mask = (uint16)*mask_ptr;
    fieldHead->next = NULL;

    DIAG_UTIL_ERR_CHK(rtk_classify_field_add(&diag_classfRule, fieldHead), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_rule_svlan_bit_data_data_mask_mask */

/*
 * classf set rule cvlan-bit data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_classf_set_rule_cvlan_bit_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    rtk_classify_field_t *fieldHead;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_PARAM_RANGE_CHK((*data_ptr > 1), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*mask_ptr > 1), CPARSER_ERR_INVALID_PARAMS);

    fieldHead = (rtk_classify_field_t*)osal_alloc(sizeof(rtk_classify_field_t));
    fieldHead->fieldType = CLASSIFY_FIELD_IS_CTAG;
    fieldHead->classify_pattern.isCtag.value = (uint16)*data_ptr;
    fieldHead->classify_pattern.isCtag.mask = (uint16)*mask_ptr;
    fieldHead->next = NULL;

    DIAG_UTIL_ERR_CHK(rtk_classify_field_add(&diag_classfRule, fieldHead), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_rule_cvlan_bit_data_data_mask_mask */

/*
 * classf set rule uni data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_classf_set_rule_uni_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    rtk_classify_field_t *fieldHead;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_PARAM_RANGE_CHK((*data_ptr > 0x7), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*mask_ptr > 0x7), CPARSER_ERR_INVALID_PARAMS);

    fieldHead = (rtk_classify_field_t*)osal_alloc(sizeof(rtk_classify_field_t));
    fieldHead->fieldType = CLASSIFY_FIELD_UNI;
    fieldHead->classify_pattern.uni.value = (uint16)*data_ptr;
    fieldHead->classify_pattern.uni.mask = (uint16)*mask_ptr;
    fieldHead->next = NULL;

    DIAG_UTIL_ERR_CHK(rtk_classify_field_add(&diag_classfRule, fieldHead), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_rule_uni_data_data_mask_mask */

/*
 * classf set rule ether-type data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_classf_set_rule_ether_type_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    rtk_classify_field_t *fieldHead;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_PARAM_RANGE_CHK((*data_ptr > 0xffff), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*mask_ptr > 0xffff), CPARSER_ERR_INVALID_PARAMS);

    fieldHead = (rtk_classify_field_t*)osal_alloc(sizeof(rtk_classify_field_t));
    fieldHead->fieldType = CLASSIFY_FIELD_ETHERTYPE;
    fieldHead->classify_pattern.etherType.value = (uint16)*data_ptr;
    fieldHead->classify_pattern.etherType.mask = (uint16)*mask_ptr;
    fieldHead->next = NULL;

    DIAG_UTIL_ERR_CHK(rtk_classify_field_add(&diag_classfRule, fieldHead), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_rule_ether_type_data_data_mask_mask */

/*
 * classf set rule range-l4port data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_classf_set_rule_range_l4port_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    rtk_classify_field_t *fieldHead;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_PARAM_RANGE_CHK((*data_ptr > (0x8|(HAL_CLASSIFY_L4PORT_RANGE_NUM()-1))), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*mask_ptr > (0x8|(HAL_CLASSIFY_L4PORT_RANGE_NUM()-1))), CPARSER_ERR_INVALID_PARAMS);

    fieldHead = (rtk_classify_field_t*)osal_alloc(sizeof(rtk_classify_field_t));
    fieldHead->fieldType = CLASSIFY_FIELD_PORT_RANGE;
    fieldHead->classify_pattern.portRange.value = (uint16)*data_ptr;
    fieldHead->classify_pattern.portRange.mask = (uint16)*mask_ptr;
    fieldHead->next = NULL;

    DIAG_UTIL_ERR_CHK(rtk_classify_field_add(&diag_classfRule, fieldHead), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_rule_range_l4port_data_data_mask_mask */

/*
 * classf set rule range-ip data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_classf_set_rule_range_ip_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    rtk_classify_field_t *fieldHead;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_PARAM_RANGE_CHK((*data_ptr > (0x8|(HAL_CLASSIFY_IP_RANGE_NUM()-1))), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*mask_ptr > (0x8|(HAL_CLASSIFY_IP_RANGE_NUM()-1))), CPARSER_ERR_INVALID_PARAMS);

    fieldHead = (rtk_classify_field_t*)osal_alloc(sizeof(rtk_classify_field_t));
    fieldHead->fieldType = CLASSIFY_FIELD_IP_RANGE;
    fieldHead->classify_pattern.ipRange.value = (uint16)*data_ptr;
    fieldHead->classify_pattern.ipRange.mask = (uint16)*mask_ptr;
    fieldHead->next = NULL;

    DIAG_UTIL_ERR_CHK(rtk_classify_field_add(&diag_classfRule, fieldHead), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_rule_range_ip_data_data_mask_mask */

/*
 * classf set rule hit-acl data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_classf_set_rule_hit_acl_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    rtk_classify_field_t *fieldHead;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_PARAM_RANGE_CHK((*data_ptr > (0x80|(HAL_MAX_NUM_OF_ACL_RULE_ENTRY()-1))), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*mask_ptr > (0x80|(HAL_MAX_NUM_OF_ACL_RULE_ENTRY()-1))), CPARSER_ERR_INVALID_PARAMS);

    fieldHead = (rtk_classify_field_t*)osal_alloc(sizeof(rtk_classify_field_t));
    fieldHead->fieldType = CLASSIFY_FIELD_ACL_HIT;
    fieldHead->classify_pattern.aclHit.value = (uint16)*data_ptr;
    fieldHead->classify_pattern.aclHit.mask = (uint16)*mask_ptr;
    fieldHead->next = NULL;

    DIAG_UTIL_ERR_CHK(rtk_classify_field_add(&diag_classfRule, fieldHead), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_rule_hit_acl_data_data_mask_mask */

/*
 * classf set rule wan-if data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_classf_set_rule_wan_if_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    rtk_classify_field_t *fieldHead;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_PARAM_RANGE_CHK(*data_ptr >= (HAL_L34_NETIF_ENTRY_MAX()), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK(*mask_ptr >= (HAL_L34_NETIF_ENTRY_MAX()), CPARSER_ERR_INVALID_PARAMS);

    fieldHead = (rtk_classify_field_t*)osal_alloc(sizeof(rtk_classify_field_t));
    fieldHead->fieldType = CLASSIFY_FIELD_WAN_IF;
    fieldHead->classify_pattern.wanIf.value = (uint16)*data_ptr;
    fieldHead->classify_pattern.wanIf.mask = (uint16)*mask_ptr;
    fieldHead->next = NULL;

    DIAG_UTIL_ERR_CHK(rtk_classify_field_add(&diag_classfRule, fieldHead), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_rule_wan_if_data_data_mask_mask */

/*
 * classf set rule ipmc-bit data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_classf_set_rule_ipmc_bit_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    rtk_classify_field_t *fieldHead;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_PARAM_RANGE_CHK((*data_ptr > 1), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*mask_ptr > 1), CPARSER_ERR_INVALID_PARAMS);

    fieldHead = (rtk_classify_field_t*)osal_alloc(sizeof(rtk_classify_field_t));
    fieldHead->fieldType = CLASSIFY_FIELD_IP4_MC;
    fieldHead->classify_pattern.ip4Mc.value = (uint16)*data_ptr;
    fieldHead->classify_pattern.ip4Mc.mask = (uint16)*mask_ptr;
    fieldHead->next = NULL;

    DIAG_UTIL_ERR_CHK(rtk_classify_field_add(&diag_classfRule, fieldHead), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_rule_ipmc_bit_data_data_mask_mask */

/*
 * classf set rule ip6mc-bit data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_classf_set_rule_ip6mc_bit_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    rtk_classify_field_t *fieldHead;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_PARAM_RANGE_CHK((*data_ptr > 1), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*mask_ptr > 1), CPARSER_ERR_INVALID_PARAMS);

    fieldHead = (rtk_classify_field_t*)osal_alloc(sizeof(rtk_classify_field_t));
    fieldHead->fieldType = CLASSIFY_FIELD_IP6_MC;
    fieldHead->classify_pattern.ip6Mc.value = (uint16)*data_ptr;
    fieldHead->classify_pattern.ip6Mc.mask = (uint16)*mask_ptr;
    fieldHead->next = NULL;

    DIAG_UTIL_ERR_CHK(rtk_classify_field_add(&diag_classfRule, fieldHead), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_rule_ip6mc_bit_data_data_mask_mask */

/*
 * classf set rule igmp-bit data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_classf_set_rule_igmp_bit_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    rtk_classify_field_t *fieldHead;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_PARAM_RANGE_CHK((*data_ptr > 1), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*mask_ptr > 1), CPARSER_ERR_INVALID_PARAMS);

    fieldHead = (rtk_classify_field_t*)osal_alloc(sizeof(rtk_classify_field_t));
    fieldHead->fieldType = CLASSIFY_FIELD_IGMP;
    fieldHead->classify_pattern.igmp.value = (uint16)*data_ptr;
    fieldHead->classify_pattern.igmp.mask = (uint16)*mask_ptr;
    fieldHead->next = NULL;

    DIAG_UTIL_ERR_CHK(rtk_classify_field_add(&diag_classfRule, fieldHead), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_rule_igmp_bit_data_data_mask_mask */

/*
 * classf set rule mld-bit data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_classf_set_rule_mld_bit_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    rtk_classify_field_t *fieldHead;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_PARAM_RANGE_CHK((*data_ptr > 1), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*mask_ptr > 1), CPARSER_ERR_INVALID_PARAMS);

    fieldHead = (rtk_classify_field_t*)osal_alloc(sizeof(rtk_classify_field_t));
    fieldHead->fieldType = CLASSIFY_FIELD_MLD;
    fieldHead->classify_pattern.mld.value = (uint16)*data_ptr;
    fieldHead->classify_pattern.mld.mask = (uint16)*mask_ptr;
    fieldHead->next = NULL;

    DIAG_UTIL_ERR_CHK(rtk_classify_field_add(&diag_classfRule, fieldHead), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_rule_mld_bit_data_data_mask_mask */

/*
 * classf set rule dei-bit data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_classf_set_rule_dei_bit_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    rtk_classify_field_t *fieldHead;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_PARAM_RANGE_CHK((*data_ptr > 1), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*mask_ptr > 1), CPARSER_ERR_INVALID_PARAMS);

    fieldHead = (rtk_classify_field_t*)osal_alloc(sizeof(rtk_classify_field_t));
    fieldHead->fieldType = CLASSIFY_FIELD_DEI;
    fieldHead->classify_pattern.dei.value = (uint16)*data_ptr;
    fieldHead->classify_pattern.dei.mask = (uint16)*mask_ptr;
    fieldHead->next = NULL;

    DIAG_UTIL_ERR_CHK(rtk_classify_field_add(&diag_classfRule, fieldHead), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_rule_dei_bit_data_data_mask_mask */

/*
 * classf set operation entry <UINT:index> ( upstream | downstream ) ( hit | not )
 */
cparser_result_t
cparser_cmd_classf_set_operation_entry_index_upstream_downstream_hit_not(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*index_ptr >= HAL_CLASSIFY_ENTRY_MAX()), CPARSER_ERR_INVALID_PARAMS);

    if ('h' == TOKEN_CHAR(6,0))
        diag_classfRule.invert = CLASSIFY_INVERT_DISABLE;
    else if ('n' == TOKEN_CHAR(6,0))
        diag_classfRule.invert = CLASSIFY_INVERT_ENABLE;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_operation_entry_index_upstream_downstream_hit_not */

/*
 * classf set upstream-action svlan-act ( nop | vs-tpid | c-tpid | del | transparent )
 */
cparser_result_t
cparser_cmd_classf_set_upstream_action_svlan_act_nop_vs_tpid_c_tpid_del_transparent(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    if('n' == TOKEN_CHAR(4,0))
        diag_classfRule.act.usAct.csAct = CLASSIFY_US_CSACT_NOP;
    else if('v' == TOKEN_CHAR(4,0))
        diag_classfRule.act.usAct.csAct = CLASSIFY_US_CSACT_ADD_TAG_VS_TPID;
    else if('c' == TOKEN_CHAR(4,0))
        diag_classfRule.act.usAct.csAct = CLASSIFY_US_CSACT_ADD_TAG_8100;
    else if('d' == TOKEN_CHAR(4,0))
        diag_classfRule.act.usAct.csAct = CLASSIFY_US_CSACT_DEL_STAG;
    else if('t' == TOKEN_CHAR(4,0))
        diag_classfRule.act.usAct.csAct = CLASSIFY_US_CSACT_TRANSPARENT;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_upstream_action_svlan_act_nop_vs_tpid_c_tpid_del_transparent */

/*
 * classf set upstream-action cvlan-act ( nop | c-tag | c2s | del | transparent )
 */
cparser_result_t
cparser_cmd_classf_set_upstream_action_cvlan_act_nop_c_tag_c2s_del_transparent(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    if('n' == TOKEN_CHAR(4,0))
        diag_classfRule.act.usAct.cAct = CLASSIFY_US_CACT_NOP;
    else if('c' == TOKEN_CHAR(4,0) && '-' == TOKEN_CHAR(4,1))
        diag_classfRule.act.usAct.cAct = CLASSIFY_US_CACT_ADD_CTAG_8100;
    else if('c' == TOKEN_CHAR(4,0) && '2' == TOKEN_CHAR(4,1))
        diag_classfRule.act.usAct.cAct = CLASSIFY_US_CACT_TRANSLATION_C2S;
    else if('d' == TOKEN_CHAR(4,0))
        diag_classfRule.act.usAct.cAct = CLASSIFY_US_CACT_DEL_CTAG;
    else if('t' == TOKEN_CHAR(4,0))
        diag_classfRule.act.usAct.cAct = CLASSIFY_US_CACT_TRANSPARENT;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_upstream_action_cvlan_act_nop_c_tag_c2s_del_transparent */

/*
 * classf set upstream-action svlan-id-act assign <UINT:vid>
 */
cparser_result_t
cparser_cmd_classf_set_upstream_action_svlan_id_act_assign_vid(
    cparser_context_t *context,
    uint32_t  *vid_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*vid_ptr > RTK_VLAN_ID_MAX), CPARSER_ERR_INVALID_PARAMS);

    diag_classfRule.act.usAct.csVidAct = CLASSIFY_US_VID_ACT_ASSIGN;
    diag_classfRule.act.usAct.sTagVid = *vid_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_upstream_action_svlan_id_act_assign_vid */

/*
 * classf set upstream-action svlan-id-act ( copy-outer | copy-inner )
 */
cparser_result_t
cparser_cmd_classf_set_upstream_action_svlan_id_act_copy_outer_copy_inner(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    if(0 == osal_strcmp("copy-inner", TOKEN_STR(4)))
        diag_classfRule.act.usAct.csVidAct = CLASSIFY_US_VID_ACT_FROM_2ND_TAG;
    else if(0 == osal_strcmp("copy-outer", TOKEN_STR(4)))
        diag_classfRule.act.usAct.csVidAct = CLASSIFY_US_VID_ACT_FROM_1ST_TAG;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_upstream_action_svlan_id_act_copy_outer_copy_inner */

/*
 * classf set upstream-action svlan-priority-act assign <UINT:priority>
 */
cparser_result_t
cparser_cmd_classf_set_upstream_action_svlan_priority_act_assign_priority(
    cparser_context_t *context,
    uint32_t  *priority_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*priority_ptr > RTK_DOT1P_PRIORITY_MAX), CPARSER_ERR_INVALID_PARAMS);

    diag_classfRule.act.usAct.csPriAct = CLASSIFY_US_PRI_ACT_ASSIGN;
    diag_classfRule.act.usAct.sTagPri = (uint8)*priority_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_upstream_action_svlan_priority_act_assign_priority */

/*
 * classf set upstream-action svlan-priority-act ( copy-outer | copy-inner | internal-priority )
 */
cparser_result_t
cparser_cmd_classf_set_upstream_action_svlan_priority_act_copy_outer_copy_inner_internal_priority(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    if('i' == TOKEN_CHAR(4,0))
        diag_classfRule.act.usAct.csPriAct = CLASSIFY_US_PRI_ACT_FROM_INTERNAL;
    else if(0 == osal_strcmp("copy-inner", TOKEN_STR(4)))
        diag_classfRule.act.usAct.csPriAct = CLASSIFY_US_PRI_ACT_FROM_2ND_TAG;
    else if(0 == osal_strcmp("copy-outer", TOKEN_STR(4)))
        diag_classfRule.act.usAct.csPriAct = CLASSIFY_US_PRI_ACT_FROM_1ST_TAG;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_upstream_action_svlan_priority_act_copy_outer_copy_inner_internal_priority */

/*
 * classf set upstream-action cvlan-id-act assign <UINT:vid>
 */
cparser_result_t
cparser_cmd_classf_set_upstream_action_cvlan_id_act_assign_vid(
    cparser_context_t *context,
    uint32_t  *vid_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*vid_ptr > RTK_VLAN_ID_MAX), CPARSER_ERR_INVALID_PARAMS);

    diag_classfRule.act.usAct.cVidAct = CLASSIFY_US_VID_ACT_ASSIGN;
    diag_classfRule.act.usAct.cTagVid = *vid_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_upstream_action_cvlan_id_act_assign_vid */

/*
 * classf set upstream-action cvlan-id-act ( copy-outer | copy-inner | internal-vid )
 */
cparser_result_t
cparser_cmd_classf_set_upstream_action_cvlan_id_act_copy_outer_copy_inner_internal_vid(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    if('i' == TOKEN_CHAR(4,0))
        diag_classfRule.act.usAct.cVidAct = CLASSIFY_US_VID_ACT_FROM_INTERNAL;
    else if(0 == osal_strcmp("copy-inner", TOKEN_STR(4)))
        diag_classfRule.act.usAct.cVidAct = CLASSIFY_US_VID_ACT_FROM_2ND_TAG;
    else if(0 == osal_strcmp("copy-outer", TOKEN_STR(4)))
        diag_classfRule.act.usAct.cVidAct = CLASSIFY_US_VID_ACT_FROM_1ST_TAG;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_upstream_action_cvlan_id_act_copy_outer_copy_inner_internal_vid */

/*
 * classf set upstream-action cvlan-priority-act assign <UINT:priority>
 */
cparser_result_t
cparser_cmd_classf_set_upstream_action_cvlan_priority_act_assign_priority(
    cparser_context_t *context,
    uint32_t  *priority_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*priority_ptr > RTK_DOT1P_PRIORITY_MAX), CPARSER_ERR_INVALID_PARAMS);

    diag_classfRule.act.usAct.cPriAct = CLASSIFY_US_PRI_ACT_ASSIGN;
    diag_classfRule.act.usAct.cTagPri = (uint8)*priority_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_upstream_action_cvlan_priority_act_assign_priority */

/*
 * classf set upstream-action cvlan-priority-act ( copy-outer | copy-inner | internal-priority )
 */
cparser_result_t
cparser_cmd_classf_set_upstream_action_cvlan_priority_act_copy_outer_copy_inner_internal_priority(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    if('i' == TOKEN_CHAR(4,0))
        diag_classfRule.act.usAct.cPriAct = CLASSIFY_US_PRI_ACT_FROM_INTERNAL;
    else if(0 == osal_strcmp("copy-inner", TOKEN_STR(4)))
        diag_classfRule.act.usAct.cPriAct = CLASSIFY_US_PRI_ACT_FROM_2ND_TAG;
    else if(0 == osal_strcmp("copy-outer", TOKEN_STR(4)))
        diag_classfRule.act.usAct.cPriAct = CLASSIFY_US_PRI_ACT_FROM_1ST_TAG;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_upstream_action_cvlan_priority_act_copy_outer_copy_inner_internal_priority */

/*
 * classf set upstream-action sid-act ( sid | qid ) <UINT:id>
 */
cparser_result_t
cparser_cmd_classf_set_upstream_action_sid_act_sid_qid_id(
    cparser_context_t *context,
    uint32_t  *id_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*id_ptr >= HAL_CLASSIFY_SID_NUM()), CPARSER_ERR_INVALID_PARAMS);

    if('q' == TOKEN_CHAR(4,0))
        diag_classfRule.act.usAct.sidQidAct = CLASSIFY_US_SQID_ACT_ASSIGN_QID;
    else if('s' == TOKEN_CHAR(4,0))
        diag_classfRule.act.usAct.sidQidAct = CLASSIFY_US_SQID_ACT_ASSIGN_SID;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    diag_classfRule.act.usAct.sidQid = *id_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_upstream_action_sid_act_sid_qid_id */

/*
 * classf set upstream-action priority-act follow-swcore
 */
cparser_result_t
cparser_cmd_classf_set_upstream_action_priority_act_follow_swcore(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    diag_classfRule.act.usAct.interPriAct = CLASSIFY_CF_PRI_ACT_NOP;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_upstream_action_priority_act_follow_swcore */

/*
 * classf set upstream-action priority-act assign <UINT:priority>
 */
cparser_result_t
cparser_cmd_classf_set_upstream_action_priority_act_assign_priority(
    cparser_context_t *context,
    uint32_t  *priority_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*priority_ptr > RTK_DOT1P_PRIORITY_MAX), CPARSER_ERR_INVALID_PARAMS);

    diag_classfRule.act.usAct.interPriAct = CLASSIFY_CF_PRI_ACT_ASSIGN;
    diag_classfRule.act.usAct.cfPri = (uint8)*priority_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_upstream_action_priority_act_assign_priority */

/*
 * classf set upstream-action remark-dscp ( enable | disable )
 */
cparser_result_t
cparser_cmd_classf_set_upstream_action_remark_dscp_enable_disable(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    if('e' == TOKEN_CHAR(4,0))
        diag_classfRule.act.usAct.dscp = CLASSIFY_DSCP_ACT_ENABLE;
    else if('d' == TOKEN_CHAR(4,0))
        diag_classfRule.act.usAct.dscp = CLASSIFY_DSCP_ACT_DISABLE;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_upstream_action_remark_dscp_enable_disable */

/*
 * classf set upstream-action drop ( enable | disable )
 */
cparser_result_t
cparser_cmd_classf_set_upstream_action_drop_enable_disable(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    if('e' == TOKEN_CHAR(4,0))
        diag_classfRule.act.usAct.drop = CLASSIFY_DROP_ACT_ENABLE;
    else if('d' == TOKEN_CHAR(4,0))
        diag_classfRule.act.usAct.drop = CLASSIFY_DROP_ACT_NONE;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_upstream_action_drop_enable_disable */

/*
 * classf set upstream-action statistic <UINT:index>
 */
cparser_result_t
cparser_cmd_classf_set_upstream_action_statistic_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*index_ptr >= HAL_MAX_NUM_OF_LOG_MIB()), CPARSER_ERR_INVALID_PARAMS);

    diag_classfRule.act.usAct.log = CLASSIFY_US_LOG_ACT_ENABLE;
    diag_classfRule.act.usAct.logCntIdx = (uint8)*index_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_upstream_action_statistic_index */

/*
 * classf set downstream-action svlan-act ( nop | vs-tpid | c-tpid | del | transparent | sp2c )
 */
cparser_result_t
cparser_cmd_classf_set_downstream_action_svlan_act_nop_vs_tpid_c_tpid_del_transparent_sp2c(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    if('n' == TOKEN_CHAR(4,0))
        diag_classfRule.act.dsAct.csAct = CLASSIFY_DS_CSACT_NOP;
    else if('v' == TOKEN_CHAR(4,0))
        diag_classfRule.act.dsAct.csAct = CLASSIFY_DS_CSACT_ADD_TAG_VS_TPID;
    else if('c' == TOKEN_CHAR(4,0))
        diag_classfRule.act.dsAct.csAct = CLASSIFY_DS_CSACT_ADD_TAG_8100;
    else if('d' == TOKEN_CHAR(4,0))
        diag_classfRule.act.dsAct.csAct = CLASSIFY_DS_CSACT_DEL_STAG;
    else if('t' == TOKEN_CHAR(4,0))
        diag_classfRule.act.dsAct.csAct = CLASSIFY_DS_CSACT_TRANSPARENT;
    else if('s' == TOKEN_CHAR(4,0))
        diag_classfRule.act.dsAct.csAct = CLASSIFY_DS_CSACT_SP2C;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_downstream_action_svlan_act_nop_vs_tpid_c_tpid_del_transparent_sp2c */

/*
 * classf set downstream-action svlan-id-act assign <UINT:vid>
 */
cparser_result_t
cparser_cmd_classf_set_downstream_action_svlan_id_act_assign_vid(
    cparser_context_t *context,
    uint32_t  *vid_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_PARAM_RANGE_CHK((*vid_ptr > RTK_VLAN_ID_MAX), CPARSER_ERR_INVALID_PARAMS);

    diag_classfRule.act.dsAct.csVidAct = CLASSIFY_DS_VID_ACT_ASSIGN;
    diag_classfRule.act.dsAct.sTagVid = *vid_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_downstream_action_svlan_id_act_assign_vid */

/*
 * classf set downstream-action svlan-id-act ( copy-outer | copy-inner )
 */
cparser_result_t
cparser_cmd_classf_set_downstream_action_svlan_id_act_copy_outer_copy_inner(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    if(0 == osal_strcmp("copy-inner", TOKEN_STR(4)))
        diag_classfRule.act.dsAct.csVidAct = CLASSIFY_DS_VID_ACT_FROM_2ND_TAG;
    else if(0 == osal_strcmp("copy-outer", TOKEN_STR(4)))
        diag_classfRule.act.dsAct.csVidAct = CLASSIFY_DS_VID_ACT_FROM_1ST_TAG;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_downstream_action_svlan_id_act_copy_outer_copy_inner */

/*
 * classf set downstream-action svlan-priority-act assign <UINT:priority>
 */
cparser_result_t
cparser_cmd_classf_set_downstream_action_svlan_priority_act_assign_priority(
    cparser_context_t *context,
    uint32_t  *priority_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_PARAM_RANGE_CHK((*priority_ptr > RTK_DOT1P_PRIORITY_MAX), CPARSER_ERR_INVALID_PARAMS);

    diag_classfRule.act.dsAct.csPriAct = CLASSIFY_DS_PRI_ACT_ASSIGN;
    diag_classfRule.act.dsAct.sTagPri = (uint8)*priority_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_downstream_action_svlan_priority_act_assign_priority */

/*
 * classf set downstream-action svlan-priority-act ( copy-outer | copy-inner | internal-priority )
 */
cparser_result_t
cparser_cmd_classf_set_downstream_action_svlan_priority_act_copy_outer_copy_inner_internal_priority(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();


    if('i' == TOKEN_CHAR(4,0))
        diag_classfRule.act.dsAct.csPriAct = CLASSIFY_DS_PRI_ACT_FROM_INTERNAL;
    else if(0 == osal_strcmp("copy-outer", TOKEN_STR(4)))
        diag_classfRule.act.dsAct.csPriAct = CLASSIFY_DS_PRI_ACT_FROM_1ST_TAG;
    else if(0 == osal_strcmp("copy-inner", TOKEN_STR(4)))
        diag_classfRule.act.dsAct.csPriAct = CLASSIFY_DS_PRI_ACT_FROM_2ND_TAG;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_downstream_action_svlan_priority_act_copy_outer_copy_inner_internal_priority */

/*
 * classf set downstream-action cvlan-act ( nop | c-tag | sp2c | del | transparent )
 */
cparser_result_t
cparser_cmd_classf_set_downstream_action_cvlan_act_nop_c_tag_sp2c_del_transparent(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    if('n' == TOKEN_CHAR(4,0))
        diag_classfRule.act.dsAct.cAct = CLASSIFY_DS_CACT_NOP;
    else if('c' == TOKEN_CHAR(4,0))
        diag_classfRule.act.dsAct.cAct = CLASSIFY_DS_CACT_ADD_CTAG_8100;
    else if('s' == TOKEN_CHAR(4,0))
        diag_classfRule.act.dsAct.cAct = CLASSIFY_DS_CACT_TRANSLATION_SP2C;
    else if('d' == TOKEN_CHAR(4,0))
        diag_classfRule.act.dsAct.cAct = CLASSIFY_DS_CACT_DEL_CTAG;
    else if('t' == TOKEN_CHAR(4,0))
        diag_classfRule.act.dsAct.cAct = CLASSIFY_DS_CACT_TRANSPARENT;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_downstream_action_cvlan_act_nop_c_tag_sp2c_del_transparent */

/*
 * classf set downstream-action cvlan-id-act ( follow-swcore | copy-outer | copy-inner | lookup-table )
 */
cparser_result_t
cparser_cmd_classf_set_downstream_action_cvlan_id_act_follow_swcore_copy_outer_copy_inner_lookup_table(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();


    if('f' == TOKEN_CHAR(4,0))
        diag_classfRule.act.dsAct.cVidAct = CLASSIFY_DS_VID_ACT_NOP;
    else if('l' == TOKEN_CHAR(4,0))
        diag_classfRule.act.dsAct.cVidAct = CLASSIFY_DS_VID_ACT_FROM_LUT;
    else if(0 == osal_strcmp("copy-outer", TOKEN_STR(4)))
        diag_classfRule.act.dsAct.cVidAct = CLASSIFY_DS_VID_ACT_FROM_1ST_TAG;
    else if(0 == osal_strcmp("copy-inner", TOKEN_STR(4)))
        diag_classfRule.act.dsAct.cVidAct = CLASSIFY_DS_VID_ACT_FROM_2ND_TAG;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_downstream_action_cvlan_id_act_follow_swcore_copy_outer_copy_inner_lookup_table */

/*
 * classf set downstream-action cvlan-id-act assign <UINT:cvid>
 */
cparser_result_t
cparser_cmd_classf_set_downstream_action_cvlan_id_act_assign_cvid(
    cparser_context_t *context,
    uint32_t  *cvid_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_PARAM_RANGE_CHK((*cvid_ptr > RTK_VLAN_ID_MAX), CPARSER_ERR_INVALID_PARAMS);

    diag_classfRule.act.dsAct.cVidAct = CLASSIFY_DS_VID_ACT_ASSIGN;
    diag_classfRule.act.dsAct.cTagVid = *cvid_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_downstream_action_cvlan_id_act_assign_cvid */

/*
 * classf set downstream-action cvlan-priority-act ( follow-swcore | copy-outer | copy-inner | internal-priority )
 */
cparser_result_t
cparser_cmd_classf_set_downstream_action_cvlan_priority_act_follow_swcore_copy_outer_copy_inner_internal_priority(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    if('f' == TOKEN_CHAR(4,0))
        diag_classfRule.act.dsAct.cPriAct = CLASSIFY_DS_PRI_ACT_NOP;
    if('i' == TOKEN_CHAR(4,0))
        diag_classfRule.act.dsAct.cPriAct = CLASSIFY_DS_PRI_ACT_FROM_INTERNAL;
    else if(0 == osal_strcmp("copy-inner", TOKEN_STR(4)))
        diag_classfRule.act.dsAct.cPriAct = CLASSIFY_DS_PRI_ACT_FROM_2ND_TAG;
    else if(0 == osal_strcmp("copy-outer", TOKEN_STR(4)))
        diag_classfRule.act.dsAct.cPriAct = CLASSIFY_DS_PRI_ACT_FROM_1ST_TAG;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_downstream_action_cvlan_priority_act_follow_swcore_copy_outer_copy_inner_internal_priority */

/*
 * classf set downstream-action cvlan-priority-act assign <UINT:priority>
 */
cparser_result_t
cparser_cmd_classf_set_downstream_action_cvlan_priority_act_assign_priority(
    cparser_context_t *context,
    uint32_t  *priority_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*priority_ptr > RTK_DOT1P_PRIORITY_MAX), CPARSER_ERR_INVALID_PARAMS);

    diag_classfRule.act.dsAct.cPriAct = CLASSIFY_DS_PRI_ACT_ASSIGN;
    diag_classfRule.act.dsAct.cTagPri = (uint8)*priority_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_downstream_action_cvlan_priority_act_assign_priority */

/*
 * classf set downstream-action priority-act follow-swcore
 */
cparser_result_t
cparser_cmd_classf_set_downstream_action_priority_act_follow_swcore(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    diag_classfRule.act.dsAct.interPriAct = CLASSIFY_CF_PRI_ACT_NOP;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_downstream_action_priority_act_follow_swcore */

/*
 * classf set downstream-action priority-act assign <UINT:priority>
 */
cparser_result_t
cparser_cmd_classf_set_downstream_action_priority_act_assign_priority(
    cparser_context_t *context,
    uint32_t  *priority_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*priority_ptr > RTK_DOT1P_PRIORITY_MAX), CPARSER_ERR_INVALID_PARAMS);

    diag_classfRule.act.dsAct.interPriAct = CLASSIFY_CF_PRI_ACT_ASSIGN;
    diag_classfRule.act.dsAct.cfPri = (uint8)*priority_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_downstream_action_priority_act_assign_priority */

/*
 * classf set downstream-action uni-forward-act ( flood | forced ) port ( <PORT_LIST:ports> | all | none )
 */
cparser_result_t
cparser_cmd_classf_set_downstream_action_uni_forward_act_flood_forced_port_ports_all_none(
    cparser_context_t *context,
    char * *ports_ptr)
{
    diag_portlist_t portlist;
    int32           ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);

    if(0 == osal_strcmp(TOKEN_STR(4),"flood"))
        diag_classfRule.act.dsAct.uniAct = CLASSIFY_DS_UNI_ACT_MASK_BY_UNIMASK;
    else if(0 == osal_strcmp(TOKEN_STR(4),"forced"))
        diag_classfRule.act.dsAct.uniAct = CLASSIFY_DS_UNI_ACT_FORCE_FORWARD;
    else
        return CPARSER_ERR_INVALID_PARAMS;

#if 0
    if(APOLLOMP_CHIP_ID == DIAG_UTIL_CHIP_TYPE)
    {
        diag_classfRule.act.dsAct.uniMask.bits[0] = portlist.portmask.bits[0];
    }
    else if(APOLLO_CHIP_ID == DIAG_UTIL_CHIP_TYPE)
    {
        /* bit 0-2:MAC0-2, bit 3-4:MAC4-5  bit5:MAC6(aka CPU) */
        diag_classfRule.act.dsAct.uniMask.bits[0] = ( (portlist.portmask.bits[0] & 0x7) |
                                             ((portlist.portmask.bits[0] & 0x70) >> 1) );
    }
#else
    diag_classfRule.act.dsAct.uniMask.bits[0] = portlist.portmask.bits[0];
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_downstream_action_uni_forward_act_flood_forced_port_ports_all_none */

/*
 * classf set downstream-action remark-dscp ( enable | disable )
 */
cparser_result_t
cparser_cmd_classf_set_downstream_action_remark_dscp_enable_disable(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    if('e' == TOKEN_CHAR(4,0))
        diag_classfRule.act.dsAct.dscp = CLASSIFY_DSCP_ACT_ENABLE;
    else if('d' == TOKEN_CHAR(4,0))
        diag_classfRule.act.dsAct.dscp = CLASSIFY_DSCP_ACT_DISABLE;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_downstream_action_remark_dscp */


/*
 * classf set upstream-unmatch-act ( drop | permit | permit-without-pon )
 */
cparser_result_t
cparser_cmd_classf_set_upstream_unmatch_act_drop_permit_permit_without_pon(
    cparser_context_t *context)
{
    rtk_classify_unmatch_action_t action;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    if(0 == osal_strcmp(TOKEN_STR(3),"drop"))
        action = CLASSIFY_UNMATCH_DROP;
    else if(0 == osal_strcmp(TOKEN_STR(3),"permit"))
        action = CLASSIFY_UNMATCH_PERMIT;
    else if(0 == osal_strcmp(TOKEN_STR(3),"permit-without-pon"))
        action = CLASSIFY_UNMATCH_PERMIT_WITHOUT_PON;

    DIAG_UTIL_ERR_CHK(rtk_classify_unmatchAction_set(action), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_upstream_unmatch_act_drop_permit_permit_without_pon */

/*
 * classf get upstream-unmatch-act
 */
cparser_result_t
cparser_cmd_classf_get_upstream_unmatch_act(
    cparser_context_t *context)
{
    rtk_classify_unmatch_action_t action;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_classify_unmatchAction_get(&action), ret);

    diag_util_printf("Upstream un-match action: %s\n\r", diagStr_cfUnmatchAct[action]);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_get_upstream_unmatch_act */

/*
 * classf set cf-sel-port ( pon | rg ) ( enable | disable )
 */
cparser_result_t
cparser_cmd_classf_set_cf_sel_port_pon_rg_enable_disable(
    cparser_context_t *context)
{
    rtk_port_t              port;
    rtk_classify_cf_sel_t   cfSel;
    int32                   ret;

    DIAG_UTIL_PARAM_CHK();

    if('p' == TOKEN_CHAR(3,0))
        port = HAL_GET_PON_PORT();
    else if('r' == TOKEN_CHAR(3,0))
        port = HAL_GET_RGMII_PORT();

    if('e' == TOKEN_CHAR(4,0))
        cfSel = CLASSIFY_CF_SEL_ENABLE;
    else if('d' == TOKEN_CHAR(4,0))
        cfSel = CLASSIFY_CF_SEL_DISABLE;

    DIAG_UTIL_ERR_CHK(rtk_classify_cfSel_set(port, cfSel), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_cf_sel_port_pon_rg_enable_disable */

/*
 * classf get cf-sel-port
 */
cparser_result_t
cparser_cmd_classf_get_cf_sel_port(
    cparser_context_t *context)
{
    rtk_port_t              port;
    rtk_classify_cf_sel_t   cfSel;
    int32                   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    port = HAL_GET_PON_PORT();
    DIAG_UTIL_ERR_CHK(rtk_classify_cfSel_get(port, &cfSel), ret);
    diag_util_printf("Cf select port: PON %s", diagStr_enable[cfSel]);

    port = HAL_GET_RGMII_PORT();
    DIAG_UTIL_ERR_CHK(rtk_classify_cfSel_get(port, &cfSel), ret);
    diag_util_printf("Cf select port: RGMII %s", diagStr_enable[cfSel]);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_get_cf_sel_port */

/*
 * classf get range-ip entry <UINT:index>
 */
cparser_result_t
cparser_cmd_classf_get_range_ip_entry_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    rtk_classify_rangeCheck_ip_t ipRange;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_PARAM_RANGE_CHK((*index_ptr >= HAL_CLASSIFY_IP_RANGE_NUM()), CPARSER_ERR_INVALID_PARAMS);

    ipRange.index = *index_ptr;
    DIAG_UTIL_ERR_CHK(rtk_classify_ipRange_get(&ipRange), ret);
    diag_util_printf("Range check of IP address\n");
    diag_util_printf("Index: %d Upper: %s ",
                                                    ipRange.index,
                                                    diag_util_inet_ntoa(ipRange.upperIp));
    diag_util_printf("Lower: %s Type: %s\n",
                                                    diag_util_inet_ntoa(ipRange.lowerIp),
                                                    diagStr_cfRangeCheckIpTypeStr[ipRange.type]);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_get_range_ip_entry_index */

/*
 * classf set range-ip entry <UINT:index> type ( sip | dip )
 */
cparser_result_t
cparser_cmd_classf_set_range_ip_entry_index_type_sip_dip(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    rtk_classify_rangeCheck_ip_t    ipRange;
    int32                   ret;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_PARAM_RANGE_CHK((*index_ptr >= HAL_CLASSIFY_IP_RANGE_NUM()), CPARSER_ERR_INVALID_PARAMS);

    ipRange.index = *index_ptr;
    DIAG_UTIL_ERR_CHK(rtk_classify_ipRange_get(&ipRange), ret);

    if('s' == TOKEN_CHAR(6,0))
        ipRange.type = CLASSIFY_IPRANGE_IPV4_SIP;
    else if('d' == TOKEN_CHAR(6,0))
        ipRange.type = CLASSIFY_IPRANGE_IPV4_DIP;

    DIAG_UTIL_ERR_CHK(rtk_classify_ipRange_set(&ipRange), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_range_ip_entry_index_type_sip_dip */

/*
 * classf set range-ip entry <UINT:index> low-bound <IPV4ADDR:low_bound_ip> up-bound <IPV4ADDR:up_bound_ip>
 */
cparser_result_t
cparser_cmd_classf_set_range_ip_entry_index_low_bound_low_bound_ip_up_bound_up_bound_ip(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *low_bound_ip_ptr,
    uint32_t  *up_bound_ip_ptr)
{
    rtk_classify_rangeCheck_ip_t    ipRange;
    int32                   ret;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_PARAM_RANGE_CHK((*index_ptr >= HAL_CLASSIFY_IP_RANGE_NUM()), CPARSER_ERR_INVALID_PARAMS);

    ipRange.index = *index_ptr;
    DIAG_UTIL_ERR_CHK(rtk_classify_ipRange_get(&ipRange), ret);

    ipRange.lowerIp = *low_bound_ip_ptr;
    ipRange.upperIp = *up_bound_ip_ptr;

    DIAG_UTIL_ERR_CHK(rtk_classify_ipRange_set(&ipRange), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_range_ip_entry_index_low_bound_low_bound_ip_up_bound_up_bound_ip */

/*
 * classf get range-l4port entry <UINT:index>
 */
cparser_result_t
cparser_cmd_classf_get_range_l4port_entry_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    rtk_classify_rangeCheck_l4Port_t portRange;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_PARAM_RANGE_CHK((*index_ptr >= HAL_CLASSIFY_L4PORT_RANGE_NUM()), CPARSER_ERR_INVALID_PARAMS);

    portRange.index = *index_ptr;
    DIAG_UTIL_ERR_CHK(rtk_classify_portRange_get(&portRange), ret);

    diag_util_printf("Range check of L4 port\n");
    diag_util_printf("Index: %d Upper: %d Lower: %d Type: %s\n",
                                                    portRange.index,
                                                    portRange.upperPort,
                                                    portRange.lowerPort,
                                                    diagStr_cfRangeCheckPortTypeStr[portRange.type]);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_get_range_l4port_entry_index */

/*
 * classf set range-l4port entry <UINT:index> type ( src-port | dst-port )
 */
cparser_result_t
cparser_cmd_classf_set_range_l4port_entry_index_type_src_port_dst_port(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    rtk_classify_rangeCheck_l4Port_t portRange;
    int32                   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*index_ptr >= HAL_CLASSIFY_L4PORT_RANGE_NUM()), CPARSER_ERR_INVALID_PARAMS);

    portRange.index = *index_ptr;
    DIAG_UTIL_ERR_CHK(rtk_classify_portRange_get(&portRange), ret);

    if('s' == TOKEN_CHAR(6,0))
        portRange.type = CLASSIFY_PORTRANGE_SPORT;
    else if('d' == TOKEN_CHAR(6,0))
        portRange.type = CLASSIFY_PORTRANGE_DPORT;

    DIAG_UTIL_ERR_CHK(rtk_classify_portRange_set(&portRange), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_range_l4port_entry_index_type_src_port_dst_port */

/*
 * classf set range-l4port entry <UINT:index> low-bound <UINT:l4lport> up-bound <UINT:l4uport>
 */
cparser_result_t
cparser_cmd_classf_set_range_l4port_entry_index_low_bound_l4lport_up_bound_l4uport(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *l4lport_ptr,
    uint32_t  *l4uport_ptr)
{
    rtk_classify_rangeCheck_l4Port_t portRange;
    int32                   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*index_ptr >= HAL_CLASSIFY_L4PORT_RANGE_NUM()), CPARSER_ERR_INVALID_PARAMS);

    portRange.index = *index_ptr;
    DIAG_UTIL_ERR_CHK(rtk_classify_portRange_get(&portRange), ret);

    portRange.lowerPort = (uint16)*l4lport_ptr;
    portRange.upperPort = (uint16)*l4uport_ptr;

    DIAG_UTIL_ERR_CHK(rtk_classify_portRange_set(&portRange), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_range_l4port_entry_index_low_bound_l4lport_up_bound_l4uport */

/*
 * classf set remarking dscp priority <UINT:priority> dscp <UINT:dscp>
 */
cparser_result_t
cparser_cmd_classf_set_remarking_dscp_priority_priority_dscp_dscp(
    cparser_context_t *context,
    uint32_t  *priority_ptr,
    uint32_t  *dscp_ptr)
{
    int32                   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*priority_ptr > RTK_DOT1P_PRIORITY_MAX), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*dscp_ptr > RTK_VALUE_OF_DSCP_MAX), CPARSER_ERR_INVALID_PARAMS);

    DIAG_UTIL_ERR_CHK(rtk_classify_cfPri2Dscp_set(*priority_ptr, *dscp_ptr), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_remarking_dscp_priority_priority_dscp_dscp */

/*
 * classf get remarking dscp
 */
cparser_result_t
cparser_cmd_classf_get_remarking_dscp(
    cparser_context_t *context)
{
    rtk_pri_t   pri;
    rtk_dscp_t  dscp;
    int32       ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_printf("CF_priority  DSCP");
    for(pri=0; pri<=RTK_DOT1P_PRIORITY_MAX; pri++)
    {
        DIAG_UTIL_ERR_CHK(rtk_classify_cfPri2Dscp_get(pri, &dscp), ret);
        diag_util_printf("%11d  %4d\n\r");
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_get_remarking_dscp */


