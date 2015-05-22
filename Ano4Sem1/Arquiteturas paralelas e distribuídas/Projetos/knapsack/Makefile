CC=gcc
UPCC=upcc

CFLAGS=
CPPFLAGS=-I.
LIBS=

# Using UDP
UPCFLAGS=
UPCSRCS=knap.upc timer.c
UPCDEPS=$(UPCSRCS) knap_defaults.h timer.h

all: knap-smp knap

knap.o: knap.c knap_defaults.h timer.h
timer.o: timer.c timer.h

knap-smp knap-gm knap-lapi knap-udp: $(UPCDEPS)

knap: knap.o timer.o
	$(CC) -o $@ $^

# SMP pthreads version
knap-smp:
	$(UPCC) -o $@ --network=smp --pthreads=8 $(UPCFLAGS) $(UPCSRCS)

# Myrinet version (on some CITRIS nodes)
knap-gm:
	$(UPCC) -o $@ --network=gm $(UPCFLAGS) $(UPCSRCS)

# IBM SP network version (on Seaborg)
knap-lapi:
	$(UPCC) -o $@ --network=lapi $(UPCFLAGS) $(UPCSRCS)

# Infiniband network version (on Jacquard)
knap-vapi:
	$(UPCC) -o $@ --network=vapi $(UPCFLAGS) $(UPCSRCS)


# MPI version
knap-mpi:
	$(UPCC) -o $@ --network=mpi $(UPCFLAGS) $(UPCSRCS)

# Network UDP version (on ethernet connected nodes)
knap-udp:
	$(UPCC) -o $@ --network=udp $(UPCFLAGS) $(UPCSRCS)

clean:
	rm -f knap knap-smp knap-gm knap-lapi knap-udp *.o *.uo 
	rm -rf knap-*-link

%.o: %.c
	$(CC) -c -o $@ $(CFLAGS) $(CPPFLAGS) $<

.PHONY: clean all deps 

