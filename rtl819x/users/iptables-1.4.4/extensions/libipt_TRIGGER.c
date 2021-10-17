/* Port-triggering target.
 *
 * Copyright (C) 2003, CyberTAN Corporation
 * All Rights Reserved.
 */

/* Shared library add-on to iptables to add port-trigger support. */

#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <xtables.h>
#include <linux/netfilter_ipv4/ip_tables.h>
#include <net/netfilter/nf_nat.h>
#include <linux/netfilter_ipv4/ipt_TRIGGER.h>

static int trigger_check_inverse(const char option[], int *invert, int *optind, int argc)
{
	if (option && strcmp(option, "!") == 0) {
		if (*invert)
			xtables_error(PARAMETER_PROBLEM,
				   "Multiple `!' flags not allowed");
		*invert = 1;
		if (optind) {
			*optind = *optind+1;
			if (argc && *optind > argc)
				xtables_error(PARAMETER_PROBLEM,
					   "no argument following `!'");
		}

		return 1;
	}
	return 0;
}

/* Function which prints out usage message. */
static void
trigger_help(void)
{
	printf(
"TRIGGER %s options:\n"
" --trigger-type (dnat|in|out)\n"
"				Trigger type\n"
" --trigger-mproto proto\n"
"				Trigger protocol\n"
" --trigger-match port[-port]\n"
"				Trigger destination port range\n"
" --trigger-rproto proto\n"
"               Related protocol\n"
" --trigger-relate port[-port]\n"
"				Related destination Port range.\n\n",
XTABLES_VERSION);
}

static const struct option trigger_opts[] = {
	{ "trigger-type", 1, 0, '1' },
	{ "trigger-mproto", 1, 0, '2' },
	{ "trigger-rproto", 1, 0, '3' },
	{ "trigger-match", 1, 0, '4' },
	{ "trigger-relate", 1, 0, '5' },
	{ .name = NULL }
};

/* Initialize the target. */
static void
trigger_init(struct xt_entry_target *t)
{
}

/* Parses ports */
static void
parse_ports(const char *arg, u_int16_t *ports)
{
	const char *dash;
	int port;

	port = atoi(arg);
	if (port == 0 || port > 65535)
		xtables_error(PARAMETER_PROBLEM, "Port range `%s' invalid\n", arg);

	dash = strchr(arg, '-');
	if (!dash)
		ports[0] = ports[1] = port;
	else {
		int maxport;

    	maxport = atoi(dash + 1);
		if (maxport == 0 || maxport > 65535)
			xtables_error(PARAMETER_PROBLEM,
				   "Port range `%s' invalid\n", dash+1);
		if (maxport < port)
			xtables_error(PARAMETER_PROBLEM,
				   "Port range `%s' invalid\n", arg);
		ports[0] = port;
		ports[1] = maxport;
	}
}


/* Function which parses command options; returns true if it
   ate an option */
static int
trigger_parse(int c, char **argv, int invert, unsigned int *flags,
      const void *entry, struct xt_entry_target **target)
{
	struct ipt_trigger_info *info = (struct ipt_trigger_info *)(*target)->data;

	switch (c) {
    /* --trigger-type */
	case '1':
		if (!strcasecmp(optarg, "dnat"))
		{
			info->type = IPT_TRIGGER_DNAT;
            info->mproto = 0;
            info->rproto = 0;
            info->ports.mport[0] = 0;
            info->ports.mport[1] = 0;
            info->ports.rport[0] = 0;
            info->ports.rport[1] = 0;
		}
		else if (!strcasecmp(optarg, "in"))
			info->type = IPT_TRIGGER_IN;
		else if (!strcasecmp(optarg, "out"))
			info->type = IPT_TRIGGER_OUT;
		else
			xtables_error(PARAMETER_PROBLEM,
				   "unknown type `%s' specified", optarg);
		return 1;

    /* --trigger-mproto */
    case '2':
		if (!strcasecmp(optarg, "tcp"))
			info->mproto = IPPROTO_TCP;
		else if (!strcasecmp(optarg, "udp"))
			info->mproto = IPPROTO_UDP;
		else if (!strcasecmp(optarg, "all"))
			info->mproto = 0;
		else
			xtables_error(PARAMETER_PROBLEM,
				   "unknown protocol `%s' specified", optarg);
		return 1;

    /* --trigger-rproto */
	case '3':
		if (!strcasecmp(optarg, "tcp"))
			info->rproto = IPPROTO_TCP;
		else if (!strcasecmp(optarg, "udp"))
			info->rproto = IPPROTO_UDP;
		else if (!strcasecmp(optarg, "all"))
			info->rproto = 0;
		else
			xtables_error(PARAMETER_PROBLEM,
				   "unknown protocol `%s' specified", optarg);
		return 1;

    /* --trigger-match */
	case '4':
		if (trigger_check_inverse(optarg, &invert, &optind, 0))
			xtables_error(PARAMETER_PROBLEM,
				   "Unexpected `!' after --trigger-match");

		parse_ports(optarg, info->ports.mport);
		return 1;

    /* --trigger-relate */
	case '5':
		if (trigger_check_inverse(optarg, &invert, &optind, 0))
			xtables_error(PARAMETER_PROBLEM,
				   "Unexpected `!' after --trigger-relate");

		parse_ports(optarg, info->ports.rport);
		*flags |= IP_NAT_RANGE_PROTO_SPECIFIED;
		return 1;

	default:
		return 0;
	}
}


/* Prints out the targinfo. */
static void
trigger_print(const void *ip,
      const struct xt_entry_target *target,
      int numeric)
{
	struct ipt_trigger_info *info = (struct ipt_trigger_info *)target->data;

	printf("TRIGGER ");
	if (info->type == IPT_TRIGGER_DNAT)
		printf("type:dnat ");
	else if (info->type == IPT_TRIGGER_IN)
		printf("type:in ");
	else if (info->type == IPT_TRIGGER_OUT)
		printf("type:out ");

    if (info->type == IPT_TRIGGER_OUT)
    {
        if (info->mproto == IPPROTO_TCP)
    		printf("mproto:tcp ");
    	else if (info->mproto == IPPROTO_UDP)
    		printf("mproto:udp ");
        else
            printf("mproto:all ");

    	printf("match:%hu-%hu", info->ports.mport[0], info->ports.mport[1]);
    	printf(" ");
    }

    if (info->ports.rport[0] > 0)
    {
        if (info->rproto == IPPROTO_TCP)
    		printf("rproto:tcp ");
    	else if (info->rproto == IPPROTO_UDP)
    		printf("rproto:udp ");
        else
            printf("rproto:all ");

    	printf("relate:%hu-%hu", info->ports.rport[0], info->ports.rport[1]);
    	printf(" ");
    }
}

/* Saves the union ipt_targinfo in parsable form to stdout. */
static void
trigger_save(const void *ip, const struct xt_entry_target *target)
{
	struct ipt_trigger_info *info = (struct ipt_trigger_info *)target->data;

    printf("--trigger-type ");
    if (info->type == IPT_TRIGGER_DNAT)
		printf("dnat ");
	else if (info->type == IPT_TRIGGER_IN)
		printf("in ");
	else if (info->type == IPT_TRIGGER_OUT)
		printf("out ");

    if (info->type == IPT_TRIGGER_OUT)
    {
        printf("--trigger-mproto ");
    	if (info->mproto == IPPROTO_TCP)
    		printf("tcp ");
    	else if (info->mproto == IPPROTO_UDP)
    		printf("udp ");
        else
            printf("all ");
        printf("--trigger-match %hu-%hu ", info->ports.mport[0], info->ports.mport[1]);
    }

    if (info->ports.rport[0] > 0)
    {
        printf("--trigger-rproto ");
    	if (info->rproto == IPPROTO_TCP)
    		printf("tcp ");
    	else if (info->rproto == IPPROTO_UDP)
    		printf("udp ");
        else
            printf("all ");
    	printf("--trigger-relate %hu-%hu ", info->ports.rport[0], info->ports.rport[1]);
    }
}

static struct xtables_target trigger_tg_reg =
{
	.name		= "TRIGGER",
	.version	= XTABLES_VERSION,
	.family		= NFPROTO_IPV4,
	.size		= XT_ALIGN(sizeof(struct ipt_trigger_info)),
	.userspacesize	= XT_ALIGN(sizeof(struct ipt_trigger_info)),
	.help		= trigger_help,
	.init		= trigger_init,
	.parse		= trigger_parse,
	.print		= trigger_print,
	.save		= trigger_save,
	.extra_opts	= trigger_opts,
};

void _init(void)
{
	xtables_register_target(&trigger_tg_reg);
}

