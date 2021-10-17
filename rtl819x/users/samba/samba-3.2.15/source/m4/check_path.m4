dnl
dnl Samba3 build environment path checks
dnl
dnl Copyright (C) Michael Adam 2008
dnl
dnl Released under the GNU General Public License
dnl http://www.gnu.org/licenses/
dnl

AC_LIBREPLACE_LOCATION_CHECKS

#################################################
# Directory handling stuff to support both the
# legacy SAMBA directories and FHS compliant
# ones...
AC_PREFIX_DEFAULT(/usr/local/samba)

rootsbindir="\${SBINDIR}"
lockdir="\${VARDIR}/locks"
piddir="\${VARDIR}/locks"
test "${mandir}" || mandir="\${prefix}/man"
logfilebase="\${VARDIR}"
privatedir="\${prefix}/private"
test "${libdir}" || libdir="\${prefix}/lib"
pammodulesdir="\${LIBDIR}/security"
configdir="\${LIBDIR}"
swatdir="\${prefix}/swat"
codepagedir="\${LIBDIR}"
statedir="\${LOCKDIR}"
cachedir="\${LOCKDIR}"

AC_ARG_WITH(fhs,
[AS_HELP_STRING([--with-fhs],[Use FHS-compliant paths (default=no)])],
[ case "$withval" in
  yes)
    lockdir="\${VARDIR}/lib/samba"
    piddir="\${VARDIR}/run"
    mandir="\${prefix}/share/man"
    logfilebase="\${VARDIR}/log/samba"
    privatedir="\${CONFIGDIR}/private"
    test "${libdir}" || libdir="\${prefix}/lib/samba"
    configdir="\${sysconfdir}/samba"
    swatdir="\${DATADIR}/samba/swat"
    codepagedir="\${LIBDIR}"
    statedir="\${VARDIR}/lib/samba"
    cachedir="\${VARDIR}/lib/samba"
    AC_DEFINE(FHS_COMPATIBLE, 1, [Whether to use fully FHS-compatible paths])
    ;;
  esac])

#################################################
# set private directory location
AC_ARG_WITH(privatedir,
[AS_HELP_STRING([--with-privatedir=DIR], [Where to put smbpasswd ($ac_default_prefix/private)])],
[ case "$withval" in
  yes|no)
  #
  # Just in case anybody calls it without argument
  #
    AC_MSG_WARN([--with-privatedir called without argument - will use default])
  ;;
  * )
    privatedir="$withval"
    ;;
  esac])

#################################################
# set root sbin directory location
AC_ARG_WITH(rootsbindir,
[AS_HELP_STRING([--with-rootsbindir=DIR], [Which directory to use for root sbin ($ac_default_prefix/sbin)])],
[ case "$withval" in
  yes|no)
  #
  # Just in case anybody calls it without argument
  #
    AC_MSG_WARN([--with-rootsbindir called without argument - will use default])
  ;;
  * )
    rootsbindir="$withval"
    ;;
  esac])

#################################################
# set lock directory location
AC_ARG_WITH(lockdir,
[AS_HELP_STRING([--with-lockdir=DIR], [Where to put lock files ($ac_default_prefix/var/locks)])],
[ case "$withval" in
  yes|no)
  #
  # Just in case anybody calls it without argument
  #
    AC_MSG_WARN([--with-lockdir called without argument - will use default])
  ;;
  * )
    lockdir="$withval"
    ;;
  esac])

#################################################
# set pid directory location
AC_ARG_WITH(piddir,
[AS_HELP_STRING([--with-piddir=DIR], [Where to put pid files ($ac_default_prefix/var/locks)])],
[ case "$withval" in
  yes|no)
  #
  # Just in case anybody calls it without argument
  #
    AC_MSG_WARN([--with-piddir called without argument - will use default])
  ;;
  * )
    piddir="$withval"
    ;;
  esac])

#################################################
# set SWAT directory location
AC_ARG_WITH(swatdir,
[AS_HELP_STRING([--with-swatdir=DIR], [Where to put SWAT files ($ac_default_prefix/swat)])],
[ case "$withval" in
  yes|no)
  #
  # Just in case anybody does it
  #
    AC_MSG_WARN([--with-swatdir called without argument - will use default])
  ;;
  * )
    swatdir="$withval"
    ;;
  esac])

#################################################
# set configuration directory location
AC_ARG_WITH(configdir,
[AS_HELP_STRING([--with-configdir=DIR], [Where to put configuration files ($libdir)])],
[ case "$withval" in
  yes|no)
  #
  # Just in case anybody does it
  #
    AC_MSG_WARN([--with-configdir called without argument - will use default])
  ;;
  * )
    configdir="$withval"
    ;;
  esac])

#################################################
# set log directory location
AC_ARG_WITH(logfilebase,
[AS_HELP_STRING([--with-logfilebase=DIR], [Where to put log files ($VARDIR)])],
[ case "$withval" in
  yes|no)
  #
  # Just in case anybody does it
  #
    AC_MSG_WARN([--with-logfilebase called without argument - will use default])
  ;;
  * )
    logfilebase="$withval"
    ;;
  esac])


#################################################
# set ctdb source directory location
AC_ARG_WITH(ctdb,
[AS_HELP_STRING([--with-ctdb=DIR], [Where to find ctdb sources])],
[ case "$withval" in
  yes|no)
    AC_MSG_WARN([--with-ctdb called without argument])
  ;;
  * )
    ctdbdir="$withval"
    ;;
  esac])

#################################################
# set lib directory location
AC_ARG_WITH(libdir,
[AS_HELP_STRING([--with-libdir=DIR], [Where to put libdir ($libdir)])],
[ case "$withval" in
  yes|no)
  #
  # Just in case anybody does it
  #
    AC_MSG_WARN([--with-libdir without argument - will use default])
  ;;
  * )
    libdir="$withval"
    ;;
  esac])

#################################################
# set PAM modules directory location
AC_ARG_WITH(pammodulesdir,
[AS_HELP_STRING([--with-pammodulesdir=DIR], [Which directory to use for PAM modules ($ac_default_prefix/$libdir/security)])],
[ case "$withval" in
  yes|no)
  #
  # Just in case anybody calls it without argument
  #
    AC_MSG_WARN([--with-pammodulesdir called without argument - will use default])
  ;;
  * )
    pammodulesdir="$withval"
    ;;
  esac])

#################################################
# set man directory location
AC_ARG_WITH(mandir,
[AS_HELP_STRING([--with-mandir=DIR], [Where to put man pages ($mandir)])],
[ case "$withval" in
  yes|no)
  #
  # Just in case anybody does it
  #
    AC_MSG_WARN([--with-mandir without argument - will use default])
  ;;
  * )
    mandir="$withval"
    ;;
  esac])

AC_SUBST(configdir)
AC_SUBST(lockdir)
AC_SUBST(piddir)
AC_SUBST(logfilebase)
AC_SUBST(ctdbdir)
AC_SUBST(privatedir)
AC_SUBST(swatdir)
AC_SUBST(bindir)
AC_SUBST(sbindir)
AC_SUBST(codepagedir)
AC_SUBST(statedir)
AC_SUBST(cachedir)
AC_SUBST(rootsbindir)
AC_SUBST(pammodulesdir)

#################################################
# set prefix for 'make test'
selftest_prefix="./st"
AC_SUBST(selftest_prefix)
AC_ARG_WITH(selftest-prefix,
[AS_HELP_STRING([--with-selftest-prefix=DIR], [The prefix where make test will be run ($selftest_prefix)])],
[ case "$withval" in
  yes|no)
    AC_MSG_WARN([--with-selftest-prefix called without argument - will use default])
  ;;
  * )
    selftest_prefix="$withval"
    ;;
  esac
])

#################################################
# set path of samba4's smbtorture
smbtorture4_path=""
AC_SUBST(smbtorture4_path)
AC_ARG_WITH(smbtorture4_path,
[AS_HELP_STRING([--with-smbtorture4-path=PATH], [The path to a samba4 smbtorture for make test (none)])],
[ case "$withval" in
  yes|no)
    AC_MSG_ERROR([--with-smbtorture4-path should take a path])
  ;;
  * )
    smbtorture4_path="$withval"
    if test -z "$smbtorture4_path" -a ! -f $smbtorture4_path; then
    	AC_MSG_ERROR(['$smbtorture_path' does not  exist!])
    fi
  ;;
 esac
])

## check for --enable-debug first before checking CFLAGS before
## so that we don't mix -O and -g
debug=no
AC_ARG_ENABLE(debug,
[AS_HELP_STRING([--enable-debug], [Turn on compiler debugging information (default=no)])],
    [if eval "test x$enable_debug = xyes"; then
	debug=yes
    fi])

developer=no
AC_ARG_ENABLE(developer, [AS_HELP_STRING([--enable-developer], [Turn on developer warnings and debugging (default=no)])],
    [if eval "test x$enable_developer = xyes"; then
        debug=yes
        developer=yes
    fi])

krb5developer=no
AC_ARG_ENABLE(krb5developer, [AS_HELP_STRING([--enable-krb5developer], [Turn on developer warnings and debugging, except -Wstrict-prototypes (default=no)])],
    [if eval "test x$enable_krb5developer = xyes"; then
        debug=yes
        developer=yes
	krb5_developer=yes
    fi])

AC_ARG_WITH(cfenc,
[AS_HELP_STRING([--with-cfenc=HEADERDIR], [Use internal CoreFoundation encoding API for optimization (Mac OS X/Darwin only)])],
[
# May be in source $withval/CoreFoundation/StringEncodings.subproj.
# Should have been in framework $withval/CoreFoundation.framework/Headers.
for d in \
    $withval/CoreFoundation/StringEncodings.subproj \
    $withval/StringEncodings.subproj \
    $withval/CoreFoundation.framework/Headers \
    $withval/Headers \
    $withval
do
    if test -r $d/CFStringEncodingConverter.h; then
        ln -sfh $d include/CoreFoundation
    fi
done
])

