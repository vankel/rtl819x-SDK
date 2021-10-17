ifeq ($(wildcard $(DIR_LINUX)/.config),)
include ../../../linux-2.6.30/.config
else
include $(DIR_LINUX)/.config
endif

ifeq ($(wildcard $(DIR_USERS)/.config),)
include ../../.config
else
include $(DIR_USERS)/.config
endif

#------------------------------------------------------------------------------
ifdef CONFIG_RTL_8198_AP_ROOT
CONFIG_RTL_AP = CONFIG_RTL_8198_AP_ROOT
endif

ifdef CONFIG_RTL_8197D_AP
CONFIG_RTL_AP = CONFIG_RTL_8197D_AP
endif

ifdef CONFIG_RTL_AP
AP=1
SKUTAG=ap
else
GW=1
SKUTAG=gw
endif

#------------------------------------------------------------------------------
COMPWEB=../tools/compweb

#------------------------------------------------------------------------------
.PHONY: clean all

#------------------------------------------------------------------------------
#Record which files need not include
EXCLUDE_FILES = 
EXCLUDE_FILES += ! -path '*/.*'
EXCLUDE_FILES += ! -path '*/*.bak'
EXCLUDE_FILES += ! -name Makefile
EXCLUDE_FILES += ! -name www_files
EXCLUDE_FILES += ! -name www_files.tar.bz2
EXCLUDE_FILES += ! -name webpages.bin

#samba web support
ifneq ($(CONFIG_APP_SAMBA_WEB),y)
EXCLUDE_FILES += ! -name administration.htm ! -name disk.htm ! -name storage_creategroup.htm ! -name storage_createsharefolder.htm
EXCLUDE_FILES += ! -name storage_createuser.htm ! -name storage_editgroup.htm ! -name storage_edituser.htm
endif

# routing support
ifneq ($(CONFIG_APP_ROUTED),y)
EXCLUDE_FILES += ! -name route.htm ! -name routetbl.htm
endif

# boa cgi support
ifneq ($(BOA_CGI_SUPPORT),y)
EXCLUDE_FILES += ! -name test.cgi ! -name nph-test.cgi
endif

#for 11s mesh
ifneq ($(CONFIG_RTL_MESH_SUPPORT),y)
EXCLUDE_FILES += ! -name buffalo.htm ! -name bull.htm ! -name wlmesh.htm ! -name wlmeshinfo.htm ! -name wlmeshACL.htm
endif

ifneq ($(CONFIG_RTK_VLAN_SUPPORT),y)
EXCLUDE_FILES += ! -name vlan.htm
endif

ifneq ($(CONFIG_PCIE_POWER_SAVING),y)
EXCLUDE_FILES += ! -name powerConsumption.htm ! -name powerConsumption_data.htm ! -name powerConsumption_proc.sxsl
EXCLUDE_FILES += ! -name num-0.gif ! -name num-1.gif ! -name num-2.gif ! -name num-3.gif ! -name num-4.gif ! -name num-5.gif
EXCLUDE_FILES += ! -name num-6.gif ! -name num-7.gif ! -name num-8.gif ! -name num-9.gif ! -name dot.gif ! -name num-0.gif
EXCLUDE_FILES += ! -name xml_data.js
endif

ifneq ($(CONFIG_RTL_ULINKER),y)
EXCLUDE_FILES += ! -name ulinker_opmode.htm
endif

ifneq ($(CONFIG_HTTP_FILE_SERVER_SUPPORT),y)
EXCLUDE_FILES += ! -name http_files.htm
EXCLUDE_FILES += ! -name http_files.htm
EXCLUDE_FILES += ! -name usb_conninfo.htm
EXCLUDE_FILES += ! -name http_files_dir.htm
endif

ifneq ($(CONFIG_RTL_P2P_SUPPORT),y)
EXCLUDE_FILES += ! -name wlp2p.htm ! -name wlp2pscanlist.htm ! -name wlp2pstate.htm ! -name wlp2pwscconnect.htm ! -name wlp2pwscsel.htm
endif

ifneq ($(CONFIG_APP_CTORRENT), y)
EXCLUDE_FILES += ! -name bt.htm ! -name btnewtorrent.htm ! -name btclient.htm
endif

#ifneq ($(CONFIG_RTL_P2P_SUPPORT),y)
#EXCLUDE_FILES += ! -name wlwapiCertManagement.htm
#endif

ifneq ($(CONFIG_IPV6),y)
EXCLUDE_FILES += ! -name dhcp6s.htm  ! -name ipv6_basic.htm ! -name ip6filter.htm ! -name dnsv6.htm ! -name ipv6_wan.htm ! -name radvd.htm ! -name tunnel6.htm ! -name ip6_qos.htm ! -name portfilter6.htm ! -name sub_menu_ipv6.htm
endif

ifeq ($(CONFIG_IPV6),y)
EXCLUDE_FILES += ! -name ipfilter.htm ! -name portfilter.htm ! -name ip_qos.htm
endif

ifneq ($(CONFIG_RTL_802_1X_CLIENT_SUPPORT),y)
EXCLUDE_FILES += ! -name rsCertInstall.htm
endif
ifneq ($(CONFIG_RTL_ETH_802DOT1X_SUPPORT),y)
EXCLUDE_FILES += ! -name eth_dot1x.htm
endif
ifneq ($(CONFIG_RTL_ETH_802DOT1X_CLIENT_MODE_SUPPORT),y)
EXCLUDE_FILES += ! -name ethdot1xCertInstall.htm
endif
ifneq ($(CONFIG_APP_SNMP),y)
EXCLUDE_FILES += ! -name snmp.htm
endif
ifneq ($(CONFIG_HTTP_FILE_SERVER_SUPPORT),y)
EXCLUDE_FILES += ! -name usb_fileinfo.htm
endif
ifeq ($(SKUTAG),ap)
EXCLUDE_FILES += ! -name portfilter.htm ! -name ipfilter.htm ! -name macfilter.htm ! -name portfw.htm ! -name urlfilter.htm ! -name dmz.htm ! -name vlan.htm ! -name ip_qos.htm ! -name tcpipwan.htm ! -name ddns.htm ! -name dos.htm ! -name opmode.htm
endif
ifeq ($(CONFIG_RTL_WAPI_SUPPORT),y)
ifneq ($(CONFIG_RTL_WAPI_LOCAL_AS_SUPPORT),y)
EXCLUDE_FILES += ! -name wlwapiCertManagement.htm ! -name  wlwapiDistribute.htm
endif
else
EXCLUDE_FILES += ! -name wlwapiCertManagement.htm ! -name  wlwapiDistribute.htm ! -name wlwapiRekey.htm ! -name  wlwapiinstallcert.htm
endif

# 8198 and POCKET ROUTER support both wapi psk and wapi cert
# 8196C (not include POCKET ROUTER) only support wapi psk
ifeq ($(CONFIG_RTL_8196C_GW),y)
ifneq ($(CONFIG_POCKET_ROUTER_SUPPORT),y)
#EXCLUDE_FILES += ! -name wlwapiCertManagement.htm ! -name  wlwapiDistribute.htm ! -name  wlwapiinstallcert.htm
EXCLUDE_FILES += ! -name wlwapiCertManagement.htm ! -name  wlwapiDistribute.htm 
endif
endif

ifneq ($(CONFIG_APP_TR069),y)
EXCLUDE_FILES += ! -name share.js ! -name tr069config.htm
endif

ifeq ($(CONFIG_APP_RTK_VOIP),y)
	EXCLUDE_FILES += ! -name vlan.htm
	EXCLUDE_FILES += ! -name route.htm ! -name routetbl.htm
	EXCLUDE_FILES += ! -name ip_qos.htm
endif
ifneq ($(CONFIG_RTK_VLAN_WAN_TAG_SUPPORT),y)
EXCLUDE_FILES += ! -name vlan_wan.htm
endif

ifeq ($(CONFIG_RTL_BT_CLIENT),y)
EXCLUDE_FILES += ! -name btclient.htm ! -name bt.htm ! -name btnewtorrent.htm
endif

ifeq ($(CONFIG_APP_BOA_NEW_UI),y)
EXCLUDE_FILES += ! -path './new/*'
EXCLUDE_FILES += ! -path './graphics/topbar.gif'
EXCLUDE_FILES += ! -path './menu-images/*'
EXCLUDE_FILES += ! -name title.htm ! -name code.htm ! -name menu_empty.html
EXCLUDE_FILES += ! -name home_old.htm  ! -name style_old.css ! -name mtmcode.js
else
EXCLUDE_FILES += ! -path './new/*'
EXCLUDE_FILES += ! -name home_old.htm ! -name style_old.css
endif
#------------------------------------------------------------------------------
all:
	rm -f www_files www_files.tar.bz2 webpages.bin webpages.*
	rm -f voip*.asp voip*.js
ifeq ($(CONFIG_APP_RTK_VOIP),y)
	rm -f voip*.asp voip*.js
	cp $(VOIP_APP)/web/voip*.asp .
	cp $(VOIP_APP)/web/voip*.js .
	sed -i "s/goform/boafrm/g" voip*.asp
	sed -i "s/method=\"get\"/method=\"post\"/g" voip*.asp
else
	rm -f voip*.asp voip*.js
endif
ifeq ($(CONFIG_APP_BOA_NEW_UI),y)
	cp  home.htm home_old.htm
	cp  style.css style_old.css
	cp  -f ./new/* .
endif
	find ./ -type f $(EXCLUDE_FILES) -name "*.*" > www_files
ifdef CONFIG_RTL_WEBPAGES_IN_ROOTFS
	cat www_files | xargs tar jcvf www_files.tar.bz2
else
#	$(COMPWEB) signature www_files webpages.bin $(CV_SIGNATURE)
	$(COMPWEB) $(SKUTAG) www_files webpages.bin
endif

ifeq ($(CONFIG_APP_BOA_NEW_UI),y)
#recovery
	ls -h ./new/ | xargs rm -f
	cp  home_old.htm home.htm
	cp  style_old.css style.css
endif

clean:
	rm -f www_files www_files.tar.bz2 webpages.bin webpages.*
ifeq ($(CONFIG_APP_RTK_VOIP),y)
	rm -f voip*.asp voip*.js
endif

