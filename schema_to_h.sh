#!/bin/bash
(echo -n 'const char *sqlschema = "'; cat schema.sql | hexdump -v -e '/1 "@x%02x"' | tr @ '\\'; echo '";') > schema.h.tmp
if [ ! "`md5sum -b schema.h | cut -d' ' -f1`" = "`md5sum -b schema.h.tmp | cut -d' ' -f1`" ]; then
	echo "gen"
	mv -f schema.h.tmp schema.h
else
	rm schema.h.tmp
fi
