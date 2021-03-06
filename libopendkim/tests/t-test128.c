/*
**  Copyright (c) 2005-2008 Sendmail, Inc. and its suppliers.
**    All rights reserved.
**
**  Copyright (c) 2009, 2011, 2012, The Trusted Domain Project.
**    All rights reserved.
*/

#include "build-config.h"

/* system includes */
#include <sys/types.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

#ifdef USE_GNUTLS
# include <gnutls/gnutls.h>
#endif /* USE_GNUTLS */

/* libopendkim includes */
#include "../dkim.h"
#include "t-testdata.h"

#define	MAXHEADER	4096

#ifndef TRUE
# define TRUE		1
#endif /* ! TRUE */

#define SIG2 "v=1; a=rsa-sha1; c=relaxed/simple; d=example.com; s=test;\r\n\tt=1172620939; bh=ll/0h2aWgG+D3ewmE4Y3pY7Ukz8=;\r\n\th=Received:Received:Received:From:To:Date:Subject:Message-ID;\r\n\tb=Q4G/ki/5soDXGxs43JfV+qEKDr5X3GgTDNeZqWL3zLLC5DXWWzmnKRcU8NH4Wsfkh\r\n\t o5tMo4NRmqnB2eZtozsyXdHo2ekUPLxuAQJomM4JHaPTfsraHwkibQIkPpW5hf/Rc2\r\n\t 0QgP48iQBjxqcOSn/Vwk5QDup4Qj1vgOxBqTqwdg="

/*
**  MAIN -- program mainline
**
**  Parameters:
**  	The usual.
**
**  Return value:
**  	Exit status.
*/

int
main(int argc, char **argv)
{
#ifdef TEST_KEEP_FILES
	u_int flags;
#endif /* TEST_KEEP_FILES */
	DKIM_STAT status;
	uint64_t fixed_time;
	dkim_sigkey_t key;
	DKIM *dkim;
	DKIM *resign;
	DKIM_LIB *lib;
	dkim_query_t qtype = DKIM_QUERY_FILE;
	unsigned char hdr[MAXHEADER + 1];
	unsigned char hdr2[MAXHEADER + 1];

	key = KEY;

#ifdef USE_GNUTLS
	(void) gnutls_global_init();
#endif /* USE_GNUTLS */

	/* instantiate the library */
	lib = dkim_init(NULL, NULL);
	assert(lib != NULL);

	if (!dkim_libfeature(lib, DKIM_FEATURE_RESIGN))
	{
		printf("*** relaxed/simple rsa-sha1 re-signing with header binding SKIPPED\n");
		dkim_close(lib);
		return 0;
	}

	printf("*** relaxed/simple rsa-sha1 re-signing with header binding\n");

#ifdef TEST_KEEP_FILES
	/* set flags */
	flags = (DKIM_LIBFLAGS_TMPFILES|DKIM_LIBFLAGS_KEEPFILES);
	(void) dkim_options(lib, DKIM_OP_SETOPT, DKIM_OPTS_FLAGS, &flags,
	                    sizeof flags);
#endif /* TEST_KEEP_FILES */

	/* test mode */
	(void) dkim_options(lib, DKIM_OP_SETOPT, DKIM_OPTS_QUERYMETHOD,
	                    &qtype, sizeof qtype);
	(void) dkim_options(lib, DKIM_OP_SETOPT, DKIM_OPTS_QUERYINFO,
	                    KEYFILE, strlen(KEYFILE));

	/* fix signing time */
	fixed_time = 1172620939;
	(void) dkim_options(lib, DKIM_OP_SETOPT, DKIM_OPTS_FIXEDTIME,
	                    &fixed_time, sizeof fixed_time);

	/* restrict signed headers */
	(void) dkim_options(lib, DKIM_OP_SETOPT, DKIM_OPTS_SIGNHDRS,
	                    dkim_should_signhdrs, sizeof(u_char **));

	dkim = dkim_verify(lib, JOBID, NULL, &status);
	assert(dkim != NULL);

	snprintf(hdr, sizeof hdr, "%s: %s", DKIM_SIGNHEADER, SIG2);
	status = dkim_header(dkim, hdr, strlen(hdr));
	assert(status == DKIM_STAT_OK);

	status = dkim_header(dkim, HEADER01, strlen(HEADER01));
	assert(status == DKIM_STAT_OK);

	status = dkim_header(dkim, HEADER02, strlen(HEADER02));
	assert(status == DKIM_STAT_OK);

	status = dkim_header(dkim, HEADER03, strlen(HEADER03));
	assert(status == DKIM_STAT_OK);

	status = dkim_header(dkim, HEADER04, strlen(HEADER04));
	assert(status == DKIM_STAT_OK);

	status = dkim_header(dkim, HEADER05, strlen(HEADER05));
	assert(status == DKIM_STAT_OK);

	status = dkim_header(dkim, HEADER06, strlen(HEADER06));
	assert(status == DKIM_STAT_OK);

	status = dkim_header(dkim, HEADER07, strlen(HEADER07));
	assert(status == DKIM_STAT_OK);

	status = dkim_header(dkim, HEADER08, strlen(HEADER08));
	assert(status == DKIM_STAT_OK);

	status = dkim_header(dkim, HEADER09, strlen(HEADER09));
	assert(status == DKIM_STAT_OK);

	resign = dkim_sign(lib, JOBID, NULL, key, SELECTOR, DOMAIN,
	                   DKIM_CANON_RELAXED, DKIM_CANON_SIMPLE,
	                   DKIM_SIGN_RSASHA1, -1L, &status);
	assert(resign != NULL);

	status = dkim_resign(dkim, resign, TRUE);
	assert(status == DKIM_STAT_INVALID);

	status = dkim_resign(resign, dkim, TRUE);
	assert(status == DKIM_STAT_OK);

	status = dkim_eoh(dkim);
	assert(status == DKIM_STAT_OK);

	status = dkim_eoh(resign);
	assert(status == DKIM_STAT_INVALID);

	status = dkim_body(dkim, BODY00, strlen(BODY00));
	assert(status == DKIM_STAT_OK);

	status = dkim_body(resign, BODY00, strlen(BODY00));
	assert(status == DKIM_STAT_INVALID);

	status = dkim_body(dkim, BODY01, strlen(BODY01));
	assert(status == DKIM_STAT_OK);

	status = dkim_body(dkim, BODY01A, strlen(BODY01A));
	assert(status == DKIM_STAT_OK);
	status = dkim_body(dkim, BODY01B, strlen(BODY01B));
	assert(status == DKIM_STAT_OK);
	status = dkim_body(dkim, BODY01C, strlen(BODY01C));
	assert(status == DKIM_STAT_OK);
	status = dkim_body(dkim, BODY01D, strlen(BODY01D));
	assert(status == DKIM_STAT_OK);
	status = dkim_body(dkim, BODY01E, strlen(BODY01E));
	assert(status == DKIM_STAT_OK);

	status = dkim_body(dkim, BODY02, strlen(BODY02));
	assert(status == DKIM_STAT_OK);

	status = dkim_body(dkim, BODY03, strlen(BODY03));
	assert(status == DKIM_STAT_OK);

	status = dkim_body(dkim, BODY04, strlen(BODY04));
	assert(status == DKIM_STAT_OK);

	status = dkim_body(dkim, BODY03, strlen(BODY03));
	assert(status == DKIM_STAT_OK);

	status = dkim_body(dkim, BODY03, strlen(BODY03));
	assert(status == DKIM_STAT_OK);

	status = dkim_body(dkim, BODY05, strlen(BODY05));
	assert(status == DKIM_STAT_OK);

	status = dkim_body(dkim, BODY03, strlen(BODY03));
	assert(status == DKIM_STAT_OK);

	status = dkim_body(dkim, BODY03, strlen(BODY03));
	assert(status == DKIM_STAT_OK);

	status = dkim_eom(resign, NULL);
	assert(status == DKIM_STAT_INVALID);

	status = dkim_eom(dkim, NULL);
	assert(status == DKIM_STAT_OK);

	status = dkim_eom(resign, NULL);
	assert(status == DKIM_STAT_OK);

	memset(hdr, '\0', sizeof hdr);
	status = dkim_getsighdr(resign, hdr, sizeof hdr,
	                        strlen(DKIM_SIGNHEADER) + 2);
	assert(status == DKIM_STAT_OK);

	status = dkim_free(dkim);
	assert(status == DKIM_STAT_INVALID);

	status = dkim_free(resign);
	assert(status == DKIM_STAT_OK);

	status = dkim_free(dkim);
	assert(status == DKIM_STAT_OK);

	/* now see if that one was valid */
	dkim = dkim_verify(lib, JOBID, NULL, &status);
	assert(dkim != NULL);

	snprintf(hdr2, sizeof hdr2, "%s: %s", DKIM_SIGNHEADER, hdr);
	status = dkim_header(dkim, hdr2, strlen(hdr2));
	assert(status == DKIM_STAT_OK);

	snprintf(hdr2, sizeof hdr2, "%s: %s", DKIM_SIGNHEADER, SIG2);
	status = dkim_header(dkim, hdr2, strlen(hdr2));
	assert(status == DKIM_STAT_OK);

	status = dkim_header(dkim, HEADER01, strlen(HEADER01));
	assert(status == DKIM_STAT_OK);

	status = dkim_header(dkim, HEADER02, strlen(HEADER02));
	assert(status == DKIM_STAT_OK);

	status = dkim_header(dkim, HEADER03, strlen(HEADER03));
	assert(status == DKIM_STAT_OK);

	status = dkim_header(dkim, HEADER04, strlen(HEADER04));
	assert(status == DKIM_STAT_OK);

	status = dkim_header(dkim, HEADER05, strlen(HEADER05));
	assert(status == DKIM_STAT_OK);

	status = dkim_header(dkim, HEADER06, strlen(HEADER06));
	assert(status == DKIM_STAT_OK);

	status = dkim_header(dkim, HEADER07, strlen(HEADER07));
	assert(status == DKIM_STAT_OK);

	status = dkim_header(dkim, HEADER08, strlen(HEADER08));
	assert(status == DKIM_STAT_OK);

	status = dkim_header(dkim, HEADER09, strlen(HEADER09));
	assert(status == DKIM_STAT_OK);

	status = dkim_eoh(dkim);
	assert(status == DKIM_STAT_OK);

	status = dkim_body(dkim, BODY00, strlen(BODY00));
	assert(status == DKIM_STAT_OK);

	status = dkim_body(dkim, BODY01, strlen(BODY01));
	assert(status == DKIM_STAT_OK);

	status = dkim_body(dkim, BODY01A, strlen(BODY01A));
	assert(status == DKIM_STAT_OK);
	status = dkim_body(dkim, BODY01B, strlen(BODY01B));
	assert(status == DKIM_STAT_OK);
	status = dkim_body(dkim, BODY01C, strlen(BODY01C));
	assert(status == DKIM_STAT_OK);
	status = dkim_body(dkim, BODY01D, strlen(BODY01D));
	assert(status == DKIM_STAT_OK);
	status = dkim_body(dkim, BODY01E, strlen(BODY01E));
	assert(status == DKIM_STAT_OK);

	status = dkim_body(dkim, BODY02, strlen(BODY02));
	assert(status == DKIM_STAT_OK);

	status = dkim_body(dkim, BODY03, strlen(BODY03));
	assert(status == DKIM_STAT_OK);

	status = dkim_body(dkim, BODY04, strlen(BODY04));
	assert(status == DKIM_STAT_OK);

	status = dkim_body(dkim, BODY03, strlen(BODY03));
	assert(status == DKIM_STAT_OK);

	status = dkim_body(dkim, BODY03, strlen(BODY03));
	assert(status == DKIM_STAT_OK);

	status = dkim_body(dkim, BODY05, strlen(BODY05));
	assert(status == DKIM_STAT_OK);

	status = dkim_body(dkim, BODY03, strlen(BODY03));
	assert(status == DKIM_STAT_OK);

	status = dkim_body(dkim, BODY03, strlen(BODY03));
	assert(status == DKIM_STAT_OK);

	status = dkim_eom(dkim, NULL);
	assert(status == DKIM_STAT_OK);

	status = dkim_free(dkim);
	assert(status == DKIM_STAT_OK);

	dkim_close(lib);

	return 0;
}
