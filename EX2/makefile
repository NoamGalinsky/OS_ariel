SUBDIRS = stage1 stage2 stage3 stage4 stage5 stage6

.PHONY: all clean coverage $(SUBDIRS)

all:
		for dir in $(SUBDIRS); do $(MAKE) -C $$dir; done

clean:
	for dir in $(SUBDIRS); do $(MAKE) -C $$dir clean; done

coverage:
	for dir in $(SUBDIRS); do $(MAKE) -C $$dir coverage;done