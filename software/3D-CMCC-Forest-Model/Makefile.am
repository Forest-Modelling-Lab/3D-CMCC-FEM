SUBDIRS = src

EXTRA_DIST = doc/*_User_Guide.txt bin input output

dist-hook:
	cp src/3D_CMCC_Forest_Model $(distdir)/bin
	rm -rf $(distdir)/src\
		$(distdir)/aclocal.m4\
		$(distdir)/config.h.in\
		$(distdir)/configure.ac\
		$(distdir)/Makefile.am\
		$(distdir)/missing\
		$(distdir)/configure\
		$(distdir)/depcomp\
		$(distdir)/install-sh\
		$(distdir)/Makefile.in\
		$(distdir)/input/input.txt\
		$(distdir)/input/Meteo.txt\
		$(distdir)/input/species.txt\
		$(distdir)/output/output.txt
