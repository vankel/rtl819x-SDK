

/*example for one field access*/



typedef enum apollo_raw_svlan_priSel_e
{
    RAW_SVLAN_PRISEL_INTERNAL_PRI  = 0,
    RAW_SVLAN_PRISEL_1QTAG_PRI,
    RAW_SVLAN_PRISEL_VSPRI,
    RAW_SVLAN_PRISEL_PBPRI,
    RAW_SVLAN_PRISEL_END,    
} apollo_raw_svlan_priSel_t;


/* Function Name:
 *      apollo_raw_svlan_egrPriSel_set
 * Description:
 *      Set SVLAN egress tag priority selection
 * Input:
 *      mode 		- egress priority selection mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 apollo_raw_svlan_egrPriSel_set(rtk_svlan_priSel_t mode)
{
    int32   ret;
    apollo_raw_svlan_priSel_t raw_mode;
	/*parameter check*/
    RT_PARAM_CHK((SVLAN_PRISEL_END <= mode), RT_ERR_INPUT);


    switch(mode)
    {
        case SVLAN_PRISEL_INTERNAL_PRI:
            raw_mode = RAW_SVLAN_PRISEL_INTERNAL_PRI;
            break;
            
        case SVLAN_PRISEL_1QTAG_PRI:
            raw_mode = RAW_SVLAN_PRISEL_1QTAG_PRI;
            break;

        case SVLAN_PRISEL_VSPRI:
            raw_mode = RAW_SVLAN_PRISEL_VSPRI;
            break;

        case SVLAN_PRISEL_PBPRI:
            raw_mode = RAW_SVLAN_PRISEL_PBPRI;
            break;
                
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }
    
    
    if ((ret = reg_field_write(SVLAN_CTRLr, VS_SPRISELf, &mode)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollo_raw_svlan_egrPriSel_set */

/* Function Name:
 *      apollo_raw_svlan_egrPriSel_get
 * Description:
 *      Get SVLAN egress tag priority selection
 * Input:
 *      None
 * Output:
 *      pMode 		- egress priority selection mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 apollo_raw_svlan_egrPriSel_get(rtk_svlan_priSel_t* pMode)
{
    int32   ret;
    apollo_raw_svlan_priSel_t raw_mode;
    
	/*parameter check*/
    RT_PARAM_CHK((pMode == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(SVLAN_CTRLr, VS_SPRISELf, &raw_mode)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    switch(raw_mode)
    {
        case RAW_SVLAN_PRISEL_INTERNAL_PRI:
            *pMode = SVLAN_PRISEL_INTERNAL_PRI;
            break;
            
        case RAW_SVLAN_PRISEL_1QTAG_PRI:
            *pMode = SVLAN_PRISEL_1QTAG_PRI;
            break;

        case RAW_SVLAN_PRISEL_VSPRI:
            *pMode = SVLAN_PRISEL_VSPRI;
            break;

        case RAW_SVLAN_PRISEL_PBPRI:
            *pMode = SVLAN_PRISEL_PBPRI;
            break;
                
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }
    


    return RT_ERR_OK;
} /* end of apollo_raw_svlan_egrPriSel_get */





/*example for multiple fields access*/

/* Function Name:
 *      apollo_raw_svlan_mrbCfg_set
 * Description:
 *      Set SLAN member configuration entry
 * Input:
 *      pMbrCfg		- SVLAN member configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_PORT_MASK
 *      RT_ERR_PRIORITY
 *      RT_ERR_FID
 *      RT_ERR_SVLAN_ENTRY_INDEX
 *      RT_ERR_SVLAN_VID
 * Note:
 *      None
 */
int32 apollo_raw_svlan_mrbCfg_set(apollo_raw_svlan_mbrCfg_t* pMbrCfg)
{
    int32   ret;
	/*parameter check*/
    RT_PARAM_CHK((APOLLO_SVIDXMAX < pMbrCfg->idx ), RT_ERR_SVLAN_ENTRY_INDEX);
	RT_PARAM_CHK((APOLLO_VIDMAX < pMbrCfg->svid), RT_ERR_SVLAN_VID);
	RT_PARAM_CHK((APOLLO_PORTMASK < pMbrCfg->mbr), RT_ERR_PORT_MASK);
	RT_PARAM_CHK((APOLLO_PORTMASK < pMbrCfg->untagset), RT_ERR_PORT_MASK);
	RT_PARAM_CHK((APOLLO_PRIMAX < pMbrCfg->spri), RT_ERR_PRIORITY);
	RT_PARAM_CHK((APOLLO_FIDMAX < pMbrCfg->fid_msti), RT_ERR_FID);
	RT_PARAM_CHK((APOLLO_EFIDMAX < pMbrCfg->efid), RT_ERR_SVLAN_EFID);
	RT_PARAM_CHK((RTK_ENABLE_END <= pMbrCfg->efid_en), RT_ERR_INPUT);
	RT_PARAM_CHK((RTK_ENABLE_END <= pMbrCfg->fid_en), RT_ERR_INPUT);
	
    if ((ret = reg_array_field_write(SVLAN_MBRCFGr, pMbrCfg->idx , REG_ARRAY_INDEX_NONE, VIDf, &pMbrCfg->svid)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    if ((ret = reg_array_field_write(SVLAN_MBRCFGr, pMbrCfg->idx , REG_ARRAY_INDEX_NONE, MBRf, &pMbrCfg->mbr)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    if ((ret = reg_array_field_write(SVLAN_MBRCFGr, pMbrCfg->idx , REG_ARRAY_INDEX_NONE, UNTAGSETf, &pMbrCfg->untagset)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    if ((ret = reg_array_field_write(SVLAN_MBRCFGr, pMbrCfg->idx , REG_ARRAY_INDEX_NONE, SPRf, &pMbrCfg->spri)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    if ((ret = reg_array_field_write(SVLAN_MBRCFGr, pMbrCfg->idx , REG_ARRAY_INDEX_NONE, FIDENf, &pMbrCfg->fid_en)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    if ((ret = reg_array_field_write(SVLAN_MBRCFGr, pMbrCfg->idx , REG_ARRAY_INDEX_NONE, FID_MSTIf, &pMbrCfg->fid_msti)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    if ((ret = reg_array_field_write(SVLAN_MBRCFGr, pMbrCfg->idx , REG_ARRAY_INDEX_NONE, EFIDENf, &pMbrCfg->efid_en)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    if ((ret = reg_array_field_write(SVLAN_MBRCFGr, pMbrCfg->idx , REG_ARRAY_INDEX_NONE, EFIDf, &pMbrCfg->efid)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollo_raw_svlan_mrbCfg_set */

/* Function Name:
 *      apollo_raw_svlan_mrbCfg_get
 * Description:
 *      Get SLAN member configuration entry
 * Input:
 *      None
 * Output:
 *      pMbrCfg		- SVLAN member configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 apollo_raw_svlan_mrbCfg_get(apollo_raw_svlan_mbrCfg_t* pMbrCfg)
{
    int32   ret;

    if ((ret = reg_array_field_read(SVLAN_MBRCFGr, pMbrCfg->idx , REG_ARRAY_INDEX_NONE, VIDf, &pMbrCfg->svid)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    if ((ret = reg_array_field_read(SVLAN_MBRCFGr, pMbrCfg->idx , REG_ARRAY_INDEX_NONE, MBRf, &pMbrCfg->mbr)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    if ((ret = reg_array_field_read(SVLAN_MBRCFGr, pMbrCfg->idx , REG_ARRAY_INDEX_NONE, UNTAGSETf, &pMbrCfg->untagset)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    if ((ret = reg_array_field_read(SVLAN_MBRCFGr, pMbrCfg->idx , REG_ARRAY_INDEX_NONE, SPRf, &pMbrCfg->spri)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    if ((ret = reg_array_field_read(SVLAN_MBRCFGr, pMbrCfg->idx , REG_ARRAY_INDEX_NONE, FIDENf, &pMbrCfg->fid_en)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    if ((ret = reg_array_field_read(SVLAN_MBRCFGr, pMbrCfg->idx , REG_ARRAY_INDEX_NONE, FID_MSTIf, &pMbrCfg->fid_msti)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    if ((ret = reg_array_field_read(SVLAN_MBRCFGr, pMbrCfg->idx , REG_ARRAY_INDEX_NONE, EFIDENf, &pMbrCfg->efid_en)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    if ((ret = reg_array_field_read(SVLAN_MBRCFGr, pMbrCfg->idx , REG_ARRAY_INDEX_NONE, EFIDf, &pMbrCfg->efid)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }
    return RT_ERR_OK;
} /* end of apollo_raw_svlan_mrbCfg_get */


