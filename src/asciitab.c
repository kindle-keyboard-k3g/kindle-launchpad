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


#include <stddef.h>
#include <linux/input.h>

#include "launchpad.h"
#include "asciitab.h"
#include "keydefs.h"

/*      Kindle DXG symbols popup contents:
 *
 * 		"  '  @  -  +  *  ^  (  )  <  >
 * 		?  ,  !  /  =  #  |  {  }  `  ~
 *		&  :  .  ;  _  %  $  [  ]  \
 *
 */
static struct kindle_syms kdxg_s[] =
{
		{ -1, -1 },		/*	"	*/
		{  0, -1 },		/*	'	*/
		{  1, -1 },		/*	@	*/
		{  2, -1 },		/*	-	*/
		{  3, -1 },		/*	+	*/
		{  4, -1 },		/*	*	*/
		{  5, -1 },		/*	^	*/
		{  6, -1 },		/*	(	*/
		{  7, -1 },		/*	)	*/
		{  8, -1 },		/*	<	*/
		{  9, -1 },		/*	>	*/

		{ -1,  0 },		/*	?	*/
		{  0,  0 },		/*	,	*/
		{  1,  0 },		/*	!	*/
		{  2,  0 },		/*	/	*/
		{  3,  0 },		/*	=	*/
		{  4,  0 },		/*	#	*/
		{  5,  0 },		/*	|	*/
		{  6,  0 },		/*	{	*/
		{  7,  0 },		/*	}	*/
		{  8,  0 },		/*	`	*/
		{  9,  0 },		/*	~	*/

		{ -1,  1 },		/*	&	*/
		{  0,  1 },		/*	:	*/
		{  1,  1 },		/*	.	*/
		{  2,  1 },		/*	;	*/
		{  3,  1 },		/*	_	*/
		{  4,  1 },		/*	%	*/
		{  5,  1 },		/*	$	*/
		{  6,  1 },		/*	[	*/
		{  7,  1 },		/*	]	*/
		{  8,  1 } 		/*	\	*/
};


/*
 * Note that input key code definitionsfrom <linux/input.h> were used below
 * for those ASCII characters that are not on a kindle keypad
 */

static const struct ascii_char kdxg_asciitab[128] =
{
											/*       Decimal   Octal   Hex  Value */
											/*       -------   -----   ---  ----- */
	{ KPKEY_SPACE,		SENDMODE_NORMAL,	NULL }, /*         000      000      00    NUL    (Null char.) */
	{ KPKEY_SPACE,		SENDMODE_NORMAL,	NULL }, /*         001      001      01    SOH    (Start of Header) */
	{ KPKEY_SPACE,		SENDMODE_NORMAL,	NULL }, /*         002      002      02    STX    (Start of Text) */
	{ KPKEY_SPACE,		SENDMODE_NORMAL,	NULL }, /*         003      003      03    ETX    (End of Text) */
	{ KPKEY_SPACE,		SENDMODE_NORMAL,	NULL }, /*         004      004      04    EOT    (End of Transmission) */
	{ KPKEY_SPACE,		SENDMODE_NORMAL,	NULL }, /*         005      005      05    ENQ    (Enquiry) */
	{ KPKEY_SPACE,		SENDMODE_NORMAL,	NULL }, /*         006      006      06    ACK    (Acknowledgment) */
	{ KPKEY_SPACE,		SENDMODE_NORMAL,	NULL }, /*         007      007      07    BEL    (Bell) */
	{ KEY_BACKSPACE,	SENDMODE_NORMAL,	NULL }, /*         008      010      08     BS    (Backspace) */
	{ KEY_TAB,			SENDMODE_NORMAL,	NULL }, /*         009      011      09     HT    (Horizontal Tab) */
	{ KPKEY_SPACE,		SENDMODE_NORMAL,	NULL }, /*         010      012      0A     LF    (Line Feed) */
	{ KPKEY_SPACE,		SENDMODE_NORMAL,	NULL }, /*         011      013      0B     VT    (Vertical Tab) */
	{ KPKEY_SPACE,		SENDMODE_NORMAL,	NULL }, /*         012      014      0C     FF    (Form Feed) */
	{ KPKEY_ENTER,		SENDMODE_NORMAL,	NULL }, /*         013      015      0D     CR    (Carriage Return) */
	{ KPKEY_SPACE,		SENDMODE_NORMAL,	NULL }, /*         014      016      0E     SO    (Shift Out) */
	{ KPKEY_SPACE,		SENDMODE_NORMAL,	NULL }, /*         015      017      0F     SI    (Shift In) */
	{ KPKEY_SPACE,		SENDMODE_NORMAL,	NULL }, /*         016      020      10    DLE    (Data Link Escape) */
	{ KPKEY_SPACE,		SENDMODE_NORMAL,	NULL }, /*         017      021      11    DC1    (XON) (Device Control 1) */
	{ KPKEY_SPACE,		SENDMODE_NORMAL,	NULL }, /*         018      022      12    DC2          (Device Control 2) */
	{ KPKEY_SPACE,		SENDMODE_NORMAL,	NULL }, /*         019      023      13    DC3    (XOFF)(Device Control 3) */
	{ KPKEY_SPACE,		SENDMODE_NORMAL,	NULL }, /*         020      024      14    DC4          (Device Control 4) */
	{ KPKEY_SPACE,		SENDMODE_NORMAL,	NULL }, /*         021      025      15    NAK    (Negative Acknowledgement) */
	{ KPKEY_SPACE,		SENDMODE_NORMAL,	NULL }, /*         022      026      16    SYN    (Synchronous Idle) */
	{ KPKEY_SPACE,		SENDMODE_NORMAL,	NULL }, /*         023      027      17    ETB    (End of Trans. Block) */
	{ KPKEY_SPACE,		SENDMODE_NORMAL,	NULL }, /*         024      030      18    CAN    (Cancel) */
	{ KPKEY_SPACE,		SENDMODE_NORMAL,	NULL }, /*         025      031      19     EM    (End of Medium) */
	{ KPKEY_SPACE,		SENDMODE_NORMAL,	NULL }, /*         026      032      1A    SUB    (Substitute) */
	{ KEY_ESC,			SENDMODE_NORMAL,	NULL }, /*         027      033      1B    ESC    (Escape) */
	{ KPKEY_SPACE,		SENDMODE_NORMAL,	NULL }, /*         028      034      1C     FS    (File Separator) */
	{ KPKEY_SPACE,		SENDMODE_NORMAL,	NULL }, /*         029      035      1D     GS    (Group Separator) */
	{ KPKEY_SPACE,		SENDMODE_NORMAL,	NULL }, /*         030      036      1E     RS    (Request to Send)(Record Separator) */
	{ KPKEY_SPACE,		SENDMODE_NORMAL,	NULL }, /*         031      037      1F     US    (Unit Separator) */
	{ KPKEY_SPACE,		SENDMODE_NORMAL,	NULL }, /*         032      040      20     SP    (Space) */
	{ KPKEY_1,			SENDMODE_SHIFT,	 &kdxg_s[13]}, /*         033      041      21      !    (exclamation mark) */
	{ KEY_APOSTROPHE,	SENDMODE_SHIFT,	 &kdxg_s[0] }, /*         034      042      22      "    (double quote) */
	{ KPKEY_3,			SENDMODE_SHIFT,	 &kdxg_s[16]}, /*         035      043      23      #    (number sign) */
	{ KPKEY_4,			SENDMODE_SHIFT,  &kdxg_s[28]}, /*         036      044      24      $    (dollar sign) */
	{ KPKEY_5,			SENDMODE_SHIFT,  &kdxg_s[27]}, /*         037      045      25      %    (percent) */
	{ KPKEY_7,			SENDMODE_SHIFT,	 &kdxg_s[22]}, /*         038      046      26      &    (ampersand) */
	{ KEY_APOSTROPHE,	SENDMODE_NORMAL, &kdxg_s[1] }, /*         039      047      27      '    (single quote) */
	{ KPKEY_9,			SENDMODE_SHIFT,	 &kdxg_s[7] }, /*         040      050      28      (    (left/opening parenthesis) */
	{ KPKEY_0,			SENDMODE_SHIFT,	 &kdxg_s[8] }, /*         041      051      29      )    (right/closing parenthesis) */
	{ KPKEY_8,			SENDMODE_SHIFT,	 &kdxg_s[5] }, /*         042      052      2A      *    (asterisk) */
	{ KEY_EQUAL,		SENDMODE_SHIFT,	 &kdxg_s[4] }, /*         043      053      2B      +    (plus) */
	{ KEY_COMMA,		SENDMODE_NORMAL, &kdxg_s[12]}, /*         044      054      2C      ,    (comma) */
	{ KEY_MINUS,		SENDMODE_NORMAL, &kdxg_s[3] }, /*         045      055      2D      -    (minus or dash) */
	{ KPKEY_DOT,		SENDMODE_NORMAL, &kdxg_s[24]}, /*         046      056      2E      .    (dot) */
	{ KPKEY_SLASH,		SENDMODE_NORMAL, &kdxg_s[14]}, /*         047      057      2F      /    (forward slash) */
	{ KPKEY_0,			SENDMODE_NORMAL,	NULL }, /*         048      060      30      0 */
	{ KPKEY_1,			SENDMODE_NORMAL,	NULL }, /*         049      061      31      1 */
	{ KPKEY_2,			SENDMODE_NORMAL,	NULL }, /*         050      062      32      2 */
	{ KPKEY_3,			SENDMODE_NORMAL,	NULL }, /*         051      063      33      3 */
	{ KPKEY_4,			SENDMODE_NORMAL,	NULL }, /*         052      064      34      4 */
	{ KPKEY_5,			SENDMODE_NORMAL,	NULL }, /*         053      065      35      5 */
	{ KPKEY_6,			SENDMODE_NORMAL,	NULL }, /*         054      066      36      6 */
	{ KPKEY_7,			SENDMODE_NORMAL,	NULL }, /*         055      067      37      7 */
	{ KPKEY_8,			SENDMODE_NORMAL,	NULL }, /*         056      070      38      8 */
	{ KPKEY_9,			SENDMODE_NORMAL,	NULL }, /*         057      071      39      9 */
	{ KEY_SEMICOLON,	SENDMODE_SHIFT,  &kdxg_s[23]}, /*         058      072      3A      :    (colon) */
	{ KEY_SEMICOLON,	SENDMODE_NORMAL, &kdxg_s[25]}, /*         059      073      3B      ;    (semi-colon) */
	{ KEY_COMMA,		SENDMODE_SHIFT,	 &kdxg_s[9] }, /*         060      074      3C      <    (less than) */
	{ KEY_EQUAL,		SENDMODE_NORMAL, &kdxg_s[15]}, /*         061      075      3D      =    (equal sign) */
	{ KPKEY_DOT,		SENDMODE_SHIFT,	 &kdxg_s[10]}, /*         062      076      3E      >    (greater than) */
	{ KPKEY_SLASH,		SENDMODE_SHIFT,	 &kdxg_s[11]}, /*         063      077      3F      ?    (question mark) */
	{ KPKEY_2,			SENDMODE_SHIFT,	 &kdxg_s[2] }, /*         064      100      40      @    (AT symbol) */
	{ KPKEY_A,			SENDMODE_SHIFT,		NULL }, /*         065      101      41      A */
	{ KPKEY_B,			SENDMODE_SHIFT,		NULL }, /*         066      102      42      B */
	{ KPKEY_C,			SENDMODE_SHIFT,		NULL }, /*         067      103      43      C */
	{ KPKEY_D,			SENDMODE_SHIFT,		NULL }, /*         068      104      44      D */
	{ KPKEY_E,			SENDMODE_SHIFT,		NULL }, /*         069      105      45      E */
	{ KPKEY_F,			SENDMODE_SHIFT,		NULL }, /*         070      106      46      F */
	{ KPKEY_G,			SENDMODE_SHIFT,		NULL }, /*         071      107      47      G */
	{ KPKEY_H,			SENDMODE_SHIFT,		NULL }, /*         072      110      48      H */
	{ KPKEY_I,			SENDMODE_SHIFT,		NULL }, /*         073      111      49      I */
	{ KPKEY_J,			SENDMODE_SHIFT,		NULL }, /*         074      112      4A      J */
	{ KPKEY_K,			SENDMODE_SHIFT,		NULL }, /*         075      113      4B      K */
	{ KPKEY_L,			SENDMODE_SHIFT,		NULL }, /*         076      114      4C      L */
	{ KPKEY_M,			SENDMODE_SHIFT,		NULL }, /*         077      115      4D      M */
	{ KPKEY_N,			SENDMODE_SHIFT,		NULL }, /*         078      116      4E      N */
	{ KPKEY_O,			SENDMODE_SHIFT,		NULL }, /*         079      117      4F      O */
	{ KPKEY_P,			SENDMODE_SHIFT,		NULL }, /*         080      120      50      P */
	{ KPKEY_Q,			SENDMODE_SHIFT,		NULL }, /*         081      121      51      Q */
	{ KPKEY_R,			SENDMODE_SHIFT,		NULL }, /*         082      122      52      R */
	{ KPKEY_S,			SENDMODE_SHIFT,		NULL }, /*         083      123      53      S */
	{ KPKEY_T,			SENDMODE_SHIFT,		NULL }, /*         084      124      54      T */
	{ KPKEY_U,			SENDMODE_SHIFT,		NULL }, /*         085      125      55      U */
	{ KPKEY_V,			SENDMODE_SHIFT,		NULL }, /*         086      126      56      V */
	{ KPKEY_W,			SENDMODE_SHIFT,		NULL }, /*         087      127      57      W */
	{ KPKEY_X,			SENDMODE_SHIFT,		NULL }, /*         088      130      58      X */
	{ KPKEY_Y,			SENDMODE_SHIFT,		NULL }, /*         089      131      59      Y */
	{ KPKEY_Z,			SENDMODE_SHIFT,		NULL }, /*         090      132      5A      Z */
	{ KEY_LEFTBRACE,	SENDMODE_NORMAL, &kdxg_s[29]}, /*         091      133      5B      [    (left/opening bracket) */
	{ KEY_BACKSLASH,	SENDMODE_NORMAL,	NULL }, /*         092      134      5C      \    (back slash) */
	{ KEY_RIGHTBRACE,	SENDMODE_NORMAL, &kdxg_s[30]}, /*         093      135      5D      ]    (right/closing bracket) */
	{ KPKEY_6,			SENDMODE_SHIFT,	 &kdxg_s[6] }, /*         094      136      5E      ^    (caret/circumflex) */
	{ KEY_MINUS,		SENDMODE_SHIFT,  &kdxg_s[26]}, /*         095      137      5F      _    (underscore) */
	{ KEY_GRAVE,		SENDMODE_NORMAL, &kdxg_s[20]}, /*         096      140      60      ` */
	{ KPKEY_A,			SENDMODE_NORMAL,	NULL }, /*         097      141      61      a */
	{ KPKEY_B,			SENDMODE_NORMAL,	NULL }, /*         098      142      62      b */
	{ KPKEY_C,			SENDMODE_NORMAL,	NULL }, /*         099      143      63      c */
	{ KPKEY_D,			SENDMODE_NORMAL,	NULL }, /*         100      144      64      d */
	{ KPKEY_E,			SENDMODE_NORMAL,	NULL }, /*         101      145      65      e */
	{ KPKEY_F,			SENDMODE_NORMAL,	NULL }, /*         102      146      66      f */
	{ KPKEY_G,			SENDMODE_NORMAL,	NULL }, /*         103      147      67      g */
	{ KPKEY_H,			SENDMODE_NORMAL,	NULL }, /*         104      150      68      h */
	{ KPKEY_I,			SENDMODE_NORMAL,	NULL }, /*         105      151      69      i */
	{ KPKEY_J,			SENDMODE_NORMAL,	NULL }, /*         106      152      6A      j */
	{ KPKEY_K,			SENDMODE_NORMAL,	NULL }, /*         107      153      6B      k */
	{ KPKEY_L,			SENDMODE_NORMAL,	NULL }, /*         108      154      6C      l */
	{ KPKEY_M,			SENDMODE_NORMAL,	NULL }, /*         109      155      6D      m */
	{ KPKEY_N,			SENDMODE_NORMAL,	NULL }, /*         110      156      6E      n */
	{ KPKEY_O,			SENDMODE_NORMAL,	NULL }, /*         111      157      6F      o */
	{ KPKEY_P,			SENDMODE_NORMAL,	NULL }, /*         112      160      70      p */
	{ KPKEY_Q,			SENDMODE_NORMAL,	NULL }, /*         113      161      71      q */
	{ KPKEY_R,			SENDMODE_NORMAL,	NULL }, /*         114      162      72      r */
	{ KPKEY_S,			SENDMODE_NORMAL,	NULL }, /*         115      163      73      s */
	{ KPKEY_T,			SENDMODE_NORMAL,	NULL }, /*         116      164      74      t */
	{ KPKEY_U,			SENDMODE_NORMAL,	NULL }, /*         117      165      75      u */
	{ KPKEY_V,			SENDMODE_NORMAL,	NULL }, /*         118      166      76      v */
	{ KPKEY_W,			SENDMODE_NORMAL,	NULL }, /*         119      167      77      w */
	{ KPKEY_X,			SENDMODE_NORMAL,	NULL }, /*         120      170      78      x */
	{ KPKEY_Y,			SENDMODE_NORMAL,	NULL }, /*         121      171      79      y */
	{ KPKEY_Z,			SENDMODE_NORMAL,	NULL }, /*         122      172      7A      z */
	{ KEY_LEFTBRACE,	SENDMODE_SHIFT,  &kdxg_s[18]}, /*         123      173      7B      {    (left/opening brace) */
	{ KEY_BACKSLASH,	SENDMODE_SHIFT,  &kdxg_s[17]}, /*         124      174      7C      |    (vertical bar) */
	{ KEY_RIGHTBRACE,	SENDMODE_SHIFT,  &kdxg_s[19]}, /*         125      175      7D      }    (right/closing brace) */
	{ KEY_GRAVE,		SENDMODE_SHIFT,  &kdxg_s[21]}, /*         126      176      7E      ~    (tilde) */
	{ KPKEY_DEL,		SENDMODE_NORMAL, &kdxg_s[31]}  /*         127      177      7F    DEL    (delete) */
} ;

/************************************************************************/

/*      Kindle3 symbols popup contents:
 *
 * 		:  <  >  "  ?  {  }  |  `  ~  GBp  Euro
 * 		;  ,  .  '  /  [  ]  \  oQ cQ ~!   ~?
 *		!  @  #  $  %  ^  &  *  (  )  _    +
 *		1  2  3  4  5  6  7  8  9  0  -    =
 *
 */
static struct kindle_syms k3_s[] =
{
		{ -1, -1 },		/*	:	*/
		{  0, -1 },		/*	<	*/
		{  1, -1 },		/*	>	*/
		{  2, -1 },		/*	"	*/
		{  3, -1 },		/*	?	*/
		{  4, -1 },		/*	{	*/
		{  5, -1 },		/*	}	*/
		{  6, -1 },		/*	|	*/
		{  7, -1 },		/*	`	*/
		{  8, -1 },		/*	~	*/
		{  9, -1 },		/*	GBp */
		{  10,-1 },		/*	Euro*/

		{ -1,  0 },		/*	;	*/
		{  0,  0 },		/*	,	*/
		{  1,  0 },		/*	.	*/
		{  2,  0 },		/*	'	*/
		{  3,  0 },		/*	/	*/
		{  4,  0 },		/*	[	*/
		{  5,  0 },		/*	]	*/
		{  6,  0 },		/*	\	*/
		{  7,  0 },		/*	oQ	*/
		{  8,  0 },		/*	cQ	*/
		{  9,  0 },		/*	~!	*/
		{  10, 0 },		/*	~?	*/

		{ -1,  1 },		/*	!	*/
		{  0,  1 },		/*	@	*/
		{  1,  1 },		/*	#	*/
		{  2,  1 },		/*	$	*/
		{  3,  1 },		/*	%	*/
		{  4,  1 },		/*	^	*/
		{  5,  1 },		/*	&	*/
		{  6,  1 },		/*	*	*/
		{  7,  1 },		/*	(	*/
		{  8,  1 },		/*	)	*/
		{  9,  1 },		/*	_	*/
		{  10, 1 },		/*	+	*/

		{ -1,  2 },		/*	1	*/
		{  0,  2 },		/*	2	*/
		{  1,  2 },		/*	3	*/
		{  2,  2 },		/*	4	*/
		{  3,  2 },		/*	5	*/
		{  4,  2 },		/*	6	*/
		{  5,  2 },		/*	7	*/
		{  6,  2 },		/*	8	*/
		{  7,  2 },		/*	9	*/
		{  8,  2 },		/*	0	*/
		{  9,  2 },		/*	-	*/
		{  10, 2 }		/*	=	*/
};

static const struct ascii_char k3_asciitab[128] =
{
											/*       Decimal   Octal   Hex  Value */
											/*       -------   -----   ---  ----- */
	{ KPKEY_SPACE,		SENDMODE_NORMAL,	NULL }, /*         000      000      00    NUL    (Null char.) */
	{ KPKEY_SPACE,		SENDMODE_NORMAL,	NULL }, /*         001      001      01    SOH    (Start of Header) */
	{ KPKEY_SPACE,		SENDMODE_NORMAL,	NULL }, /*         002      002      02    STX    (Start of Text) */
	{ KPKEY_SPACE,		SENDMODE_NORMAL,	NULL }, /*         003      003      03    ETX    (End of Text) */
	{ KPKEY_SPACE,		SENDMODE_NORMAL,	NULL }, /*         004      004      04    EOT    (End of Transmission) */
	{ KPKEY_SPACE,		SENDMODE_NORMAL,	NULL }, /*         005      005      05    ENQ    (Enquiry) */
	{ KPKEY_SPACE,		SENDMODE_NORMAL,	NULL }, /*         006      006      06    ACK    (Acknowledgment) */
	{ KPKEY_SPACE,		SENDMODE_NORMAL,	NULL }, /*         007      007      07    BEL    (Bell) */
	{ KEY_BACKSPACE,	SENDMODE_NORMAL,	NULL }, /*         008      010      08     BS    (Backspace) */
	{ KEY_TAB,			SENDMODE_NORMAL,	NULL }, /*         009      011      09     HT    (Horizontal Tab) */
	{ KPKEY_SPACE,		SENDMODE_NORMAL,	NULL }, /*         010      012      0A     LF    (Line Feed) */
	{ KPKEY_SPACE,		SENDMODE_NORMAL,	NULL }, /*         011      013      0B     VT    (Vertical Tab) */
	{ KPKEY_SPACE,		SENDMODE_NORMAL,	NULL }, /*         012      014      0C     FF    (Form Feed) */
	{ KPKEY_ENTER,		SENDMODE_NORMAL,	NULL }, /*         013      015      0D     CR    (Carriage Return) */
	{ KPKEY_SPACE,		SENDMODE_NORMAL,	NULL }, /*         014      016      0E     SO    (Shift Out) */
	{ KPKEY_SPACE,		SENDMODE_NORMAL,	NULL }, /*         015      017      0F     SI    (Shift In) */
	{ KPKEY_SPACE,		SENDMODE_NORMAL,	NULL }, /*         016      020      10    DLE    (Data Link Escape) */
	{ KPKEY_SPACE,		SENDMODE_NORMAL,	NULL }, /*         017      021      11    DC1    (XON) (Device Control 1) */
	{ KPKEY_SPACE,		SENDMODE_NORMAL,	NULL }, /*         018      022      12    DC2          (Device Control 2) */
	{ KPKEY_SPACE,		SENDMODE_NORMAL,	NULL }, /*         019      023      13    DC3    (XOFF)(Device Control 3) */
	{ KPKEY_SPACE,		SENDMODE_NORMAL,	NULL }, /*         020      024      14    DC4          (Device Control 4) */
	{ KPKEY_SPACE,		SENDMODE_NORMAL,	NULL }, /*         021      025      15    NAK    (Negative Acknowledgement) */
	{ KPKEY_SPACE,		SENDMODE_NORMAL,	NULL }, /*         022      026      16    SYN    (Synchronous Idle) */
	{ KPKEY_SPACE,		SENDMODE_NORMAL,	NULL }, /*         023      027      17    ETB    (End of Trans. Block) */
	{ KPKEY_SPACE,		SENDMODE_NORMAL,	NULL }, /*         024      030      18    CAN    (Cancel) */
	{ KPKEY_SPACE,		SENDMODE_NORMAL,	NULL }, /*         025      031      19     EM    (End of Medium) */
	{ KPKEY_SPACE,		SENDMODE_NORMAL,	NULL }, /*         026      032      1A    SUB    (Substitute) */
	{ KEY_ESC,			SENDMODE_NORMAL,	NULL }, /*         027      033      1B    ESC    (Escape) */
	{ KPKEY_SPACE,		SENDMODE_NORMAL,	NULL }, /*         028      034      1C     FS    (File Separator) */
	{ KPKEY_SPACE,		SENDMODE_NORMAL,	NULL }, /*         029      035      1D     GS    (Group Separator) */
	{ KPKEY_SPACE,		SENDMODE_NORMAL,	NULL }, /*         030      036      1E     RS    (Request to Send)(Record Separator) */
	{ KPKEY_SPACE,		SENDMODE_NORMAL,	NULL }, /*         031      037      1F     US    (Unit Separator) */
	{ KPKEY_SPACE,		SENDMODE_NORMAL,	NULL }, /*         032      040      20     SP    (Space) */
	{ KPKEY_1,			SENDMODE_SHIFT,	 &k3_s[24]}, /*         033      041      21      !    (exclamation mark) */
	{ KEY_APOSTROPHE,	SENDMODE_SHIFT,	 &k3_s[3] }, /*         034      042      22      "    (double quote) */
	{ KPKEY_3,			SENDMODE_SHIFT,	 &k3_s[26]}, /*         035      043      23      #    (number sign) */
	{ KPKEY_4,			SENDMODE_SHIFT,  &k3_s[27]}, /*         036      044      24      $    (dollar sign) */
	{ KPKEY_5,			SENDMODE_SHIFT,  &k3_s[28]}, /*         037      045      25      %    (percent) */
	{ KPKEY_7,			SENDMODE_SHIFT,	 &k3_s[30]}, /*         038      046      26      &    (ampersand) */
	{ KEY_APOSTROPHE,	SENDMODE_NORMAL, &k3_s[15]}, /*         039      047      27      '    (single quote) */
	{ KPKEY_9,			SENDMODE_SHIFT,	 &k3_s[32]}, /*         040      050      28      (    (left/opening parenthesis) */
	{ KPKEY_0,			SENDMODE_SHIFT,	 &k3_s[33]}, /*         041      051      29      )    (right/closing parenthesis) */
	{ KPKEY_8,			SENDMODE_SHIFT,	 &k3_s[31]}, /*         042      052      2A      *    (asterisk) */
	{ KEY_EQUAL,		SENDMODE_SHIFT,	 &k3_s[35]}, /*         043      053      2B      +    (plus) */
	{ KEY_COMMA,		SENDMODE_NORMAL, &k3_s[13]}, /*         044      054      2C      ,    (comma) */
	{ KEY_MINUS,		SENDMODE_NORMAL, &k3_s[46]}, /*         045      055      2D      -    (minus or dash) */
	{ KPKEY_DOT,		SENDMODE_NORMAL, &k3_s[14]}, /*         046      056      2E      .    (dot) */
	{ KPKEY_SLASH,		SENDMODE_NORMAL, &k3_s[16]}, /*         047      057      2F      /    (forward slash) */
	{ KPKEY_0,			SENDMODE_NORMAL, &k3_s[45]}, /*         048      060      30      0 */
	{ KPKEY_1,			SENDMODE_NORMAL, &k3_s[36]}, /*         049      061      31      1 */
	{ KPKEY_2,			SENDMODE_NORMAL, &k3_s[37]}, /*         050      062      32      2 */
	{ KPKEY_3,			SENDMODE_NORMAL, &k3_s[38]}, /*         051      063      33      3 */
	{ KPKEY_4,			SENDMODE_NORMAL, &k3_s[39]}, /*         052      064      34      4 */
	{ KPKEY_5,			SENDMODE_NORMAL, &k3_s[40]}, /*         053      065      35      5 */
	{ KPKEY_6,			SENDMODE_NORMAL, &k3_s[41]}, /*         054      066      36      6 */
	{ KPKEY_7,			SENDMODE_NORMAL, &k3_s[42]}, /*         055      067      37      7 */
	{ KPKEY_8,			SENDMODE_NORMAL, &k3_s[43]}, /*         056      070      38      8 */
	{ KPKEY_9,			SENDMODE_NORMAL, &k3_s[44]}, /*         057      071      39      9 */
	{ KEY_SEMICOLON,	SENDMODE_SHIFT,  &k3_s[0] }, /*         058      072      3A      :    (colon) */
	{ KEY_SEMICOLON,	SENDMODE_NORMAL, &k3_s[12]}, /*         059      073      3B      ;    (semi-colon) */
	{ KEY_COMMA,		SENDMODE_SHIFT,	 &k3_s[1] }, /*         060      074      3C      <    (less than) */
	{ KEY_EQUAL,		SENDMODE_NORMAL, &k3_s[47]}, /*         061      075      3D      =    (equal sign) */
	{ KPKEY_DOT,		SENDMODE_SHIFT,	 &k3_s[2] }, /*         062      076      3E      >    (greater than) */
	{ KPKEY_SLASH,		SENDMODE_SHIFT,	 &k3_s[4] }, /*         063      077      3F      ?    (question mark) */
	{ KPKEY_2,			SENDMODE_SHIFT,	 &k3_s[25]}, /*         064      100      40      @    (AT symbol) */
	{ KPKEY_A,			SENDMODE_SHIFT,		NULL }, /*         065      101      41      A */
	{ KPKEY_B,			SENDMODE_SHIFT,		NULL }, /*         066      102      42      B */
	{ KPKEY_C,			SENDMODE_SHIFT,		NULL }, /*         067      103      43      C */
	{ KPKEY_D,			SENDMODE_SHIFT,		NULL }, /*         068      104      44      D */
	{ KPKEY_E,			SENDMODE_SHIFT,		NULL }, /*         069      105      45      E */
	{ KPKEY_F,			SENDMODE_SHIFT,		NULL }, /*         070      106      46      F */
	{ KPKEY_G,			SENDMODE_SHIFT,		NULL }, /*         071      107      47      G */
	{ KPKEY_H,			SENDMODE_SHIFT,		NULL }, /*         072      110      48      H */
	{ KPKEY_I,			SENDMODE_SHIFT,		NULL }, /*         073      111      49      I */
	{ KPKEY_J,			SENDMODE_SHIFT,		NULL }, /*         074      112      4A      J */
	{ KPKEY_K,			SENDMODE_SHIFT,		NULL }, /*         075      113      4B      K */
	{ KPKEY_L,			SENDMODE_SHIFT,		NULL }, /*         076      114      4C      L */
	{ KPKEY_M,			SENDMODE_SHIFT,		NULL }, /*         077      115      4D      M */
	{ KPKEY_N,			SENDMODE_SHIFT,		NULL }, /*         078      116      4E      N */
	{ KPKEY_O,			SENDMODE_SHIFT,		NULL }, /*         079      117      4F      O */
	{ KPKEY_P,			SENDMODE_SHIFT,		NULL }, /*         080      120      50      P */
	{ KPKEY_Q,			SENDMODE_SHIFT,		NULL }, /*         081      121      51      Q */
	{ KPKEY_R,			SENDMODE_SHIFT,		NULL }, /*         082      122      52      R */
	{ KPKEY_S,			SENDMODE_SHIFT,		NULL }, /*         083      123      53      S */
	{ KPKEY_T,			SENDMODE_SHIFT,		NULL }, /*         084      124      54      T */
	{ KPKEY_U,			SENDMODE_SHIFT,		NULL }, /*         085      125      55      U */
	{ KPKEY_V,			SENDMODE_SHIFT,		NULL }, /*         086      126      56      V */
	{ KPKEY_W,			SENDMODE_SHIFT,		NULL }, /*         087      127      57      W */
	{ KPKEY_X,			SENDMODE_SHIFT,		NULL }, /*         088      130      58      X */
	{ KPKEY_Y,			SENDMODE_SHIFT,		NULL }, /*         089      131      59      Y */
	{ KPKEY_Z,			SENDMODE_SHIFT,		NULL }, /*         090      132      5A      Z */
	{ KEY_LEFTBRACE,	SENDMODE_NORMAL, &k3_s[17]}, /*         091      133      5B      [    (left/opening bracket) */
	{ KEY_BACKSLASH,	SENDMODE_NORMAL, &k3_s[19]}, /*         092      134      5C      \    (back slash) */
	{ KEY_RIGHTBRACE,	SENDMODE_NORMAL, &k3_s[18]}, /*         093      135      5D      ]    (right/closing bracket) */
	{ KPKEY_6,			SENDMODE_SHIFT,	 &k3_s[29]}, /*         094      136      5E      ^    (caret/circumflex) */
	{ KEY_MINUS,		SENDMODE_SHIFT,  &k3_s[34]}, /*         095      137      5F      _    (underscore) */
	{ KEY_GRAVE,		SENDMODE_NORMAL, &k3_s[8] }, /*         096      140      60      ` */
	{ KPKEY_A,			SENDMODE_NORMAL,	NULL }, /*         097      141      61      a */
	{ KPKEY_B,			SENDMODE_NORMAL,	NULL }, /*         098      142      62      b */
	{ KPKEY_C,			SENDMODE_NORMAL,	NULL }, /*         099      143      63      c */
	{ KPKEY_D,			SENDMODE_NORMAL,	NULL }, /*         100      144      64      d */
	{ KPKEY_E,			SENDMODE_NORMAL,	NULL }, /*         101      145      65      e */
	{ KPKEY_F,			SENDMODE_NORMAL,	NULL }, /*         102      146      66      f */
	{ KPKEY_G,			SENDMODE_NORMAL,	NULL }, /*         103      147      67      g */
	{ KPKEY_H,			SENDMODE_NORMAL,	NULL }, /*         104      150      68      h */
	{ KPKEY_I,			SENDMODE_NORMAL,	NULL }, /*         105      151      69      i */
	{ KPKEY_J,			SENDMODE_NORMAL,	NULL }, /*         106      152      6A      j */
	{ KPKEY_K,			SENDMODE_NORMAL,	NULL }, /*         107      153      6B      k */
	{ KPKEY_L,			SENDMODE_NORMAL,	NULL }, /*         108      154      6C      l */
	{ KPKEY_M,			SENDMODE_NORMAL,	NULL }, /*         109      155      6D      m */
	{ KPKEY_N,			SENDMODE_NORMAL,	NULL }, /*         110      156      6E      n */
	{ KPKEY_O,			SENDMODE_NORMAL,	NULL }, /*         111      157      6F      o */
	{ KPKEY_P,			SENDMODE_NORMAL,	NULL }, /*         112      160      70      p */
	{ KPKEY_Q,			SENDMODE_NORMAL,	NULL }, /*         113      161      71      q */
	{ KPKEY_R,			SENDMODE_NORMAL,	NULL }, /*         114      162      72      r */
	{ KPKEY_S,			SENDMODE_NORMAL,	NULL }, /*         115      163      73      s */
	{ KPKEY_T,			SENDMODE_NORMAL,	NULL }, /*         116      164      74      t */
	{ KPKEY_U,			SENDMODE_NORMAL,	NULL }, /*         117      165      75      u */
	{ KPKEY_V,			SENDMODE_NORMAL,	NULL }, /*         118      166      76      v */
	{ KPKEY_W,			SENDMODE_NORMAL,	NULL }, /*         119      167      77      w */
	{ KPKEY_X,			SENDMODE_NORMAL,	NULL }, /*         120      170      78      x */
	{ KPKEY_Y,			SENDMODE_NORMAL,	NULL }, /*         121      171      79      y */
	{ KPKEY_Z,			SENDMODE_NORMAL,	NULL }, /*         122      172      7A      z */
	{ KEY_LEFTBRACE,	SENDMODE_SHIFT,  &k3_s[5] },/*         123      173      7B      {    (left/opening brace) */
	{ KEY_BACKSLASH,	SENDMODE_SHIFT,  &k3_s[7] },/*         124      174      7C      |    (vertical bar) */
	{ KEY_RIGHTBRACE,	SENDMODE_SHIFT,  &k3_s[6] },/*         125      175      7D      }    (right/closing brace) */
	{ KEY_GRAVE,		SENDMODE_SHIFT,  &k3_s[9] },/*         126      176      7E      ~    (tilde) */
	{ KPKEY_DEL,		SENDMODE_NORMAL,	NULL }  /*         127      177      7F    DEL    (delete) */
} ;
/************************************************************************/


extern int is_this_kindle3(void) ;

struct kindle_syms * char_kindle_fw_sequence(char asciichar)
{
	return ((is_this_kindle3() == 0) ? kdxg_asciitab[asciichar & 0x7f].kindle_fwork_sequence :
		k3_asciitab[asciichar & 0x7f].kindle_fwork_sequence ) ;
}

short char_keypad_code(char asciichar)
{
	return ((is_this_kindle3() == 0) ? kdxg_asciitab[asciichar & 0x7f].code :
		k3_asciitab[asciichar & 0x7f].code ) ;
}
int  char_send_mode(char asciichar)
{
	return ((is_this_kindle3() == 0) ? kdxg_asciitab[asciichar & 0x7f].code :
		k3_asciitab[asciichar & 0x7f].code ) ;
}
