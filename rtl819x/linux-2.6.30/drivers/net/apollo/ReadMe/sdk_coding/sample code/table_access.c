
typedef struct  apollo_raw_vlan4kentry_s{
    uint32 	vid;
    uint32 	mbr;
    uint32 	untag;
    uint32 	dslMbr;
    uint32 	exMbr;
    uint32  fid_msti;
    uint32  envlanpol;
    uint32  meteridx;
    uint32  vbpen;
    uint32  vbpri;
    uint32 	ivl_svl;
}apollo_raw_vlan4kentry_t;





 /* Function Name:
 *      apollo_raw_vlan_4kEntry_set
 * Description:
 *      Set VID mapped entry to 4K VLAN table
 * Input:
 *      pVlan4kEntry - 4K VLAN configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 					- Success
 *      RT_ERR_SMI  				- SMI access error
 *      RT_ERR_INPUT  				- Invalid input parameter
 *      RT_ERR_L2_FID  				- Invalid FID
 *      RT_ERR_VLAN_VID 			- Invalid VID parameter (0~4095)
 *      RT_ERR_PORT_MASK  			- Invalid portmask
 *      RT_ERR_FILTER_METER_ID  	- Invalid meter
 *      RT_ERR_QOS_INT_PRIORITY  	- Invalid priority
 * Note:
 *      None
 */
int32 apollo_raw_vlan_4kEntry_set(apollo_raw_vlan4kentry_t *pVlan4kEntry )
{
	int32 ret;
    vlan_entry_t vlan_entry;  

    RT_PARAM_CHK((pVlan4kEntry->vid > APOLLO_VIDMAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((pVlan4kEntry->mbr > APOLLO_PORTMASK), RT_ERR_PORT_MASK);
    RT_PARAM_CHK((pVlan4kEntry->untag > APOLLO_PORTMASK), RT_ERR_PORT_MASK);
    RT_PARAM_CHK((pVlan4kEntry->fid_msti > APOLLO_FIDMAX), RT_ERR_L2_FID);
    RT_PARAM_CHK((pVlan4kEntry->meteridx > APOLLO_METERMAX), RT_ERR_FILTER_METER_ID);
    RT_PARAM_CHK((pVlan4kEntry->vbpri > APOLLO_PRIMAX), RT_ERR_QOS_INT_PRIORITY);

    osal_memset(&vlan_entry, 0, sizeof(vlan_entry));

    /*MBR*/
    if ((ret = table_field_set(VLANt, VLAN_MBRtf, (uint32 *)&pVlan4kEntry->mbr, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), ""); 
        return RT_ERR_FAILED;
    }

    /*UNTAG*/
    if ((ret = table_field_set(VLANt, VLAN_UNTAGtf, (uint32 *)&pVlan4kEntry->untag, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), ""); 
        return RT_ERR_FAILED;
    }

    /*EXT_MBR*/
    if ((ret = table_field_set(VLANt, VLAN_EXT_MBRtf, (uint32 *)&pVlan4kEntry->exMbr, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), ""); 
        return RT_ERR_FAILED;
    }

    /*DSL_MBR*/
    if ((ret = table_field_set(VLANt, VLAN_DSL_MBRtf, (uint32 *)&pVlan4kEntry->dslMbr, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), ""); 
        return RT_ERR_FAILED;
    }

    /*IVL_SVL*/
    if ((ret = table_field_set(VLANt, VLAN_IVL_SVLtf, (uint32 *)&pVlan4kEntry->ivl_svl, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), ""); 
        return RT_ERR_FAILED;
    }

    /*FID_MSTI*/
    if ((ret = table_field_set(VLANt, VLAN_FID_MSTItf, (uint32 *)&pVlan4kEntry->fid_msti, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), ""); 
        return RT_ERR_FAILED;
    }

    /*METERIDX*/
    if ((ret = table_field_set(VLANt, VLAN_METERIDXtf, (uint32 *)&pVlan4kEntry->meteridx, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), ""); 
        return RT_ERR_FAILED;
    }

    /*POLICING*/
    if ((ret = table_field_set(VLANt, VLAN_POLICINGtf, (uint32 *)&pVlan4kEntry->envlanpol, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), ""); 
        return RT_ERR_FAILED;
    }

    /*VB_EN*/
    if ((ret = table_field_set(VLANt, VLAN_VB_ENtf, (uint32 *)&pVlan4kEntry->vbpen, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), ""); 
        return RT_ERR_FAILED;
    }

    /*VBPRI*/        
    if ((ret = table_field_set(VLANt, VLAN_VBPRItf, (uint32 *)&pVlan4kEntry->vbpri, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), ""); 
        return RT_ERR_FAILED;
    }

    if ((ret = table_write(VLANt, pVlan4kEntry->vid, (uint32 *)&vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), ""); 
        return RT_ERR_FAILED;
    }


    return RT_ERR_OK;
}
/* Function Name:
 *      apollo_raw_vlan_4kEntry_get
 * Description:
 *      Get VID mapped entry to 4K VLAN table
 * Input:
 *      pVlan4kEntry - 4K VLAN configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 				- Success
 *      RT_ERR_VLAN_VID 		- Invalid VID parameter (0~4095)
 *      RT_ERR_BUSYWAIT_TIMEOUT - LUT is busy at retrieving
 * Note:
 *      None
 */
int32 apollo_raw_vlan_4kEntry_get(apollo_raw_vlan4kentry_t *pVlan4kEntry )
{
	int32 ret;
    uint32 tmp_val;
    vlan_entry_t vlan_entry;
    int i;
    
	/*parameter check*/
    RT_PARAM_CHK((pVlan4kEntry  == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pVlan4kEntry->vid > APOLLO_VIDMAX), RT_ERR_VLAN_VID);

    osal_memset(&vlan_entry, 0, sizeof(vlan_entry));

    if ((ret = table_read(VLANt, pVlan4kEntry->vid, (uint32 *)&vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), ""); 
        return RT_ERR_FAILED;
    }


    /*MBR*/
        /*get from vlan table*/
    if ((ret = table_field_get(VLANt, VLAN_MBRtf, (uint32 *)&pVlan4kEntry->mbr, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), ""); 
        return RT_ERR_FAILED;
    }

    /*UNTAG*/
    if ((ret = table_field_get(VLANt, VLAN_UNTAGtf, (uint32 *)&pVlan4kEntry->untag, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), ""); 
        return RT_ERR_FAILED;
    }

    /*EXT_MBR*/
    if ((ret = table_field_get(VLANt, VLAN_EXT_MBRtf, (uint32 *)&pVlan4kEntry->exMbr, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), ""); 
        return RT_ERR_FAILED;
    }

    /*DSL_MBR*/
    if ((ret = table_field_get(VLANt, VLAN_DSL_MBRtf, (uint32 *)&pVlan4kEntry->dslMbr, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), ""); 
        return RT_ERR_FAILED;
    }

    /*IVL_SVL*/
    if ((ret = table_field_get(VLANt, VLAN_IVL_SVLtf, (uint32 *)&pVlan4kEntry->ivl_svl, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), ""); 
        return RT_ERR_FAILED;
    }

    /*FID_MSTI*/
    if ((ret = table_field_get(VLANt, VLAN_FID_MSTItf, (uint32 *)&pVlan4kEntry->fid_msti, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), ""); 
        return RT_ERR_FAILED;
    }

    /*METERIDX*/
    if ((ret = table_field_get(VLANt, VLAN_METERIDXtf, (uint32 *)&pVlan4kEntry->meteridx, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), ""); 
        return RT_ERR_FAILED;
    }

    /*POLICING*/
    if ((ret = table_field_get(VLANt, VLAN_POLICINGtf, (uint32 *)&pVlan4kEntry->envlanpol, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), ""); 
        return RT_ERR_FAILED;
    }

    /*VB_EN*/
    if ((ret = table_field_get(VLANt, VLAN_VB_ENtf, (uint32 *)&pVlan4kEntry->vbpen, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), ""); 
        return RT_ERR_FAILED;
    }


    /*VBPRI*/        
    if ((ret = table_field_get(VLANt, VLAN_VBPRItf, (uint32 *)&pVlan4kEntry->vbpri, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), ""); 
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}
