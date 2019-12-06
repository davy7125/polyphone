LINGUAS = en ru
TARGETS = all html clean

$(TARGETS):
	@for dir in $(LINGUAS); do \
	    make -C $$dir $@; \
	done

.PHONY : all clean
