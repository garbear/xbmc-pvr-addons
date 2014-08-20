if ADDON_MYTHTV
          ADDON_MYTHTV_SUBDIRS = pvr.mythtv.cmyth
endif
if ADDON_IPTVSIMPLE
          ADDON_IPTVSIMPLE_SUBDIRS = pvr.iptvsimple
endif

SUBDIRS = pvr.demo \
          pvr.hts \
          pvr.vdr.vnsi

clean:
	-rm -f *.zip
	for d in $(SUBDIRS); do if test -f $$d/Makefile; then $(MAKE) -C $$d clean; fi; done

zip:
	for d in $(SUBDIRS); do if test -f $$d/Makefile; then $(MAKE) -C $$d zip; fi; done

install:
	for d in $(SUBDIRS); do if test -f $$d/Makefile; then $(MAKE) -C $$d install; fi; done

