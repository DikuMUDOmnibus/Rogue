/***************************************************************************\
[*]    ___    ____   ____   __   __  ____ [*]   ROGUE: ROM With Attitude  [*]
[*]   /#/ )  /#/  ) /#/  ) /#/  /#/ /#/   [*]    All rights reserved      [*]
[*]  /#/ <  /#/  / /#/ _  /#/  /#/ /#/--  [*]   Copyright(C) 2000-2001    [*]
[*] /#/   \(#(__/ (#(__/ (#(__/#/ (#(___  [*] Kenneth Conley (Mendanbar)  [*]
[*]  Expression of Digital Creativity..   [*]  scmud@mad.scientist.com    [*]
[-]---------------------------------------+-+-----------------------------[-]
[*] File: pueblo.c							  [*]
[*] Usage: Pueblo Funtions						  [*]
\***************************************************************************/

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "merc.h"
#include "interp.h"
#include "recycle.h"
#include "tables.h"
#include "pueblo.h"

void do_pmusic(CHAR_DATA *ch, char *argument)
{
    char arg[MSL];
    char buf[MSL];
    int i, x;

      const char *def_music[][2] = {
        { "Nadesico",		"nd.mid"	},
	{ "FF2 World",		"ff2world.mid"	},
	{ "FF3 Main",		"ff3main.mid"	},
	{ "FF3 World",		"ff3wld.mid"	},
	{ "DragonLance",	"dragonl.mid"	},
	{ "Unknown Anime",	"anime1.mid"	},
	{ "FF2 Cecil",		"ff2cecil.mid"	},
	{ "Organ Theme",	"organ.mid"	},
	{ "FF5 Theme",		"ff5theme.mid"	},
	{ "Aeris Theme",	"aeris.mid"	},
	{ "Sephiroth",		"sephiroth.mid"	},
	{ "FF7 World",		"ff7world.mid"	},
	{ "FF7 Ancients",	"ancients.mid"	},
	{ "Heroic Song",	"heroic.mid"	},
	{ "Crystal Prelude",	"prelude.mid"	},
        { "\n", "\n"}
        };

        argument = one_argument(argument, arg);

        if (IS_NPC(ch) || !IS_SET(ch->act,PLR_PUEBLO)) {
        send_to_char("Huh?\n\r",ch);
        }
        else if (!arg || !*arg) {
	send_to_char("The following pre-set music is availible...\n\r",ch);
        for (i = 0; *def_music[i][0] != '\n'; i++) {
		sprintf(buf,"  %2d. %-25s  {y%s{x\r\n",i,def_music[i][0],
			IS_STAFF(ch) ? def_music[i][1] : "");
                send_to_char(buf,ch);
        }
                send_to_char(   "\r\nUsage: pmusic <number>\r\n", ch);
        } else if (!isdigit(*arg))
                send_to_char(   "Usage: pmusic <number>\r\n"
			"Type \"pmusic\" without arguments for a list of pre-set music.\r\n",ch);
        else {
        i = atoi(arg);
        if (i < 0) {
                send_to_char("The number cannot be negative.\r\n", ch);
        } else {
           for (x = 0; *def_music[x][0] != '\n'; x++);

                if (i >= x) {
	        sprintf(buf,"The range for the prompt number is 0-%d.\r\n", x);
                send_to_char(buf,ch);
                } else {
		sprintf(buf,"Activating Music: %s, Infinate Loop...\n\r",def_music[i][0]);
		send_to_char(buf,ch);
			send_to_char("</xch_mudtext><img xch_mode=html>",ch);
			sprintf(pueblo_buf,"<img xch_sound=loop xch_volume=%d src=\"%s%s\">",ch->pcdata->volume,PUEBLO_DIR,def_music[i][1]);
			send_to_char(pueblo_buf,ch);
			send_to_char("<br><img xch_mode=text>",ch);
                        }
                }
        }
}

void do_pstop(CHAR_DATA *ch, char *argument)
{
	if (IS_NPC(ch) || !IS_SET(ch->act,PLR_PUEBLO)) {
	send_to_char("Huh?\n\r",ch); return;
	}
	send_to_char("Deactivating Music: Please Standby...\n\r",ch);
	send_to_char("</xch_mudtext><img xch_mode=html>",ch);
		sprintf(pueblo_buf,"<img xch_sound=stop>");
	send_to_char(pueblo_buf,ch);
	send_to_char("<br><img xch_mode=text>",ch);
    return;
}

void do_volume( CHAR_DATA *ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_INPUT_LENGTH];
    sh_int	value;

	argument = one_argument( argument, arg );

	if (IS_NPC(ch)) {
		send_to_char("Not on NPCs.\n\r",ch);
		return;
	}
	if (!IS_SET(ch->act, PLR_PUEBLO)) {
		send_to_char("Huh?\n\r",ch);
		return;
	}
	if (arg[0] == '\0') {
		send_to_char("Syntax: volume <vol level>\n\r"
		    "Values from 0-100\n\r",ch);
		return;
	}
	if ((atoi(arg) > 100) || (atoi(arg) < 1)) {
		send_to_char("Value must be within 0-100\n\r",ch);
		return;
	}
	value = atoi(arg);
	ch->pcdata->volume = value;
	sprintf(buf,"Pueblo Volume set to %d.\n\r", value);
	send_to_char(buf,ch);
	send_to_char("</xch_mudtext><img xch_mode=html>",ch);
		sprintf(pueblo_buf,"<img xch_sound=play xch_volume=%d src=\"%s%s\">",ch->pcdata->volume,PUEBLO_DIR,PUEBLO_V_TEST);
	send_to_char(pueblo_buf,ch);
	send_to_char("<br><img xch_mode=text>",ch);
	return;
}
