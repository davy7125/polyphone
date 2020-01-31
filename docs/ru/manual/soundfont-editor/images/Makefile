PDFs := $(patsubst %.svg,%.pdf,$(wildcard ./*.svg))
DPI ?= 270

all: pdf

pdf: $(PDFs)

%.pdf: %.svg
	echo Convert $< to $@ using inkscape
	inkscape -z -f $< -d $(DPI) -A $@

clean:
	@rm ./*.pdf

.PHONY: all pdf clean
