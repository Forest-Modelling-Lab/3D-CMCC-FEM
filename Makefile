# Makefile.in generated by automake 1.15.1 from Makefile.am.
# src/Makefile.  Generated from Makefile.in by configure.

# Copyright (C) 1994-2017 Free Software Foundation, Inc.

# This Makefile.in is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.




am__is_gnu_make = { \
  if test -z '$(MAKELEVEL)'; then \
    false; \
  elif test -n '$(MAKE_HOST)'; then \
    true; \
  elif test -n '$(MAKE_VERSION)' && test -n '$(CURDIR)'; then \
    true; \
  else \
    false; \
  fi; \
}
am__make_running_with_option = \
  case $${target_option-} in \
      ?) ;; \
      *) echo "am__make_running_with_option: internal error: invalid" \
              "target option '$${target_option-}' specified" >&2; \
         exit 1;; \
  esac; \
  has_opt=no; \
  sane_makeflags=$$MAKEFLAGS; \
  if $(am__is_gnu_make); then \
    sane_makeflags=$$MFLAGS; \
  else \
    case $$MAKEFLAGS in \
      *\\[\ \	]*) \
        bs=\\; \
        sane_makeflags=`printf '%s\n' "$$MAKEFLAGS" \
          | sed "s/$$bs$$bs[$$bs $$bs	]*//g"`;; \
    esac; \
  fi; \
  skip_next=no; \
  strip_trailopt () \
  { \
    flg=`printf '%s\n' "$$flg" | sed "s/$$1.*$$//"`; \
  }; \
  for flg in $$sane_makeflags; do \
    test $$skip_next = yes && { skip_next=no; continue; }; \
    case $$flg in \
      *=*|--*) continue;; \
        -*I) strip_trailopt 'I'; skip_next=yes;; \
      -*I?*) strip_trailopt 'I';; \
        -*O) strip_trailopt 'O'; skip_next=yes;; \
      -*O?*) strip_trailopt 'O';; \
        -*l) strip_trailopt 'l'; skip_next=yes;; \
      -*l?*) strip_trailopt 'l';; \
      -[dEDm]) skip_next=yes;; \
      -[JT]) skip_next=yes;; \
    esac; \
    case $$flg in \
      *$$target_option*) has_opt=yes; break;; \
    esac; \
  done; \
  test $$has_opt = yes
am__make_dryrun = (target_option=n; $(am__make_running_with_option))
am__make_keepgoing = (target_option=k; $(am__make_running_with_option))
pkgdatadir = $(datadir)/3d_cmcc_forest_model
pkgincludedir = $(includedir)/3d_cmcc_forest_model
pkglibdir = $(libdir)/3d_cmcc_forest_model
pkglibexecdir = $(libexecdir)/3d_cmcc_forest_model
am__cd = CDPATH="$${ZSH_VERSION+.}$(PATH_SEPARATOR)" && cd
install_sh_DATA = $(install_sh) -c -m 644
install_sh_PROGRAM = $(install_sh) -c
install_sh_SCRIPT = $(install_sh) -c
INSTALL_HEADER = $(INSTALL_DATA)
transform = $(program_transform_name)
NORMAL_INSTALL = :
PRE_INSTALL = :
POST_INSTALL = :
NORMAL_UNINSTALL = :
PRE_UNINSTALL = :
POST_UNINSTALL = :
bin_PROGRAMS = 3D_CMCC_Forest_Model$(EXEEXT)
subdir = src
ACLOCAL_M4 = $(top_srcdir)/aclocal.m4
am__aclocal_m4_deps = $(top_srcdir)/configure.ac
am__configure_deps = $(am__aclocal_m4_deps) $(CONFIGURE_DEPENDENCIES) \
	$(ACLOCAL_M4)
DIST_COMMON = $(srcdir)/Makefile.am $(am__DIST_COMMON)
mkinstalldirs = $(install_sh) -d
CONFIG_HEADER = $(top_builddir)/config.h
CONFIG_CLEAN_FILES =
CONFIG_CLEAN_VPATH_FILES =
am__installdirs = "$(DESTDIR)$(bindir)"
PROGRAMS = $(bin_PROGRAMS)
am_3D_CMCC_Forest_Model_OBJECTS = allometry.$(OBJEXT) \
	CN-allocation.$(OBJEXT) litter_nitrogen_balance.$(OBJEXT) \
	print_output.$(OBJEXT) CN-balance.$(OBJEXT) logger.$(OBJEXT) \
	regeneration.$(OBJEXT) aut_respiration.$(OBJEXT) \
	common.$(OBJEXT) main.$(OBJEXT) remove_tree_class.$(OBJEXT) \
	biomass.$(OBJEXT) settings.$(OBJEXT) compare.$(OBJEXT) \
	snow.$(OBJEXT) cue.$(OBJEXT) soil_carbon_balance.$(OBJEXT) \
	canopy_cover.$(OBJEXT) decomposition.$(OBJEXT) \
	management.$(OBJEXT) soil_evaporation.$(OBJEXT) \
	dendrometry.$(OBJEXT) matrix.$(OBJEXT) soil_model.$(OBJEXT) \
	canopy_evapotranspiration.$(OBJEXT) recruitment.$(OBJEXT) \
	met_data.$(OBJEXT) soil_model_check_balance.$(OBJEXT) \
	evapotranspiration.$(OBJEXT) meteo.$(OBJEXT) \
	soil_nitrogen_balance.$(OBJEXT) canopy_interception.$(OBJEXT) \
	fluxes.$(OBJEXT) modifiers.$(OBJEXT) \
	soil_radiation_sw_band.$(OBJEXT) \
	canopy_net_radiation.$(OBJEXT) g-function.$(OBJEXT) \
	mortality.$(OBJEXT) soil_respiration.$(OBJEXT) \
	canopy_radiation_lw_band.$(OBJEXT) soil_settings.$(OBJEXT) \
	canopy_radiation_sw_band.$(OBJEXT) heat_fluxes.$(OBJEXT) \
	new_forest_tree_class.$(OBJEXT) soil_water_balance.$(OBJEXT) \
	canopy_sensible_heat.$(OBJEXT) het_respiration.$(OBJEXT) \
	output.$(OBJEXT) nc.$(OBJEXT) structure.$(OBJEXT) \
	canopy_temperature.$(OBJEXT) initialization.$(OBJEXT) \
	peak_lai.$(OBJEXT) topo.$(OBJEXT) C-assimilation.$(OBJEXT) \
	lai.$(OBJEXT) Penman_Monteith.$(OBJEXT) tree_model.$(OBJEXT) \
	C-deciduous-partitioning.$(OBJEXT) leaf_fall.$(OBJEXT) \
	phenology.$(OBJEXT) tree_model_check_balance.$(OBJEXT) \
	cell_model.$(OBJEXT) litter_carbon_balance.$(OBJEXT) \
	photosynthesis2.$(OBJEXT) turnover.$(OBJEXT) \
	cell_model_check_balance.$(OBJEXT) littering.$(OBJEXT) \
	photosynthesis3.$(OBJEXT) utility.$(OBJEXT) \
	C-evergreen-partitioning.$(OBJEXT) litter_model.$(OBJEXT) \
	photosynthesis.$(OBJEXT) water_balance.$(OBJEXT) \
	C-fruit-partitioning.$(OBJEXT) \
	litter_model_check_balance.$(OBJEXT) print.$(OBJEXT) \
	wue.$(OBJEXT)
3D_CMCC_Forest_Model_OBJECTS = $(am_3D_CMCC_Forest_Model_OBJECTS)
3D_CMCC_Forest_Model_LDADD = $(LDADD)
AM_V_P = $(am__v_P_$(V))
am__v_P_ = $(am__v_P_$(AM_DEFAULT_VERBOSITY))
am__v_P_0 = false
am__v_P_1 = :
AM_V_GEN = $(am__v_GEN_$(V))
am__v_GEN_ = $(am__v_GEN_$(AM_DEFAULT_VERBOSITY))
am__v_GEN_0 = @echo "  GEN     " $@;
am__v_GEN_1 = 
AM_V_at = $(am__v_at_$(V))
am__v_at_ = $(am__v_at_$(AM_DEFAULT_VERBOSITY))
am__v_at_0 = @
am__v_at_1 = 
DEFAULT_INCLUDES = -I. -I$(top_builddir)
depcomp = $(SHELL) $(top_srcdir)/depcomp
am__depfiles_maybe = depfiles
am__mv = mv -f
COMPILE = $(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) \
	$(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)
AM_V_CC = $(am__v_CC_$(V))
am__v_CC_ = $(am__v_CC_$(AM_DEFAULT_VERBOSITY))
am__v_CC_0 = @echo "  CC      " $@;
am__v_CC_1 = 
CCLD = $(CC)
LINK = $(CCLD) $(AM_CFLAGS) $(CFLAGS) $(AM_LDFLAGS) $(LDFLAGS) -o $@
AM_V_CCLD = $(am__v_CCLD_$(V))
am__v_CCLD_ = $(am__v_CCLD_$(AM_DEFAULT_VERBOSITY))
am__v_CCLD_0 = @echo "  CCLD    " $@;
am__v_CCLD_1 = 
SOURCES = $(3D_CMCC_Forest_Model_SOURCES)
DIST_SOURCES = $(3D_CMCC_Forest_Model_SOURCES)
am__can_run_installinfo = \
  case $$AM_UPDATE_INFO_DIR in \
    n|no|NO) false;; \
    *) (install-info --version) >/dev/null 2>&1;; \
  esac
am__tagged_files = $(HEADERS) $(SOURCES) $(TAGS_FILES) $(LISP)
# Read a list of newline-separated strings from the standard input,
# and print each of them once, without duplicates.  Input order is
# *not* preserved.
am__uniquify_input = $(AWK) '\
  BEGIN { nonempty = 0; } \
  { items[$$0] = 1; nonempty = 1; } \
  END { if (nonempty) { for (i in items) print i; }; } \
'
# Make sure the list of sources is unique.  This is necessary because,
# e.g., the same source file might be shared among _SOURCES variables
# for different programs/libraries.
am__define_uniq_tagged_files = \
  list='$(am__tagged_files)'; \
  unique=`for i in $$list; do \
    if test -f "$$i"; then echo $$i; else echo $(srcdir)/$$i; fi; \
  done | $(am__uniquify_input)`
ETAGS = etags
CTAGS = ctags
am__tty_colors_dummy = \
  mgn= red= grn= lgn= blu= brg= std=; \
  am__color_tests=no
am__tty_colors = { \
  $(am__tty_colors_dummy); \
  if test "X$(AM_COLOR_TESTS)" = Xno; then \
    am__color_tests=no; \
  elif test "X$(AM_COLOR_TESTS)" = Xalways; then \
    am__color_tests=yes; \
  elif test "X$$TERM" != Xdumb && { test -t 1; } 2>/dev/null; then \
    am__color_tests=yes; \
  fi; \
  if test $$am__color_tests = yes; then \
    red='[0;31m'; \
    grn='[0;32m'; \
    lgn='[1;32m'; \
    blu='[1;34m'; \
    mgn='[0;35m'; \
    brg='[1m'; \
    std='[m'; \
  fi; \
}
am__vpath_adj_setup = srcdirstrip=`echo "$(srcdir)" | sed 's|.|.|g'`;
am__vpath_adj = case $$p in \
    $(srcdir)/*) f=`echo "$$p" | sed "s|^$$srcdirstrip/||"`;; \
    *) f=$$p;; \
  esac;
am__strip_dir = f=`echo $$p | sed -e 's|^.*/||'`;
am__install_max = 40
am__nobase_strip_setup = \
  srcdirstrip=`echo "$(srcdir)" | sed 's/[].[^$$\\*|]/\\\\&/g'`
am__nobase_strip = \
  for p in $$list; do echo "$$p"; done | sed -e "s|$$srcdirstrip/||"
am__nobase_list = $(am__nobase_strip_setup); \
  for p in $$list; do echo "$$p $$p"; done | \
  sed "s| $$srcdirstrip/| |;"' / .*\//!s/ .*/ ./; s,\( .*\)/[^/]*$$,\1,' | \
  $(AWK) 'BEGIN { files["."] = "" } { files[$$2] = files[$$2] " " $$1; \
    if (++n[$$2] == $(am__install_max)) \
      { print $$2, files[$$2]; n[$$2] = 0; files[$$2] = "" } } \
    END { for (dir in files) print dir, files[dir] }'
am__base_list = \
  sed '$$!N;$$!N;$$!N;$$!N;$$!N;$$!N;$$!N;s/\n/ /g' | \
  sed '$$!N;$$!N;$$!N;$$!N;s/\n/ /g'
am__uninstall_files_from_dir = { \
  test -z "$$files" \
    || { test ! -d "$$dir" && test ! -f "$$dir" && test ! -r "$$dir"; } \
    || { echo " ( cd '$$dir' && rm -f" $$files ")"; \
         $(am__cd) "$$dir" && rm -f $$files; }; \
  }
am__recheck_rx = ^[ 	]*:recheck:[ 	]*
am__global_test_result_rx = ^[ 	]*:global-test-result:[ 	]*
am__copy_in_global_log_rx = ^[ 	]*:copy-in-global-log:[ 	]*
# A command that, given a newline-separated list of test names on the
# standard input, print the name of the tests that are to be re-run
# upon "make recheck".
am__list_recheck_tests = $(AWK) '{ \
  recheck = 1; \
  while ((rc = (getline line < ($$0 ".trs"))) != 0) \
    { \
      if (rc < 0) \
        { \
          if ((getline line2 < ($$0 ".log")) < 0) \
	    recheck = 0; \
          break; \
        } \
      else if (line ~ /$(am__recheck_rx)[nN][Oo]/) \
        { \
          recheck = 0; \
          break; \
        } \
      else if (line ~ /$(am__recheck_rx)[yY][eE][sS]/) \
        { \
          break; \
        } \
    }; \
  if (recheck) \
    print $$0; \
  close ($$0 ".trs"); \
  close ($$0 ".log"); \
}'
# A command that, given a newline-separated list of test names on the
# standard input, create the global log from their .trs and .log files.
am__create_global_log = $(AWK) ' \
function fatal(msg) \
{ \
  print "fatal: making $@: " msg | "cat >&2"; \
  exit 1; \
} \
function rst_section(header) \
{ \
  print header; \
  len = length(header); \
  for (i = 1; i <= len; i = i + 1) \
    printf "="; \
  printf "\n\n"; \
} \
{ \
  copy_in_global_log = 1; \
  global_test_result = "RUN"; \
  while ((rc = (getline line < ($$0 ".trs"))) != 0) \
    { \
      if (rc < 0) \
         fatal("failed to read from " $$0 ".trs"); \
      if (line ~ /$(am__global_test_result_rx)/) \
        { \
          sub("$(am__global_test_result_rx)", "", line); \
          sub("[ 	]*$$", "", line); \
          global_test_result = line; \
        } \
      else if (line ~ /$(am__copy_in_global_log_rx)[nN][oO]/) \
        copy_in_global_log = 0; \
    }; \
  if (copy_in_global_log) \
    { \
      rst_section(global_test_result ": " $$0); \
      while ((rc = (getline line < ($$0 ".log"))) != 0) \
      { \
        if (rc < 0) \
          fatal("failed to read from " $$0 ".log"); \
        print line; \
      }; \
      printf "\n"; \
    }; \
  close ($$0 ".trs"); \
  close ($$0 ".log"); \
}'
# Restructured Text title.
am__rst_title = { sed 's/.*/   &   /;h;s/./=/g;p;x;s/ *$$//;p;g' && echo; }
# Solaris 10 'make', and several other traditional 'make' implementations,
# pass "-e" to $(SHELL), and POSIX 2008 even requires this.  Work around it
# by disabling -e (using the XSI extension "set +e") if it's set.
am__sh_e_setup = case $$- in *e*) set +e;; esac
# Default flags passed to test drivers.
am__common_driver_flags = \
  --color-tests "$$am__color_tests" \
  --enable-hard-errors "$$am__enable_hard_errors" \
  --expect-failure "$$am__expect_failure"
# To be inserted before the command running the test.  Creates the
# directory for the log if needed.  Stores in $dir the directory
# containing $f, in $tst the test, in $log the log.  Executes the
# developer- defined test setup AM_TESTS_ENVIRONMENT (if any), and
# passes TESTS_ENVIRONMENT.  Set up options for the wrapper that
# will run the test scripts (or their associated LOG_COMPILER, if
# thy have one).
am__check_pre = \
$(am__sh_e_setup);					\
$(am__vpath_adj_setup) $(am__vpath_adj)			\
$(am__tty_colors);					\
srcdir=$(srcdir); export srcdir;			\
case "$@" in						\
  */*) am__odir=`echo "./$@" | sed 's|/[^/]*$$||'`;;	\
    *) am__odir=.;; 					\
esac;							\
test "x$$am__odir" = x"." || test -d "$$am__odir" 	\
  || $(MKDIR_P) "$$am__odir" || exit $$?;		\
if test -f "./$$f"; then dir=./;			\
elif test -f "$$f"; then dir=;				\
else dir="$(srcdir)/"; fi;				\
tst=$$dir$$f; log='$@'; 				\
if test -n '$(DISABLE_HARD_ERRORS)'; then		\
  am__enable_hard_errors=no; 				\
else							\
  am__enable_hard_errors=yes; 				\
fi; 							\
case " $(XFAIL_TESTS) " in				\
  *[\ \	]$$f[\ \	]* | *[\ \	]$$dir$$f[\ \	]*) \
    am__expect_failure=yes;;				\
  *)							\
    am__expect_failure=no;;				\
esac; 							\
$(AM_TESTS_ENVIRONMENT) $(TESTS_ENVIRONMENT)
# A shell command to get the names of the tests scripts with any registered
# extension removed (i.e., equivalently, the names of the test logs, with
# the '.log' extension removed).  The result is saved in the shell variable
# '$bases'.  This honors runtime overriding of TESTS and TEST_LOGS.  Sadly,
# we cannot use something simpler, involving e.g., "$(TEST_LOGS:.log=)",
# since that might cause problem with VPATH rewrites for suffix-less tests.
# See also 'test-harness-vpath-rewrite.sh' and 'test-trs-basic.sh'.
am__set_TESTS_bases = \
  bases='$(TEST_LOGS)'; \
  bases=`for i in $$bases; do echo $$i; done | sed 's/\.log$$//'`; \
  bases=`echo $$bases`
RECHECK_LOGS = $(TEST_LOGS)
AM_RECURSIVE_TARGETS = check recheck
TEST_SUITE_LOG = test-suite.log
TEST_EXTENSIONS =  .test
LOG_DRIVER = $(SHELL) $(top_srcdir)/test-driver
LOG_COMPILE = $(LOG_COMPILER) $(AM_LOG_FLAGS) $(LOG_FLAGS)
am__set_b = \
  case '$@' in \
    */*) \
      case '$*' in \
        */*) b='$*';; \
          *) b=`echo '$@' | sed 's/\.log$$//'`; \
       esac;; \
    *) \
      b='$*';; \
  esac
am__test_logs1 = $(TESTS:=.log)
am__test_logs2 = $(am__test_logs1:.log=.log)
TEST_LOGS = $(am__test_logs2:.test.log=.log)
TEST_LOG_DRIVER = $(SHELL) $(top_srcdir)/test-driver
TEST_LOG_COMPILE = $(TEST_LOG_COMPILER) $(AM_TEST_LOG_FLAGS) \
	$(TEST_LOG_FLAGS)
am__DIST_COMMON = $(srcdir)/Makefile.in $(top_srcdir)/depcomp \
	$(top_srcdir)/test-driver
DISTFILES = $(DIST_COMMON) $(DIST_SOURCES) $(TEXINFOS) $(EXTRA_DIST)
ACLOCAL = ${SHELL} /home/vincenzo/Desktop/3D-CMCC-FEM-v.5.6.reg/missing aclocal-1.15
AMTAR = $${TAR-tar}
AM_DEFAULT_VERBOSITY = 1
AUTOCONF = ${SHELL} /home/vincenzo/Desktop/3D-CMCC-FEM-v.5.6.reg/missing autoconf
AUTOHEADER = ${SHELL} /home/vincenzo/Desktop/3D-CMCC-FEM-v.5.6.reg/missing autoheader
AUTOMAKE = ${SHELL} /home/vincenzo/Desktop/3D-CMCC-FEM-v.5.6.reg/missing automake-1.15
AWK = mawk
CC = gcc
CCDEPMODE = depmode=gcc3
CFLAGS = -g -O2
CPP = gcc -E
CPPFLAGS = 
CYGPATH_W = echo
DEFS = -DHAVE_CONFIG_H
DEPDIR = .deps
ECHO_C = 
ECHO_N = -n
ECHO_T = 
EGREP = /usr/bin/grep -E
EXEEXT = 
GREP = /usr/bin/grep
INSTALL = /usr/bin/install -c
INSTALL_DATA = ${INSTALL} -m 644
INSTALL_PROGRAM = ${INSTALL}
INSTALL_SCRIPT = ${INSTALL}
INSTALL_STRIP_PROGRAM = $(install_sh) -c -s
LDFLAGS = 
LIBOBJS = 
LIBS = -lm 
LTLIBOBJS = 
MAKEINFO = ${SHELL} /home/vincenzo/Desktop/3D-CMCC-FEM-v.5.6.reg/missing makeinfo
MKDIR_P = /usr/bin/mkdir -p
OBJEXT = o
PACKAGE = 3d_cmcc_forest_model
PACKAGE_BUGREPORT = daniela.dalmonech@gmail.com
PACKAGE_NAME = 3D_CMCC_Forest_Model
PACKAGE_STRING = 3D_CMCC_Forest_Model 5.5
PACKAGE_TARNAME = 3d_cmcc_forest_model
PACKAGE_URL = 
PACKAGE_VERSION = 5.5
PATH_SEPARATOR = :
POW_LIB = 
SET_MAKE = 
SHELL = /bin/bash
STRIP = 
VERSION = 5.5
abs_builddir = /home/vincenzo/Desktop/3D-CMCC-FEM-v.5.6.reg/src
abs_srcdir = /home/vincenzo/Desktop/3D-CMCC-FEM-v.5.6.reg/src
abs_top_builddir = /home/vincenzo/Desktop/3D-CMCC-FEM-v.5.6.reg
abs_top_srcdir = /home/vincenzo/Desktop/3D-CMCC-FEM-v.5.6.reg
ac_ct_CC = gcc
am__include = include
am__leading_dot = .
am__quote = 
am__tar = $${TAR-tar} chof - "$$tardir"
am__untar = $${TAR-tar} xf -
bindir = ${exec_prefix}/bin
build_alias = 
builddir = .
datadir = ${datarootdir}
datarootdir = ${prefix}/share
docdir = ${datarootdir}/doc/${PACKAGE_TARNAME}
dvidir = ${docdir}
exec_prefix = ${prefix}
host_alias = 
htmldir = ${docdir}
includedir = ${prefix}/include
infodir = ${datarootdir}/info
install_sh = ${SHELL} /home/vincenzo/Desktop/3D-CMCC-FEM-v.5.6.reg/install-sh
libdir = ${exec_prefix}/lib
libexecdir = ${exec_prefix}/libexec
localedir = ${datarootdir}/locale
localstatedir = ${prefix}/var
mandir = ${datarootdir}/man
mkdir_p = $(MKDIR_P)
oldincludedir = /usr/include
pdfdir = ${docdir}
prefix = /usr/local
program_transform_name = s,x,x,
psdir = ${docdir}
runstatedir = ${localstatedir}/run
sbindir = ${exec_prefix}/sbin
sharedstatedir = ${prefix}/com
srcdir = .
sysconfdir = ${prefix}/etc
target_alias = 
top_build_prefix = ../
top_builddir = ..
top_srcdir = ..
3D_CMCC_Forest_Model_SOURCES = \
        allometry.c\
        CN-allocation.c\
        litter_nitrogen_balance.c\
        print_output.c\
        CN-balance.c\
        logger.c\
        regeneration.c\
        aut_respiration.c\
        common.c\
        main.c\
        remove_tree_class.c\
        biomass.c\
        settings.c\
                    compare.c\
                    snow.c\
cue.c\
soil_carbon_balance.c\
canopy_cover.c\
decomposition.c\
management.c\
soil_evaporation.c\
               dendrometry.c\
               matrix.c\
               soil_model.c\
canopy_evapotranspiration.c\
recruitment.c\
met_data.c\
soil_model_check_balance.c\
  evapotranspiration.c\
  meteo.c\
  soil_nitrogen_balance.c\
canopy_interception.c\
fluxes.c\
modifiers.c\
soil_radiation_sw_band.c\
canopy_net_radiation.c\
g-function.c\
mortality.c\
soil_respiration.c\
canopy_radiation_lw_band.c\
soil_settings.c\
canopy_radiation_sw_band.c\
heat_fluxes.c\
new_forest_tree_class.c\
soil_water_balance.c\
canopy_sensible_heat.c\
het_respiration.c\
output.c\
nc.c\
structure.c\
canopy_temperature.c\
initialization.c\
peak_lai.c\
topo.c\
C-assimilation.c\
lai.c\
Penman_Monteith.c\
tree_model.c\
C-deciduous-partitioning.c\
leaf_fall.c\
phenology.c\
tree_model_check_balance.c\
cell_model.c\
litter_carbon_balance.c\
photosynthesis2.c\
turnover.c\
cell_model_check_balance.c\
littering.c\
photosynthesis3.c\
utility.c\
C-evergreen-partitioning.c\
litter_model.c\
photosynthesis.c\
water_balance.c\
C-fruit-partitioning.c\
litter_model_check_balance.c\
print.c\
wue.c

check_SCRIPTS = test_1.sh
TESTS = $(check_SCRIPTS)
CLEANFILES = $(check_SCRIPTS)
all: all-am

.SUFFIXES:
.SUFFIXES: .c .log .o .obj .test .test$(EXEEXT) .trs
$(srcdir)/Makefile.in:  $(srcdir)/Makefile.am  $(am__configure_deps)
	@for dep in $?; do \
	  case '$(am__configure_deps)' in \
	    *$$dep*) \
	      ( cd $(top_builddir) && $(MAKE) $(AM_MAKEFLAGS) am--refresh ) \
	        && { if test -f $@; then exit 0; else break; fi; }; \
	      exit 1;; \
	  esac; \
	done; \
	echo ' cd $(top_srcdir) && $(AUTOMAKE) --foreign src/Makefile'; \
	$(am__cd) $(top_srcdir) && \
	  $(AUTOMAKE) --foreign src/Makefile
Makefile: $(srcdir)/Makefile.in $(top_builddir)/config.status
	@case '$?' in \
	  *config.status*) \
	    cd $(top_builddir) && $(MAKE) $(AM_MAKEFLAGS) am--refresh;; \
	  *) \
	    echo ' cd $(top_builddir) && $(SHELL) ./config.status $(subdir)/$@ $(am__depfiles_maybe)'; \
	    cd $(top_builddir) && $(SHELL) ./config.status $(subdir)/$@ $(am__depfiles_maybe);; \
	esac;

$(top_builddir)/config.status: $(top_srcdir)/configure $(CONFIG_STATUS_DEPENDENCIES)
	cd $(top_builddir) && $(MAKE) $(AM_MAKEFLAGS) am--refresh

$(top_srcdir)/configure:  $(am__configure_deps)
	cd $(top_builddir) && $(MAKE) $(AM_MAKEFLAGS) am--refresh
$(ACLOCAL_M4):  $(am__aclocal_m4_deps)
	cd $(top_builddir) && $(MAKE) $(AM_MAKEFLAGS) am--refresh
$(am__aclocal_m4_deps):
install-binPROGRAMS: $(bin_PROGRAMS)
	@$(NORMAL_INSTALL)
	@list='$(bin_PROGRAMS)'; test -n "$(bindir)" || list=; \
	if test -n "$$list"; then \
	  echo " $(MKDIR_P) '$(DESTDIR)$(bindir)'"; \
	  $(MKDIR_P) "$(DESTDIR)$(bindir)" || exit 1; \
	fi; \
	for p in $$list; do echo "$$p $$p"; done | \
	sed 's/$(EXEEXT)$$//' | \
	while read p p1; do if test -f $$p \
	  ; then echo "$$p"; echo "$$p"; else :; fi; \
	done | \
	sed -e 'p;s,.*/,,;n;h' \
	    -e 's|.*|.|' \
	    -e 'p;x;s,.*/,,;s/$(EXEEXT)$$//;$(transform);s/$$/$(EXEEXT)/' | \
	sed 'N;N;N;s,\n, ,g' | \
	$(AWK) 'BEGIN { files["."] = ""; dirs["."] = 1 } \
	  { d=$$3; if (dirs[d] != 1) { print "d", d; dirs[d] = 1 } \
	    if ($$2 == $$4) files[d] = files[d] " " $$1; \
	    else { print "f", $$3 "/" $$4, $$1; } } \
	  END { for (d in files) print "f", d, files[d] }' | \
	while read type dir files; do \
	    if test "$$dir" = .; then dir=; else dir=/$$dir; fi; \
	    test -z "$$files" || { \
	      echo " $(INSTALL_PROGRAM_ENV) $(INSTALL_PROGRAM) $$files '$(DESTDIR)$(bindir)$$dir'"; \
	      $(INSTALL_PROGRAM_ENV) $(INSTALL_PROGRAM) $$files "$(DESTDIR)$(bindir)$$dir" || exit $$?; \
	    } \
	; done

uninstall-binPROGRAMS:
	@$(NORMAL_UNINSTALL)
	@list='$(bin_PROGRAMS)'; test -n "$(bindir)" || list=; \
	files=`for p in $$list; do echo "$$p"; done | \
	  sed -e 'h;s,^.*/,,;s/$(EXEEXT)$$//;$(transform)' \
	      -e 's/$$/$(EXEEXT)/' \
	`; \
	test -n "$$list" || exit 0; \
	echo " ( cd '$(DESTDIR)$(bindir)' && rm -f" $$files ")"; \
	cd "$(DESTDIR)$(bindir)" && rm -f $$files

clean-binPROGRAMS:
	-test -z "$(bin_PROGRAMS)" || rm -f $(bin_PROGRAMS)

3D_CMCC_Forest_Model$(EXEEXT): $(3D_CMCC_Forest_Model_OBJECTS) $(3D_CMCC_Forest_Model_DEPENDENCIES) $(EXTRA_3D_CMCC_Forest_Model_DEPENDENCIES) 
	@rm -f 3D_CMCC_Forest_Model$(EXEEXT)
	$(AM_V_CCLD)$(LINK) $(3D_CMCC_Forest_Model_OBJECTS) $(3D_CMCC_Forest_Model_LDADD) $(LIBS)

mostlyclean-compile:
	-rm -f *.$(OBJEXT)

distclean-compile:
	-rm -f *.tab.c

include ./$(DEPDIR)/C-assimilation.Po
include ./$(DEPDIR)/C-deciduous-partitioning.Po
include ./$(DEPDIR)/C-evergreen-partitioning.Po
include ./$(DEPDIR)/C-fruit-partitioning.Po
include ./$(DEPDIR)/CN-allocation.Po
include ./$(DEPDIR)/CN-balance.Po
include ./$(DEPDIR)/Penman_Monteith.Po
include ./$(DEPDIR)/allometry.Po
include ./$(DEPDIR)/aut_respiration.Po
include ./$(DEPDIR)/biomass.Po
include ./$(DEPDIR)/canopy_cover.Po
include ./$(DEPDIR)/canopy_evapotranspiration.Po
include ./$(DEPDIR)/canopy_interception.Po
include ./$(DEPDIR)/canopy_net_radiation.Po
include ./$(DEPDIR)/canopy_radiation_lw_band.Po
include ./$(DEPDIR)/canopy_radiation_sw_band.Po
include ./$(DEPDIR)/canopy_sensible_heat.Po
include ./$(DEPDIR)/canopy_temperature.Po
include ./$(DEPDIR)/cell_model.Po
include ./$(DEPDIR)/cell_model_check_balance.Po
include ./$(DEPDIR)/common.Po
include ./$(DEPDIR)/compare.Po
include ./$(DEPDIR)/cue.Po
include ./$(DEPDIR)/decomposition.Po
include ./$(DEPDIR)/dendrometry.Po
include ./$(DEPDIR)/recruitment.Po
include ./$(DEPDIR)/evapotranspiration.Po
include ./$(DEPDIR)/fluxes.Po
include ./$(DEPDIR)/g-function.Po
include ./$(DEPDIR)/heat_fluxes.Po
include ./$(DEPDIR)/het_respiration.Po
include ./$(DEPDIR)/initialization.Po
include ./$(DEPDIR)/lai.Po
include ./$(DEPDIR)/leaf_fall.Po
include ./$(DEPDIR)/litter_carbon_balance.Po
include ./$(DEPDIR)/litter_model.Po
include ./$(DEPDIR)/litter_model_check_balance.Po
include ./$(DEPDIR)/litter_nitrogen_balance.Po
include ./$(DEPDIR)/littering.Po
include ./$(DEPDIR)/logger.Po
include ./$(DEPDIR)/main.Po
include ./$(DEPDIR)/management.Po
include ./$(DEPDIR)/matrix.Po
include ./$(DEPDIR)/met_data.Po
include ./$(DEPDIR)/meteo.Po
include ./$(DEPDIR)/modifiers.Po
include ./$(DEPDIR)/mortality.Po
include ./$(DEPDIR)/nc.Po
include ./$(DEPDIR)/new_forest_tree_class.Po
include ./$(DEPDIR)/output.Po
include ./$(DEPDIR)/peak_lai.Po
include ./$(DEPDIR)/phenology.Po
include ./$(DEPDIR)/photosynthesis.Po
include ./$(DEPDIR)/photosynthesis2.Po
include ./$(DEPDIR)/photosynthesis3.Po
include ./$(DEPDIR)/print.Po
include ./$(DEPDIR)/print_output.Po
include ./$(DEPDIR)/regeneration.Po
include ./$(DEPDIR)/remove_tree_class.Po
include ./$(DEPDIR)/settings.Po
include ./$(DEPDIR)/snow.Po
include ./$(DEPDIR)/soil_carbon_balance.Po
include ./$(DEPDIR)/soil_evaporation.Po
include ./$(DEPDIR)/soil_model.Po
include ./$(DEPDIR)/soil_model_check_balance.Po
include ./$(DEPDIR)/soil_nitrogen_balance.Po
include ./$(DEPDIR)/soil_radiation_sw_band.Po
include ./$(DEPDIR)/soil_respiration.Po
include ./$(DEPDIR)/soil_settings.Po
include ./$(DEPDIR)/soil_water_balance.Po
include ./$(DEPDIR)/structure.Po
include ./$(DEPDIR)/topo.Po
include ./$(DEPDIR)/tree_model.Po
include ./$(DEPDIR)/tree_model_check_balance.Po
include ./$(DEPDIR)/turnover.Po
include ./$(DEPDIR)/utility.Po
include ./$(DEPDIR)/water_balance.Po
include ./$(DEPDIR)/wue.Po

.c.o:
	$(AM_V_CC)$(COMPILE) -MT $@ -MD -MP -MF $(DEPDIR)/$*.Tpo -c -o $@ $<
	$(AM_V_at)$(am__mv) $(DEPDIR)/$*.Tpo $(DEPDIR)/$*.Po
#	$(AM_V_CC)source='$<' object='$@' libtool=no \
#	DEPDIR=$(DEPDIR) $(CCDEPMODE) $(depcomp) \
#	$(AM_V_CC_no)$(COMPILE) -c -o $@ $<

.c.obj:
	$(AM_V_CC)$(COMPILE) -MT $@ -MD -MP -MF $(DEPDIR)/$*.Tpo -c -o $@ `$(CYGPATH_W) '$<'`
	$(AM_V_at)$(am__mv) $(DEPDIR)/$*.Tpo $(DEPDIR)/$*.Po
#	$(AM_V_CC)source='$<' object='$@' libtool=no \
#	DEPDIR=$(DEPDIR) $(CCDEPMODE) $(depcomp) \
#	$(AM_V_CC_no)$(COMPILE) -c -o $@ `$(CYGPATH_W) '$<'`

ID: $(am__tagged_files)
	$(am__define_uniq_tagged_files); mkid -fID $$unique
tags: tags-am
TAGS: tags

tags-am: $(TAGS_DEPENDENCIES) $(am__tagged_files)
	set x; \
	here=`pwd`; \
	$(am__define_uniq_tagged_files); \
	shift; \
	if test -z "$(ETAGS_ARGS)$$*$$unique"; then :; else \
	  test -n "$$unique" || unique=$$empty_fix; \
	  if test $$# -gt 0; then \
	    $(ETAGS) $(ETAGSFLAGS) $(AM_ETAGSFLAGS) $(ETAGS_ARGS) \
	      "$$@" $$unique; \
	  else \
	    $(ETAGS) $(ETAGSFLAGS) $(AM_ETAGSFLAGS) $(ETAGS_ARGS) \
	      $$unique; \
	  fi; \
	fi
ctags: ctags-am

CTAGS: ctags
ctags-am: $(TAGS_DEPENDENCIES) $(am__tagged_files)
	$(am__define_uniq_tagged_files); \
	test -z "$(CTAGS_ARGS)$$unique" \
	  || $(CTAGS) $(CTAGSFLAGS) $(AM_CTAGSFLAGS) $(CTAGS_ARGS) \
	     $$unique

GTAGS:
	here=`$(am__cd) $(top_builddir) && pwd` \
	  && $(am__cd) $(top_srcdir) \
	  && gtags -i $(GTAGS_ARGS) "$$here"
cscopelist: cscopelist-am

cscopelist-am: $(am__tagged_files)
	list='$(am__tagged_files)'; \
	case "$(srcdir)" in \
	  [\\/]* | ?:[\\/]*) sdir="$(srcdir)" ;; \
	  *) sdir=$(subdir)/$(srcdir) ;; \
	esac; \
	for i in $$list; do \
	  if test -f "$$i"; then \
	    echo "$(subdir)/$$i"; \
	  else \
	    echo "$$sdir/$$i"; \
	  fi; \
	done >> $(top_builddir)/cscope.files

distclean-tags:
	-rm -f TAGS ID GTAGS GRTAGS GSYMS GPATH tags

# Recover from deleted '.trs' file; this should ensure that
# "rm -f foo.log; make foo.trs" re-run 'foo.test', and re-create
# both 'foo.log' and 'foo.trs'.  Break the recipe in two subshells
# to avoid problems with "make -n".
.log.trs:
	rm -f $< $@
	$(MAKE) $(AM_MAKEFLAGS) $<

# Leading 'am--fnord' is there to ensure the list of targets does not
# expand to empty, as could happen e.g. with make check TESTS=''.
am--fnord $(TEST_LOGS) $(TEST_LOGS:.log=.trs): $(am__force_recheck)
am--force-recheck:
	@:

$(TEST_SUITE_LOG): $(TEST_LOGS)
	@$(am__set_TESTS_bases); \
	am__f_ok () { test -f "$$1" && test -r "$$1"; }; \
	redo_bases=`for i in $$bases; do \
	              am__f_ok $$i.trs && am__f_ok $$i.log || echo $$i; \
	            done`; \
	if test -n "$$redo_bases"; then \
	  redo_logs=`for i in $$redo_bases; do echo $$i.log; done`; \
	  redo_results=`for i in $$redo_bases; do echo $$i.trs; done`; \
	  if $(am__make_dryrun); then :; else \
	    rm -f $$redo_logs && rm -f $$redo_results || exit 1; \
	  fi; \
	fi; \
	if test -n "$$am__remaking_logs"; then \
	  echo "fatal: making $(TEST_SUITE_LOG): possible infinite" \
	       "recursion detected" >&2; \
	elif test -n "$$redo_logs"; then \
	  am__remaking_logs=yes $(MAKE) $(AM_MAKEFLAGS) $$redo_logs; \
	fi; \
	if $(am__make_dryrun); then :; else \
	  st=0;  \
	  errmsg="fatal: making $(TEST_SUITE_LOG): failed to create"; \
	  for i in $$redo_bases; do \
	    test -f $$i.trs && test -r $$i.trs \
	      || { echo "$$errmsg $$i.trs" >&2; st=1; }; \
	    test -f $$i.log && test -r $$i.log \
	      || { echo "$$errmsg $$i.log" >&2; st=1; }; \
	  done; \
	  test $$st -eq 0 || exit 1; \
	fi
	@$(am__sh_e_setup); $(am__tty_colors); $(am__set_TESTS_bases); \
	ws='[ 	]'; \
	results=`for b in $$bases; do echo $$b.trs; done`; \
	test -n "$$results" || results=/dev/null; \
	all=`  grep "^$$ws*:test-result:"           $$results | wc -l`; \
	pass=` grep "^$$ws*:test-result:$$ws*PASS"  $$results | wc -l`; \
	fail=` grep "^$$ws*:test-result:$$ws*FAIL"  $$results | wc -l`; \
	skip=` grep "^$$ws*:test-result:$$ws*SKIP"  $$results | wc -l`; \
	xfail=`grep "^$$ws*:test-result:$$ws*XFAIL" $$results | wc -l`; \
	xpass=`grep "^$$ws*:test-result:$$ws*XPASS" $$results | wc -l`; \
	error=`grep "^$$ws*:test-result:$$ws*ERROR" $$results | wc -l`; \
	if test `expr $$fail + $$xpass + $$error` -eq 0; then \
	  success=true; \
	else \
	  success=false; \
	fi; \
	br='==================='; br=$$br$$br$$br$$br; \
	result_count () \
	{ \
	    if test x"$$1" = x"--maybe-color"; then \
	      maybe_colorize=yes; \
	    elif test x"$$1" = x"--no-color"; then \
	      maybe_colorize=no; \
	    else \
	      echo "$@: invalid 'result_count' usage" >&2; exit 4; \
	    fi; \
	    shift; \
	    desc=$$1 count=$$2; \
	    if test $$maybe_colorize = yes && test $$count -gt 0; then \
	      color_start=$$3 color_end=$$std; \
	    else \
	      color_start= color_end=; \
	    fi; \
	    echo "$${color_start}# $$desc $$count$${color_end}"; \
	}; \
	create_testsuite_report () \
	{ \
	  result_count $$1 "TOTAL:" $$all   "$$brg"; \
	  result_count $$1 "PASS: " $$pass  "$$grn"; \
	  result_count $$1 "SKIP: " $$skip  "$$blu"; \
	  result_count $$1 "XFAIL:" $$xfail "$$lgn"; \
	  result_count $$1 "FAIL: " $$fail  "$$red"; \
	  result_count $$1 "XPASS:" $$xpass "$$red"; \
	  result_count $$1 "ERROR:" $$error "$$mgn"; \
	}; \
	{								\
	  echo "$(PACKAGE_STRING): $(subdir)/$(TEST_SUITE_LOG)" |	\
	    $(am__rst_title);						\
	  create_testsuite_report --no-color;				\
	  echo;								\
	  echo ".. contents:: :depth: 2";				\
	  echo;								\
	  for b in $$bases; do echo $$b; done				\
	    | $(am__create_global_log);					\
	} >$(TEST_SUITE_LOG).tmp || exit 1;				\
	mv $(TEST_SUITE_LOG).tmp $(TEST_SUITE_LOG);			\
	if $$success; then						\
	  col="$$grn";							\
	 else								\
	  col="$$red";							\
	  test x"$$VERBOSE" = x || cat $(TEST_SUITE_LOG);		\
	fi;								\
	echo "$${col}$$br$${std}"; 					\
	echo "$${col}Testsuite summary for $(PACKAGE_STRING)$${std}";	\
	echo "$${col}$$br$${std}"; 					\
	create_testsuite_report --maybe-color;				\
	echo "$$col$$br$$std";						\
	if $$success; then :; else					\
	  echo "$${col}See $(subdir)/$(TEST_SUITE_LOG)$${std}";		\
	  if test -n "$(PACKAGE_BUGREPORT)"; then			\
	    echo "$${col}Please report to $(PACKAGE_BUGREPORT)$${std}";	\
	  fi;								\
	  echo "$$col$$br$$std";					\
	fi;								\
	$$success || exit 1

check-TESTS:
	@list='$(RECHECK_LOGS)';           test -z "$$list" || rm -f $$list
	@list='$(RECHECK_LOGS:.log=.trs)'; test -z "$$list" || rm -f $$list
	@test -z "$(TEST_SUITE_LOG)" || rm -f $(TEST_SUITE_LOG)
	@set +e; $(am__set_TESTS_bases); \
	log_list=`for i in $$bases; do echo $$i.log; done`; \
	trs_list=`for i in $$bases; do echo $$i.trs; done`; \
	log_list=`echo $$log_list`; trs_list=`echo $$trs_list`; \
	$(MAKE) $(AM_MAKEFLAGS) $(TEST_SUITE_LOG) TEST_LOGS="$$log_list"; \
	exit $$?;
recheck: all $(check_SCRIPTS)
	@test -z "$(TEST_SUITE_LOG)" || rm -f $(TEST_SUITE_LOG)
	@set +e; $(am__set_TESTS_bases); \
	bases=`for i in $$bases; do echo $$i; done \
	         | $(am__list_recheck_tests)` || exit 1; \
	log_list=`for i in $$bases; do echo $$i.log; done`; \
	log_list=`echo $$log_list`; \
	$(MAKE) $(AM_MAKEFLAGS) $(TEST_SUITE_LOG) \
	        am__force_recheck=am--force-recheck \
	        TEST_LOGS="$$log_list"; \
	exit $$?
test_1.sh.log: test_1.sh
	@p='test_1.sh'; \
	b='test_1.sh'; \
	$(am__check_pre) $(LOG_DRIVER) --test-name "$$f" \
	--log-file $$b.log --trs-file $$b.trs \
	$(am__common_driver_flags) $(AM_LOG_DRIVER_FLAGS) $(LOG_DRIVER_FLAGS) -- $(LOG_COMPILE) \
	"$$tst" $(AM_TESTS_FD_REDIRECT)
.test.log:
	@p='$<'; \
	$(am__set_b); \
	$(am__check_pre) $(TEST_LOG_DRIVER) --test-name "$$f" \
	--log-file $$b.log --trs-file $$b.trs \
	$(am__common_driver_flags) $(AM_TEST_LOG_DRIVER_FLAGS) $(TEST_LOG_DRIVER_FLAGS) -- $(TEST_LOG_COMPILE) \
	"$$tst" $(AM_TESTS_FD_REDIRECT)
#.test$(EXEEXT).log:
#	@p='$<'; \
#	$(am__set_b); \
#	$(am__check_pre) $(TEST_LOG_DRIVER) --test-name "$$f" \
#	--log-file $$b.log --trs-file $$b.trs \
#	$(am__common_driver_flags) $(AM_TEST_LOG_DRIVER_FLAGS) $(TEST_LOG_DRIVER_FLAGS) -- $(TEST_LOG_COMPILE) \
#	"$$tst" $(AM_TESTS_FD_REDIRECT)

distdir: $(DISTFILES)
	@srcdirstrip=`echo "$(srcdir)" | sed 's/[].[^$$\\*]/\\\\&/g'`; \
	topsrcdirstrip=`echo "$(top_srcdir)" | sed 's/[].[^$$\\*]/\\\\&/g'`; \
	list='$(DISTFILES)'; \
	  dist_files=`for file in $$list; do echo $$file; done | \
	  sed -e "s|^$$srcdirstrip/||;t" \
	      -e "s|^$$topsrcdirstrip/|$(top_builddir)/|;t"`; \
	case $$dist_files in \
	  */*) $(MKDIR_P) `echo "$$dist_files" | \
			   sed '/\//!d;s|^|$(distdir)/|;s,/[^/]*$$,,' | \
			   sort -u` ;; \
	esac; \
	for file in $$dist_files; do \
	  if test -f $$file || test -d $$file; then d=.; else d=$(srcdir); fi; \
	  if test -d $$d/$$file; then \
	    dir=`echo "/$$file" | sed -e 's,/[^/]*$$,,'`; \
	    if test -d "$(distdir)/$$file"; then \
	      find "$(distdir)/$$file" -type d ! -perm -700 -exec chmod u+rwx {} \;; \
	    fi; \
	    if test -d $(srcdir)/$$file && test $$d != $(srcdir); then \
	      cp -fpR $(srcdir)/$$file "$(distdir)$$dir" || exit 1; \
	      find "$(distdir)/$$file" -type d ! -perm -700 -exec chmod u+rwx {} \;; \
	    fi; \
	    cp -fpR $$d/$$file "$(distdir)$$dir" || exit 1; \
	  else \
	    test -f "$(distdir)/$$file" \
	    || cp -p $$d/$$file "$(distdir)/$$file" \
	    || exit 1; \
	  fi; \
	done
check-am: all-am
	$(MAKE) $(AM_MAKEFLAGS) $(check_SCRIPTS)
	$(MAKE) $(AM_MAKEFLAGS) check-TESTS
check: check-am
all-am: Makefile $(PROGRAMS)
installdirs:
	for dir in "$(DESTDIR)$(bindir)"; do \
	  test -z "$$dir" || $(MKDIR_P) "$$dir"; \
	done
install: install-am
install-exec: install-exec-am
install-data: install-data-am
uninstall: uninstall-am

install-am: all-am
	@$(MAKE) $(AM_MAKEFLAGS) install-exec-am install-data-am

installcheck: installcheck-am
install-strip:
	if test -z '$(STRIP)'; then \
	  $(MAKE) $(AM_MAKEFLAGS) INSTALL_PROGRAM="$(INSTALL_STRIP_PROGRAM)" \
	    install_sh_PROGRAM="$(INSTALL_STRIP_PROGRAM)" INSTALL_STRIP_FLAG=-s \
	      install; \
	else \
	  $(MAKE) $(AM_MAKEFLAGS) INSTALL_PROGRAM="$(INSTALL_STRIP_PROGRAM)" \
	    install_sh_PROGRAM="$(INSTALL_STRIP_PROGRAM)" INSTALL_STRIP_FLAG=-s \
	    "INSTALL_PROGRAM_ENV=STRIPPROG='$(STRIP)'" install; \
	fi
mostlyclean-generic:
	-test -z "$(TEST_LOGS)" || rm -f $(TEST_LOGS)
	-test -z "$(TEST_LOGS:.log=.trs)" || rm -f $(TEST_LOGS:.log=.trs)
	-test -z "$(TEST_SUITE_LOG)" || rm -f $(TEST_SUITE_LOG)

clean-generic:
	-test -z "$(CLEANFILES)" || rm -f $(CLEANFILES)

distclean-generic:
	-test -z "$(CONFIG_CLEAN_FILES)" || rm -f $(CONFIG_CLEAN_FILES)
	-test . = "$(srcdir)" || test -z "$(CONFIG_CLEAN_VPATH_FILES)" || rm -f $(CONFIG_CLEAN_VPATH_FILES)

maintainer-clean-generic:
	@echo "This command is intended for maintainers to use"
	@echo "it deletes files that may require special tools to rebuild."
clean: clean-am

clean-am: clean-binPROGRAMS clean-generic mostlyclean-am

distclean: distclean-am
	-rm -rf ./$(DEPDIR)
	-rm -f Makefile
distclean-am: clean-am distclean-compile distclean-generic \
	distclean-tags

dvi: dvi-am

dvi-am:

html: html-am

html-am:

info: info-am

info-am:

install-data-am:

install-dvi: install-dvi-am

install-dvi-am:

install-exec-am: install-binPROGRAMS

install-html: install-html-am

install-html-am:

install-info: install-info-am

install-info-am:

install-man:

install-pdf: install-pdf-am

install-pdf-am:

install-ps: install-ps-am

install-ps-am:

installcheck-am:

maintainer-clean: maintainer-clean-am
	-rm -rf ./$(DEPDIR)
	-rm -f Makefile
maintainer-clean-am: distclean-am maintainer-clean-generic

mostlyclean: mostlyclean-am

mostlyclean-am: mostlyclean-compile mostlyclean-generic

pdf: pdf-am

pdf-am:

ps: ps-am

ps-am:

uninstall-am: uninstall-binPROGRAMS

.MAKE: check-am install-am install-strip

.PHONY: CTAGS GTAGS TAGS all all-am check check-TESTS check-am clean \
	clean-binPROGRAMS clean-generic cscopelist-am ctags ctags-am \
	distclean distclean-compile distclean-generic distclean-tags \
	distdir dvi dvi-am html html-am info info-am install \
	install-am install-binPROGRAMS install-data install-data-am \
	install-dvi install-dvi-am install-exec install-exec-am \
	install-html install-html-am install-info install-info-am \
	install-man install-pdf install-pdf-am install-ps \
	install-ps-am install-strip installcheck installcheck-am \
	installdirs maintainer-clean maintainer-clean-generic \
	mostlyclean mostlyclean-compile mostlyclean-generic pdf pdf-am \
	ps ps-am recheck tags tags-am uninstall uninstall-am \
	uninstall-binPROGRAMS

.PRECIOUS: Makefile


test_1.sh:
	echo "./3D_CMCC_Forest_Model -i ../input -o ../output/output.txt -d ../input/1526_962_input.txt -m ../input/1526_962_2007.txt,../input/1526_962_2008.txt -s ../input/site.txt -c ../input/settings.txt" > test_1.sh
	chmod +x test_1.sh

# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
