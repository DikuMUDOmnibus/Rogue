/* This code inspired by a snippet from :                                  */

/************************************************************************/
/* mlkesl@stthomas.edu	=====>	Ascii Automapper utility                */
/* Let me know if you use this. Give a newbie some _credit_,            */
/* at least I'm not asking how to add classes...                        */
/* Also, if you fix something could ya send me mail about, thanks       */
/* PLEASE mail me if you use this or like it, that way I will keep it up*/
/************************************************************************/

#include <ctype.h>  /* for isalpha */
#include <string.h>
#include <stdlib.h>
#include <stdio.h> 
#include <time.h> 
#include "mapper.h"

extern char * compass_name[];

int door_marks[4][2] ={ {-1, 0},{ 0, 1},{ 1, 0},{ 0,-1} };
int offsets[4][2] ={ {-2, 0},{ 0, 2},{ 2, 0},{ 0,-2} };

#define SECT_HERE	SECT_MAX
#define SECT_UNSEEN	( SECT_MAX + 1 )
#define SECT_BLOCKED	( SECT_UNSEEN + 1 )
#define SECT_TOP	( SECT_BLOCKED + 1 )


const struct map_info_type door_info[] =
{
  { DOOR_LOCKED, "{R" , "#{x", "{f","Locked door" },
  { DOOR_OPEN, "{W" , "#{x", "{f","Open door" },
  { DOOR_CLOSED, "{r" , "#{x", "{f","Closed Door" },
  { DOOR_NS, "{W" , "|{x", "{f","N/S Exit" },
  { DOOR_EW, "{W" , "-{x", "{f","E/W Exit" },
  { DOOR_HIDDEN, "{x", " {x" "{f", "Hidden" },
  { DOOR_NULL, "{x", " ", "{f","Invalid" }
};

const struct map_info_type map_info[] =
{
  { SECT_BLOCKED, "{m", "~{x", "", "blocked" },
  { SECT_UNSEEN, "{K"," {x", "", "unknown" },
  { SECT_HERE, "{W", "*{x", "", "you!" },
  { SECT_CITY, "{w{i", " {x", "", "city" },
  { SECT_FIELD, "{G{i", " {x", "", "field" },
  { SECT_FOREST, "{2", " {x", "{W", "forest" },
  { SECT_HILLS,  "{3", " {x", "{W","hills" },
  { SECT_MOUNTAIN, "{1", " {x", "", "mountain" },
  { SECT_WATER_SWIM,"{B{i", " {x", "", "shallow water" },
  { SECT_WATER_NOSWIM, "{4", " {x", "", "deep running water" },
  { SECT_AIR, "{C{i", " {x", "","air" },
  { SECT_DESERT, "{Y{i", " {x", "","desert" },
  { SECT_INSIDE, "{K{i", " {x", "","inside" },
  { SECT_TOP, "{y", "~{x", "","bad sector type" }
};

char * get_sector_display( int sector )
{
  sh_int looper;
  for ( looper = 0; ; looper++ )
    if (  ( map_info[looper].sector_type == sector )
       || ( map_info[looper].sector_type == SECT_TOP )   )
      break;
  return ( map_info[looper].display_code );
}
char * get_sector_color( int sector )
{
  sh_int looper;
  for ( looper = 0; ; looper++ )
    if (  ( map_info[looper].sector_type == sector )
       || ( map_info[looper].sector_type == SECT_TOP )   )
      break;
  return ( map_info[looper].display_color );
}
char * get_invert_color( int sector )
{
  sh_int looper;
  for ( looper = 0; ; looper++ )
    if (  ( map_info[looper].sector_type == sector )
       || ( map_info[looper].sector_type == SECT_TOP )   )
      break;
  return ( map_info[looper].invert_color );
}
char * get_door_display( int door )
{
  sh_int looper;
  for ( looper = 0; ; looper++ )
    if (  ( door_info[looper].sector_type == door )
       || ( door_info[looper].sector_type == DOOR_NULL )   )
      break;
  return ( door_info[looper].display_code );
}
char * get_door_color( int door )
{
  sh_int looper;
  for ( looper = 0; ; looper++ )
    if (  ( door_info[looper].sector_type == door )
       || ( door_info[looper].sector_type == DOOR_NULL )   )
      break;
  return ( door_info[looper].display_color );
}
char * get_sector_name( int sector )
{
  sh_int looper;
  for ( looper = 0; ; looper++ )
    if (  ( map_info[looper].sector_type == sector )
       || ( map_info[looper].sector_type == SECT_TOP )   )
      break;
  return ( map_info[looper].desc );
}


/*
 * This code written by Altrag for Ack!Mud 
 */

#define iseol(c)	((c)=='\n' || (c)=='\r')

/* Like one_argument but saves case, and if len isnt NULL, fills it in
 * with the length.  Also accounts for color.  Does NOT account for
 * quoted text. */
char *break_arg(char *str, char *first_arg, int bufsize, int max, int *buflen, int *len)
{
  int slen=0;
  char *arg;
  
  while (isspace(*str))
    ++str;
  if (*str=='\\' && str[1]=='b' && str[2]=='r')
  {
    strcpy(first_arg, "\n\r");
    if (buflen)
      *buflen=0;
    if (len)
      *len=0;
    str+=3;
    while (isspace(*str))
      ++str;
    return str;
  }
  arg=first_arg;
  while (*str && arg-first_arg<bufsize && slen<max)
  {
    if (isspace(*str))
    {
      ++str;
      break;
    }
    else if (*str=='@' && str[1]=='@' && str[2]!='\0')
    {
      if (arg-first_arg>=max-3)
        break;
      *arg++=*str++;
      *arg++=*str++;
      *arg++=*str++;
    }
    else if (*str=='\\' && str[1]=='b' && str[2]=='r')
      break;
    else
    {
      *arg++=*str++;
      ++slen;
    }
  }
  *arg='\0';
  if (len)
    *len=slen;
  if (buflen)
    *buflen=arg-first_arg;
  while (isspace(*str))
    ++str;
  return str;
}

char *string_justify(char *str, int len, int width, int numwords, int *rlen)
{
  static char buf[MSL];
  char arg[MAX_INPUT_LENGTH];
  int minspaces = numwords-1;
  int spaces_needed;
  float space_ratio, space_between;
  int i, j = 0, alen;
  char *bp = buf;
  
  spaces_needed = minspaces+(width-(len+1));
  if (spaces_needed<=minspaces || minspaces<=0)
  {
    sprintf(buf, "%s\n\r", str);
    return buf;
  }
  space_ratio = (float)spaces_needed/(float)minspaces;
  space_between = space_ratio;
  for (i = 0; i < minspaces; ++i)
  {
    str = break_arg(str, arg, sizeof(arg), width, &alen, NULL);
    strcpy(bp, arg);
    bp += alen;
    for (; j < (int)(space_between+0.5); ++j)
      *bp++ = ' ';
    space_between += space_ratio;
  }
  str = break_arg(str, arg, sizeof(arg), width, &alen, NULL);
  strcpy(bp, arg);
  bp += alen;
/*  bp += sprintf(bp, "\n\r%d:%d:%d", len, width, numwords);
  bp += sprintf(bp, "\n\r%d:%d:%f", minspaces, spaces_needed, space_ratio);*/
  *bp++ = '\n';
  *bp++ = '\r';
  *bp = '\0';
  if (rlen)
    *rlen = bp-buf;
  return buf;
}

char last_color(char *str)
{
  char *end;
  
  for (end=str+strlen(str)-2; end > str; --end)
    if (*end == '@' && end[-1]=='@')
      return end[1];
  return '\0';
}

char *string_format(char *str, int *numlines, int width, int height, bool unjust)
{
  static char ret[MSL];
  char buf[MSL];
  char arg[MAX_INPUT_LENGTH];
  int alen;
  int blen=0;
  char *pbuf=buf, *pret=ret;
  int len;
  int currline = 0;
  int last;
  char *sp;
  char c;
  int numwords = 0;
  int jlen;

  --height;
  --width;
  *pret='\0';
  for (sp = break_arg(str, arg, sizeof(arg), width, &len, &alen); *arg;
       sp = break_arg(sp, arg, sizeof(arg), width, &len, &alen))
  {
    blen += alen;
    if (blen+1>=width || iseol(*arg))
    {
      *pbuf++='\n';
      *pbuf++='\r';
      *pbuf='\0';
      c = last_color(buf);
      if (unjust || iseol(*arg))
      {
        strcpy(pret, buf);
        pret += pbuf-buf;
      }
      else
      {
        strcpy(pret, string_justify(buf, blen-alen, width, numwords, &jlen));
        pret += jlen;
      }
      pbuf=buf;
      if (++currline>height)
        break;
      if (c)
      {
        *pbuf++='@';
        *pbuf++='@';
        *pbuf++=c;
      }
      blen=alen;
      if (iseol(*arg))
        *arg='\0';
      numwords = 0;
    }
    else if (pbuf>buf)
    {
      if (pbuf-buf>2 && pbuf[-2]=='@' && pbuf[-3]=='@')
      {
        if (pbuf-buf==3)
          last=0;
        else
          last=-4;
      }
      else
        last=-1;
      if (last)
      {
        if (unjust && pbuf[last]=='.')
          *pbuf++=' ', ++blen;
//        if (!iseol(pbuf[last]))
        *pbuf++=' ', ++blen;
      }
    }
    strcpy(pbuf, arg);
    pbuf+=len;
    ++numwords;
  }
  if (pbuf>buf)
  {
    if (pbuf-buf>2 && pbuf[-2]=='@' && pbuf[-3]=='@')
    {
      if (pbuf-buf==3)
        last=0;
      else
        last=-4;
    }
    else
      last=-1;
    if (last && pbuf[last]!='\n' && pbuf[last]!='\r')
    {
      *pbuf++='\n';
      *pbuf++='\r';
      ++currline;
    }
  }
  *pbuf='\0';
  strcpy(pret, buf);
  if (numlines)
    *numlines=currline;
  return ret;
}

char *map_format(char *str, int start, char map[MAP_Y][MSL], int *numlines,
		 int term_width, int height, bool unjust)
{
  static char ret[MSL];
  char buf[MSL];
  char arg[MAX_INPUT_LENGTH];
  int width = (start<MAP_Y ? term_width-15 : term_width-1);
  int alen;
  int blen=0;
  char *pbuf=buf, *pret=ret;
  int len;
  int currline = start;
  int last;
  char *sp;
  char c;
  int numwords = 0;
  int jlen;

  --height;
  *pret='\0';
  for (sp = break_arg(str, arg, sizeof(arg), width, &len, &alen); *arg;
       sp = break_arg(sp, arg, sizeof(arg), width, &len, &alen))
  {
    blen += alen;
    if (blen+1>=width || iseol(*arg))
    {
      *pbuf++='\n';
      *pbuf++='\r';
      *pbuf='\0';
      c = last_color(buf);
      if (currline<MAP_Y)
        pret += sprintf(pret, "%s   ", map[currline]);
      else if (currline==MAP_Y)
        strcpy(pret, "              "), pret+=14;
      if (unjust || iseol(*arg))
      {
        strcpy(pret, buf);
        pret += pbuf-buf;
      }
      else
      {
        strcpy(pret, string_justify(buf, blen-alen, width, numwords, &jlen));
        pret += jlen;
      }
      if (currline==MAP_Y)
        width=term_width-1;
      pbuf=buf;
      if (++currline>height)
        break;
      if (c)
      {
        *pbuf++='@';
        *pbuf++='@';
        *pbuf++=c;
      }
      blen=alen;
      if (iseol(*arg))
        *arg='\0';
      numwords = 0;
    }
    else if (pbuf>buf)
    {
      if (pbuf-buf>2 && pbuf[-2]=='@' && pbuf[-3]=='@')
      {
        if (pbuf-buf==3)
          last=0;
        else
          last=-4;
      }
      else
        last=-1;
      if (last)
      {
        if (unjust && pbuf[last]=='.')
          *pbuf++=' ', ++blen;
//        if (!iseol(pbuf[last]))
        *pbuf++=' ', ++blen;
      }
    }
    strcpy(pbuf, arg);
    pbuf+=len;
    ++numwords;
  }
/*  sprintf(bug_buf, "%d:%d", width, blen);
  monitor_chan(bug_buf, MONITOR_DEBUG);*/
  if (pbuf>buf)
  {
    if (pbuf-buf>2 && pbuf[-2]=='@' && pbuf[-3]=='@')
    {
      if (pbuf-buf==3)
        last=0;
      else
        last=-4;
    }
    else
      last=-1;
    if (last && pbuf[last]!='\n' && pbuf[last]!='\r')
    {
      *pbuf++='\n';
      *pbuf++='\r';
      if (currline<MAP_Y)
        pret += sprintf(pret, "%s   ", map[currline]);
      else if (currline==MAP_Y || (currline==MAP_Y+1 && blen<=term_width-15))
        strcpy(pret, "              "), pret+=14;
      ++currline;
    }
  }
  *pbuf='\0';
  strcpy(pret, buf);
  if (numlines)
    *numlines=currline;
  return ret;
}

void disp_map(char *border, char *map, CHAR_DATA *ch)
{
  int cols=80;
  int rows=9999;
  char bufs[MAP_Y][MSL];
  char disp[MSL];
  char cdisp[MSL];
  int y, ty=MAP_Y-1;
  char *x, *ox;

  strcpy(bufs[0], border);
  strcpy(bufs[ty], border);
  x = map;
  for (y = 1; y < ty && *x; ++y)
  {
    while (*x=='\n' || *x=='\r')
      ++x;
    ox = x;
    while (*x!='\n' && *x!='\r' && *x!='\0')
      ++x;
    sprintf(bufs[y], "%.*s", (x-ox), ox);
  }
sprintf(cdisp, "{c%s{x",ch->in_room->name);
strcpy(disp, map_format(cdisp, 0, bufs, &y, cols, rows, TRUE));
strcat(disp, map_format(ch->in_room->description, y, bufs, &y, cols,rows, TRUE));

  if (y<MAP_Y)
  {
    x = disp+strlen(disp);
    while (y<MAP_Y)
      x += sprintf(x, "%s\n\r", bufs[y]), ++y;
  }
  send_to_char(disp, ch);
#ifdef ACK_43
  if (!IS_NPC(ch) && IS_SET(ch->config, CONFIG_FULL_ANSI))
  {
    sprintf(disp, "%s%i;%ir%s%i;%iH",
    	    CRS_CMD,
    	    0,
    	    ch->pcdata->term_rows-12,
    	    CRS_CMD,
    	    ch->pcdata->term_rows-13,
    	    0);
    send_to_char(disp, ch);
  }
#endif
  return;
}




void MapArea 
(ROOM_INDEX_DATA *room, CHAR_DATA *ch, 
     int x, int y, int min, int max, int line_of_sight )
{
  ROOM_INDEX_DATA *prospect_room;
  EXIT_DATA *pexit;
  int door;
  sh_int door_type = 0;
  if ( map[x][y] <= 0 )
    return; /* it's a door, not a room in the map */

  /* marks the room as visited */
  map[x][y]=room->sector_type;

/* displays seen mobs nearby as numbers */

  for ( door = 0; door < MAX_MAP_DIR; door++ ) 
  { /* cycles through for each exit */
    if (  ( (pexit = room->exit[door]) != NULL )
	     && ( pexit->u1.to_room != NULL ) )

    { /* if exit there */
      if ((x<min)||(y<min)||(x>max)||(y>max)) return;

      prospect_room = pexit->u1.to_room;
      if (  ( prospect_room->exit[rev_dir[door]] )
         && ( prospect_room->exit[rev_dir[door]]->u1.to_room != room )  )
      { /* if not two way */
        map[x][y]=SECT_BLOCKED; /* one way into area OR maze */	
        return;
      } /* end two way */

/* selects door symbol */

      door_type = 0;
      if ( !IS_SET(pexit->exit_info, EX_ISDOOR) &&
		!IS_SET(pexit->exit_info, EX_HIDDEN))
      {
        if (  ( door == 0 )
           || ( door == 2 ) )
          door_type = DOOR_NS;
        else
          door_type = DOOR_EW;
      }
	else if (IS_SET(pexit->exit_info, EX_HIDDEN))
	{
	   door_type = DOOR_HIDDEN;
	}
      else if ( IS_SET( pexit->exit_info, EX_LOCKED ) )
      {
        door_type = DOOR_LOCKED;
      }
      else if ( IS_SET( pexit->exit_info, EX_CLOSED ) )   
      {
        door_type = DOOR_CLOSED;
      }
      else
       door_type = DOOR_OPEN;
      if (  ( !IS_NPC( ch ) )
         && ( !str_cmp( pexit->keyword, "" )  )
         && (  ( door_type <= DOOR_OPEN )
            || ( !IS_SET(pexit->exit_info, EX_ISDOOR ) )
            || (  ( IS_SET( pexit->exit_info, EX_CLOSED ) )
               && ( !str_cmp( pexit->keyword, "" ) )  )  ) )
      {
        map[x+door_marks[door][0]][y+door_marks[door][1]]=door_type;
        if (  ( door_type < DOOR_CLOSED )
           && (  ( line_of_sight == LOS_INITIAL )
              || ( door == line_of_sight )  )
           && (map[x+offsets[door][0]][y+offsets[door][1]]==SECT_UNSEEN) )
        {
          MapArea (pexit->u1.to_room,ch,
                    x+offsets[door][0], y+offsets[door][1],min,max, 
           ( line_of_sight == LOS_INITIAL ) ?
             door :
             line_of_sight );
        }
      }
    }
/* end if exit there */
  }
  return;
}

void ShowRoom( CHAR_DATA *ch, int min, int max, int size, int center )
{
  void disp_map(char *border, char *map, CHAR_DATA *ch);
  int x,y, looper;
  char outbuf[MSL];
  char catbuf[MSL];
  char borderbuf[MSL];
  char colorbuf[MSL];
  char displaybuf[MSL];
  outbuf[0] = '\0';
  sprintf( outbuf, "%s", "\n\r" );
  sprintf( borderbuf, "%s", "{y+{W" );
  for ( looper = 0; looper <= size+1; looper++ )
  {
    sprintf( catbuf, "%s", "-" );
    strcat( borderbuf, catbuf );
  }
  strcat( borderbuf, "{y+{x" );
  for (x = min; x <= max; ++x) 
  { /* every row */

    strcat( outbuf, "{W| " );
    for (y = min; y <= max; ++y)
    { /* every column */
      if ( (y==min) || (map[x][y-1]!=map[x][y]) )
      {
        sprintf( colorbuf, "%s%s",
          ( ( map[x][y]<= 0 ) ?
              get_door_color( map[x][y] ) :
              get_sector_color( map[x][y] ) ),
          ( ( contents[x][y].string[0] == '\0' ) ?
              "" :
              get_invert_color( map[x][y] ) )  );
        sprintf( displaybuf, "%s",
          ( ( map[x][y]<= 0 ) ?
              get_door_display( map[x][y] ) :
              ( ( contents[x][y].string[0] == '\0' ) ?
                get_sector_display(map[x][y] ) :
                contents[x][y].string )  )  );
        sprintf( catbuf, "%s%s", 
                 colorbuf, displaybuf );
        strcat( outbuf, catbuf  );

      }
      else 
      {
        sprintf( catbuf, "%s", 
          (map[x][y]<=0) ?
          get_door_display( map[x][y] ) :
          get_sector_display(map[x][y]) );
        strcat( outbuf, catbuf  );
      }

    }
    strcat( outbuf, " {W|{x\n\r" );
  }

  disp_map(borderbuf, outbuf, ch);

/* NOTE: with your imp, probably put the three together into one string? */

  return;
}




/* mlk :: shows a map, specified by size */


void ShowMap( CHAR_DATA *ch, int min, int max, int size, int center )
{
  int x,y, looper;
  char outbuf[MSL];
  char catbuf[MSL];
  char borderbuf[MSL];
  char colorbuf[MSL];
  char displaybuf[MSL];
  outbuf[0] = '\0';
  sprintf( outbuf, "%s", "\n\r" );
  sprintf( borderbuf, "%s", "{y+{W" );
  for ( looper = 0; looper <= size+1; looper++ )
  {
    sprintf( catbuf, "%s", "-" );
    strcat( borderbuf, catbuf );
  }
  strcat( borderbuf, "{y+{x" );
  for (x = min; x <= max; ++x) 
  { /* every row */

    strcat( outbuf, "{W| " );
    for (y = min; y <= max; ++y)
    { /* every column */
      if ( (y==min) || (map[x][y-1]!=map[x][y]) )
      {
        sprintf( colorbuf, "%s%s",
          ( ( map[x][y]<= 0 ) ?
              get_door_color( map[x][y] ) :
              get_sector_color( map[x][y] ) ),
          ( ( contents[x][y].string[0] == '\0' ) ?
              "" :
              get_invert_color( map[x][y] ) )  );
        sprintf( displaybuf, "%s",
          ( ( map[x][y]<= 0 ) ?
              get_door_display( map[x][y] ) :
              ( ( contents[x][y].string[0] == '\0' ) ?
                get_sector_display(map[x][y] ) :
                contents[x][y].string )  )  );
        sprintf( catbuf, "%s%s", 
                 colorbuf, displaybuf );
        strcat( outbuf, catbuf  );

      }
      else 
      {
        sprintf( catbuf, "%s", 
          (map[x][y]<=0) ?
          get_door_display( map[x][y] ) :
          get_sector_display(map[x][y]) );
        strcat( outbuf, catbuf  );
      }

    }
    strcat( outbuf, " {W|{x\n\r" );
  }
  send_to_char( "\n\r", ch );
  /* this is the top line of the map itself, currently not part of the mapstring */
  send_to_char( borderbuf, ch );
  /* this is the contents of the map */
  send_to_char( outbuf, ch );
  /* this is the bottom line of the map */
  send_to_char( borderbuf, ch );
  send_to_char( "\n\r", ch );
  send_to_char( "Also try map legend.\n\r", ch );

  return;
}










void do_mapper( CHAR_DATA *ch, char *argument )
{
  int size,center,x,y,min,max,looper;
  char arg1[MSL];
  char catbuf[MSL];
  char outbuf[MSL];
 one_argument( argument, arg1 );
 if ( is_name( arg1, "legend key help" ) )
 {
   sprintf( outbuf, "{WMap Legend:{x\n\r" );
   for ( looper = 0; looper < SECT_UNSEEN;looper++ )
   {
      sprintf( catbuf, "%s%s : {x%s\n\r" , 
                map_info[looper].display_color,
                map_info[looper].display_code, 
                map_info[looper].desc );
      strcat( outbuf, catbuf );
   }
   for ( looper = 0; looper < 5 ;looper++ )
   {
      sprintf( catbuf, "%s%s : {x%s\n\r" , 
                door_info[looper].display_color,
                door_info[looper].display_code, 
                door_info[looper].desc );
      strcat( outbuf, catbuf );
   }
   send_to_char( outbuf, ch );
   return;
 }
 size = ( is_number(  arg1 ) ) ? 
         atoi (arg1) :
         11;

  if (size > 25) { send_to_char("Map too big!\n\r",ch); return; }

  if ( size != 7 )
  {
     size = IS_IMMORTAL( ch ) ? size :
       (get_curr_stat(ch,STAT_INT) / 2 + 100/25);
    if ( size % 2  == 0 )
      size +=1;
    size = URANGE( 9, size, MAX_MAP );
  }
 size=URANGE(3,size,MAX_MAP);

 center=MAX_MAP/2;

 min = MAX_MAP/2-size/2;
 max = MAX_MAP/2+size/2;

 for (x = 0; x < MAX_MAP; ++x)
   for (y = 0; y < MAX_MAP; ++y)
   {
      map[x][y]=SECT_UNSEEN;
      contents[x][y].string[0] = '\0';
   }

/* starts the mapping with the center room */
  MapArea(ch->in_room, ch, center, center, min-1, max, LOS_INITIAL); 

/* marks the center, where ch is */
  sprintf( contents[center][center].string, "{f%s{x", "{f*{x" ); 
  if ( size == 7 )
    ShowRoom( ch, min, max, size, center ); 
  else
    ShowMap( ch, min, max, size, center );
  return;

}


