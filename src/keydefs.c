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
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <libgen.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>

#include <linux/input.h>

#include "launchpad.h"
#include "inifile.h"
#include "keydefs.h"

/*
 * default input keys, as found on Kindle DX Graphite..
 */
static const struct input_key default_input_keys[] =
{
	{ KPKEY_1, 			"1"         },
	{ KPKEY_2, 			"2"         },
	{ KPKEY_3, 			"3"         },
	{ KPKEY_4, 			"4"         },
	{ KPKEY_5, 			"5"         },
	{ KPKEY_6, 			"6"         },
	{ KPKEY_7, 			"7"         },
	{ KPKEY_8, 			"8"         },
	{ KPKEY_9, 			"9"         },
	{ KPKEY_0, 			"0"         },
	{ KPKEY_Q, 			"Q"         },
	{ KPKEY_W, 			"W"         },
	{ KPKEY_E, 			"E"         },
	{ KPKEY_R, 			"R"         },
	{ KPKEY_T, 			"T"         },
	{ KPKEY_Y, 			"Y"         },
	{ KPKEY_U, 			"U"         },
	{ KPKEY_I, 			"I"         },
	{ KPKEY_O, 			"O"         },
	{ KPKEY_P, 			"P"         },
	{ KPKEY_A, 			"A"         },
	{ KPKEY_S, 			"S"         },
	{ KPKEY_D, 			"D"         },
	{ KPKEY_F, 			"F"         },
	{ KPKEY_G, 			"G"         },
	{ KPKEY_H, 			"H"         },
	{ KPKEY_J, 			"J"         },
	{ KPKEY_K, 			"K"         },
	{ KPKEY_L, 			"L"         },
	{ KPKEY_DEL, 		"Del"       },
	{ KPKEY_Z, 			"Z"         },
	{ KPKEY_X, 			"X"         },
	{ KPKEY_C, 			"C"         },
	{ KPKEY_V, 			"V"         },
	{ KPKEY_B, 			"B"         },
	{ KPKEY_N, 			"N"         },
	{ KPKEY_M, 			"M"         },
	{ KPKEY_DOT, 		"Dot"       },
	{ KPKEY_SLASH, 		"Slash"     },
	{ KPKEY_ENTER, 		"Enter"     },
	{ KPKEY_SHIFT, 		"Shift"     },
	{ KPKEY_ALT, 		"Alt"       },
	{ KPKEY_SPACE, 		"Space"     },
	{ KPKEY_AA, 		"aA"        },
	{ KPKEY_SYM, 		"Sym"       },
	{ KPKEY_VPLUS, 		"VPlus"     },
	{ KPKEY_VMINUS, 	"VMinus"    },
	{ KPKEY_HOME , 		"Home"     },
	{ KPKEY_PGBCK, 		"<"         },
	{ KPKEY_PGFWD, 		">"         },
	{ KPKEY_MENU , 		"Menu"      },
	{ KPKEY_BACK , 		"Back"      },
	{ KPKEY_FW_LEFT, 	"FW_Left"   },
	{ KPKEY_FW_RIGHT, 	"FW_Right"  },
	{ KPKEY_FW_UP, 		"FW_Up"     },
	{ KPKEY_FW_DOWN, 	"FW_Down"   },
	{ KPKEY_FW_PRESS, 	"FW_Press"  },

	{ K3KEY_AA, "k3_aA" },		// 	190
	{ K3KEY_SYM, "k3_Sym" }, 		//	126
	{ K3KEY_HOME, "k3_Home" }, 	//	102
	{ K3KEY_BACK, "k3_Back" }, 	//	158

	{ K3KEY_PGFWD, "k3_>" }, 	//	191
	{ K3KEY_LPGBCK, "lk3_<" }, 	//	193
	{ K3KEY_LPGFWD, "lk3_>" }, 	//	104

	{ K3KEY_VPLUS, "VPlus" }, 		//	115
	{ K3KEY_VMINUS, "VMinus" }, 	//	114

	{ K3KEY_FW_UP, "k3_FW_Up" }, 	//	103
	{ K3KEY_FW_DOWN, "k3_FW_Down" }, 	//	108
	{ K3KEY_FW_PRESS, "k3_FW_Press" } 	//	194

};

/* this is used to map a richer/different Kindle3 key code repertoire
 * into a narrower set found on Kindle DXG
 */
static const struct input_key_alias default_key_alias[] =
{
	{ K3KEY_AA, KPKEY_AA },
	{ K3KEY_SYM, KPKEY_SYM },
	{ K3KEY_HOME, KPKEY_HOME },
	{ K3KEY_BACK, KPKEY_BACK },
	{ K3KEY_PGFWD, KPKEY_PGFWD },
	{ K3KEY_LPGBCK, KPKEY_PGBCK },
	{ K3KEY_LPGFWD, KPKEY_PGFWD },
	{ K3KEY_VPLUS,  KPKEY_VPLUS },
	{ K3KEY_VMINUS, KPKEY_VMINUS },
	{ K3KEY_FW_UP, KPKEY_FW_UP },
	{ K3KEY_FW_DOWN, KPKEY_FW_DOWN },
	{ K3KEY_FW_PRESS, KPKEY_FW_PRESS }
} ;

static int convert_key_code(char *pbuf)
{
	int rc = -1 ;

	if (pbuf)
	{
		switch(*pbuf)
		{
			case 0x27:		/* apostrophe -- code specified as a character constant */
				rc = *(pbuf+1) & 0xff ;
				break ;

			case '0':
				if (tolower(*(pbuf+1)) == 'x')	/* hex number ? */
					sscanf(pbuf, "%x", &rc) ;
				else	/* octal  number */
				{
					char *p = pbuf ;

					for (rc=0; (*p >= '0') && (*p <= '7'); p++)
						rc = (rc << 3) + (*p - '0') ;
				}
				break ;

			default: /* decimal number */
				rc = atoi(pbuf) ;
				break ;
		}
	}

	if ((rc < 0) || (rc > MAXCODE))
			rc = -1 ;

	return rc ;
}

int get_default_input_keydefs(struct input_key *ptab, int *pcodes)
{
	int i ;
	int n = sizeof(default_input_keys) / sizeof(struct input_key) ;

	memset(ptab, 0, sizeof(struct input_key) * MAXKEYS) ;
	for ( i = 0; i <= MAXCODE; i++)
	{
		*(pcodes+i) = -1 ;
	}

	memcpy(ptab, &default_input_keys, sizeof(default_input_keys)) ;

	for (i=0; i < n; i++)
		*(pcodes + default_input_keys[i].input_key_code) = i ;

	return n ;
}

int get_input_keydefs(struct input_key *ptab, int *pcodes)
{
	char *my_process_directory(void) ;
	int rc = 0 ;
	char tmpbuf[MAXPATH] ;
	int s_idx ;
	char tmpname[MAXNAME] ;
	char *p ;
	int i , n, code ;
	dbdesc_t *pdb = NULL ;

    {
        strcpy(tmpbuf, my_process_directory()) ;
        strcat(tmpbuf, KEYDEFS_FILE) ;

        DIAG_PRINT("Attempting to read key definitions from %s\n", tmpbuf) ;

    	if ( (pdb = inidb_open(tmpbuf)) != NULL)
    	{

    		p = "InputKeys" ;
    		if ((s_idx = inidb_find_section(pdb, p)) >= 0)
    		{
    			n =inidb_get_number_of_keys(pdb, s_idx) ;
    			DIAG_PRINT("[%s] section has %d keys\n", p, n) ;
    			if (n > MAXKEYS)
    				n = MAXKEYS ;

    			for (i = 0; i < n; i++)
    			{
    				if ((inidb_get_key_name(pdb, s_idx, i, tmpname)) > 0)
    				{
    					if ((inidb_get_key_value(pdb, s_idx, i, tmpbuf)) > 0)
    					{
    						code = convert_key_code(tmpbuf) ;
    						if (code >= 0)
    						{
    							if (*(pcodes+code) == -1)
    							{
    								*(pcodes+code) = i ;
    								strncpy(&ptab[i].input_key_name[0], tmpname, MAXNAME-1) ;
    								ptab[i].input_key_code = code ;

    								rc += 1 ;

    								DIAG_PRINT("\t%d: %s = %d\n", i, &ptab[i].input_key_name[0], code) ;
    							}
    							else
    								DIAG_PRINT("key code %d already used by %s\n", code, &ptab[pcodes[code]].input_key_name[0]) ;
    						}
    						else
    							DIAG_PRINT("%s -- wrong key code %s\n", tmpname, tmpbuf) ;
    					}
    					else
    						DIAG_PRINT("%s:%s -- cannot get key value\n", p, tmpname) ;
    				}
    				else
    					DIAG_PRINT("%s:#%d -- cannot get key name\n", p, i) ;

    			}
    		}

    		inidb_close(pdb) ;
    	}
    }

	return rc ;
}

int get_key_substitute_code(int code)
{
	int i ;
	int rc = code ;

	for (i = 0; i < (sizeof(default_key_alias)/ sizeof(struct input_key_alias)); i++)
	{
		if (code == default_key_alias[i].input_key_code)
		{
			rc = default_key_alias[i].substitute_code ;
			break ;
		}
	}

	return rc ;
}
