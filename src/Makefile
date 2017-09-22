MUD			= smud
CC			= gcc -Wall
BIN			= ./
.OBJDIR		= ./junk

#
# Specific flags according to your operating system
#

UNAME = FREEBSD
#UNAME != perl -e'print uc(`uname`)'
OS = -D$(UNAME)

OBJECTS = 	atm.o \
		auto.o \
		combat.o \
		commun.o \
		custom.o \
		debate.o \
		editor.o \
		guilds.o \
		home.o \
		hostname.o \
		info.o \
		init.o \
		lists.o \
		log.o \
		login.o \
		mail.o \
		mccp.o \
		memory.o \
		misc.o \
		mobs.o \
		news.o \
		objs.o \
		pfiles.o \
		reboot.o \
		rooms.o \
		script.o \
		scroll.o \
		settings.o \
		shop.o \
		sign.o \
		signals.o \
		skills.o \
		smud.o \
		socket.o \
		transport.o \
		use.o \
		weather.o \
		wiz.o \
		write.o

#.ifdef ${UNAME} == FREEBSD
#.if !exists(/usr/lib/liblthread.a) && !exists(/usr/local/lib/liblthread.a)
#.error You must first install Linux Threads (/usr/ports/devel/linuxthreads)
#.endif
#.endif

# ==========================================================================
#

#
# Set up compilation flags
#

#
CFLAGS	= -I./include -I../libmff -O3 -ggdb3
LDFLAGS	= -L/usr/local/lib -L../libmff -ggdb3
.ifdef ${UNAME} == LINUX
LIBS	= -lpthread -lm -lcrypt -lmff -lz -O3
.else
LIBS	= -lpthread -lm -lcrypt -lmff -lz -O3 -L/usr/local/lib/pth
.endif

CFLAGS	+= $(OS)

#
# Defaults
#

default:        all

#
# Options and actions
#

all: versioning startperc $(MUD)
	
$(MUD): $(OBJECTS) Makefile
# lets do the versioning
	$(CC) $(LDFLAGS) -o $(MUD) $(OBJECTS) $(LIBS)
#	@-rm -f .perc
	@-printf "\n\nSmud is now built.\n"

startperc:
#	@-bash ./perc.sh
versioning:
	@-./dover.sh
clean:
	@-rm -f $(MUD) *.o

.c.o:	Makefile include/smud.h include/config.h include/events.h
	$(CC) $(CFLAGS) $(DEFS) -c $*.c -o $*.o
#	@-bash ./updateperc.sh
