## Process this file with automake to produce Makefile.in

AM_CFLAGS = -Wall \
            @OGG_CFLAGS@

INCLUDES = -I$(top_srcdir)/include

SKELETONDIR = ../src
SKELETON_LIBS = $(SKELETONDIR)/libskeleton.la @OGG_LIBS@

# Tools

bin_PROGRAMS = skeleton-apply skeleton-info

skeleton_apply_SOURCE = skeleton-apply.c
skeleton_apply_LDADD = $(SKELETON_LIBS)
skeleton_info_SOURCES = skeleton-info.c
skeleton_info_LDADD = $(SKELETON_LIBS)
