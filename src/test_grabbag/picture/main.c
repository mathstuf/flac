/* test_picture - Simple tester for cuesheet routines in grabbag
 * Copyright (C) 2006  Josh Coalson
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#if HAVE_CONFIG_H
#  include <config.h>
#endif

#include <string.h>
#include "FLAC/assert.h"
#include "share/grabbag.h"

typedef struct {
	const char *path;
	const char *mime_type;
	const char *description;
	FLAC__uint32 width;
	FLAC__uint32 height;
	FLAC__uint32 depth;
	FLAC__uint32 colors;
} PictureFile;

PictureFile picturefiles[] = {
	{ "0.gif", "image/gif" , "", 24, 24, 24, 2 },
	{ "1.gif", "image/gif" , "", 12,  8, 24, 256 },
	{ "2.gif", "image/gif" , "", 16, 14, 24, 128 },
	{ "0.jpg", "image/jpeg", "", 30, 20,  8, 0 },
	{ "4.jpg", "image/jpeg", "", 31, 47, 24, 0 },
	{ "0.png", "image/png" , "", 30, 20,  8, 0 },
	{ "1.png", "image/png" , "", 30, 20,  8, 0 },
	{ "2.png", "image/png" , "", 30, 20, 24, 7 },
	{ "3.png", "image/png" , "", 30, 20, 24, 7 },
	{ "4.png", "image/png" , "", 31, 47, 24, 0 },
	{ "5.png", "image/png" , "", 31, 47, 24, 0 },
	{ "6.png", "image/png" , "", 31, 47, 24, 23 },
	{ "7.png", "image/png" , "", 31, 47, 24, 23 },
	{ "8.png", "image/png" , "", 32, 32, 32, 0 }
};

static FLAC__bool debug_ = false;

static FLAC__bool failed_(const char *msg)
{
    if(msg)
        printf("FAILED, %s\n", msg);
    else
        printf("FAILED\n");

    return false;
}

static FLAC__bool test_one_picture(const char *prefix, const PictureFile *pf, const char *res)
{
	FLAC__StreamMetadata *obj;
	const char *error;
	char s[4096];
	snprintf(s, sizeof(s)-1, "%s|%s|%s|%s/%s", pf->mime_type, pf->description, res, prefix, pf->path);

	printf("testing grabbag__picture_parse_specification(\"%s\")... ", s);
	if(0 == (obj = grabbag__picture_parse_specification(s, &error)))
		return failed_(error);
	if(debug_) {
		printf("\nmime_type=%s\ndescription=%s\nwidth=%u\nheight=%u\ndepth=%u\ncolors=%u\ndata_length=%u\n",
			obj->data.picture.mime_type,
			obj->data.picture.description,
			obj->data.picture.width,
			obj->data.picture.height,
			obj->data.picture.depth,
			obj->data.picture.colors,
			obj->data.picture.data_length
		);
	}
	if(strcmp(obj->data.picture.mime_type, pf->mime_type))
		return failed_("picture MIME type mismatch");
	if(strcmp((const char *)obj->data.picture.description, (const char *)pf->description))
		return failed_("picture description mismatch");
	if(obj->data.picture.width != pf->width)
		return failed_("picture width mismatch");
	if(obj->data.picture.height != pf->height)
		return failed_("picture height mismatch");
	if(obj->data.picture.depth != pf->depth)
		return failed_("picture depth mismatch");
	if(obj->data.picture.colors != pf->colors)
		return failed_("picture colors mismatch");
	printf("OK\n");
	FLAC__metadata_object_delete(obj);
	return true;
}

static FLAC__bool do_picture(const char *prefix)
{
	FLAC__StreamMetadata *obj;
	const char *error;
	size_t i;

    printf("\n+++ grabbag unit test: picture\n\n");

	/* invalid spec: no filename */
	printf("testing grabbag__picture_parse_specification(\"\")... ");
	if(0 != (obj = grabbag__picture_parse_specification("", &error)))
		return failed_("expected error, got object");
	printf("OK (failed as expected, error: %s)\n", error);

	/* invalid spec: no filename */
	printf("testing grabbag__picture_parse_specification(\"|||\")... ");
	if(0 != (obj = grabbag__picture_parse_specification("|||", &error)))
		return failed_("expected error, got object");
	printf("OK (failed as expected: %s)\n", error);

	/* invalid spec: no filename */
	printf("testing grabbag__picture_parse_specification(\"image/gif|||\")... ");
	if(0 != (obj = grabbag__picture_parse_specification("image/gif|||", &error)))
		return failed_("expected error, got object");
	printf("OK (failed as expected: %s)\n", error);

	/* invalid spec: bad resolution */
	printf("testing grabbag__picture_parse_specification(\"image/gif|desc|320|0.gif\")... ");
	if(0 != (obj = grabbag__picture_parse_specification("image/gif|desc|320|0.gif", &error)))
		return failed_("expected error, got object");
	printf("OK (failed as expected: %s)\n", error);

	/* invalid spec: bad resolution */
	printf("testing grabbag__picture_parse_specification(\"image/gif|desc|320x240|0.gif\")... ");
	if(0 != (obj = grabbag__picture_parse_specification("image/gif|desc|320x240|0.gif", &error)))
		return failed_("expected error, got object");
	printf("OK (failed as expected: %s)\n", error);

	/* invalid spec: no filename */
	printf("testing grabbag__picture_parse_specification(\"image/gif|desc|320x240x9|\")... ");
	if(0 != (obj = grabbag__picture_parse_specification("image/gif|desc|320x240x9|", &error)))
		return failed_("expected error, got object");
	printf("OK (failed as expected: %s)\n", error);

	/* invalid spec: #colors exceeds color depth */
	printf("testing grabbag__picture_parse_specification(\"image/gif|desc|320x240x9/2345|0.gif\")... ");
	if(0 != (obj = grabbag__picture_parse_specification("image/gif|desc|320x240x9/2345|0.gif", &error)))
		return failed_("expected error, got object");
	printf("OK (failed as expected: %s)\n", error);

	/* invalid spec: need resolution for linked URL */
	printf("testing grabbag__picture_parse_specification(\"-->|desc||http://blah.blah.blah/z.gif\")... ");
	if(0 != (obj = grabbag__picture_parse_specification("-->|desc||http://blah.blah.blah/z.gif", &error)))
		return failed_("expected error, got object");
	printf("OK (failed as expected: %s)\n", error);

	printf("testing grabbag__picture_parse_specification(\"-->|desc|320x240x9|http://blah.blah.blah/z.gif\")... ");
	if(0 == (obj = grabbag__picture_parse_specification("-->|desc|320x240x9|http://blah.blah.blah/z.gif", &error)))
		return failed_(error);
	printf("OK\n");

	/* test automatic parsing of picture files to get resolution/color info */
	for(i = 0; i < sizeof(picturefiles)/sizeof(picturefiles[0]); i++)
		if(!test_one_picture(prefix, picturefiles+i, ""))
			return false;

	picturefiles[0].width = 320;
	picturefiles[0].height = 240;
	picturefiles[0].depth = 3;
	picturefiles[0].colors = 2;
	if(!test_one_picture(prefix, picturefiles+0, "320x240x3/2"))
		return false;

	return true;
}

int main(int argc, char *argv[])
{
	const char *usage = "usage: test_pictures path_prefix\n";

	if(argc > 1 && 0 == strcmp(argv[1], "-h")) {
		printf(usage);
		return 0;
	}

	if(argc != 2) {
		fprintf(stderr, usage);
		return 255;
	}

	return do_picture(argv[1])? 0 : 1;
}