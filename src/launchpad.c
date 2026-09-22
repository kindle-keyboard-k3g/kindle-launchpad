/*
* Copyright (C) 2010 Andy M. aka h1uke	h1ukeguy @ gmail.com
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/


#include <sys/types.h>
#include <aio.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>

#include <sys/time.h>

#include "launchpad.h"
#include "inifile.h"
#include "asciitab.h"
#include "keydefs.h"

#include "statusbar.h"

#include "screenshot.h"
#include "rce.h"

extern void send_key(short code, int mode) ;

static char kbd_input[MAXPATH] = KEYBOARD_INPUT ;
static char fw_input[MAXPATH] = FIVEWAY_INPUT ;
static char k3_vol_input[MAXPATH] = K3_VOL_INPUT ;

static dbdesc_t *pdb = NULL ;

static int introducer = KPKEY_SHIFT ;	/* Shift key on Kindle DXG */
static int trailer = KPKEY_ENTER ;		/* Enter key on Kindle DXG */
static int hot_interval = 500 ; /* 500 msec by default */
static int key_delay = 300 ;	/* delay between consequtive kindle keys, when sent */
static char script_dir[MAXPATH] ;

/*
 * Note: names starting with 'input_key' refer to the input devices. Other uses of the
 * word 'key' here refer to a key/value pair in .ini configuration files
 */

static int n_input_keys = 0 ;

/*
 * each element corresponding to a particular input key code
 * contains corresponding index within the keys structure
 */
static int input_key_codes[MAXCODE+1] ;

static struct input_key input_keys[MAXKEYS] ;


static int nactions = 0 ;
static struct action *pactions = NULL ;

static fbscreen_t *pfbscreen = NULL ;

/*
 * look for the specified key in a section at given index.
 * If found, copies the key value to a specified buffer and returns
 * length of it. Otherwise returns 0
 */
static int retrieve_key_value(dbdesc_t *pdb, int s_idx, char *pkey, char *pbuf)
{
	int k_idx ;
	int rc = 0 ;

	k_idx = inidb_find_key(pdb, s_idx, pkey) ;
	if (k_idx >= 0)
	{
		rc = inidb_get_key_value(pdb, s_idx, k_idx, pbuf) ;
#if 0
		if (rc > 0)
			DIAG_PRINT("Section #%d: %s=%s , %d bytes\n", s_idx, pkey, pbuf, rc) ;
		else
			DIAG_PRINT("Section #%d: -- '%s' has empty value\n", s_idx, pkey) ;
#endif
	}
#if 0
	else
		DIAG_PRINT("Section #%d: -- '%s' key not found\n", s_idx, pkey) ;
#endif

	return rc ;
}

/*
 * looks for the given input key name. Returns its code, if found. Returns -1 otherwise
 * Uses global statc data structures ...
 */
static int find_input_key_code(char *pkey)
{
	int rc = -1 ;
	int i ;

	for (i = 0; i < n_input_keys; i++)
	{
		if ((strcasecmp(input_keys[i].input_key_name, pkey)) == 0)
		{
			rc = input_keys[i].input_key_code ;
			break ;
		}
	}

	return rc ;
}

static int parse_action_sequence(char *pbuf, struct action *pact)
{
	int rc = 0 ;
	char *saveptr, *p ;
	char tmpbuf[MAXPATH] ;
	int i ;
	int code ;

	saveptr = NULL ;

	strcpy(tmpbuf, pbuf) ;

	pact->len = 0 ;

	for (i = 0; i < MAXSEQ; i++)
	{
		p = strtok_r((i == 0) ? tmpbuf : NULL, " ", &saveptr ) ;
		if (p)
		{
			if ((code = find_input_key_code(p)) != -1)
			{
				pact->sequence[pact->len] = code ;
				pact->len += 1 ;
			}
			else
			{
				DIAG_PRINT("%s key not found -- bad sequence %s\n", p, pbuf) ;
				pact->len = 0 ;
				break ;
			}
		}
		else
			break ;
	}

	if ((pact->len) > 0)
	{
		strncpy(&pact->dispname[0], pbuf, sizeof(pact->dispname) - 1) ;
	}

	rc = pact->len ;

	return rc ;
}

static int parse_action_command(char *pbuf, struct action *pact)
{
	int rc = 0 ;
	char *p ;

	p = pbuf ;

	switch(*p)
	{
		case '!':
			pact->type = ACTION_SHELL_COMMAND ;
			++p ;
			break ;
		case '#':
			pact->type = ACTION_KINDLE_KEYS ;
			++p ;
			break ;
		case '@':
			pact->type = ACTION_KINDLE_SCRIPT ;
			++p ;
			break ;
		case '\\':
			++p ;
		default:
			pact->type = ACTION_SENDKEYS ;
			break ;
	}

	if ((pact->type == ACTION_KINDLE_SCRIPT) && (*p != '/')) 		/* prepend script directory if relative path specified */
			sprintf(pact->command, "%s/%s", script_dir, p) ;
	else
		strcpy(pact->command, p) ;

	rc = strlen(pact->command) ;

	return rc ;
}

static int process_ini_file(char *path)
{
	int rc = -1 ;
	int s_idx ;
	char tmpbuf[MAXPATH] ;
	char *p ;
	int i , n, len, code ;

	DIAG_PRINT(".. processing %s\n", path) ;

	if ( (pdb = inidb_open(path)) != NULL)
	{
		p = "Settings" ;
		if ((s_idx = inidb_find_section(pdb, p)) >= 0)
		{
			if ((len = retrieve_key_value(pdb, s_idx, "Introducer", tmpbuf)) > 0)
			{
				if ((code = find_input_key_code(tmpbuf)) != -1)
				{
					introducer = code ;
				}
				else
					DIAG_PRINT("Warning: no code for %s input key\n", tmpbuf) ;
			}
			if ((len = retrieve_key_value(pdb, s_idx, "Trailer", tmpbuf)) > 0)
			{
				if ((code = find_input_key_code(tmpbuf)) != -1)
				{
					trailer = code ;
				}
				else
				{
					DIAG_PRINT("Warning: no code for %s input key\n", tmpbuf) ;
					trailer = -1 ;
				}
			}
			if ((len = retrieve_key_value(pdb, s_idx, "HotInterval", tmpbuf)) > 0)
			{
				hot_interval = atoi(tmpbuf) ;
			}
			if ((len = retrieve_key_value(pdb, s_idx, "ScriptDirectory", tmpbuf)) > 0)
			{
				strncpy(script_dir, tmpbuf, (len < MAXPATH) ? len : MAXPATH-1) ;
			}
			if ((len = retrieve_key_value(pdb, s_idx, "KeyboardInput", tmpbuf)) > 0)
			{
				strncpy(kbd_input, tmpbuf, (len < MAXPATH) ? len : MAXPATH-1) ;
			}
			if ((len = retrieve_key_value(pdb, s_idx, "FivewayInput", tmpbuf)) > 0)
			{
				strncpy(fw_input, tmpbuf, (len < MAXPATH) ? len : MAXPATH-1) ;
			}
			if ((len = retrieve_key_value(pdb, s_idx, "InterKeyDelay", tmpbuf)) > 0)
			{
				key_delay = atoi(tmpbuf) ;
			}
		}

		p = "Actions" ;
		if ((s_idx = inidb_find_section(pdb, p)) >= 0)
		{
			n =inidb_get_number_of_keys(pdb, s_idx) ;

			if (n > 0)
			{
				void *ptmp = pactions ;
				if (ptmp == NULL)
				{
					ptmp = malloc(sizeof(struct action) * n) ;
					pactions = ptmp ;
				}
				else
				{
					ptmp = realloc(ptmp, sizeof(struct action) * (n+nactions)) ;
					if (ptmp)
						pactions = ptmp ;
				}

				if (ptmp != NULL)
				{
					for (i = 0; i < n; i++)
					{
						if ((inidb_get_key_name(pdb, s_idx, i, tmpbuf)) > 0)
						{
							memset(pactions+nactions, 0, sizeof(*pactions)) ;
							if ((parse_action_sequence(tmpbuf, pactions + nactions)) > 0)
							{
								int j ;
								struct action *pnew = (pactions + nactions) ;
								int found = 0 ;

								for (j = 0 ; j < nactions; j++)
								{
									if ((pactions+j)->len == pnew->len)
									{
										if (memcmp(&(pactions+j)->sequence, &pnew->sequence, pnew->len * 2) == 0)
										{
											found = 1 ;
										}
									}
								}
								if (found == 0)
								{
									if ((inidb_get_key_value(pdb, s_idx, i, tmpbuf)) > 0)
									{
										if ((parse_action_command(tmpbuf, pactions + nactions)) > 0)
										{
											nactions += 1 ;
										}
										else
											DIAG_PRINT("%s:#%d -- cannot parse action command\n", p, i) ;
									}
									else
										DIAG_PRINT("%s:#%d -- cannot get key value\n", p, i) ;
								}
								else
									DIAG_PRINT("%s -- action already defined. Duplicate definition ignored\n", &tmpbuf[0]) ;
							}
							else
								DIAG_PRINT("%s:#%d -- cannot parse action sequence\n", p, i) ;
						}
						else
							DIAG_PRINT("%s:%d -- cannot get key name\n", p, i) ;
					}
					rc = 1 ;
				}
				else
				{
					DIAG_PRINT("cannot allocate memory for %d action structures\n", n) ;
					inidb_close(pdb) ;
					pdb = NULL ;
				}
			}
		}

		p = "RemoteCommandEntry" ;
		if ((s_idx = inidb_find_section(pdb, p)) >= 0)
		{
			if ((len = retrieve_key_value(pdb, s_idx, "AutoEnable", tmpbuf)) > 0)
			{
				set_rce_autoenable(&tmpbuf[0]) ;
			}
			if ((len = retrieve_key_value(pdb, s_idx, "enableRCE", tmpbuf)) > 0)
			{
				set_rce_enable_string(&tmpbuf[0]) ;
			}
			if ((len = retrieve_key_value(pdb, s_idx, "disableRCE", tmpbuf)) > 0)
			{
				set_rce_disable_string(&tmpbuf[0]) ;
			}
			if ((len = retrieve_key_value(pdb, s_idx, "Interface", tmpbuf)) > 0)
			{
				set_rce_interface(&tmpbuf[0]) ;
			}
			if ((len = retrieve_key_value(pdb, s_idx, "MulticastGroup", tmpbuf)) > 0)
			{
				set_rce_mcgroup(&tmpbuf[0]) ;
			}
			if ((len = retrieve_key_value(pdb, s_idx, "Port", tmpbuf)) > 0)
			{
				set_rce_port(&tmpbuf[0]) ;
			}
		}

		inidb_close(pdb) ;
		pdb = NULL ;
	}
	else
		DIAG_PRINT("%s -- not found or bad\n", path) ;

	return rc ;
}

int launchpad_init(char *path)
{
	int rc = -1 ;
    DIR *dirp;
    struct dirent *dp;
	char tmpbuf[MAXPATH] ;
	int i ;

	memset(input_keys, 0, sizeof(input_keys)) ;
	for ( i = 0; i <= MAXCODE; i++)
	{
		input_key_codes[i] = -1 ;
	}

	if ((n_input_keys = get_input_keydefs(&input_keys[0], &input_key_codes[0])) == 0)
		n_input_keys = get_default_input_keydefs(&input_keys[0], &input_key_codes[0]) ;

	set_rce_defaults() ;

    if ((dirp = opendir(path)) != NULL)
    {
        while ((dp = readdir(dirp)) != NULL)
        {
        	if (dp->d_type == DT_REG)
        	{
        		int n = strlen(dp->d_name) ;
        		if ((n > 3) && (strcmp(&dp->d_name[n-4], ".ini") == 0))
        		{
        			strcpy(tmpbuf, path) ;
        			if (tmpbuf[strlen(&tmpbuf[0])-1] != '/')
        				strcat(tmpbuf, "/") ;
        			strcat(&tmpbuf[0], &dp->d_name[0]) ;
        			rc = process_ini_file(&tmpbuf[0]) ;
        		}
        	}
        }

        rc = nactions ;

        pfbscreen = fbscreen_open() ;

    	closedir(dirp);
    	{
    		int i ;

    		DIAG_PRINT("*** action summary:\n") ;

    		for (i = 0; i < nactions ; i++)
    		{
    			DIAG_PRINT("%d: %s = %s\n", i+1, pactions[i].dispname, pactions[i].command) ;
    		}
    	}
    }
    else
    {
    	DIAG_PRINT("couldn't open %s for scanning\n", path);
    }

	return rc ;
}

void launchpad_deinit(void)
{
	if (pdb)
	{
		inidb_close(pdb) ;
		pdb = NULL ;
	}
	if (pactions)
	{
		free(pactions) ;
		pactions = NULL ;
	}
	nactions = 0 ;

	if (pfbscreen)
	{
		fbscreen_close(pfbscreen) ;
		pfbscreen = NULL ;
	}
}

/***************************************************/

char * input_key_name(int code)
{
	char *rc = NULL ;

	if ((code >=0) && (code <= MAXCODE))
	{
		if (input_key_codes[code] != -1)
			rc = &input_keys[input_key_codes[code]].input_key_name[0] ;
	}

	return rc ;
}


char * keyboard_events(void)
{
	return &kbd_input[0] ;
}
char * fiveway_events(void)
{
	return &fw_input[0] ;
}
char * k3_vol_events(void)
{
	return &k3_vol_input[0] ;
}

int inroducer_key_code(void)
{
	return introducer ;
}

int trailer_key_code(void)
{
	return trailer ;
}

int hot_interval_msec(void)
{
	return hot_interval ;
}

int key_delay_msec(void)
{
	return key_delay ;
}

struct action *find_sequence_action(short *pseq, int len)
{
	struct action *pa = NULL ;
	int i ;
	const short triple_shift[] = { KPKEY_SHIFT, KPKEY_SHIFT } ; // introducer Shift has been counted already

	pa = pactions ;

	for (i = 0; i < nactions; i++)
	{
		if ((pa->len == len) && (memcmp(pseq, &pa->sequence[0], len*sizeof(pa->sequence[0])) == 0))
		{
			break ;
		}
		++pa ;
	}

	if (i >= nactions)		/* return NULL if not found */
	{
		i = (sizeof(triple_shift)/sizeof(triple_shift[0])) ;

		if ((len == i) && (memcmp(&triple_shift[0], pseq, i*sizeof(*pseq)) == 0) )
		{
			pa = (struct action *) -1 ;
		}
		else
			pa = NULL ;
	}

	return pa ;
}

static char * get_kindle_script_file(char *path)
{
	int fd ;
	char *p = NULL ;
	unsigned long flen ;

	if ((fd = open(path, O_RDONLY)) != -1)
	{
		flen = lseek(fd, 0, SEEK_END) ;
		lseek(fd, 0, SEEK_SET) ;

		if ((p = malloc(flen+1)))
		{
			if (read(fd, p, flen) == flen)
			{
				*(p + flen) = '\0' ;
			}
			else
				DIAG_PRINT("Can't read %d bytes from kindle script file %s\n", (int)flen, path) ;
		}
		else
			DIAG_PRINT("Can't allocate %d bytes for kindle script file %s\n", (int)flen, path) ;
	}
	else
		DIAG_PRINT("Can't open kindle script file %s\n", path) ;

	return p;
}

static int process_kindle_script(char *p)
{
	char *saveptr, *ptok ;
	char tmpbuf[MAXPATH] ;
	char *tokens[64] ;
	int i, ntokens, mode ;
	short code ;
	int toskip = 0 ;

	int kdelay ;

	strcpy(tmpbuf, p) ;
	kdelay = key_delay_msec() ;

	/* very first token is always 'send_string' -- we don't need it */
	ptok = strtok_r(&tmpbuf[0], " \t\n", &saveptr ) ;

	ntokens = 0 ;

	while ((ptok = strtok_r(NULL, " \t\n", &saveptr)) != NULL)
	{
		tokens[ntokens++] = ptok ;
		if (ntokens >= sizeof(tokens)/sizeof(tokens[0]) )
		{
			break ;
		}
	}

	/* we also do not need the very last token */
	if (ntokens > 0)
		--ntokens ;

	for (i = 0; i < ntokens; i++)
	{
		ptok = tokens[i] ;

		switch(*ptok)
		{
			case 0x27:	/* apostrophe */
				++ptok ;
				*(ptok + strlen(ptok) -1) = '\0' ;
				mode = SENDMODE_NORMAL ;
				if ((code = find_input_key_code(ptok)) != -1)
				{
					if (strlen(ptok) == 1)
					{
						if ((*ptok >= 'A') && (*ptok <= 'Z'))
							mode = SENDMODE_SHIFT ;
					}
					send_key(code, mode) ;
				}
				else
				{
					toskip += send_kindle_input_key(*ptok) ;
				}
				msdelay(kdelay) ;
				break ;
			case '"':
				++ptok ;
				*(ptok + strlen(ptok) -1) = '\0' ;
			default:
				toskip += send_kindle_ascii_string(ptok) ;

				break ;
		}
	}

	return toskip ;
}

static void process_sendkeys_command(char *p)
{
	char *q, *ptok, *plimit ;
	char tmpbuf[MAXPATH] ;
	short code ;
	extern void send_ascii_string(char *p) ;

	memset(tmpbuf, 0, sizeof(tmpbuf)) ;

	strcpy(tmpbuf, p) ;
	plimit = &tmpbuf[0] + strlen(tmpbuf) ;

	for (q = &tmpbuf[0] ; q < plimit ; ++q)
	{
		if (*q <= ' ')
			continue ;

		ptok = q ;
		if (*q == '"')	/* ascii string token */
		{
			++ptok ;
			++q ;
			while ((*q != '"') && (*q != '\0'))
			{
				if (*q == '\\')	/* backslash screens the next character */
						++q ;
				++q ;
			}

			if (*q == '"')
				*q = '\0' ;
			send_ascii_string(ptok) ;
			continue ;
		}
		else
		{
			while (*q > ' ')
				++q ;
			if (*q != '\0')
				*q = '\0' ;
			if ((code = find_input_key_code(ptok)) != -1)
			{
				send_key(code, SENDMODE_NORMAL) ;
			}
			else
			{
				send_ascii_string(ptok) ;
			}
			continue ;
		}
	}
}

int execute_action(struct action *pact, int fd_kbd, int fd_fw )
{
	char tmpbuf[MAXPATH] ;
	char *p ;
	int toskip = 0 ;
	int rc ;

	pixmap_t *statusbar ;

	//DIAG_PRINT("Executing action, type=%d: %s\n", pact->type, pact->command) ;


	// special processing for tripleShift -- take screenshot internal action
	if (pact == ((struct action *) -1) )
	{
		rc = do_screenshot("/mnt/us/screenshot.bmp") ;

		statusbar = statusbar_show(pfbscreen) ;

		strcpy(&tmpbuf[0], "[Screenshot]") ;
		statusbar_text(pfbscreen, statusbar, &tmpbuf[0]) ;
		msdelay(400) ;
		statusbar_flash(pfbscreen, statusbar, 1, 800, 1) ;

		statusbar_text(pfbscreen, statusbar, (rc==0) ? "Success!" : "Failure.") ;
		msdelay(400) ;
		statusbar_flash(pfbscreen, statusbar, 1, 800, 1) ;

		statusbar_hide(pfbscreen, statusbar) ;

		return 0 ;
	}

	statusbar = statusbar_show(pfbscreen) ;

	sprintf(&tmpbuf[0], "^[%s]", &pact->dispname[0]) ;
	statusbar_text(pfbscreen, statusbar, &tmpbuf[0]) ;
	msdelay(400) ;
	statusbar_flash(pfbscreen, statusbar, 1, 800, 1) ;


	switch(pact->type)
	{
		case ACTION_KINDLE_SCRIPT:
			strcpy(tmpbuf, pact->command) ;

			if ((p = get_kindle_script_file(tmpbuf)) != NULL)
			{
				toskip += process_kindle_script(p) ;

				free(p) ;
			}
			else
				DIAG_PRINT("Cannot get kindle script file %s\n", tmpbuf) ;
			break ;

		case ACTION_KINDLE_KEYS:
			/* add unused first and last token -- immitate kindle script format */
			sprintf(&tmpbuf[0], "send_string %s 'DEL'", pact->command) ;
			toskip += process_kindle_script(&tmpbuf[0]) ;

			break ;
		case ACTION_SHELL_COMMAND:
			rc = system(pact->command) ;

			statusbar_text(pfbscreen, statusbar, (rc==0) ? "Success!" : "Failure.") ;
			msdelay(400) ;
			statusbar_flash(pfbscreen, statusbar, 1, 800, 1) ;

			break ;
		case ACTION_SENDKEYS:
		default:
			process_sendkeys_command(pact->command) ;
			break ;

	}

	//msdelay(500) ;
	statusbar_hide(pfbscreen, statusbar) ;

	return toskip ;
}

