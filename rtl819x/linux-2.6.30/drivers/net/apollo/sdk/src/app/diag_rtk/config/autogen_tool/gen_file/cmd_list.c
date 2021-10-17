acl init
acl show 
acl clear 
acl add entry <UINT:index> 
acl del entry <UINT:index> 
acl del entry all 
acl get entry <UINT:index> 
acl set rule ( dmac | smac ) data <MACADDR:mac> mask <MACADDR:mac_mask>
acl set rule ( dmac | smac ) data <MACADDR:mac> 
acl set rule ( sip | dip ) data <IPV4ADDR:ip> mask <IPV4ADDR:ip_mask>
acl set rule ( sip | dip ) data <IPV4ADDR:ip>
acl set rule ( sip6 | dip6 ) data <IPV6ADDR:ip6> 
acl set rule ( sip6 | dip6 ) data <IPV6ADDR:ip6> mask <IPV6ADDR:ip6_mask>
acl set rule ( ethertype | gemport-llid | next-header ) data <UINT:data> 
acl set rule ( ethertype | gemport-llid | next-header ) data <UINT:data> mask <UINT:mask>
acl set rule range-l4port care-range <MASK_LIST:list> 
acl set rule ( range-vid | range-ip | range-length ) care-range <MASK_LIST:list> 
acl set rule ctag data vid <UINT:vid> priority <UINT:priority> cfi <UINT:cfi>
acl set rule ctag data vid <UINT:vid> priority <UINT:priority> cfi <UINT:cfi> mask vid <UINT:vid_mask> priority <UINT:priority_mask> cfi <UINT:cfi_mask>
acl set rule stag data vid <UINT:vid> priority <UINT:priority> dei <UINT:dei>
acl set rule stag data vid <UINT:vid> priority <UINT:priority> dei <UINT:dei> mask vid <UINT:vid_mask> priority <UINT:priority_mask> dei <UINT:dei_mask>
acl set rule user-field <UINT:index> data <UINT:data>
acl set rule user-field <UINT:index> data <UINT:data> mask <UINT:mask>
acl set rule pattern field-index <UINT:index> data <UINT:data>
acl set rule pattern field-index <UINT:index> data <UINT:data> mask <UINT:mask>
acl set rule care-tags { ctag } { ip } { ipv6 } { pppoe } { stag } { tcp } { udp } 
acl set rule port ( <PORT_LIST:ports> | all | none )
acl set rule template entry <UINT:index>
acl set rule operation reverse-state ( disable | enable )  
acl set rule state ( valid | invalid ) 
acl get entry <UINT:index> action
acl get range-vid index <UINT:index>
acl set range-vid entry <UINT:index> state ( invalid | valid )
acl set range-vid entry <UINT:index> type ( svid | cvid ) 
acl set range-vid entry <UINT:index> low-bound <UINT:vid>
acl set range-vid entry <UINT:index> up-bound <UINT:vid>
acl get range-ip entry <UINT:index>
acl set range-ip entry <UINT:index> state ( invalid | valid )
acl set range-ip entry <UINT:index> type ( sip | dip | sip6 | dip6 ) 
acl set range-ip entry <UINT:index> low-bound <IPV4ADDR:low_bound_ip> up-bound <IPV4ADDR:up_bound_ip>
acl get range-l4port entry <UINT:index>
acl set range-l4port entry <UINT:index> state ( invalid | valid )
acl set range-l4port entry <UINT:index> type ( src-port | dst-port )
acl set range-l4port entry <UINT:index> low-bound <UINT:l4port>
acl set range-l4port entry <UINT:index> up-bound <UINT:l4port>
acl get range-length entry <UINT:index>
acl set range-length entry <UINT:index> low-bound <UINT:length>
acl set range-length entry <UINT:index> up-bound <UINT:length>
acl set range-length entry <UINT:index> reverse-state ( disable | enable )
acl get port ( <PORT_LIST:ports> | all ) state
acl set port ( <PORT_LIST:ports> | all ) state ( enable | disable )
acl get port ( <PORT_LIST:ports> | all ) permit
acl set port ( <PORT_LIST:ports> | all ) permit ( enable | disable )
acl set action cvlan ingress vid <UINT:vid>
acl set action cvlan egress vid <UINT:vid>
acl set action cvlan using-svid
acl set action cvlan meter <UINT:index> 
acl set action cvlan statistic <UINT:index> 
acl set action cvlan remark dot1p-priority <UINT:priority>
acl set action svlan ingress svid <UINT:svid>
acl set action svlan egress svid <UINT:svid>
acl set action svlan using-cvid
acl set action svlan meter <UINT:index> 
acl set action svlan statistic <UINT:index> 
acl set action svlan remark dscp <UINT:dscp>
acl set action svlan remark dot1p-priority <UINT:priority>
acl set action priority assign-priority <UINT:priority>
acl set action priority remark dscp <UINT:dscp>
acl set action priority remark dot1p-priority <UINT:priority>
acl set action priority meter <UINT:index> 
acl set action priority statistic <UINT:index> 
acl set action meter <UINT:index> 
acl set action statistic <UINT:index> 
acl set action trap-to-cpu
acl set action copy port ( <PORT_LIST:ports> | all | none ) 
acl set action redirect port ( <PORT_LIST:ports> | all | none ) 
acl set action mirror port ( <PORT_LIST:ports> | all ) 
acl set action interrupt
acl set action latch-index
acl set action classf none 
acl set action classf sid <UINT:sid> 
acl set action classf llid <UINT:llid> 
acl set action classf ext-member ( <PORT_LIST:ext> | all )
acl set action none
acl show action 
acl clear action 
acl show template
acl clear template
acl set template ( stag | ctag | ethertype | gem-llid | ipv6-next-header | unknown | range-ip )
acl set template ( dip | sip | range-length )
acl set template ( smac | dmac | range-l4port )
acl set template ( sip6 | dip6 | range-vid )
acl set template user-field <UINT:index> 
acl ( add | del | get ) template entry <UINT:index>
acl set mode ( 64-entries | 128-entries )
acl get mode 
field-selector set index <UINT:index> format ( default | raw | llc | arp | ipv4-header | ipv6-header | ip-payload | l4-payload ) offset <UINT:offset>
field-selector get index <UINT:index>
acl dump log index <UINT:index>
acl reset log index <UINT:index>
acl set log mode index <UINT:index> ( bits-32 | bits-64 )
acl get log mode index <UINT:index> 
acl get log type index <UINT:index> 
acl set log type index <UINT:index> ( byte-count | packet-count )
acl get reason 
acl get reason ( all | cvlan | svlan | priority | policing | forward | extend )
auto-fallback get ignore-timeout state
auto-fallback set ignore-timeout state ( disable | enable )
auto-fallback set error-count ( 1 | 2 | 4 | 8 | 16 | 32 | 64 | 128 )
auto-fallback get error-count
auto-fallback get monitor-count
auto-fallback set monitor-count ( 8K | 16K | 32K | 64K | 128K | 256K | 512K | 1M )
auto-fallback get port <PORT_LIST:ports> error-counter
auto-fallback get port <PORT_LIST:ports> monitor-counter
auto-fallback get port <PORT_LIST:ports> restore
auto-fallback get port <PORT_LIST:ports> valid-flow
auto-fallback get port <PORT_LIST:ports> state
auto-fallback get port <PORT_LIST:ports> current-power-level
auto-fallback set port <PORT_LIST:ports> state ( disable | enable )
auto-fallback set reduce-power-level state ( disable | enable )
auto-fallback get reduce-power-level state
auto-fallback set timer state ( disable | enable )
auto-fallback get timer state
auto-fallback set timer <UINT:timer>
auto-fallback get timer
bandwidth init 
bandwidth get egress ifg
bandwidth set egress ifg ( exclude | include )
bandwidth get egress ifg port ( <PORT_LIST:ports> | all ) 
bandwidth set egress ifg port ( <PORT_LIST:ports> | all ) ( exclude | include )
bandwidth get egress port ( <PORT_LIST:ports> | all )
bandwidth get egress port ( <PORT_LIST:ports> | all ) queue-id ( <MASK_LIST:qid> | all )
bandwidth set egress port ( <PORT_LIST:ports> | all ) queue-id <UINT:qid> apr-index <UINT:index>
bandwidth set egress port ( <PORT_LIST:ports> | all ) queue-id <UINT:qid> share-bandwidth state ( disable | enable )
bandwidth set egress port ( <PORT_LIST:ports> | all ) rate <UINT:rate>
bandwidth get ingress bypass-packet state
bandwidth set ingress bypass-packet state ( disable | enable )
bandwidth get ingress flow-control high-threshold
bandwidth set ingress flow-control high-threshold <UINT:threshold>
bandwidth get ingress flow-control low-threshold
bandwidth set ingress flow-control low-threshold <UINT:threshold>
bandwidth get ingress flow-control port ( <PORT_LIST:ports> | all ) state
bandwidth set ingress flow-control port ( <PORT_LIST:ports> | all ) state ( disable | enable )
bandwidth get ingress ifg port ( <PORT_LIST:ports> | all )
bandwidth set ingress ifg port ( <PORT_LIST:ports> | all ) ( exclude | include )
bandwidth get ingress port ( <PORT_LIST:ports> | all )
bandwidth set ingress port ( <PORT_LIST:ports> | all ) state ( disable | enable )
bandwidth set ingress port ( <PORT_LIST:ports> | all ) rate <UINT:rate>
classf show rule
classf clear
classf init
classf add entry <UINT:index>
classf del entry <UINT:index>
classf del entry all
classf get entry <UINT:index>
classf set rule direction ( upstream | downstream )
classf set rule tos-sid data <UINT:data> mask <UINT:mask>
classf set rule tag-vid data <UINT:data> mask <UINT:mask>
classf set rule tag-priority data <UINT:data> mask <UINT:mask>
classf set rule internal-priority data <UINT:data> mask <UINT:mask>
classf set rule svlan-bit data <UINT:data> mask <UINT:mask>
classf set rule cvlan-bit data <UINT:data> mask <UINT:mask>
classf set rule uni data <UINT:data> mask <UINT:mask>
classf set rule ether-type data <UINT:data> mask <UINT:mask>
classf set rule range-l4port data <UINT:data> mask <UINT:mask>
classf set rule range-ip data <UINT:data> mask <UINT:mask>
classf set rule hit-acl data <UINT:data> mask <UINT:mask>
classf set rule wan-if data <UINT:data> mask <UINT:mask>
classf set rule ipmc-bit data <UINT:data> mask <UINT:mask>
classf set rule ip6mc-bit data <UINT:data> mask <UINT:mask>
classf set rule igmp-bit data <UINT:data> mask <UINT:mask>
classf set rule mld-bit data <UINT:data> mask <UINT:mask>
classf set rule dei-bit data <UINT:data> mask <UINT:mask>
classf set operation entry <UINT:index> ( upstream | downstream ) ( hit | not )
classf set upstream-action svlan-act ( nop | vs-tpid | c-tpid | del | transparent )
classf set upstream-action cvlan-act ( nop | c-tag | c2s | del | transparent )
classf set upstream-action svlan-id-act assign <UINT:vid>
classf set upstream-action svlan-id-act ( copy-outer | copy-inner )
classf set upstream-action svlan-priority-act assign <UINT:priority>
classf set upstream-action svlan-priority-act ( copy-outer | copy-inner | internal-priority )
classf set upstream-action cvlan-id-act assign <UINT:vid>
classf set upstream-action cvlan-id-act ( copy-outer | copy-inner | internal-vid )
classf set upstream-action cvlan-priority-act assign <UINT:priority>
classf set upstream-action cvlan-priority-act ( copy-outer | copy-inner | internal-priority )
classf set upstream-action sid-act ( sid | qid ) <UINT:id>
classf set upstream-action priority-act follow-swcore
classf set upstream-action priority-act assign <UINT:priority>
classf set upstream-action remark-dscp ( enable | disable )
classf set upstream-action drop  ( enable | disable )
classf set upstream-action statistic <UINT:index>
classf set downstream-action svlan-act ( nop | vs-tpid | c-tpid | del | transparent | sp2c )
classf set downstream-action svlan-id-act assign <UINT:vid>
classf set downstream-action svlan-id-act ( copy-outer | copy-inner )
classf set downstream-action svlan-priority-act assign <UINT:priority>
classf set downstream-action svlan-priority-act ( copy-outer | copy-inner | internal-priority )
classf set downstream-action cvlan-act ( nop | c-tag | sp2c | del | transparent )
classf set downstream-action cvlan-id-act ( follow-swcore | copy-outer | copy-inner | lookup-table )
classf set downstream-action cvlan-id-act assign <UINT:cvid>
classf set downstream-action cvlan-priority-act ( follow-swcore | copy-outer | copy-inner | internal-priority )
classf set downstream-action cvlan-priority-act assign <UINT:priority>
classf set downstream-action priority-act follow-swcore
classf set downstream-action priority-act assign <UINT:priority>
classf set downstream-action uni-forward-act ( flood | forced ) port ( <PORT_LIST:ports> | all | none )
classf set downstream-action remark-dscp ( enable | disable )
classf set upstream-unmatch-act ( drop | permit | permit-without-pon )
classf get upstream-unmatch-act
classf set cf-sel-port ( pon | rg ) ( enable | disable )
classf get cf-sel-port
classf get range-ip entry <UINT:index>
classf set range-ip entry <UINT:index> type ( sip | dip )
classf set range-ip entry <UINT:index> low-bound <IPV4ADDR:low_bound_ip> up-bound <IPV4ADDR:up_bound_ip>
classf get range-l4port entry <UINT:index>
classf set range-l4port entry <UINT:index> type ( src-port | dst-port )
classf set range-l4port entry <UINT:index> low-bound <UINT:l4lport> up-bound <UINT:l4uport>
classf set remarking dscp priority <UINT:priority> dscp <UINT:dscp>
classf get remarking dscp
mib init
mib dump counter dot1dTpLearnedEntryDiscards
mib dump counter port ( <PORT_LIST:ports> | all )
mib dump counter port ( <PORT_LIST:ports> | all ) ( dot1dTpPortInDiscards | dot3ControlInUnknownOpcodes | dot3InPauseFrames | dot3OutPauseFrames | dot3StatsDeferredTransmissions | dot3StatsExcessiveCollisions | dot3StatsLateCollisions | dot3StatsMultipleCollisionFrames | dot3StatsSingleCollisionFrames | dot3StatsSymbolErrors )
mib dump counter port ( <PORT_LIST:ports> | all ) ( etherStatsCRCAlignErrors | etherStatsCollisions | etherStatsDropEvents | etherStatsFragments | etherStatsJabbers | etherStatsUndersizeDropPkts )
mib dump counter port ( <PORT_LIST:ports> | all ) ( etherStatsPkts64Octets | etherStatsPkts65to127Octets | etherStatsPkts128to255Octets | etherStatsPkts256to511Octets | etherStatsPkts512to1023Octets | etherStatsPkts1024to1518Octets | etherStatsPkts1519toMaxOctets | etherStatsOversizePkts | etherStatsUndersizePkts ) ( rx | tx )
mib dump counter port ( <PORT_LIST:ports> | all ) ( etherStatsTxBroadcastPkts | etherStatsTxMulticastPkts | inOamPduPkts | outOamPduPkts )
mib dump counter port ( <PORT_LIST:ports> | all ) ( ifInOctets | ifInUcastPkts | ifInMulticastPkts | ifInBroadcastPkts | ifOutOctets | ifOutUcastPkts | ifOutMulticastPkts | ifOutBroadcastPkts | ifOutDiscards )
mib get count-mode
mib set count-mode by-timer latch-time <UINT:timer>
mib set count-mode freerun
mib get ctag-length ( rx-counter | tx-counter )
mib set ctag-length ( rx-counter | tx-counter ) ( exclude | include )
mib get reset-value
mib set reset-value ( 0 | 1 )
mib get sync-mode
mib set sync-mode ( freerun | stop )
mib packet-debug-reason port ( <PORT_LIST:ports> | all )
mib reset counter global
mib reset counter port ( <PORT_LIST:ports> | all )
mib dump log index <UINT:index>
mib reset log index <UINT:index>
mib set log mode index <UINT:index> ( bits-32 | bits-64 )
mib get log mode index <UINT:index> 
mib get log type index <UINT:index> 
mib set log type index <UINT:index> ( byte-count | packet-count )cpu init
cpu set aware-port ( <PORT_LIST:ports> | all | none )
cpu get aware-port
cpu set tag-format ( apollo | normal )
cpu get tag-format
cpu set trap-insert-tag state ( disable | enable )
cpu get trap-insert-tag state
debug batch a <UINT:loop>
debug batch A <UINT:loop>
debug batch w <UINT:reg> <UINT:msb> <UINT:lsb> <HEX:data>
debug batch W <UINT:reg> <UINT:msb> <UINT:lsb> <HEX:data>
debug batch r <UINT:reg> <UINT:msb> <UINT:lsb>
debug batch R <UINT:reg> <UINT:msb> <UINT:lsb>
debug batch phy <UINT:phyid>
debug batch list
debug batch execute { debug }
debug dump ( hsa | hsb | hsd )
debug dump hsd port <UINT:port>
debug dump hsd latest
debug get log
debug set log state ( disable | enable )
debug set log level <UINT:value>
debug set log level-mask <UINT:bitmask>
debug set log level-type ( level | level-mask )
debug set log format ( normal | detail )
debug set log module <UINT64:bitmask>
debug set memory <UINT:address> <UINT:value>
debug get memory <UINT:address> { <UINT:words> }
debug set soc-memory <UINT:address> <UINT:value>
debug get soc-memory <UINT:address> { <UINT:words> }
debug get table <UINT:table_idx> <UINT:address>
debug rtk-init
debug fpga-init
debug set phy <UINT:phy_id> <UINT:reg_address> <UINT:value>
debug get phy <UINT:phy_id> <UINT:reg_address>
debug set register dump ( enable | disable )
debug get version { detail }
debug register-rw-test register <UINT:address> data <UINT:value> count <UINT:count>
debug set ( dbgo_wrap_gphy | dbgo_sys_gen | dbgo_ctrlckt | dbgo_misc | dbg_rrcp_o | dbg_rldp_o )
debug set ( dbg_led_o | dbg_rtct_o | dbg_mib_o | dbg_phy_sts_o | dbg_intrpt_o | dbg_afbk_o | dbg_diag_o | dbgo_hwpkt | dbgo_efuse )
debug set ( dbgo_wrap_sds | dbgo_chip_clk_gen_0 | dbgo_chip_clk_gen_1 | dbgo_chip_clk_gen_2 | dbgo_chip_rst_gen )
debug set ( dbgo_chip_misc | dbgo_cfgif | dbgo_soc | dbgo_ssc | dbgo_pll_root | dbgo_ponctrl | dbgo_clkctrl )
debug set ( dbgo_rstgen | dbgo_clkgen | dbgo_sw_lxslv | PLLTOUT )
debug set dbgo_regctrl 
debug set dbgo_regctrl ( dbgo_swarb | dbgo_gphyarb | dbgo_smimst | dbgo_iicmst | dbgo_iicmst_1 ) bits-3-0 <UINT:bits_3_0>
debug set dbgo_fctrl
debug set dbgo_fctrl ( dbgo_ingress | dbgo_egress ) bits-3-0 <UINT:bits_3_0>
debug set dbgo_swcore_cen
debug set dbgo_swcore_cen ( dbgo_sel_sch | dbgo_sel_outq | dbgo_sel_mtr | dbgo_sel_hsactrl | dbgo_sel_inq | dbgo_sel_out_drp ) bits-7-0 <UINT:bits_7_0>
debug set dbgo_swcore_cen ( dbgo_sel_sch_pon | dbg_dpm_o | dbg_l2_o | dbg_acl_o | dbg_misc_o ) bits-7-0 <UINT:bits_7_0>
debug set dbgo_mac
debug set dbgo_mac ( p0_dbgo_tx | p0_dbgo_rx | p0_dbgo_eee ) bits-7-0 <UINT:bits_7_0>
debug set dbgo_mac ( p1_dbgo_tx | p1_dbgo_rx | p1_dbgo_eee ) bits-7-0 <UINT:bits_7_0>
debug set dbgo_mac ( p2_dbgo_tx | p2_dbgo_rx | p2_dbgo_eee ) bits-7-0 <UINT:bits_7_0>
debug set dbgo_mac ( p3_dbgo_tx | p3_dbgo_rx | p3_dbgo_eee ) bits-7-0 <UINT:bits_7_0>
debug set dbgo_mac ( p4_dbgo_tx | p4_dbgo_rx | p4_dbgo_eee ) bits-7-0 <UINT:bits_7_0>
debug set dbgo_mac ( p5_dbgo_tx | p5_dbgo_rx | p5_dbgo_eee ) bits-7-0 <UINT:bits_7_0>
debug set dbgo_mac ( p6_dbgo_tx | p6_dbgo_rx | p6_dbgo_eee ) bits-7-0 <UINT:bits_7_0>
debug get hsb latch-mode
debug set hsb latch-mode ( all | none | first-drop | first-pass | first-trap | drop | trap | acl )
debug get bond-chip-mode
debug test <UINT:index>
debug packet rx dump { <UINT:byte> }
debug packet rx { enable | disable }
debug packet tx set pkt <UINT:pos> <STRING:data>
debug packet tx set addr <MACADDR:da> <MACADDR:sa>
debug packet tx set l2payload <UINT:pos> <STRING:payload>
debug packet tx set padding <UINT:start> <UINT:end> <UINT:pkt_len>
debug packet tx get preview { <UINT:length> }
debug packet tx clear
debug packet tx send
debug packet tx set cputag l3cs ( enable | disable )
debug packet tx set cputag l4cs ( enable | disable )
debug packet tx set cputag keep ( enable | disable )
debug packet tx set cputag learning ( enable | disable )
debug packet tx set cputag l2br ( enable | disable )
debug packet tx set cputag l34keep ( enable | disable )
debug packet tx set cputag efid ( enable | disable ) <UINT:efid>
debug packet tx set cputag prisel ( enable | disable ) <UINT:priority>
debug packet tx set cputag vsel ( enable | disable )
debug packet tx set cputag txmask_vidx <UINT:value>
debug packet tx set cputag psel ( enable | disable ) <UINT:streamid>
debug packet tx set cputag pppoeact ( intact | addhdr | removehdr | remarking ) <UINT:index>
debug packet tx set cputag extspa <UINT:port>
debug packet tx set cputag clear
debug packet tx get cputag
debug classf get hit-entrydot1x init
dot1x get guest-vlan
dot1x set guest-vlan to-auth-da ( allow | disallow )
dot1x set guest-vlan vid <UINT:vid>
dot1x get mac-based direction
dot1x set mac-based direction ( both | in )
dot1x get mac-based port ( <PORT_LIST:ports> | all ) state
dot1x set mac-based port ( <PORT_LIST:ports> | all ) state ( disable | enable )
dot1x get port-based port ( <PORT_LIST:ports> | all ) 
dot1x set port-based port ( <PORT_LIST:ports> | all ) state ( disable | enable )
dot1x set port-based port ( <PORT_LIST:ports> | all ) ( auth | unauth )
dot1x set port-based port ( <PORT_LIST:ports> | all ) direction ( both | in )
dot1x get trap-priority
dot1x set trap-priority <UINT:priority>
dot1x set unauth-packet port ( <PORT_LIST:ports> | all ) action ( drop | guest-vlan | trap-to-cpu )
dot1x get unauth-packet port ( <PORT_LIST:ports> | all ) action
epon init
epon get bypass-fec state
epon set bypass-fec state ( disable | enable )
epon get llid-table <UINT:index>
epon get llid-table
epon set llid-table <UINT:index> state ( disable | enable )
epon set llid-table <UINT:index> llid <UINT:llid>
epon set llid-table <UINT:index> report-timer <UINT:timer>
epon get mpcp-gate action
epon set mpcp-gate action ( asic-only | trap-and-asic ) 
epon get mpcp-invalid-len action
epon set mpcp-invalid-len action ( drop | pass ) 
 
epon get register mode
 
epon set register mode ( asic | sw ) 
epon get register llid-idx
epon set register llid-idx <UINT:index> 
epon get register state
epon set register state ( disable | enable )
epon get register mac-address
epon set register mac-address <MACADDR:mac> 
epon get register pendding-grant
epon set register pendding-grant <UINT:number> 
flowctrl dump threshold ( switch | pon )
flowctrl dump used-page ( switch | pon )
flowctrl set type ( ingress | egress )
flowctrl get type 
flowctrl set jumbo state ( enable | disable )
flowctrl get jumbo state 
flowctrl set jumbo size ( 3k | 4k | 6k | max )
flowctrl get jumbo size
flowctrl set drop-all threshold <UINT:threshold>
flowctrl get drop-all threshold
flowctrl set pause-all threshold <UINT:threshold>
flowctrl get pause-all threshold
flowctrl set ingress system ( flowctrl-threshold | drop-threshold ) ( high-off | high-on | low-off | low-on ) threshold <UINT:threshold>
flowctrl get ingress system (  flowctrl-threshold | drop-threshold ) ( high-off | high-on | low-off | low-on ) threshold
flowctrl set ingress port ( flowctrl-threshold | drop-threshold ) ( high-off | high-on | low-off | low-on ) threshold <UINT:threshold>
flowctrl get ingress port ( flowctrl-threshold | drop-threshold ) ( high-off | high-on | low-off | low-on ) threshold
flowctrl set ingress jumbo-global ( high-off | high-on | low-off | low-on ) threshold <UINT:threshold>
flowctrl get ingress jumbo-global ( high-off | high-on | low-off | low-on ) threshold
flowctrl set ingress jumbo-port ( high-off | high-on | low-off | low-on ) threshold <UINT:threshold>
flowctrl get ingress jumbo-port ( high-off | high-on | low-off | low-on ) threshold
flowctrl set ingress egress-drop port ( <PORT_LIST:ports> | all ) threshold <UINT:threshold>
flowctrl get ingress egress-drop port ( <PORT_LIST:ports> | all ) threshold
flowctrl set ingress egress-drop queue-id ( <MASK_LIST:qid> | all ) threshold <UINT:threshold>
flowctrl get ingress egress-drop queue-id ( <MASK_LIST:qid> | all ) threshold
flowctrl set ingress egress-drop ( port-gap | queue-gap ) threshold <UINT:threshold>
flowctrl get ingress egress-drop ( port-gap | queue-gap ) threshold
flowctrl set ingress egress-drop port ( <PORT_LIST:ports> | all ) queue-id ( <MASK_LIST:qid> | all ) drop ( enable | disable )
flowctrl get ingress egress-drop port ( <PORT_LIST:ports> | all ) queue-id ( <MASK_LIST:qid> | all ) drop
flowctrl set ingress pon system ( high-off | high-on | low-off | low-on ) threshold <UINT:threshold>
flowctrl get ingress pon system ( high-off | high-on | low-off | low-on ) threshold
flowctrl set ingress pon port ( high-off | high-on | low-off | low-on ) threshold <UINT:threshold>
flowctrl get ingress pon port ( high-off | high-on | low-off | low-on ) threshold
flowctrl set ingress pon egress-drop queue-id ( <MASK_LIST:qid> | all ) threshold-index <UINT:index> 
flowctrl get ingress pon egress-drop queue-id ( <MASK_LIST:qid> | all ) threshold-index
flowctrl set ingress pon egress-drop queue-threshold-index ( <MASK_LIST:index> | all ) threshold <UINT:threshold>
flowctrl get ingress pon egress-drop queue-threshold-index ( <MASK_LIST:idx> | all ) threshold
flowctrl set ingress pon egress-drop queue-gap threshold <UINT:threshold>
flowctrl get ingress pon egress-drop queue-gap threshold
flowctrl set max-page-clear egress-port ( <PORT_LIST:ports> | all )
flowctrl set total-pktcnt-clear
flowctrl set max-page-clear used-page
flowctrl set max-page-clear egress-queue port ( <PORT_LIST:ports> | all ) queue-id ( <MASK_LIST:qid> | all ) 
flowctrl get used-page-cnt ( ingress | egress ) port ( <PORT_LIST:ports> | all )
flowctrl get used-page-cnt egress-queue port ( <PORT_LIST:ports> | all ) queue-id ( <MASK_LIST:qid> | all ) 
flowctrl get total-page-cnt
flowctrl get used-page-cnt ( total | public | public-off | public-jumbo )
flowctrl get used-page-cnt packet ( <PORT_LIST:ports> | all )
flowctrl get used-page-cnt pon queue-id ( <MASK_LIST:qid> | all )
flowctrl set max-page-clear pon queue-id <UINT:qid>
flowctrl set prefetch threshold <UINT:threshold>
flowctrl get prefetch threshold
flowctrl set low-queue threshold <UINT:threshold>
flowctrl get low-queue threshold
flowctrl set egress system ( flowctrl-threshold | drop-threshold ) ( high-off | high-on | low-off | low-on ) threshold <UINT:threshold>
flowctrl get egress system ( flowctrl-threshold | drop-threshold ) ( high-off | high-on | low-off | low-on ) threshold
flowctrl set egress queue-id ( <MASK_LIST:qid> | all ) threshold <UINT:threshold> 
flowctrl get egress queue-id ( <MASK_LIST:qid> | all ) threshold
flowctrl set egress port ( <PORT_LIST:ports> | all ) threshold <UINT:threshold> 
flowctrl get egress port ( <PORT_LIST:ports> | all ) threshold
flowctrl set egress port ( <PORT_LIST:ports> | all ) queue-id ( <MASK_LIST:qid> | all ) queue-drop state ( enable | disable )
flowctrl get egress port ( <PORT_LIST:ports> | all ) queue-id ( <MASK_LIST:qid> | all ) queue-drop state
flowctrl set egress port-gap-threshold <UINT:threshold>
flowctrl get egress port-gap-threshold
flowctrl set egress queue-gap-threshold <UINT:threshold>
flowctrl get egress queue-gap-threshold
flowctrl set high-queue port ( <PORT_LIST:port> | all ) queue-mask ( <PORT_LIST:queue> | all )
flowctrl get high-queue port ( <PORT_LIST:port> | all )
flowctrl set patch ( gpon-35m | fiber-35m | 20m )
gpon set serial-number <STRING:vendor_id> <UINT:serial_number>
gpon get serial-number
gpon set password <STRING:password>
gpon get password
gpon set active-timer to1 <UINT:to1_timer> to2 <UINT:to2_timer>
gpon get active-timer
gpon set ds-laser ( opt-los | cdr-los ) state ( enable | disable ) polarity ( high | low )
gpon set ds-laser los-holdover ( enable | disable )
gpon set ds-phy descramble ( enable | disable )
gpon set ds-phy fec-state ( enable | disable ) fec-threshold <UINT:threshold>
gpon set ds-ploam ( drop-crc-error | filter-onuid | accept-broadcast ) ( enable | disable )
gpon set ds-bwmap ( drop-crc-error | filter-onuid | strict-plen ) ( enable | disable )
gpon set ds-gem assembly-threshold <UINT:threshold>
gpon set ds-eth drop-crc-error ( enable | disable )
gpon set ds-eth pti-pettern <UINT:pettern> pti-mask <UINT:mask>
gpon set ds-omci pti-pettern <UINT:pettern> pti-mask <UINT:mask>
gpon get ( ds-laser | ds-phy | ds-ploam | ds-bwmap | ds-gem | ds-eth | ds-omci )
gpon set us-laser on-offset <UINT:on_offset> off-offset <UINT:off_offset>
gpon set us-phy scramble ( enable | disable )
gpon set us-phy burst-polarity ( high | low )
gpon set us-phy auto-sstart ( enable | disable )
gpon set us-phy suppress-laser ( enable | disable )
gpon set us-ploam state ( enable | disable )
gpon set us-dbr state ( enable | disable )
gpon get ( us-laser | us-phy | us-ploam | us-dbr )
gpon activate init-state ( o1 | o7 )
gpon deactivate
gpon get onu-state
gpon add tcont alloc-id <UINT:id>
gpon del tcont alloc-id <UINT:id>
gpon get tcont alloc-id <UINT:id>
gpon add ds-flow flow-id <UINT:id> gem-port <UINT:gem> ether { multicast } { aes }
gpon add ds-flow flow-id <UINT:id> gem-port <UINT:gem> omci { aes }
gpon del ds-flow flow-id <UINT:id>
gpon get ds-flow flow-id <UINT:id>
gpon add us-flow flow-id <UINT:id> gem-port <UINT:gem> ( ether | omci )
gpon del us-flow flow-id <UINT:id>
gpon get us-flow flow-id <UINT:id>
gpon set multicast-filter ( broadcast-pass | non-multicast-pass ) ( enable | disable )
gpon set multicast-filter ( prefix-ip | prefix-ip6 ) <UINT:prefix>
gpon set multicast-filter ( force-ip | force-ip6 ) ( pass | drop | normal )
gpon set multicast-filter filter-mode ( include | exclude )
gpon get multicast-filter
gpon add multicast-filter-entry mac-address <MACADDR:mac>
gpon del multicast-filter-entry mac-address <MACADDR:mac>
gpon get multicast-filter-entry index <UINT:index>
gpon set rdi ( enable | disable )
gpon get rdi
gpon get alarm-status
gpon set tx-laser ( force-on | force-off | normal )
gpon set tx-force-idle ( enable | disable )
gpon get tx
gpon show ( version | dev-info | gtc | tcont | ds-flow | us-flow | multicast-filter-entry )
gpon show counter global ( active | ds-phy | ds-plm | ds-bw | ds-gem | ds-eth | ds-omci | us-phy | us-dbr | us-plm | us-gem | us-eth | us-omci )
gpon show counter tcont <UINT:tcont_id>
gpon show counter flow <UINT:flow_id>
gpon set test <UINT:data>
gpon get test
gpon initial
gpon deinitial
gpon set debug ( enable | disable )
gpon reg-get page <UINT:page> offset <UINT:offset>
gpon reg-set page <UINT:page> offset <UINT:offset> value <UINT:value>
gpon unit-test <UINT:id>
gpon pkt-gen-buf item <UINT:item> da <MACADDR:dmac> sa <MACADDR:smac> vid <UINT:vid> pattern <UINT:pattern> length <UINT:length>
gpon pkt-gen-cfg item <UINT:item> tcont <UINT:tcont> gem <UINT:gem> tx-length <UINT:length> { omci }
gpon omci-tx <UINT:data1> <UINT:data2> <UINT:data3> <UINT:data4> <UINT:data5> <UINT:data6> <UINT:data7> <UINT:data8> <UINT:data9> <UINT:data10> <UINT:data11> <UINT:data12>
gpon set auto-tcont ( enable | disable )
gpon get auto-tcont
gpon set auto-boh ( enable | disable )
gpon get auto-boh
gpon set eqd-offset ( plus | minus ) <UINT:offset>
gpon get eqd-offset
gpon set aes-framecnt <UINT:framecnt>
gpon get aes-framecnt
gpon set aes-key <UINT:byte0> <UINT:byte1> <UINT:byte2> <UINT:byte3> <UINT:byte4> <UINT:byte5> <UINT:byte6> <UINT:byte7> <UINT:byte8> <UINT:byte9> <UINT:byte10> <UINT:byte11> <UINT:byte12> <UINT:byte13> <UINT:byte14> <UINT:byte15>
igmp get ip-mcast-lookup-mode
igmp set ip-mcast-lookup-mode ( dip-and-sip | dip-only )
igmp set ip-mcast-table index <UINT:index> group-ip <IPV4ADDR:dip> port ( <PORT_LIST:ports> | all | none )
igmp get ip-mcast-table index <UINT:index>
igmp set ( igmpv1 | igmpv2 | igmpv3 | mldv1 | mldv2 ) port ( <PORT_LIST:ports> | all ) action ( drop | forward | trap-to-cpu )
igmp get ( igmpv1 | igmpv2 | igmpv3 | mldv1 | mldv2 ) port ( <PORT_LIST:ports> | all ) action
igmp set igmp-mld ( vlan-leaky | isolation-leaky ) state ( disable | enable )
igmp get igmp-mld ( vlan-leaky | isolation-leaky ) state
igmp set igmp-mld checksum-error action ( drop | trap-to-cpu | forward )
igmp get igmp-mld checksum-error action
iol set max-length state ( disable | enable )
iol get max-length
iol set error-length state ( disable | enable )
iol get error-length
iol set 16-collision state ( disable | enable )
iol get 16-collision
l2-table init
l2-table del all { include-static }
l2-table del ip-mcast dip <IPV4ADDR:dip>
l2-table add ip-mcast dip <IPV4ADDR:dip> port none
l2-table add ip-mcast dip <IPV4ADDR:dip> port ( <PORT_LIST:ports> | all )
l2-table add ip-mcast dip <IPV4ADDR:dip> port ( <PORT_LIST:ports> | all ) l3-interface <UINT:index>
l2-table add ip-mcast dip <IPV4ADDR:dip> priority <UINT:priority>
l2-table add ip-mcast dip <IPV4ADDR:dip> priority state ( disable | enable )
l2-table add ip-mcast dip <IPV4ADDR:dip> ext ( <PORT_LIST:ext> | all | none )
l2-table add ip-mcast dip <IPV4ADDR:dip> l3routing state ( disable | enable )
l2-table add ip-mcast dip <IPV4ADDR:dip> forcedl3routing state ( disable | enable )
l2-table get ip-mcast dip <IPV4ADDR:dip>
l2-table del ip-mcast sip <IPV4ADDR:sip> dip <IPV4ADDR:dip>
l2-table add ip-mcast sip <IPV4ADDR:sip> dip <IPV4ADDR:dip> port ( <PORT_LIST:ports> | all | none )
l2-table add ip-mcast sip <IPV4ADDR:sip> dip <IPV4ADDR:dip> priority <UINT:priority>
l2-table add ip-mcast sip <IPV4ADDR:sip> dip <IPV4ADDR:dip> priority state ( disable | enable )
l2-table add ip-mcast sip <IPV4ADDR:sip> dip <IPV4ADDR:dip> ext ( <PORT_LIST:ext> | all | none )
l2-table get ip-mcast sip <IPV4ADDR:sip> dip <IPV4ADDR:dip>
l2-table del ip-mcast vid <UINT:vid> dip <IPV4ADDR:dip>
l2-table add ip-mcast vid <UINT:vid> dip <IPV4ADDR:dip> port ( <PORT_LIST:ports> | all | none )
l2-table add ip-mcast vid <UINT:vid> dip <IPV4ADDR:dip> priority <UINT:priority>
l2-table add ip-mcast vid <UINT:vid> dip <IPV4ADDR:dip> priority state ( disable | enable )
l2-table add ip-mcast vid <UINT:vid> dip <IPV4ADDR:dip> ext ( <PORT_LIST:ext> | all | none )
l2-table get ip-mcast vid <UINT:vid> dip <IPV4ADDR:dip>
l2-table del mac-mcast filter-id <UINT:fid> mac-address <MACADDR:mac>
l2-table add mac-mcast filter-id <UINT:fid> mac-address <MACADDR:mac> port ( <PORT_LIST:ports> | all | none )
l2-table add mac-mcast filter-id <UINT:fid> mac-address <MACADDR:mac> priority state ( disable | enable )
l2-table add mac-mcast filter-id <UINT:fid> mac-address <MACADDR:mac> priority <UINT:priority>
l2-table add mac-mcast filter-id <UINT:fid> mac-address <MACADDR:mac> ext ( <PORT_LIST:ext> | all | none )
l2-table get mac-mcast filter-id <UINT:fid> mac-address <MACADDR:mac>
l2-table del mac-mcast vid <UINT:vid> mac-address <MACADDR:mac>
l2-table add mac-mcast vid <UINT:vid> mac-address <MACADDR:mac> port ( <PORT_LIST:ports> | all | none )
l2-table add mac-mcast vid <UINT:vid> mac-address <MACADDR:mac> priority state ( disable | enable )
l2-table add mac-mcast vid <UINT:vid> mac-address <MACADDR:mac> priority <UINT:priority>
l2-table add mac-mcast vid <UINT:vid> mac-address <MACADDR:mac> ext ( <PORT_LIST:ext> | all | none )
l2-table get mac-mcast vid <UINT:vid> mac-address <MACADDR:mac>
l2-table del mac-ucast vid <UINT:vid> mac-address <MACADDR:mac>
l2-table add mac-ucast vid <UINT:vid> mac-address <MACADDR:mac> spn <UINT:port>
l2-table add mac-ucast vid <UINT:vid> mac-address <MACADDR:mac> filter-id <UINT:fid>
l2-table add mac-ucast vid <UINT:vid> mac-address <MACADDR:mac> age <UINT:age>
l2-table set mac-ucast enhanced-filter-id <UINT:efid>
l2-table add mac-ucast vid <UINT:vid> mac-address <MACADDR:mac> priority state ( disable | enable )
l2-table add mac-ucast vid <UINT:vid> mac-address <MACADDR:mac> sa-priority state ( disable | enable )
l2-table add mac-ucast vid <UINT:vid> mac-address <MACADDR:mac> priority <UINT:priority>
l2-table add mac-ucast vid <UINT:vid> mac-address <MACADDR:mac> arp-usage state ( disable | enable )
l2-table add mac-ucast vid <UINT:vid> mac-address <MACADDR:mac> auth state ( disable | enable )
l2-table add mac-ucast vid <UINT:vid> mac-address <MACADDR:mac> da-block state ( disable | enable )
l2-table add mac-ucast vid <UINT:vid> mac-address <MACADDR:mac> sa-block state ( disable | enable )
l2-table add mac-ucast vid <UINT:vid> mac-address <MACADDR:mac> static state ( disable | enable )
l2-table add mac-ucast vid <UINT:vid> mac-address <MACADDR:mac> ext-spn <UINT:port>
l2-table del mac-ucast filter-id <UINT:fid> mac-address <MACADDR:mac>
l2-table add mac-ucast filter-id <UINT:fid> mac-address <MACADDR:mac> spn <UINT:port>
l2-table add mac-ucast filter-id <UINT:fid> mac-address <MACADDR:mac> vid <UINT:vid>
l2-table add mac-ucast filter-id <UINT:fid> mac-address <MACADDR:mac> age <UINT:age>
l2-table add mac-ucast filter-id <UINT:fid> mac-address <MACADDR:mac> priority state ( disable | enable )
l2-table add mac-ucast filter-id <UINT:fid> mac-address <MACADDR:mac> sa-priority state ( disable | enable )
l2-table add mac-ucast filter-id <UINT:fid> mac-address <MACADDR:mac> priority <UINT:priority>
l2-table add mac-ucast filter-id <UINT:fid> mac-address <MACADDR:mac> arp-usage state ( disable | enable )
l2-table add mac-ucast filter-id <UINT:fid> mac-address <MACADDR:mac> auth state ( disable | enable )
l2-table add mac-ucast filter-id <UINT:fid> mac-address <MACADDR:mac> da-block state ( disable | enable )
l2-table add mac-ucast filter-id <UINT:fid> mac-address <MACADDR:mac> sa-block state ( disable | enable )
l2-table add mac-ucast filter-id <UINT:fid> mac-address <MACADDR:mac> static state ( disable | enable )
l2-table add mac-ucast filter-id <UINT:fid> mac-address <MACADDR:mac> ext-spn <UINT:port>
l2-table get mac-ucast filter-id <UINT:fid> mac-address <MACADDR:mac> enhanced-filter-id <UINT:efid>
l2-table get mac-ucast filter-id <UINT:fid> mac-address <MACADDR:mac>
l2-table get mac-ucast vid <UINT:vid> mac-address <MACADDR:mac> enhanced-filter-id <UINT:efid>
l2-table get mac-ucast vid <UINT:vid> mac-address <MACADDR:mac>
l2-table get aging-out port ( <PORT_LIST:ports> | all ) state
l2-table set aging-out port ( <PORT_LIST:ports> | all ) state ( disable | enable )
l2-table get aging-time
l2-table set aging-time <UINT:time>
l2-table get cam state
l2-table set cam state ( disable | enable )
l2-table get limit-learning action
l2-table set limit-learning action ( copy-to-cpu | drop | forward | trap-to-cpu )
l2-table get limit-learning count
l2-table set limit-learning count <UINT:count>
l2-table set limit-learning count unlimited
l2-table get limit-learning port ( <PORT_LIST:ports> | all )
l2-table get learning-count
l2-table get learning-count port ( <PORT_LIST:ports> | all )
l2-table set limit-learning port ( <PORT_LIST:ports> | all ) ( copy-to-cpu | drop | forward | trap-to-cpu )
l2-table get limit-learning port ( <PORT_LIST:ports> | all ) count
l2-table set limit-learning port ( <PORT_LIST:ports> | all ) count <UINT:count>
l2-table set limit-learning port ( <PORT_LIST:ports> | all ) count unlimited
l2-table get link-down-flush state
l2-table set link-down-flush state ( disable | enable )
l2-table get lookup-miss multicast trap-priority
l2-table set lookup-miss multicast trap-priority <UINT:priority>
l2-table get lookup-miss ( broadcast | unicast | multicast ) flood-ports
l2-table set lookup-miss ( broadcast | unicast | multicast ) flood-ports ( <PORT_LIST:ports> | all )
l2-table get lookup-miss port ( <PORT_LIST:ports> | all ) ( multicast | ip-mcast | ip6-mcast | unicast ) action
l2-table set lookup-miss port ( <PORT_LIST:ports> | all ) ( ip-mcast | ip6-mcast ) action ( drop | flood-in-vlan | trap-to-cpu )
l2-table set lookup-miss port ( <PORT_LIST:ports> | all ) multicast action ( drop | drop-exclude-rma | flood-in-vlan | trap-to-cpu )
l2-table set lookup-miss port ( <PORT_LIST:ports> | all ) unicast action ( drop | flood-in-vlan | trap-to-cpu )
l2-table set ip-mcast-mode ( dip-and-sip | dip-and-vid | vid-and-mac )
l2-table get ip-mcast-mode
l2-table get ( port-move | unknown-sa ) port ( <PORT_LIST:ports> | all ) action
l2-table set ( port-move | unknown-sa ) port ( <PORT_LIST:ports> | all ) action ( copy-to-cpu | drop | forward | trap-to-cpu )
l2-table set flush mac-ucast
l2-table set flush mac-ucast include-static
l2-table set flush mac-ucast static-only
l2-table set flush mac-ucast port ( <PORT_LIST:ports> | all ) filter-id <UINT:fid>
l2-table set flush mac-ucast port ( <PORT_LIST:ports> | all ) filter-id <UINT:fid> include-static
l2-table set flush mac-ucast port ( <PORT_LIST:ports> | all ) filter-id <UINT:fid> static-only
l2-table set flush mac-ucast port ( <PORT_LIST:ports> | all ) vid <UINT:vid>
l2-table set flush mac-ucast port ( <PORT_LIST:ports> | all ) vid <UINT:vid> include-static
l2-table set flush mac-ucast port ( <PORT_LIST:ports> | all ) vid <UINT:vid> static-only
l2-table set flush mac-ucast port ( <PORT_LIST:ports> | all )
l2-table set flush mac-ucast port ( <PORT_LIST:ports> | all ) include-static
l2-table set flush mac-ucast port ( <PORT_LIST:ports> | all ) static-only
l2-table set ip-mcast-data port ( <PORT_LIST:ports> | all ) action ( forward | drop )
l2-table get ip-mcast-data port ( <PORT_LIST:ports> | all ) action
l2-table get entry address <UINT:address>
l2-table get next-entry address <UINT:address>
l2-table get next-entry mac-ucast address <UINT:address>
l2-table get next-entry mac-ucast address <UINT:address> spn <UINT:port>
l2-table get next-entry l2-mcast address <UINT:address>
l2-table get next-entry ip-mcast address <UINT:address>
l2-table get next-entry l2-ip-mcast address <UINT:address>
l2-table get src-port-egress-filter port ( <PORT_LIST:ports> | all ) state
l2-table set src-port-egress-filter port ( <PORT_LIST:ports> | all ) state ( disable | enable )
l34 reset table ( l3 | pppoe | nexthop | interface | external-ip | arp | naptr | napt )
l34 get arp
l34 get arp <UINT:index>
l34 set arp <UINT:index> next-hop-table <UINT:nh_index>
l34 del arp <UINT:index>
l34 get external-ip 
l34 get external-ip <UINT:index>
l34 set external-ip <UINT:index> type ( nat | napt | lp )
l34 set external-ip <UINT:index> external-ip <IPV4ADDR:ip>
l34 set external-ip <UINT:index> internal-ip <IPV4ADDR:ip>
l34 set external-ip <UINT:index> next-hop-table <UINT:nh_index>
l34 set external-ip <UINT:index> nat-priority state ( disable | enable )
l34 set external-ip <UINT:index> nat-priority <UINT:priority>
l34 set external-ip <UINT:index> state ( disable | enable )
l34 del external-ip <UINT:index>
l34 get routing 
l34 get routing <UINT:index>
l34 set routing <UINT:index> ip <IPV4ADDR:ip> ip-mask <UINT:mask>
l34 set routing <UINT:index> interface-type ( internal | external )
l34 set routing <UINT:index> type ( drop | trap )
l34 set routing <UINT:index> type local-route destination-netif <UINT:netif_index>
l34 set routing <UINT:index> type local-route arp-start-address <UINT:start_addr> arp-end-address <UINT:end_addr>
l34 set routing <UINT:index> type global-route next-hop-table <UINT:nh_index>
l34 set routing <UINT:index> type global-route next-hop-start <UINT:address> next-hop-number <UINT:nh_number>
l34 set routing <UINT:index> type global-route next-hop-algo ( per-packet | per-session | per-source_ip )
l34 set routing <UINT:index> type global-route ip-domain-range <UINT:range>
l34 set routing <UINT:index> state ( disable | enable )
l34 del routing <UINT:index>
l34 get netif 
l34 get netif <UINT:index>
l34 set netif <UINT:index> gateway-mac <MACADDR:mac> mac-mask ( no-mask | 1bit-mask | 2bit-mask | 3bit-mask )
l34 set netif <UINT:index> vid <UINT:vid>
l34 set netif <UINT:index> mtu <UINT:mtu>
l34 set netif <UINT:index> state ( disable | enable )
l34 set netif <UINT:index> l3-route state ( enable | disable )
l34 del netif <UINT:index>
l34 get nexthop 
l34 get nexthop <UINT:index>
l34 set nexthop <UINT:index> netif <UINT:netif_index>
l34 set nexthop <UINT:index> l2 <UINT:l2_index>
l34 set nexthop <UINT:index> type ( ethernet | pppoe )
l34 set nexthop <UINT:index> pppoe <UINT:pppoe_index>
l34 get pppoe 
l34 get pppoe <UINT:index>
l34 set pppoe <UINT:index> session-id <UINT:session_id>
l34 get napt 
l34 get napt <UINT:index>
l34 set napt <UINT:index> hash-index <UINT:hash_index>
l34 set napt <UINT:index> napt-priority state ( disable | enable )  
l34 set napt <UINT:index> napt-priority <UINT:priority>
l34 set napt <UINT:index> state ( disable | enable )
l34 del napt <UINT:index>
l34 get naptr 
l34 get naptr <UINT:index>
l34 set naptr <UINT:index> internal-ip <IPV4ADDR:ip> internal-port <UINT:port> 
l34 set naptr <UINT:index> protocol ( tcp | udp )
l34 set naptr <UINT:index> external-ip <UINT:extip_index> external-port-lsb <UINT:export_lsb>
l34 set naptr <UINT:index> naptr-priority state ( disable | enable )  
l34 set naptr <UINT:index> naptr-priority <UINT:priority>
l34 set naptr <UINT:index> remote-hash-type ( none | remote_ip | remote_ip_remote_port )
l34 set naptr <UINT:index> hash-value <UINT:value>
l34 set naptr <UINT:index> state disable
l34 del naptr <UINT:index>
l34 set port ( <PORT_LIST:ports> | all ) netif <UINT:index>
l34 get port ( <PORT_LIST:ports> | all )
 
l34 set ext ( <PORT_LIST:ext> | all ) netif <UINT:index>
l34 get ext ( <PORT_LIST:ext> | all )
l34 set l4-fragment action ( trap-to-cpu | forward )
l34 get l4-fragment
l34 set l3-checksum-error action ( forward | drop )
l34 get l3-checksum-error
l34 set l4-checksum-error action ( forward | drop )
l34 get l4-checksum-error
l34 set ttl-minus state ( enable | disable )
l34 get ttl-minus state
l34 set interface-decision-mode ( vlan-based | port-based | mac-based )
l34 get interface-decision-mode
l34 set nat-attack action ( drop | trap-to-cpu )
l34 get nat-attack
l34 set wan-route action ( drop | trap-to-cpu | forward )
l34 get wan-route
l34 set route-mode ( l3 | l3-l4 | disable )
l34 get route-mode
l34 get pppoe-traffic-indicator
l34 get arp-traffic-indicator index <UINT:index>
l34 get arp-traffic-indicator
l34 reset arp-traffic-indicator ( table0 | table1 )
l34 select arp-traffic-indicator ( table0 | table1 )
l34 get l4-traffic-indicator index <UINT:index>
l34 get l4-traffic-indicator
l34 reset l4-traffic-indicator ( table0 | table1 )
l34 select l4-traffic-indicator ( table0 | table1 )
debug l34 set hsb l2-bridge <UINT:l2bridge> 
debug l34 set hsb ip-fragments <UINT:is_fragments> 
debug l34 set hsb ip-more-fragments <UINT:is_more> 
debug l34 set hsb l4-checksum-ok <UINT:is_ok> 
debug l34 set hsb l3-checksum-ok <UINT:is_ok> 
debug l34 set hsb direct-tx <UINT:is_direct_tx> 
debug l34 set hsb udp-no-chksum <UINT:udp_no_chk> 
debug l34 set hsb parse-fail <UINT:parse_fail> 
debug l34 set hsb pppoe-if <UINT:pppoe_if> 
debug l34 set hsb svlan-if <UINT:svlan_if> 
debug l34 set hsb ttls <UINT:ttls> 
debug l34 set hsb pkt-type <UINT:pkt_type> 
debug l34 set hsb tcp-flag <UINT:tcp_flag> 
debug l34 set hsb cvlan-if <UINT:cvlan_if> 
debug l34 set hsb source-port <UINT:spa> 
debug l34 set hsb cvid <UINT:cvid> 
debug l34 set hsb packet-length <UINT:length> 
debug l34 set hsb dport <UINT:dport> 
debug l34 set hsb pppoe-id <UINT:pppoe> 
debug l34 set hsb dip <IPV4ADDR:ip> 
debug l34 set hsb sip <IPV4ADDR:ip> 
debug l34 set hsb sport <UINT:sport> 
debug l34 set hsb dmac <MACADDR:mac>
debug l34 get hsb
 
debug l34 get hsa
debug l34 set hsba log-mode <UINT:mode>
debug l34 get hsba log-mode 
meter init
meter get entry <MASK_LIST:index>
meter get entry <MASK_LIST:index> burst-size
meter set entry <MASK_LIST:index> burst-size <UINT:size>
meter get entry <MASK_LIST:index> ifg
meter set entry <MASK_LIST:index> ifg ( exclude | include )
meter get entry <MASK_LIST:index> meter-exceed
meter reset entry <MASK_LIST:index> meter-exceed
meter get tick-token
meter set tick-token tick-period <UINT:period> token <UINT:token>
meter get pon-tick-token
meter set pon-tick-token tick-period <UINT:period> token <UINT:token>
meter get entry <MASK_LIST:index> rate
meter set entry <MASK_LIST:index> rate <UINT:rate>
mirror init
mirror dump
mirror set egress-mode ( all-pkt | mirrored-only )
mirror get egress-mode
mirror set mirroring-port <UINT:port> mirrored-port ( <PORT_LIST:ports> | none ) { rx-mirror } { tx-mirror }
oam init
oam dump
oam get state
oam set state ( disable | enable ) 
oam get multiplexer port ( <PORT_LIST:ports> | all )
oam set multiplexer port ( <PORT_LIST:ports> | all ) action ( forward | discard | from-cpu-only )
oam get parser port ( <PORT_LIST:port> | all )
oam set parser port ( <PORT_LIST:port> | all ) action ( forward | loopback | discard )
oam set trap-priority <UINT:priority>
oam get trap-priority
pon init 
pon clear 
pon set drain-out t-cont <UINT:tcont> queue-id <MASK_LIST:qid>
pon get drain-out status
pon get stream <MASK_LIST:sid>
pon set stream <MASK_LIST:sid> t-cont <UINT:tcont> queue-id <UINT:qid>
pon set stream <MASK_LIST:sid> llid <UINT:llid> queue-id <UINT:qid>
pon get t-cont <UINT:tcont> queue-id <UINT:qid>
pon add t-cont <UINT:tcont> queue-id <UINT:qid>
pon del t-cont <UINT:tcont> queue-id <UINT:qid>
pon set t-cont <UINT:tcont> queue-id <UINT:qid> pir rate <UINT:rate> 
pon set t-cont <UINT:tcont> queue-id <UINT:qid> cir rate <UINT:rate> 
pon set t-cont <UINT:tcont> queue-id <UINT:qid> scheduling type ( strict | wfq ) 
pon set t-cont <UINT:tcont> queue-id <UINT:qid> scheduling weight <UINT:weight>
pon set t-cont <UINT:tcont> queue-id <UINT:qid> egress-drop state ( enable | disable ) 
pon get llid <UINT:llid> queue-id <UINT:qid>
pon add llid <UINT:llid> queue-id <UINT:qid>
pon del llid <UINT:llid> queue-id <UINT:qid>
pon set llid <UINT:llid> queue-id <UINT:qid> pir rate <UINT:rate> 
pon set llid <UINT:llid> queue-id <UINT:qid> cir rate <UINT:rate> 
pon set llid <UINT:llid> queue-id <UINT:qid> scheduling type ( strict | wfq ) 
pon set llid <UINT:llid> queue-id <UINT:qid> scheduling weight <UINT:weight>
pon set llid <UINT:llid> queue-id <UINT:qid> egress-drop state ( enable | disable ) 
port init
port dump isolation
port get auto-nego port ( <PORT_LIST:ports> | all ) ability
port get auto-nego port ( <PORT_LIST:ports> | all ) state
port get status port ( <PORT_LIST:ports> | all )
port get enhanced-fid port ( <PORT_LIST:ports> | all )
port set enhanced-fid port ( <PORT_LIST:ports> | all ) enhanced-fid <UINT:efid>
port get force-dmp port ( <PORT_LIST:ports> | all )
port get force-dmp
port set force-dmp port ( <PORT_LIST:ports> | all ) port-mask ( <PORT_LIST:port_mask> | all )
port set force-dmp state ( disable | enable )
port get isolation ext <PORT_LIST:ext>
port get isolation ext-l34 <PORT_LIST:ext>
port get isolation port ( <PORT_LIST:ports> | all )
port get isolation port-l34 ( <PORT_LIST:ports> | all )
port set isolation ext <PORT_LIST:ext> vlan-index <UINT:vidx>
port set isolation ext-l34 <PORT_LIST:ext> vlan-index <UINT:vidx>
port set isolation port ( <PORT_LIST:ports> | all ) vlan-index <UINT:vidx>
port set isolation port-l34 ( <PORT_LIST:ports> | all ) vlan-index <UINT:vidx>
port set isolation port ( <PORT_LIST:ports> | all ) ( mode0 | mode1 ) egress-port ( <PORT_LIST:egressports> | none )
port set isolation port ( <PORT_LIST:ports> | all ) ( mode0 | mode1 ) egress-ext ( <PORT_LIST:egress_ext> | none )
port set isolation port ( <PORT_LIST:ports> | all ) ( mode0 | mode1 ) cpu ( exclude | include )
port get isolation port ( <PORT_LIST:ports> | all ) ( mode0 | mode1 )
port set isolation ext <PORT_LIST:ext> ( mode0 | mode1 ) egress-port ( <PORT_LIST:egressports> | none )
port set isolation ext <PORT_LIST:ext> ( mode0 | mode1 ) egress-ext ( <PORT_LIST:egress_ext> | none )
port set isolation ext <PORT_LIST:ext> ( mode0 | mode1 ) cpu ( exclude | include )
port get isolation ext <PORT_LIST:ext> ( mode0 | mode1 )
port set isolation ctag ( mode0 | mode1 )
port get isolation ctag
port set isolation l34 ( mode0 | mode1 )
port get isolation l34
port get master-slave port ( <PORT_LIST:ports> | all )
port set master-slave port ( <PORT_LIST:ports> | all ) ( auto | master | slave )
port get phy-reg port ( <PORT_LIST:ports> | all ) page <UINT:page> register <UINT:register>
port set phy-reg port ( <PORT_LIST:ports> | all ) page <UINT:page> register <UINT:register> data <UINT:data>
port set auto-nego port ( <PORT_LIST:ports> | all ) ability { 10h } { 10f } { 100h } { 100f } { 1000f } { flow-control } { asy-flow-control }
port set auto-nego port ( <PORT_LIST:ports> | all ) state ( disable | enable )
port get mac-force port ( <PORT_LIST:ports> | all )
port set mac-force port ( <PORT_LIST:ports> | all ) ability ( 10h | 10f | 100h | 100f | 1000f ) flow-control ( disable | enable )
port get rtct ( <PORT_LIST:ports> | all )
port set rtct ( <PORT_LIST:ports> | all ) start
port set mac-force port ( <PORT_LIST:ports> | all ) ( lpi-100M | lpi-giga ) state ( disable | enable )
port set mac-force port ( <PORT_LIST:ports> | all ) link-state ( link-down | link-up )
port set isolation leaky ip-mcast port ( <PORT_LIST:ports> | all ) state ( enable | disable )
port get isolation leaky ip-mcast port ( <PORT_LIST:ports> | all ) state
port get special-congest ( <PORT_LIST:ports> | all )
port set special-congest ( <PORT_LIST:ports> | all ) sustain-timer <UINT:second>
port get special-congest ( <PORT_LIST:ports> | all ) indicator
port clear special-congest ( <PORT_LIST:ports> | all ) indicator
port set port ( <PORT_LIST:ports> | all ) state ( disable | enable )
port get port ( <PORT_LIST:ports> | all ) state
 
qos init 
 
qos get priority-selector
qos set priority-selector port <UINT:weight> 
qos set priority-selector dot1q <UINT:weight> 
qos set priority-selector dscp <UINT:weight> 
qos set priority-selector acl <UINT:weight> 
qos set priority-selector lookup-table <UINT:weight> 
qos set priority-selector smac <UINT:weight> 
qos set priority-selector svlan <UINT:weight> 
qos set priority-selector vlan <UINT:weight> 
qos set priority-selector l4 <UINT:weight> 
qos get priority-to-queue port ( <PORT_LIST:ports> | all )
qos set priority-to-queue port ( <PORT_LIST:ports> | all ) table <UINT:index>
qos get priority-to-queue 
qos get priority-to-queue table <UINT:index>
qos set priority-to-queue table <UINT:index> priority <MASK_LIST:priority> queue-id <UINT:qid>
qos get remarking port ( <PORT_LIST:ports> | all ) dscp source
qos set remarking port ( <PORT_LIST:ports> | all ) dscp source ( internal-priority | dscp )
qos get remarking dscp source ( internal-priority | dscp )
qos set remarking dscp inter-priority <UINT:priority> dscp <UINT:dscp>
qos set remarking dscp original-dscp <MASK_LIST:dscp> remarking-dscp <UINT:remarking_dscp>
qos get remapping dot1p
qos set remapping dot1p dot1p-priority <UINT:dot1p_priority> internal-priority <UINT:internal_priority>
qos get remapping dscp
qos set remapping dscp dscp <MASK_LIST:dscp> internal-priority <UINT:priority>
qos get avb remapping
qos get avb remapping internal-priority <UINT:priority>
qos set avb remapping internal-priority <UINT:priority> user-priority <UINT:user_priority>
qos get avb remapping port ( <PORT_LIST:ports> | all ) state
qos set avb remapping port ( <PORT_LIST:ports> | all ) state ( enable | disable )
qos get remapping forward-to-cpu
qos get remapping forward-to-cpu internal-priority <UINT:priority>
qos set remapping forward-to-cpu internal-priority <UINT:priority> remapping-priority <UINT:remapping_priority>
qos get remapping port ( <PORT_LIST:ports> | all )
qos set remapping port ( <PORT_LIST:ports> | all ) internal-priority <UINT:priority>
qos get remarking ( dot1p | dscp ) port ( <PORT_LIST:ports> | all ) state
qos set remarking ( dot1p | dscp ) port ( <PORT_LIST:ports> | all ) state ( disable | enable ) 
qos get remarking dot1p 
qos set remarking dot1p inter-priority <UINT:priority> dot1p-priority <UINT:dot1p_priority>
qos get scheduling algorithm port ( <PORT_LIST:ports> | all ) queue-id <UINT:qid>
qos set scheduling algorithm port ( <PORT_LIST:ports> | all ) queue-id <UINT:qid> ( strict | wfq )
qos get scheduling queue-weight port ( <PORT_LIST:ports> | all ) queue-id <UINT:qid>
qos set scheduling queue-weight port ( <PORT_LIST:ports> | all ) queue-id <UINT:qid> weight <UINT:weight>
register set <UINT:address> <UINT:value>
register get <UINT:address> { <UINT:words> }
register get all
rldp init
rldp set state ( disable | enable )
rldp get state
rldp set bypass-flow-control state ( disable | enable )
rldp get bypass-flow-control state
rldp set mode ( sa-moving | periodic )
rldp get mode
rldp set magic <MACADDR:mac>
rldp get magic
rldp get identifier
rldp set compare-type ( magic-and-identifier | magic-only )
rldp get compare-type
rldp set handle ( hardware | software )
rldp get handle
rldp set re-generate-identifier
rldp set ( check | loop ) period <UINT:time>
rldp get ( check | loop ) period
rldp set ( check | loop ) number <UINT:count>
rldp get ( check | loop ) number
rldp set port ( <PORT_LIST:port> | all ) state ( disable | enable )
rldp get port ( <PORT_LIST:port> | all ) state
rldp clear port ( <PORT_LIST:port> | all ) status ( entering | leaving )
rldp get port ( <PORT_LIST:port> | all ) status
rldp set port ( <PORT_LIST:port> | all ) control-state ( none-looping | looping )
rldp get port ( <PORT_LIST:port> | all ) control-state
rldp get port ( <PORT_LIST:port> | all ) looped-port-id
rlpp set trap state ( disable | enable )
rlpp get trap state
rma dump
rma set priority <UINT:priority>
rma get priority
rma set address <UINT:rma_tail> action ( drop | forward | forward-exclude-cpu | trap-to-cpu )
rma get address <UINT:rma_tail> action
rma set address <UINT:rma_tail> ( vlan-leaky | isolation-leaky | keep-vlan-format | bypass-storm-control )  state ( disable | enable )
rma get address <UINT:rma_tail> ( vlan-leaky | isolation-leaky | keep-vlan-format | bypass-storm-control ) state
sdk test group <STRING:item>
sdk test case-id <UINT:start> { <UINT:end> }
security init
security set attack-prevent port ( <PORT_LIST:port> | all ) state ( disable | enable )
security get attack-prevent port ( <PORT_LIST:port> | all ) state
security get attack-prevent ( daeqsa-deny | land-deny | blat-deny | synfin-deny | xma-deny | nullscan-deny | tcphdr-min-check | syn-sportl1024-deny ) action
security set attack-prevent ( daeqsa-deny | land-deny | blat-deny | synfin-deny | xma-deny | nullscan-deny | tcphdr-min-check | syn-sportl1024-deny ) action ( forward | drop | trap-to-cpu )
security get attack-prevent ( syn-flood | fin-flood | icmp-flood ) action
security set attack-prevent ( syn-flood | fin-flood | icmp-flood ) action ( forward | drop | trap-to-cpu )
security get attack-prevent ( syn-flood | fin-flood | icmp-flood ) threshold
security set attack-prevent ( syn-flood | fin-flood | icmp-flood ) threshold <UINT:threshold>
security get attack-prevent ( tcp-frag-off-min-check | icmp-frag-pkts-deny | pod-deny | udp-bomb | syn-with-data ) action
security set attack-prevent ( tcp-frag-off-min-check | icmp-frag-pkts-deny | pod-deny | udp-bomb | syn-with-data ) action ( forward | drop | trap-to-cpu )
storm-control get ( broadcast | multicast | unknown-multicast | unknown-unicast ) alternated
storm-control set ( broadcast | multicast | unknown-multicast | unknown-unicast ) alternated ( disabled | arp-storm | dhcp-storm | igmp-mld-storm )
storm-control get ( broadcast | multicast | unknown-multicast | unknown-unicast | arp-storm | dhcp-storm | igmp-mld-storm )
storm-control set ( broadcast | multicast | unknown-multicast | unknown-unicast | arp-storm | dhcp-storm | igmp-mld-storm ) state ( disable | enable )
storm-control get ( broadcast | multicast | unknown-multicast | unknown-unicast | arp-storm | dhcp-storm | igmp-mld-storm ) port ( <PORT_LIST:ports> | all )
storm-control set ( broadcast | multicast | unknown-multicast | unknown-unicast | arp-storm | dhcp-storm | igmp-mld-storm ) port ( <PORT_LIST:ports> | all ) state ( disable | enable )
storm-control set ( broadcast | multicast | unknown-multicast | unknown-unicast | arp-storm | dhcp-storm | igmp-mld-storm ) port ( <PORT_LIST:ports> | all ) meter <UINT:index>
storm-control get bypass-packet ( igmp | cdp | csstp ) state
storm-control set bypass-packet ( igmp | cdp | csstp ) state ( disable | enable )
storm-control get bypass-packet rma <UINT:rma_tail> state
storm-control set bypass-packet rma <UINT:rma_tail> state  ( disable | enable )
stp init
stp get stp-table instance <UINT:instance> port ( <PORT_LIST:ports> | all ) state 
stp set stp-table instance <UINT:instance> port ( <PORT_LIST:ports> | all ) state ( blocking | disable | forwarding | learning )
svlan init
svlan create svid <UINT:svid>
svlan destroy all
svlan destroy svid <UINT:svid>
svlan dump
svlan dump svid <UINT:svid>
svlan set entry <UINT:index> svid <UINT:svid>
svlan set entry <UINT:index> member ( <PORT_LIST:ports> | all | none ) 
svlan set entry <UINT:index> ( tag-member | untag-member ) ( <PORT_LIST:ports> | all | none )
svlan set entry <UINT:index> priority <UINT:priority>
svlan set entry <UINT:index> fid state ( disable | enable )
svlan set entry <UINT:index> fid <UINT:fid>
svlan set entry <UINT:index> enhanced-fid state ( disable | enable )
svlan set entry <UINT:index> enhanced-fid <UINT:efid>
svlan get entry <UINT:index>
svlan get entry all
svlan get vlan-conversion c2s entry <UINT:index> 
svlan get vlan-conversion c2s entry all
svlan set vlan-conversion c2s entry <UINT:index> enhanced-vid <UINT:evid>
svlan set vlan-conversion c2s entry <UINT:index> member ( <PORT_LIST:ports> | all | none )
svlan set vlan-conversion c2s entry <UINT:index> svlan-index <UINT:svidx>
svlan del vlan-conversion c2s vid <UINT:vid> port ( <PORT_LIST:ports> | all ) svid <UINT:svid>
svlan add vlan-conversion c2s vid <UINT:vid> port ( <PORT_LIST:ports> | all ) svid <UINT:svid>
svlan get vlan-conversion c2s vid <UINT:vid> port ( <PORT_LIST:ports> | all ) 
svlan get vlan-conversion mc2s entry <UINT:index>
svlan get vlan-conversion mc2s entry all
svlan set vlan-conversion mc2s entry <UINT:index> state ( invalid | valid )
svlan set vlan-conversion mc2s entry <UINT:index> format ( dip | dmac )
svlan set vlan-conversion mc2s entry <UINT:index> ip <IPV4ADDR:ip> ip-mask <IPV4ADDR:ip_mask>
svlan set vlan-conversion mc2s entry <UINT:index> mac-address <MACADDR:mac> mac-mask <MACADDR:mac_mask>
svlan set vlan-conversion mc2s entry <UINT:index> svlan-index <UINT:svidx>
svlan add vlan-conversion mc2s ip <IPV4ADDR:ip> ip-mask <IPV4ADDR:ip_mask> svid <UINT:svid>
svlan del vlan-conversion mc2s ip <IPV4ADDR:ip> ip-mask <IPV4ADDR:ip_mask>
svlan get vlan-conversion mc2s ip <IPV4ADDR:ip> ip-mask <IPV4ADDR:ip_mask>
svlan add vlan-conversion mc2s mac-address <MACADDR:mac> mac-mask <MACADDR:mac_mask> svid <UINT:svid>
svlan del vlan-conversion mc2s mac-address <MACADDR:mac> mac-mask <MACADDR:mac_mask>
svlan get vlan-conversion mc2s mac-address <MACADDR:mac> mac-mask <MACADDR:mac_mask>
svlan get port ( <PORT_LIST:ports> | all ) svid
svlan get port ( <PORT_LIST:ports> | all ) svlan-index
svlan set port ( <PORT_LIST:ports> | all ) svid <UINT:svid>
svlan set port ( <PORT_LIST:ports> | all ) svlan-index <UINT:svidx>
svlan get priority-source 
svlan set priority-source ( internal-priority | dot1q-priority | svlan-member-config | port-based-priority ) 
svlan get service-port
svlan set service-port ( <PORT_LIST:ports> | all | none )
svlan get vlan-conversion sp2c entry <UINT:index> 
svlan get vlan-conversion sp2c entry all
svlan set vlan-conversion sp2c entry <UINT:index> state ( invalid | valid )
svlan set vlan-conversion sp2c entry <UINT:index> svlan-index <UINT:svidx>
svlan set vlan-conversion sp2c entry <UINT:index> port <UINT:port>
svlan set vlan-conversion sp2c entry <UINT:index> vid <UINT:vid>
svlan add vlan-conversion sp2c svid <UINT:svid> port <UINT:port> vid <UINT:vid>
svlan del vlan-conversion sp2c svid <UINT:svid> port <UINT:port>
svlan get vlan-conversion sp2c svid <UINT:svid> port <UINT:port>
svlan get svlan-table svid <UINT:svid>
svlan set svlan-table svid <UINT:svid> member ( <PORT_LIST:ports> | all | none )
svlan set svlan-table svid <UINT:svid> tag-member ( <PORT_LIST:ports> | all | none )
svlan set svlan-table svid <UINT:svid> untag-member ( <PORT_LIST:ports> | all | none )
svlan get tpid
svlan set tpid <UINT:tpid>
svlan get trap-priority
svlan set trap-priority <UINT:priority>
svlan get unmatch
svlan set unmatch ( drop | trap-to-cpu ) 
svlan set unmatch ( assign-svlan | assign-and-keep ) svid <UINT:svid> 
svlan get untag
svlan set untag ( drop | trap-to-cpu | using-cvid ) 
svlan set untag assign-svlan svid <UINT:svid>
svlan get vlan-aggregation port ( <PORT_LIST:ports> | all ) state 
svlan set vlan-aggregation port ( <PORT_LIST:ports> | all ) state ( disable | enable ) 
svlan set dei-keep state ( disable | enable ) 
svlan get dei-keep state 
svlan set lookup-type ( svlan-table | vlan-4k-table ) 
svlan get lookup-type 
svlan get vlan-conversion sp2c unmatch-action
svlan set vlan-conversion sp2c unmatch-action ( untag | ctag ) 
switch init
switch get 48-pass-1 state
switch set 48-pass-1 state ( disable | enable )
switch set ipg-compensation state ( disable | enable )
switch set ipg-compensation ( 65ppm | 90ppm )
switch get ipg-compensation state
switch get ipg-compensation
switch set bypass-tx-crc state ( disable | enable )
switch get bypass-tx-crc state
switch set rx-check-crc port ( <PORT_LIST:ports> | all ) state ( disable | enable )
switch get rx-check-crc port ( <PORT_LIST:ports> | all ) state
switch set mac-address <MACADDR:mac>
switch get mac-address
switch set max-pkt-len ( fe | ge ) port ( <PORT_LIST:ports> | all ) index <UINT:index>
switch get max-pkt-len ( fe | ge ) port ( <PORT_LIST:ports> | all )
switch set max-pkt-len index <UINT:index> length <UINT:len>
switch get max-pkt-len index <UINT:index>
switch set limit-pause state ( disable | enable )
switch get limit-pause state
switch set small-ipg-tag port ( <PORT_LIST:ports> | all ) state ( enable | disable )
switch get small-ipg-tag port ( <PORT_LIST:ports> | all ) state
switch get back-pressure
switch set back-pressure ( jam | defer )
switch set small-pkt port ( <PORT_LIST:ports> | all ) state ( enable | disable )
switch get small-pkt port ( <PORT_LIST:ports> | all ) state
switch reset ( global | chip ) 
switch set output-drop port ( <PORT_LIST:ports> | all ) state ( disable | enable )
switch get output-drop port ( <PORT_LIST:ports> | all ) state 
switch set output-drop ( broadcast | unknown-unicast | multicast ) state ( disable | enable )
switch get output-drop ( broadcast | unknown-unicast | multicast ) state
trap init
trap set ( cdp | csstp ) action ( drop | forward | forward-exclude-cpu | trap-to-cpu )
trap get ( cdp | csstp ) action
trap set ( cdp | csstp ) ( vlan-leaky | isolation-leaky | keep-vlan-format | bypass-storm-control )  state ( disable | enable )
trap get ( cdp | csstp ) ( vlan-leaky | isolation-leaky | keep-vlan-format | bypass-storm-control )
trunk init
trunk dump
trunk get distribute-algorithm
trunk set distribute-algorithm { dst-ip } { dst-l4-port } { dst-mac } { src-ip } { src-l4-port } { src-mac } { src-port }
trunk get queue-empty
trunk get flow-control state
trunk set flow-control state ( disable | enable )
trunk get hash-mapping hash-value all
trunk set hash-mapping hash-value <UINT:value> port <UINT:port>
trunk set member-port ( <PORT_LIST:ports> | none )
trunk get member-port
trunk set mode ( dumb | normal )
trunk get mode
trunk get traffic-separation flooding state
trunk set traffic-separation flooding state ( disable | enable )vlan init
vlan create vid <UINT:vid>
vlan destroy all
vlan destroy all untag
vlan destroy entry all
vlan destroy entry <UINT:index>
vlan get state
vlan set state ( enable | disable )
vlan get transparent state
vlan set transparent state ( enable | disable )
vlan get vlan-treat vid ( 0 | 4095 ) type
vlan set vlan-treat vid ( 0 | 4095 ) type ( tagging | un-tagging )
vlan get accept-frame-type port ( <PORT_LIST:ports> | all )
vlan set accept-frame-type port ( <PORT_LIST:ports> | all ) ( all | tag-only | untag-only | priority-tag-and-tag )
vlan get egress port ( <PORT_LIST:egr_ports> | all ) keep-tag ingress-port ( <PORT_LIST:igr_ports> | all ) state
vlan set egress port ( <PORT_LIST:egr_ports> | all ) keep-tag ingress-port ( <PORT_LIST:igr_ports> | all ) state ( enable | disable )
vlan get ingress-filter port ( <PORT_LIST:ports> | all ) state
vlan set ingress-filter port ( <PORT_LIST:ports> | all ) state ( enable | disable )
vlan get entry <UINT:index>
vlan get entry all
vlan set entry <UINT:index> enhanced-vid <UINT:evid>
vlan set entry <UINT:index> filter-id <UINT:fid>
vlan set entry <UINT:index> member ( <PORT_LIST:ports> | all | none )
vlan set entry <UINT:index> ext-member ( <PORT_LIST:ext> | all | none )
vlan set entry <UINT:index> vlan-based-policing state ( disable | enable )
vlan set entry <UINT:index> meter <UINT:meter>
vlan set entry <UINT:index> vlan-based-priority state ( disable | enable )
vlan set entry <UINT:index> vlan-based-priority priority <UINT:priority>
vlan get protocol-vlan group <UINT:index>
vlan set protocol-vlan group <UINT:index> frame-type ( ethernet | snap | llc-other ) <UINT:frame_type>
vlan get protocol-vlan port ( <PORT_LIST:ports> | all )
vlan set protocol-vlan port ( <PORT_LIST:ports> | all ) group <UINT:index> state ( enable | disable )
vlan set protocol-vlan port ( <PORT_LIST:ports> | all ) group <UINT:index> vid <UINT:vid> priority <UINT:priority>
vlan get pvid port ( <PORT_LIST:ports> | all ) vlan-index
vlan set pvid port ( <PORT_LIST:ports> | all ) vlan-index <UINT:vidx>
vlan get pvid port ( <PORT_LIST:ports> | all )
vlan set pvid port ( <PORT_LIST:ports> | all ) <UINT:pvid>
vlan get ext-pvid port <PORT_LIST:ports> vlan-index
vlan set ext-pvid port <PORT_LIST:ports> vlan-index <UINT:vidx>
vlan get tag-mode port ( <PORT_LIST:ports> | all )
vlan set tag-mode port ( <PORT_LIST:ports> | all ) ( original | keep-format | priority-tag )
vlan get vlan-table vid <UINT:vid>
vlan set vlan-table vid <UINT:vid> fid-msti <UINT:fid_msti>
vlan set vlan-table vid <UINT:vid> hash-mode ( ivl | svl )
vlan set vlan-table vid <UINT:vid> member ( <PORT_LIST:ports> | all | none )
vlan set vlan-table vid <UINT:vid> tag-member ( <PORT_LIST:ports> | all | none )
vlan set vlan-table vid <UINT:vid> untag-member ( <PORT_LIST:ports> | all | none )
vlan set vlan-table vid <UINT:vid> ext-member ( <PORT_LIST:ext> | all | none )
vlan set vlan-table vid <UINT:vid> vlan-based-policing state ( disable | enable )
vlan set vlan-table vid <UINT:vid> meter <UINT:meter>
vlan set vlan-table vid <UINT:vid> vlan-based-priority state ( disable | enable )
vlan set vlan-table vid <UINT:vid> vlan-based-priority priority <UINT:priority>
vlan get cfi-keep
vlan set cfi-keep ( cfi-to-0 | keep-cfi )
vlan set port-based-fid port ( <PORT_LIST:ports> | all ) filter-id <UINT:fid>
vlan set port-based-fid port ( <PORT_LIST:ports> | all ) state ( disable | enable )
vlan get port-based-fid port ( <PORT_LIST:ports> | all )
vlan get port-based-pri port ( <PORT_LIST:ports> | all )
vlan set port-based-pri port ( <PORT_LIST:ports> | all ) priority <UINT:priority>
vlan set leaky ip-mcast port ( <PORT_LIST:ports> | all ) state ( enable | disable )
vlan get leaky ip-mcast port ( <PORT_LIST:ports> | all ) state
