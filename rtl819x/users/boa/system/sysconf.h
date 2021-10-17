#ifndef INCLUDE_SYSCONF_H
#define INCLUDE_SYSCONF_H


//#define SYS_DEBUG 1

#define NULL_FILE 0
#define NULL_STR ""
#define PROC_FASTNAT_FILE "/proc/fast_nat"
#define PROC_FASTPPTP_FILE "/proc/fast_pptp"
#define PROC_FASTL2TP_FILE "/proc/fast_l2tp"
#define PROC_FASTPPPOE_FILE "/proc/fast_pppoe"
#define PROC_PPTP_CONN_FILE "/proc/pptp_conn_ck"
#define PROC_BR_IGMPPROXY "/proc/br_igmpProxy"
#define PROC_BR_IGMPSNOOP "/proc/br_igmpsnoop"
#define PROC_BR_IGMPVERSION "/proc/br_igmpVersion"
#define PROC_BR_IGMPQUERY "/proc/br_igmpquery"
#define PROC_BR_MCASTFASTFWD "/proc/br_mCastFastFwd"
#define PROC_BR_MLDSNOOP "/proc/br_mldsnoop"
#define PROC_BR_MLDQUERY "/proc/br_mldquery"
#define PROC_BR_IGMPDB "/proc/br_igmpDb"
#define PROC_IGMP_MAX_MEMBERS "/proc/sys/net/ipv4/igmp_max_memberships"
#define PROC_GPIO "/proc/gpio"
#define BR_IFACE_FILE "/var/system/br_iface"
#define MESH_PATHSEL "/bin/pathsel" 
#define BR_INIT_FILE "/tmp/bridge_init"
#define ETH_VLAN_SWITCH "/proc/disable_l2_table"
#define PROC_RTK_VLAN_SUPPORT "/proc/rtk_vlan_support"
#if defined(CONFIG_APP_RTK_INBAND_CTL)
#define PROC_INBAND_CTL_ACL "/proc/inband_ctl_acl"
#endif
//Initial system time especilly for cert usage
#define SET_TIME "/var/system/set_time"

//For wapi cert
#define CA_CERT_FILE "/var/myca/CA.cert"
#define CA4AP_CERT "/var/myca/ca4ap.cert"
#define AP_CERT "/var/myca/ap.cert"

#define AP_CERT_AS0 "/var/myca/ap_as0.cert"	//From remote AS0
#define CA4AP_CERT_AS0 "/var/myca/ca4ap_as0.cert"		//From remote AS0
#define AP_CERT_AS1 "/var/myca/ap_as1.cert"	//From remote AS1
#define CA4AP_CERT_AS1 "/var/myca/ca4ap_as1.cert"		//From remote AS1

#define WLAN_INTERFACE_LIST "wlan0,wlan0-va0,wlan0-va1,wlan0-va2,wlan0-va3"
#define WLAN_INTERFACE_LIST_1 "wlan1,wlan1-va0,wlan1-va1,wlan1-va2,wlan1-va3"

#define HW_NAT_FILE "/proc/hw_nat"
#define SOFTWARE_NAT_FILE "/proc/sw_nat"
#define IGD_PID_FILE "/var/run/miniupnpd.pid"
#define RIP_PID_FILE "/var/run/routed.pid"
#define TR069_PID_FILE "var/run/cwmp.pid"
#define DNRD_PID_FILE "/var/run/dnrd.pid"
#define L2TPD_PID_FILE "/var/run/l2tpd.pid"
#define IGMPPROXY_PID_FILE "/var/run/igmp_pid"
#define LLTD_PID_FILE "/var/run/lld2d-br0.pid"
#define DHCPD_PID_FILE "/var/run/udhcpd.pid"
#define LLTD_PROCESS_FILE "/bin/lld2d"
#define SNMPD_PROCESS_FILE "/bin/snmpd"
#define SNMPD_PID_FILE  "/var/run/snmpd.pid"
#define SNMPD_CONF_FILE "/etc/snmpd.conf"
#define NMSD_PROCESS_FILE "/bin/nmsd"

#define RESOLV_CONF "/var/resolv.conf"

#define PPPLINKFILE "/etc/ppp/link"
#define PPPD_PID_FILE "/var/run/ppp0.pid"
#define PPP_CONNECT_FILE "/etc/ppp/connectfile"
#define PPP_PATCH_FILE "/etc/ppp/pppPatch"
#define FIRSTDDNS "/var/firstddns"
#define PPP_RESOLV_FILE "/etc/ppp/resolv.conf"
#define PPP_PAP_FILE1 "/etc/ppp/pap-secrets"
#define PPP_CHAP_FILE1 "/etc/ppp/chap-secrets"
#define PPP_FIRST_FILE "/etc/ppp/first"
#define PPP_FIRST_DEMAND "/etc/ppp/firstdemand"
#define PPP_FILE "/var/ppp"
#define PPP_OPTIONS_FILE1 "/etc/ppp/options"
#define ROUTED_CONF_FILE "/var/run/routed.conf"
#define LAST_WAN_TYPE_FILE "/var/system/last_wan"
#define PPTP_PEERS_FILE "/etc/ppp/peers/rpptp"
#ifdef CONFIG_IPV6
#define DHCP6S_PID_FILE "/var/run/dhcp6s.pid"
#define DHCP6S_CONF_FILE "/var/dhcp6s.conf"
#define DHCP6C_PID_FILE "/var/run/dhcp6c.pid"
#define DHCP6C_CONF_FILE "/var/dhcp6c.conf"
#define DNSV6_PID_FILE "/var/run/dnsmasq.pid"
#define DNRD_PID_FILE "/var/run/dnrd.pid"
#define DNSV6_CONF_FILE "/var/dnsmasq.conf"

#define DSLITE_SCRIPT "/var/ds-lite.script"
#define DHCP6C_SCRIPT "/var/dhcp6c.sh"

#define DNSV6_RESOLV_FILE "/var/dnsmasq_resolv.conf"

#define RESOLV_CONF_FILE "/etc/resolv.conf"
#define RADVD_CONF_FILE "/var/radvd.conf"
#define RADVD_PID_FILE "/var/run/radvd.pid"
#define ECMH_PID_FILE	"/var/run/ecmh.pid"
#define DHCP6C_DUID_FILE  "/var/dhcpv6_duid"
#define IPV6_ROUTE_PROC "/proc/net/ipv6_route"
#define IPV6_ADDR_PROC "/proc/net/if_inet6"
#define IPV6_GATEWAY_FILE	"/var/gateway_ipv6"
#define DHCP6PD_CONF_FILE "/var/dhcp6pd.conf"
#define DNSV6_ADDR_FILE	"/var/dns6.conf"
#ifdef CONFIG_IPV6_CE_ROUTER_SUPPORT
#define SIPV6_ADDR_FILE_CE	"/var/sip6_ce.conf"
#define DNSV6_ADDR_FILE_CE	"/var/dns6_ce.conf"
#define DNSNAMEV6_ADDR_FILE_CE	"/var/dnsname6_ce.conf"
#endif
//mldproxy
#define PROC_BR_MLDPROXY "/proc/br_mldProxy"
#define PROC_BR_MLDSNOOP "/proc/br_mldsnoop"
#define PROC_BR_MLDVERSION "/proc/br_mldVersion"
#define PROC_BR_MLDQUERY "/proc/br_mldquery"
//#define PROC_MLD_MAX_MEMBERS "/proc/sys/net/ipv6/mld_max_memberships"

#define MLDPROXY_PID_FILE "/var/run/mld_pid"


#endif
#ifdef MULTI_PPPOE
#define PPP_PAP_FILE2 "/etc/ppp/pap-secrets2"
#define PPP_CHAP_FILE2 "/etc/ppp/chap-secrets2"
#define PPP_OPTIONS_FILE2 "/etc/ppp/options2"

#define PPP_PAP_FILE3 "/etc/ppp/pap-secrets3"
#define PPP_CHAP_FILE3 "/etc/ppp/chap-secrets3"
#define PPP_OPTIONS_FILE3 "/etc/ppp/options3"

#define PPP_PAP_FILE4 "/etc/ppp/pap-secrets4"
#define PPP_CHAP_FILE4 "/etc/ppp/chap-secrets4"
#define PPP_OPTIONS_FILE4 "/etc/ppp/options4"
#endif
#define L2TPCONF "/etc/ppp/l2tpd.conf"
#define WEBS_PID_FILE "/var/run/webs.pid"
#define RT_CACHE_REBUILD_COUNT "/proc/sys/net/ipv4/rt_cache_rebuild_count"

#define REINIT_FILE "/var/reinit"

#ifdef CONFIG_RTK_VOIP
#define FWUPDATE_PID_FILE "/var/run/fwupdate.pid"
#endif
#if defined(CONFIG_DYNAMIC_WAN_IP)
#define TEMP_WAN_CHECK "/var/tmp_wan_check"
#define TEMP_WAN_DHCP_INFO "/var/tmp_wan_dhcp_info"
#define MANUAL_CONNECT_NOW "/var/ppp/manual_connect_now"
#define TEMP_WAN_PPPOE_L2TP_CHECK "/var/tmp_wan_pppoe_l2tp_check"
#endif
#define RESTART_IAPP "/var/restart_iapp"
#define PREVIOUS_WAN_DHCP_INFO "/var/previous_wan_dhcp_info"
#define PREVIOUS_LAN_DHCP_INFO "/var/previous_lan_dhcp_info"
#endif

#ifdef CONFIG_CPU_UTILIZATION
#define CPU_LOAD_FILE "/web/cpu_data0.cpudat"
#endif
