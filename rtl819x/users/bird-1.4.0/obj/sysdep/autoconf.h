/* obj/sysdep/autoconf.h.  Generated from autoconf.h.in by configure.  */
/*
 *	This file contains all system parameters automatically
 *	discovered by the configure script.
 */

/* System configuration file */
#define SYSCONF_INCLUDE "./sysdep/cf/linux-v6.h"

/* Include debugging code */
/* #undef DEBUGGING */

/* 8-bit integer type */
#define INTEGER_8 char

/* 16-bit integer type */
#define INTEGER_16 short int

/* 32-bit integer type */
#define INTEGER_32 int

/* 64-bit integer type */
#define INTEGER_64 long long int

/* CPU endianity */
/* #undef CPU_LITTLE_ENDIAN */
#define CPU_BIG_ENDIAN 1

/* Usual alignment for structures */
#define CPU_STRUCT_ALIGN 16

/* Characteristics of time_t */
/* #undef TIME_T_IS_64BIT */
#define TIME_T_IS_SIGNED 1

/* We have struct ip_mreqn in <netinet/in.h> */
#define HAVE_STRUCT_IP_MREQN 1

/* Protocols compiled in */
/* #undef CONFIG_STATIC */
#define CONFIG_RIP 1
/* #undef CONFIG_RADV */
/* #undef CONFIG_BFD */
/* #undef CONFIG_BGP */
/* #undef CONFIG_OSPF */
/* #undef CONFIG_PIPE */

/* We use multithreading */
#define USE_PTHREADS 1

/* We have <syslog.h> and syslog() */
#define HAVE_SYSLOG 1

/* We have <alloca.h> */
#define HAVE_ALLOCA_H 1

/* Are we using dmalloc? */
/* #undef HAVE_LIBDMALLOC */

/* Readline stuff */
/* #undef HAVE_RL_CRLF */
/* #undef HAVE_RL_DING */

/* struct sockaddr_in(6) */
/* #undef HAVE_SIN_LEN */

/* We have stdint.h */
#define HAVE_STDINT_H 1

#define CONFIG_PATH ?
