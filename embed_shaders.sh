#!/usr/bin/env sh

# Script to embed the shaders (or any file, really) into a header,
# because I found it dumb that xxd uses unsigned char even though
# literally nothing takes unsigned char, so it's essentially useless.

EMBED_PATH=./src/include/embedded_shaders.h
TEMP_EMBED_PATH=./src/include/_embedded_shaders.h

touch $TEMP_EMBED_PATH

echo "#ifndef EMBEDDED_HEADER_H_" > $TEMP_EMBED_PATH
echo "#define EMBEDDED_HEADER_H_" >> $TEMP_EMBED_PATH

for shader in ./shaders/*; do
	# Wrap file contents with quotations and add C newlines.
	shader_data=$(sed "s/^/\"/" $shader | sed "s/$/\\\\\\\n\"/")
	# Lmao. What a mess.

	# Convert file name to valid C syntax.
	shader_c_name="embedded_shader_$(basename $shader | tr -d "[:space:]" | sed "s/\./_/")"

	echo "static const char *$shader_c_name =" >> $TEMP_EMBED_PATH
	echo "$shader_data;" >> $TEMP_EMBED_PATH
done

echo "#endif // EMBEDDED_HEADER_H_" >> $TEMP_EMBED_PATH

mv $TEMP_EMBED_PATH $EMBED_PATH
