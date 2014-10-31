OBJS := cursor.h dialog.h font_large_dat.h font_large.h font_mono_dat.h font_mono.h font_normal_dat.h font_normal.h message.h

all: $(OBJS)

%.h: %.bmp
	ConvertImg --format=ngl $^ $@

%.h: %.png
	ConvertImg --format=ngl $^ $@

%_dat.h: %.dat
	xxd -i $^ > $@

clean:
	rm -f $(OBJS)
