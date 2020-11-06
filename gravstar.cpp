/*
Copyright (C) 2019 BrerDawg

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

//gravstar.cpp
//v1.01	  05-jan-2016
//v1.02	  02-jun-2016		//added: meteors, auto adj floating cb_timer1 time value, this keeps frame time constant accross platforms, see: 'measured_tframe', 'corrected_timer1_trame', 'measured_tframe_avg'

//v1.03	  21-nov-2018		//minor 64 bit mods, more features added
//v1.04	  23-oct-2020		//added window/taskbar icon, added magnetic mines


//mingw needs library -lmsvcp60 for mcrtomb type calls:
//from Makefile.win that dev-c++ uses, these params were used to compile and link:
//LIBS =  -L"C:/Dev-Cpp/lib" -lmsvcp60 -lfltk_images -lfltk_jpeg -mwindows -lfltk -lole32 -luuid -lcomctl32 -lwsock32 -lm 
//INCS =  -I"C:/Dev-Cpp/include" 
//CXXINCS =  -I"C:/Dev-Cpp/lib/gcc/mingw32/3.4.2/include"  -I"C:/Dev-Cpp/include/c++/3.4.2/backward"  -I"C:/Dev-Cpp/include/c++/3.4.2/mingw32"  -I"C:/Dev-Cpp/include/c++/3.4.2"  -I"C:/Dev-Cpp/include" 
//CXXFLAGS = $(CXXINCS)  
//CFLAGS = $(INCS) -DWIN32 -mms-bitfields 

//#define compile_for_windows	//!!!!!!!!!!! uncomment for a dos/windows compile

//use -mconsole in make's LIBS shown above for windows to use the dos console, rather a attaching a console

#include "gravstar.h"
#include <FL/Fl_Pixmap.H>
#include <FL/Fl_RGB_Image.H>

//#include "16x16music01.xmp"
//#include "audacity16.xpm"
#include "gravstar.xpm"



//asm("int3");						//useful to trigger debugger






//global objs
dble_wnd *wndMain;
mywnd *a_wnd = 0;
PrefWnd* pref_wnd2 = 0;
int dbg_framecntr = 0;


Fl_Text_Buffer *tb_csl = 0;
Fl_Text_Editor *te_csl = 0;

Fl_Menu_Bar* meMain;
Fl_Menu_Bar* menu2;


//global vars
string csIniFilename;
int iBorderWidth;
int iBorderHeight;
int gi0,gi1,gi2,gi3,gi4;
double gd0,gd1,gd2,gd3,gd4;
string app_path;						//path this app resides on
string dir_seperator="\\";				//assume dos\windows folder directory seperator
string sglobal;
string sg_col1, sg_col2;
int gi;
int mode;
int font_num = cnFontEditor;
int font_size = cnSizeEditor;
//static Fl_Font extra_font;


int iAppExistDontRun = 1;	//if set, app probably exist and user doesn't want to run another
							//set it to assume we don't want to run another incase user closes
							//wnd without pressing either 'Run Anyway' or 'Don't Run, Exit' buttons


#ifndef compile_for_windows
Display *gDisp;
#endif

//windows code
#ifdef compile_for_windows
PROCESS_INFORMATION processInformation;
#endif

string sdebug;							//holds all cslpf output 
bool my_verbose = 1;


int pref_always_have_meteors = 0;
int pref_always_have_grav_objs = 0;
int pref_always_have_magmines = 0;
int pref_frames_per_second = 1;


//function prototypes
void LoadSettings(string csIniFilename);
void SaveSettings(string csIniFilename);
int CheckInstanceExists(string csAppName);
int RunShell( string sin );




//callbacks
void cslpf( const char *fmt,... );
void cb_wndmain(Fl_Widget*, void* v);
void cb_btAbout(Fl_Widget *, void *);
void cb_btQuit(Fl_Widget *, void *);
void cb_timer1(void *);
void DoQuit();
void cb_bt_help( Fl_Widget *, void *);
void cb_pref(Fl_Widget*w, void* v);
void cb_bt_new_game(Fl_Widget*w, void* v);





//--------------------- Main Menu --------------------------
Fl_Menu_Item menuitems[] =
{
	{ "&File",              0, 0, 0, FL_SUBMENU },
//		{ "&New Match", FL_CTRL + 'n'	, (Fl_Callback *)cb_bt_new_match, 0 },
		{ "&New Game", 0	, (Fl_Callback *)cb_bt_new_game, 0 },
		{ "E&xit", 0	, (Fl_Callback *)cb_btQuit, 0 },
		{ 0 },

	{ "&Edit", 0, 0, 0, FL_SUBMENU },
		{ "&Preferences...",  0, (Fl_Callback *)cb_pref},
		{ 0 },

	{ "&Help", 0, 0, 0, FL_SUBMENU },
		{ "&About", 0				, (Fl_Callback *)cb_btAbout, 0 },
		{ "'help.txt'", 0			, (Fl_Callback *)cb_bt_help, 0 },
		{ 0 },


	{ 0 }
};


















mywnd::mywnd( int xx, int yy, int wid, int hei, const char *label ) : dble_wnd( xx, yy, wid ,hei, label )
{
menu_hei = 25;
is_inside = 0;

m1_timer.time_start( m1_timer.ns_tim_start );

pi = M_PI;
pi2 = M_PI/2.0;
twopi = 2.0 *  M_PI;

sizex = w();
sizey = h();

midx = sizex / 2;
midy = sizey / 2;


//buf = 0;
//offscr = 0;
//jpg = 0;
ctrl_key = 0;
shift_key = 0;
up_key = 0;
down_key = 0;
left_key = 0;
right_key = 0;
space_key = 0;
n_key = 0;
y_key = 0;
q_key = 0;
x_key = 0;
z_key = 0;
p_key = 0;
option_key1 = 0;
option_key2 = 0;
option_key3 = 0;
option_key4 = 0;
option_key5 = 0;

paused = 1;
game_to_start = 1;
game_ended = 0;
attract_mode_active = 0;

mousewheel = 0;
//time_factor = 1.0;

ref_tframe = 0.02;

tframe = ref_tframe;							//desired time per frame
corrected_timer1_trame = tframe;
measured_tframe = tframe;
frame_factor = ref_tframe / 0.02;				//this will be 1.0 if frame time is 0.02 (50fps), or 0.5 when frame time is 0.01 (100fps), its used to scale values
measured_tframe_avg = ref_tframe;
measured_tframe_avg_cnt = 0;

frames_till_next_medic = 60.0 / tframe;

bounding_scale_down = 0.8;

disable_deaths = 0;								//set this to stop any ships being destroyed, useful for debugging

init( 1 );

string s1;
mystr m1, m2;


//load training data

//string sfname = "zip.train";
//m1.readfile( sfname.c_str(), 500000 );

//printf("attempting load of file: '%s'\n", sfname.c_str() );




//jpg = new Fl_JPEG_Image( "earthmap.jpg" );      // load jpeg image into ram
//printf("Image dimensions w=%d, h=%d, depth=%d\n", jpg->w(), jpg->h(), jpg->d() );

//bx_image = new Fl_Box( 50, h() - 50, jpg->w(), jpg->h() );


//bx_image->image( jpg );

}







mywnd::~mywnd()
{

//if ( buf != 0 ) delete buf;

//if ( offscr != 0 ) fl_delete_offscreen( offscr );




//if ( jpg != 0 ) delete jpg;

}









//collects indexes of missles in flight for spec owner, if any 
//returns number of found indexes if any, else -1
int mywnd::missile_in_flight( int owner_idx, vector<int> &vint )
{

vint.clear();

for( int i = 0; i < vobj.size(); i++ )
	{
	st_obj_tag o = vobj[i];
	
	if( ( owner_idx == 0 ) && ( o.type == en_ot_missile_user ) )
		{
		if( ( o.time_to_live > 0 ) | ( !o.mark_to_delete ) )
			{
			vint.push_back( i );
			}
		}

	if( ( owner_idx == 1 ) && ( o.type == en_ot_missile_cpu ) )
		{
		if( ( o.time_to_live > 0 ) | ( !o.mark_to_delete ) )
			{
			vint.push_back( i );
			}
		}

	}

if( vint.size() != 0 ) return vint.size();
else return -1;
}









//collects indexes of chaff objects, if any 
//returns number of found indexes if any, else -1
int mywnd::get_chaff_objs(  int owner_idx, vector<int> &vint )
{

vint.clear();

for( int i = 0; i < vobj.size(); i++ )
	{
	st_obj_tag o = vobj[i];

	if( ( owner_idx == 0 ) && ( o.type == en_ot_chaff_user ) )
		{
		if( ( o.time_to_live > 0 ) | ( !o.mark_to_delete ) )
			{
			vint.push_back( i );
			}
		}

	if( ( owner_idx == 1 ) && ( o.type == en_ot_chaff_cpu ) )
		{
		if( ( o.time_to_live > 0 ) | ( !o.mark_to_delete ) )
			{
			vint.push_back( i );
			}
		}

	}

if( vint.size() != 0 ) return vint.size();
else return -1;
}






//collects indexes of gravity objects, if any 
//returns number of found indexes if any, else -1
int mywnd::get_grav_objs( vector<int> &vint )
{

vint.clear();

for( int i = 0; i < vobj.size(); i++ )
	{
	st_obj_tag o = vobj[i];
	
	if( o.type == en_ot_grav_obj )
		{
		vint.push_back( i );
		}

	}

if( vint.size() != 0 ) return vint.size();
else return -1;
}









//collects indexes of meteor objects, if any 
//returns number of found indexes if any, else -1
int mywnd::get_meteor_objs( vector<int> &vint )
{

vint.clear();

for( int i = 0; i < vobj.size(); i++ )
	{
	st_obj_tag o = vobj[i];
	
	if( o.type == en_ot_meteor )
		{
		vint.push_back( i );
		}

	}

if( vint.size() != 0 ) return vint.size();
else return -1;
}







//add a medic obj, its used to repair the ship that docks/steals it first
void mywnd::add_medic( double xx, double yy, double dx, double dy )
{
st_obj_tag o;
st_line_tag oo;

//build medic obj
o.vline.clear();

o.col = 0x000080ff;
o.col_temp = 0x000000ff;
o.col_temp_frames_total = 10;
o.col_temp_frame_cnt = 0;

o.type = en_ot_medic;
o.x = xx;
o.y = yy;
o.theta = M_PI / 2;
o.thrust = 0;
o.delta_spin = 0.01;
o.dx = dx;
o.dy = dy;
o.dv = sqrt( pow( o.dx, 2 ) + pow( o.dy, 2 ) );

o.accel = 0;
o.accelx = 0;
o.accely = 0;
o.drag = 1;

o.finite_life = 0;
o.time_to_live = 0;
o.mark_to_delete = 0;
o.has_thrust = 1;
o.can_wrap = 1;
o.hits = 0;
o.frames_per_bullet = 8;
o.bullet_frame_cnt = 0;                         
o.is_exploding = 0;
o.explode_frames_total = 5.0 / tframe;;
o.explode_frame_cnt = o.explode_frames_total;
o.explode_shape = ((rand() % 4 ) >= 2 );     //just uses set or reset
o.grav_affected = 1;


oo.x1 = -3;
oo.y1 = 10;
oo.x2 = 3;
oo.y2 = 10;

oo.col = 0x000080ff;
o.vline.push_back( oo );


oo.x1 = 3;
oo.y1 = 10;
oo.x2 = 3;
oo.y2 = 3;

oo.col = 0x000080ff;
o.vline.push_back( oo );


oo.x1 = 3;
oo.y1 = 3;
oo.x2 = 10;
oo.y2 = 3;

oo.col = 0x000080ff;
o.vline.push_back( oo );


oo.x1 = 10;
oo.y1 = 3;
oo.x2 = 10;
oo.y2 = -3;

oo.col = 0x000080ff;
o.vline.push_back( oo );


oo.x1 = 10;
oo.y1 = -3;
oo.x2 = 3;
oo.y2 = -3;

oo.col = 0x000080ff;
o.vline.push_back( oo );


oo.x1 = 3;
oo.y1 = -3;
oo.x2 = 3;
oo.y2 = -10;

oo.col = 0x000080ff;
o.vline.push_back( oo );


oo.x1 = 3;
oo.y1 = -10;
oo.x2 = -3;
oo.y2 = -10;

oo.col = 0x000080ff;
o.vline.push_back( oo );

oo.x1 = -3;
oo.y1 = -10;
oo.x2 = -3;
oo.y2 = -3;

oo.col = 0x000080ff;
o.vline.push_back( oo );


oo.x1 = -3;
oo.y1 = -3;
oo.x2 = -10;
oo.y2 = -3;

oo.col = 0x000080ff;
o.vline.push_back( oo );

oo.x1 = -10;
oo.y1 = -3;
oo.x2 = -10;
oo.y2 = 3;

oo.col = 0x000080ff;
o.vline.push_back( oo );

oo.x1 = -10;
oo.y1 = 3;
oo.x2 = -3;
oo.y2 = 3;

oo.col = 0x000080ff;
o.vline.push_back( oo );


oo.x1 = -3;
oo.y1 = 3;
oo.x2 = -3;
oo.y2 = 10;

oo.col = 0x000080ff;
o.vline.push_back( oo );

vobj.push_back( o );
set_bounding_rect( vobj.size() - 1, bounding_scale_down, bounding_scale_down );

}


















//add a meteor obj
void mywnd::add_meteor( int shape, bool affected_by_grav, bool apply_dxdy, double scale, double xx, double yy, double dx, double dy )
{

st_obj_tag o;
st_line_tag oo;

	
double d1 = -1.0;										//for explosion
double d2 = 2.0;


o.scale = scale;

if( shape == 0 )
	{
	//build meteor obj
	o.vline.clear();

	o.col = 0x00a0a0a0;
	o.col_temp = 0x000000ff;
	o.col_temp_frames_total = 10;
	o.col_temp_frame_cnt = 0;

	o.type = en_ot_meteor;
	o.x = xx;
	o.y = yy;
	o.theta = M_PI / 2;
	o.thrust = 0;
	o.delta_spin = twopi / 120.0 * frame_factor;
	o.dx = dx;
	o.dy = dy;
	if( !apply_dxdy ) { o.dx = 0; o.dy = 0; }
	o.dv = sqrt( pow( o.dx, 2 ) + pow( o.dy, 2 ) );

	o.mass = 1e60 * scale;
	o.accel = 0;
	o.accelx = 0;
	o.accely = 0;
	o.drag = 1e-60;

	o.finite_life = 0;
	o.time_to_live = 0;
	o.mark_to_delete = 0;
	o.has_thrust = 1;
	o.can_wrap = 1;
	o.hits = 0;
	o.frames_per_bullet = 8;
	o.bullet_frame_cnt = 0;                         
	o.is_exploding = 0;
	o.explode_frames_total = 5.0 / tframe;;
	o.explode_frame_cnt = o.explode_frames_total;
	o.explode_shape = ((rand() % 4 ) >= 2 );     //just uses set or reset
	o.grav_affected = affected_by_grav;

	oo.x1 = 7;
	oo.y1 = 16;
	oo.x2 = 17;
	oo.y2 = 6;
	oo.dx = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );
	oo.dy = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );

	oo.col = o.col;
	o.vline.push_back( oo );


	oo.x1 = 17;
	oo.y1 = 6;
	oo.x2 = 27;
	oo.y2 = 10;
	oo.dx = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );
	oo.dy = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );

	//oo.col = 0x000080ff;
	o.vline.push_back( oo );


	oo.x1 = 27;
	oo.y1 = 10;
	oo.x2 = 36;
	oo.y2 = 5;
	oo.dx = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );
	oo.dy = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );

	//oo.col = 0x000080ff;
	o.vline.push_back( oo );


	oo.x1 = 36;
	oo.y1 = 5;
	oo.x2 = 46;
	oo.y2 = 15;
	oo.dx = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );
	oo.dy = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );

	//oo.col = 0x000080ff;
	o.vline.push_back( oo );


	oo.x1 = 46;
	oo.y1 = 15;
	oo.x2 = 37;
	oo.y2 = 21;
	oo.dx = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );
	oo.dy = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );

	//oo.col = 0x000080ff;
	o.vline.push_back( oo );


	oo.x1 = 37;
	oo.y1 = 21;
	oo.x2 = 46;
	oo.y2 = 30;
	oo.dx = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );
	oo.dy = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );

	//oo.col = 0x000080ff;
	o.vline.push_back( oo );


	oo.x1 = 46;
	oo.y1 = 30;
	oo.x2 = 36;
	oo.y2 = 44;
	oo.dx = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );
	oo.dy = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );

	//oo.col = 0x000080ff;
	o.vline.push_back( oo );

	oo.x1 = 36;
	oo.y1 = 44;
	oo.x2 = 23;
	oo.y2 = 40;
	oo.dx = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );
	oo.dy = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );

	//oo.col = 0x000080ff;
	o.vline.push_back( oo );


	oo.x1 = 23;
	oo.y1 = 40;
	oo.x2 = 17;
	oo.y2 = 44;
	oo.dx = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );
	oo.dy = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );

	//oo.col = 0x000080ff;
	o.vline.push_back( oo );

	oo.x1 = 17;
	oo.y1 = 44;
	oo.x2 = 7;
	oo.y2 = 34;
	oo.dx = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );
	oo.dy = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );

	//oo.col = 0x000080ff;
	o.vline.push_back( oo );

	oo.x1 = 7;
	oo.y1 = 34;
	oo.x2 = 11;
	oo.y2 = 25;
	oo.dx = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );
	oo.dy = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );

	//oo.col = 0x000080ff;
	o.vline.push_back( oo );


	oo.x1 = 11;
	oo.y1 = 25;
	oo.x2 = 7;
	oo.y2 = 16;
	oo.dx = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );
	oo.dy = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );

	//oo.col = 0x000080ff;
	o.vline.push_back( oo );

	vobj.push_back( o );

	obj_vertex_offset( vobj.size() - 1,  -( 46 - 7 ) / 2.0,   -( 44 - 4 ) / 2.0  );
	obj_vertex_scale( vobj.size() - 1,  scale,   scale );
	set_bounding_rect( vobj.size() - 1, bounding_scale_down, bounding_scale_down );
	}


if( shape == 1 )
	{
	//build meteor obj
	o.vline.clear();

	o.col = 0x00a0a0a0;
	o.col_temp = 0x000000ff;
	o.col_temp_frames_total = 10;
	o.col_temp_frame_cnt = 0;

	o.type = en_ot_meteor;
	o.x = xx;
	o.y = yy;
	o.theta = M_PI / 2;
	o.thrust = 0;
	o.delta_spin = twopi / 100.0;
	o.dx = dx;
	o.dy = dy;
	if( !apply_dxdy ) { o.dx = 0; o.dy = 0; }
	o.dv = sqrt( pow( o.dx, 2 ) + pow( o.dy, 2 ) );

	o.mass = 1e60 * scale;
	o.accel = 0;
	o.accelx = 0;
	o.accely = 0;
	o.drag = 1e-60;

	o.finite_life = 0;
	o.time_to_live = 0;
	o.mark_to_delete = 0;
	o.has_thrust = 1;
	o.can_wrap = 1;
	o.hits = 0;
	o.frames_per_bullet = 8;
	o.bullet_frame_cnt = 0;                         
	o.is_exploding = 0;
	o.explode_frames_total = 5.0 / tframe;;
	o.explode_frame_cnt = o.explode_frames_total;
	o.explode_shape = ((rand() % 4 ) >= 2 );     //just uses set or reset
	o.grav_affected = affected_by_grav;

	oo.x1 = 16;
	oo.y1 = 4;
	oo.x2 = 30;
	oo.y2 = 4;
	oo.dx = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );
	oo.dy = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );

	oo.col = o.col;
	o.vline.push_back( oo );


	oo.x1 = 30;
	oo.y1 = 4;
	oo.x2 = 43;
	oo.y2 = 13;
	oo.dx = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );
	oo.dy = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );

	o.vline.push_back( oo );


	oo.x1 = 43;
	oo.y1 = 13;
	oo.x2 = 44;
	oo.y2 = 19;
	oo.dx = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );
	oo.dy = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );

	o.vline.push_back( oo );


	oo.x1 = 44;
	oo.y1 = 19;
	oo.x2 = 31;
	oo.y2 = 24;
	oo.dx = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );
	oo.dy = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );

	o.vline.push_back( oo );


	oo.x1 = 31;
	oo.y1 = 24;
	oo.x2 = 43;
	oo.y2 = 32;
	oo.dx = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );
	oo.dy = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );

	o.vline.push_back( oo );


	oo.x1 = 43;
	oo.y1 = 32;
	oo.x2 = 33;
	oo.y2 = 43;
	oo.dx = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );
	oo.dy = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );

	o.vline.push_back( oo );


	oo.x1 = 33;
	oo.y1 = 43;
	oo.x2 = 29;
	oo.y2 = 38;
	oo.dx = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );
	oo.dy = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );

	o.vline.push_back( oo );

	oo.x1 = 29;
	oo.y1 = 38;
	oo.x2 = 15;
	oo.y2 = 43;
	oo.dx = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );
	oo.dy = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );

	o.vline.push_back( oo );


	oo.x1 = 15;
	oo.y1 = 43;
	oo.x2 = 5;
	oo.y2 = 28;
	oo.dx = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );
	oo.dy = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );

	o.vline.push_back( oo );

	oo.x1 = 15;
	oo.y1 = 43;
	oo.x2 = 5;
	oo.y2 = 28;
	oo.dx = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );
	oo.dy = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );

	o.vline.push_back( oo );

	oo.x1 = 5;
	oo.y1 = 28;
	oo.x2 = 5;
	oo.y2 = 14;
	oo.dx = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );
	oo.dy = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );

	o.vline.push_back( oo );

	oo.x1 = 5;
	oo.y1 = 14;
	oo.x2 = 9;
	oo.y2 = 14;
	oo.dx = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );
	oo.dy = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );

	o.vline.push_back( oo );

	oo.x1 = 9;
	oo.y1 = 14;
	oo.x2 = 16;
	oo.y2 = 4;
	oo.dx = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );
	oo.dy = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );

	o.vline.push_back( oo );

	vobj.push_back( o );

	obj_vertex_offset( vobj.size() - 1,  -( 44 - 5 ) / 2.0,   -( 43 - 4 ) / 2.0  );
	obj_vertex_scale( vobj.size() - 1,  scale,   scale );
	set_bounding_rect( vobj.size() - 1, bounding_scale_down, bounding_scale_down );
	}








if( shape == 2 )
	{
	//build meteor obj
	o.vline.clear();

	o.col = 0x00a0a0a0;
	o.col_temp = 0x000000ff;
	o.col_temp_frames_total = 10;
	o.col_temp_frame_cnt = 0;

	o.type = en_ot_meteor;
	o.x = xx;
	o.y = yy;
	o.theta = M_PI / 2;
	o.thrust = 0;
	o.delta_spin = -twopi / 90.0;
	o.dx = dx;
	o.dy = dy;
	if( !apply_dxdy ) { o.dx = 0; o.dy = 0; }
	o.dv = sqrt( pow( o.dx, 2 ) + pow( o.dy, 2 ) );

	o.mass = 1e60 * scale;
	o.accel = 0;
	o.accelx = 0;
	o.accely = 0;
	o.drag = 1e-60;

	o.finite_life = 0;
	o.time_to_live = 0;
	o.mark_to_delete = 0;
	o.has_thrust = 1;
	o.can_wrap = 1;
	o.hits = 0;
	o.frames_per_bullet = 8;
	o.bullet_frame_cnt = 0;                         
	o.is_exploding = 0;
	o.explode_frames_total = 5.0 / tframe;;
	o.explode_frame_cnt = o.explode_frames_total;
	o.explode_shape = ((rand() % 4 ) >= 2 );     //just uses set or reset
	o.grav_affected = affected_by_grav;

	oo.x1 = 13;
	oo.y1 = 4;
	oo.x2 = 22;
	oo.y2 = 14;
	oo.dx = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );
	oo.dy = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );

	oo.col = o.col;
	o.vline.push_back( oo );


	oo.x1 = 22;
	oo.y1 = 14;
	oo.x2 = 33;
	oo.y2 = 4;
	oo.dx = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );
	oo.dy = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );

	o.vline.push_back( oo );


	oo.x1 = 33;
	oo.y1 = 4;
	oo.x2 = 42;
	oo.y2 = 14;
	oo.dx = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );
	oo.dy = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );

	o.vline.push_back( oo );


	oo.x1 = 42;
	oo.y1 = 14;
	oo.x2 = 36;
	oo.y2 = 23;
	oo.dx = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );
	oo.dy = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );

	o.vline.push_back( oo );


	oo.x1 = 36;
	oo.y1 = 23;
	oo.x2 = 41;
	oo.y2 = 32;
	oo.dx = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );
	oo.dy = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );

	o.vline.push_back( oo );


	oo.x1 = 41;
	oo.y1 = 32;
	oo.x2 = 26;
	oo.y2 = 43;
	oo.dx = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );
	oo.dy = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );

	o.vline.push_back( oo );


	oo.x1 = 26;
	oo.y1 = 43;
	oo.x2 = 13;
	oo.y2 = 43;
	oo.dx = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );
	oo.dy = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );

	o.vline.push_back( oo );

	oo.x1 = 13;
	oo.y1 = 43;
	oo.x2 = 3;
	oo.y2 = 32;
	oo.dx = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );
	oo.dy = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );

	o.vline.push_back( oo );


	oo.x1 = 3;
	oo.y1 = 32;
	oo.x2 = 3;
	oo.y2 = 14;
	oo.dx = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );
	oo.dy = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );

	o.vline.push_back( oo );

	oo.x1 = 3;
	oo.y1 = 14;
	oo.x2 = 13;
	oo.y2 = 4;
	oo.dx = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );
	oo.dy = d1 + d2 * ( rand_range( 0, 100 ) / 100.0 );

	o.vline.push_back( oo );

	vobj.push_back( o );

	obj_vertex_offset( vobj.size() - 1,  -( 42 - 3 ) / 2.0,   -( 43 - 4 ) / 2.0  );
	obj_vertex_scale( vobj.size() - 1,  scale,   scale );
	set_bounding_rect( vobj.size() - 1, bounding_scale_down, bounding_scale_down );
	}
}











void mywnd::obj_vertex_offset( int idx, double offx, double offy )
{
if( idx >= vobj.size() ) return;

vector<st_line_tag> vo;
vo = vobj[ idx ].vline;

for( int j = 0; j < vo.size(); j++ )
	{
	double x1 = vo[ j ].x1;
	double y1 = vo[ j ].y1;
	double x2 = vo[ j ].x2;
	double y2 = vo[ j ].y2;
	
	vo[ j ].x1 = x1 + offx;
	vo[ j ].y1 = y1 + offy;
	vo[ j ].x2 = x2 + offx;
	vo[ j ].y2 = y2 + offy;
	}
	
vobj[ idx ].vline = vo;
}









void mywnd::obj_vertex_scale( int idx, double scalex, double scaley )
{
if( idx >= vobj.size() ) return;

vector<st_line_tag> vo;
vo = vobj[ idx ].vline;

for( int j = 0; j < vo.size(); j++ )
	{
	double x1 = vo[ j ].x1;
	double y1 = vo[ j ].y1;
	double x2 = vo[ j ].x2;
	double y2 = vo[ j ].y2;
	
	vo[ j ].x1 = x1 * scalex;
	vo[ j ].y1 = y1 * scaley;
	vo[ j ].x2 = x2 * scalex;
	vo[ j ].y2 = y2 * scaley;
	}
	
vobj[ idx ].vline = vo;
}








//make an integer random number between low and high, inclusively
int mywnd::rand_range( int low, int high )
{
return rand() % ( high - low + 1 ) + low;
}



















//generates a rnd num between -1.0 -> 1.0
double mywnd::rnd()
{
double drnd =  (double)( RAND_MAX / 2 - rand() ) / (double)( RAND_MAX / 2 );

return drnd;
}









//forms a bounding rect around obj extremes, applies a scale(fudge) to bounding rect to cater for poss of obj being rotate
void mywnd::set_bounding_rect( int idx, double scalex, double scaley )
{

if( idx >= vobj.size() ) return;

double minx = 0;
double miny = 0;
double maxx = 0;
double maxy = 0;


vector<st_line_tag> vo;
vo = vobj[ idx ].vline;

for( int j = 0; j < vo.size(); j++ )
	{
	double x1 = vo[ j ].x1;
	double y1 = vo[ j ].y1;
	double x2 = vo[ j ].x2;
	double y2 = vo[ j ].y2;
	
	if( x1 < minx ) minx = x1;
	if( x1 > maxx ) maxx = x1;
	
	if( y1 < miny ) miny = y1;
	if( y1 > maxy ) maxy = y1;

	if( x2 < minx ) minx = x2;
	if( x2 > maxx ) maxx = x2;
	
	if( y2 < miny ) miny = y2;
	if( y2 > maxy ) maxy = y2;
	}

vobj[ idx ].bound_x1 = minx * scalex;
vobj[ idx ].bound_y1 = miny * scaley;

vobj[ idx ].bound_x2 = maxx * scalex;
vobj[ idx ].bound_y2 = maxy * scaley;

}







void mywnd::new_game()
{
a_wnd->init( 1 );
a_wnd->game_ended = 1;
a_wnd->paused = 1;
a_wnd->game_to_start = 1;

attract_mode_active = 0;
attract_mode_cnt = cn_attract_mode_delaystart / tframe;
}






void mywnd::init( bool new_game )
{
srand( time(NULL) );

ship_mass_user = 7000;
ship_mass_cpu = 1000;

attract_mode_cnt = cn_attract_mode_delaystart / tframe;

bool heads_tails = rand_range( 0, 1 );     //just uses set or reset

missle_in_flight_cpu = -1;
missle_in_flight_user = -1;
chaff_user = -1;
chaff_cpu = -1;

evade_hits = max_ship_hits * 0.9;		//cpu ship to evade when this is reached

keyhit_disable_cnt = 0;

//if ( buf != 0 ) delete buf;


//buf = new unsigned char [ sizex * sizey * cn_bytes_per_pixel ];


if( new_game )
    {
    ships_left_user = 5;
    ships_left_cpu = 5;
    frame_counter = 0;
    frames_per_missle_launch = cn_time_per_missle_launch / tframe;
    next_missle_frame_cnt = frames_per_missle_launch;
    deaths_combined = 0;
    }



have_grav_objs = 0;
have_grav_collisions = 0;
have_meteor_objs = 0;
have_magmine_objs = 0;




//if( deaths_combined != 0 )			//have had the first death?
	{
	have_meteor_objs = rand_range( 0, 1 );
	have_magmine_objs = rand_range( 0, 1 );
	have_grav_objs = rand_range( 0, 1 );
	have_grav_collisions = 0;
	}

if( pref_always_have_meteors )  have_meteor_objs = 1;
if( pref_always_have_magmines )  have_magmine_objs = 1;
if( pref_always_have_grav_objs )  have_grav_objs = 1;

if( option_key1 )
	{
	}

if( option_key2 )
	{
	have_grav_objs = 0;
	}

if( option_key3 )
	{
	have_grav_objs = 0;
	have_meteor_objs = 0;
	}

if( option_key4 )
	{
	have_meteor_objs = 0;
	have_magmine_objs = 0;
	}

if( option_key5 )
	{
	have_grav_objs = 0;
	have_meteor_objs = 0;
	have_magmine_objs = 0;
	}




//have_grav_objs = 0;
//have_meteor_objs = 0;

vobj.clear();

max_ship_hits = 500;
max_meteor_hits = 2;
damage_warning_threshold = max_ship_hits * 0.8;
frames_till_next_medic = 60.0 / tframe;
have_a_ship_explosion = 0;
exploding_ship_user = 0;
exploding_ship_cpu = 0;
pause_ship_explosion = 0;


//cpu_ship_hard_turn_angle = 0;
//cpu_ship_thrust_jump = 0;
//cpu_ship_bullet_avoid_cnt = 0;

zoomx = 1;
zoomy = 1;

st_obj_tag o;
st_line_tag oo;

//build user's ship

o.col = 0x0000ffff;
o.col_temp = 0x000000ff;
o.col_temp_frames_total = 10;
o.col_temp_frame_cnt = 0;

o.type = en_ot_user_ship;
o.x = (double)midx / 1.2;
o.y =  (double)midy / 1.2;
//o.x = (double)midx / 3;
//o.y =  (double)midy / 3;
o.theta = M_PI;
o.theta_req = 0;
o.delta_theta_per_frame = 0;
o.delta_spin = 0;
o.thrust = 0;
o.thrust_max = 10000.0 * 1;			//make it relative to tframe: 10000 / 0.02, was: 10000
o.thrust_inc_per_frame = 0.1 * 1;
o.dx = 0;
o.dy = 0;
o.max_dx = 10.0 * 1;					//make it relative to tframe: 10 / 0.02, was: 10
o.max_dy = 10.0 * 1;

o.mass = ship_mass_user;
o.accel = 0;
o.accelx = 0;
o.accely = 0;
o.drag = 0.001 / 1;					//make it relative to tframe: 10 / 0.02, was: 0.01
o.grav_affected = 1;

o.finite_life = 0;
o.time_to_live = 0;
o.time_to_live = 0;
o.mark_to_delete = 0;
o.has_thrust = 1;
o.can_wrap = 1;
o.hits = 0;
o.time_per_bullet = 0.16;			//slightly less bullets for user ship compare to cpu's ship (8 * 0.02 was: 8 per frame)
o.time_bullet_remaining = 0;
o.time_per_magmine_fire = 0.25;
o.time_magmine_fire_remaining = 0;
o.frames_per_bullet = 8;			//slightly less bullets for user ship compare to cpu's ship
o.bullet_frame_cnt = 0;                         
o.is_exploding = 0;
o.explode_frames_total = 5.0 / tframe;;
o.explode_frame_cnt = o.explode_frames_total;
o.explode_shape = heads_tails;                  //just uses set or reset

o.timed_burn = 0;
o.timed_burn_thrust = 0;

o.idbg = 1;
o.dbgx = 0;
o.dbgy = 0;


oo.x1 = -7;
oo.y1 = -10;
oo.x2 = 0;
oo.y2 = 10;

oo.col = 0x0000ffff;
o.vline.push_back( oo );


oo.x1 = 0;
oo.y1 = 10;
oo.x2 = 7;
oo.y2 = -10;

oo.col = 0x0000ffff;
o.vline.push_back( oo );


oo.x1 = 7;
oo.y1 = -10;
oo.x2 = 0;
oo.y2 = -7;

oo.col = 0x0000ffff;
o.vline.push_back( oo );


oo.x1 = 0;
oo.y1 = -7;
oo.x2 = -7;
oo.y2 = -10;

oo.col = 0x0000ffff;
o.vline.push_back( oo );


//append_arcseg( o, 20, 100, 0, twopi, 0, 0 );

vobj.push_back( o );
set_bounding_rect( 0, bounding_scale_down, bounding_scale_down );




//build cpu's ship
o.vline.clear();

o.col = 0x0000ff00;
o.col_temp = 0x000000ff;
o.col_temp_frames_total = 10;
o.col_temp_frame_cnt = 0;

o.type = en_ot_cpu_ship;
o.x = -midx / 1.2;
o.y = -midy / 1.2;
//o.x = 0;
//o.y = 0;
o.theta = 0;
o.theta_req = 0;
o.delta_theta_per_frame = 0.1;
o.delta_spin = 0;
o.thrust_max = 3000.0 * 1;		//make it relative to tframe: 3000 / 0.02, was: 3000
o.thrust = 0;
o.dx = 0;
o.dy = 0;
o.max_dx = 100.0 * 1;				//make it relative to tframe: 2 / 0.02, was: 2
o.max_dy = 100.0 * 1;

o.mass = ship_mass_cpu;
o.accel = 0;
o.accelx = 0;
o.accely = 0;

o.drag = 0.01;				//make it relative to tframe: 0.995 / 0.02, was: 0.995
o.grav_affected = 1;

o.finite_life = 0;
o.time_to_live = 0;
o.time_to_live = 0;
o.mark_to_delete = 0;
o.has_thrust = 1;
o.can_wrap = 1;
o.hits = 0;
o.time_bullet_remaining = 0;
o.time_per_bullet = 0.14;			//slightly more bullets for cpu ship compare to user's ship (7 * 0.02 was: 7 per frame)
o.frames_per_bullet = 7;			//slightly more bullets for cpu ship compare to user's ship
o.bullet_frame_cnt = 0;                         
o.is_exploding = 0;
o.explode_frames_total = 5.0 / tframe;;
o.explode_frame_cnt = o.explode_frames_total;
o.explode_shape = !heads_tails;     //just uses set or reset

o.timed_burn = 0;
o.timed_burn_thrust = 0;

oo.x1 = -7;
oo.y1 = -10;
oo.x2 = 0;
oo.y2 = 10;

oo.col = 0x0000ff00;
o.vline.push_back( oo );


oo.x1 = 0;
oo.y1 = 10;
oo.x2 = 7;
oo.y2 = -10;

oo.col = 0x0000ff00;
o.vline.push_back( oo );


oo.x1 = 7;
oo.y1 = -10;
oo.x2 = 0;
oo.y2 = -7;

oo.col = 0x0000ff00;
o.vline.push_back( oo );


oo.x1 = 0;
oo.y1 = -7;
oo.x2 = -7;
oo.y2 = -10;

oo.col = 0x0000ff00;
o.vline.push_back( oo );


vobj.push_back( o );
set_bounding_rect( 1, bounding_scale_down, bounding_scale_down );


//add_missile( 1 );
//add_missile( 0 );

//have_grav_objs = 1;
if( have_grav_objs )
	{
	add_grav_obj( 0, 0, 0.1, 0.1 );

	if( !have_meteor_objs )
		{
		bool add_second_grav_obj = rand_range( 0, 1 );
		if( add_second_grav_obj ) add_grav_obj( -300, -200, 0.1, 0.1 );
		}
	}


//have_meteor_objs = 1;
if( have_meteor_objs )
	{
	
	bool affected_by_grav = 0;
	bool apply_dxdy = 0;
	
	if( rnd() > 0.0 )
		{
		affected_by_grav = 1;
		apply_dxdy = 1;
		}
	add_meteor( 0,    affected_by_grav,  affected_by_grav, 0.5 + rnd() / 4.0,    150,   120,   rnd() * 1.5,  rnd() * 2.8 );
	add_meteor( 1,    affected_by_grav,  affected_by_grav,  0.5 + rnd() / 4.0,    -150,  150,   rnd() * 1.5,  rnd() * 2.8 );
	add_meteor( 2,    affected_by_grav,  affected_by_grav,  0.5 + rnd() / 4.0,    -150, -150,   rnd() * 1.5,  rnd() * 2.8 );
	add_meteor( 0,    affected_by_grav,  affected_by_grav,  0.5,    -120,   -250,  rnd() * 1.5,  rnd() * 2.8 );
	add_meteor( 1,    affected_by_grav,  affected_by_grav,  0.5,    -50,   -250,    rnd() * 1.5,  rnd() * 2.8 );
	}


if( have_magmine_objs )
	{
	for( int i = 0; i < 10; i++ )
		{
		int xx1, yy1;
		double dx1, dy1, scle;
		bool affected_by_grav = 1;
		bool apply_dxdy = 1;
		float time_to_live = 0;
		xx1 = rnd() * 400.0;
		yy1 = rnd() * 400.0;
		dx1 = 0.0;
		dy1 = 0.0;
		scle = 1;
		add_magmine( 0, affected_by_grav,  apply_dxdy, scle,    xx1,   yy1,   dx1,  dy1, time_to_live );
		}
	}

}











void mywnd::setcolref( colref col )
{
fl_color( col.r , col.g , col.b );
}












void mywnd::delete_marked_objs()
{

loop1:

for( int i = 0; i < vobj.size(); i++ )
    {
    if ( vobj[ i ].mark_to_delete )                     //obj needs deletion?
        {
        vobj.erase( vobj.begin() + i ); 
        goto loop1;                                 //start from begining as vector is now shorter
        }

    }

}











bool once_off_collision_dbg = 0;



void mywnd::check_obj_collisions()
{
double x1, y1, x2, y2, xx1, yy1, xx2, yy2;

//return;


//make ship rects
x1 = vobj[ 1 ].x + vobj[ 1 ].bound_x1;				//cpu's ship
x2 = vobj[ 1 ].x + vobj[ 1 ].bound_x2;
y1 = vobj[ 1 ].y + vobj[ 1 ].bound_y1;
y2 = vobj[ 1 ].y + vobj[ 1 ].bound_y2;

xx1 = vobj[ 0 ].x + vobj[ 0 ].bound_x1;				//user's ship
xx2 = vobj[ 0 ].x + vobj[ 0 ].bound_x2;
yy1 = vobj[ 0 ].y + vobj[ 0 ].bound_y1;
yy2 = vobj[ 0 ].y + vobj[ 0 ].bound_y2;



/*
x1 = vobj[ 1 ].x - 5;
x2 = vobj[ 1 ].x + 5;
y1 = vobj[ 1 ].y - 5;
y2 = vobj[ 1 ].y + 5;


xx1 = vobj[ 0 ].x - 5;
xx2 = vobj[ 0 ].x + 5;
yy1 = vobj[ 0 ].y - 5;
yy2 = vobj[ 0 ].y + 5;
*/

bool ships_collided = 0;

if( !disable_deaths )
	{
	//check if cpu ship has any collisions
	for( int i = 0; i < vobj.size(); i++ )
		{
		double x, y;

		if( vobj[ i ].is_exploding ) continue;

		if( vobj[ i ].type == en_ot_bul_user )          	//user bullet collision?
			{
			x = vobj[ i ].x;
			y = vobj[ i ].y;
			
			if( ( x >= x1 ) && ( x <= x2 ) )
				{
				if( ( y >= y1 ) && ( y <= y2 ) )
					{
//					vobj[ 1 ].hits += 10;
					add_hits( 1, 10 );
					vobj[ i ].mark_to_delete = 1;           //remove bullet to show ship took the blow
					
					vobj[ 1 ].col_temp_frame_cnt = vobj[ 1 ].col_temp_frames_total;		//set temp 'hit' colour

					if( vobj[ 1 ].hits >= max_ship_hits )   //check if cpu ship exceeds its max hits
						{
//						if( !disable_deaths )
							{
							vobj[ 1 ].is_exploding = 1;
							ships_left_cpu--;
							deaths_combined++;
							printf( "cpu destroyed by 'en_ot_bul_user'\n" );
							sdeath = "cpu ship destroyed by bullet";
							}
						}
					}
				}
			}


		if( vobj[ i ].type == en_ot_user_ship )             //ship to ship collision?
			{
			
			if( ( xx1 >= x1 ) && ( xx1 <= x2 ) )
				{
				if( ( yy1 >= y1 ) && ( yy1 <= y2 ) )
					{
					ships_collided = 1;
					}
				}

			if( ( xx2 >= x1 ) && ( xx2 <= x2 ) )
				{
				if( ( yy2 >= y1 ) && ( yy2 <= y2 ) )
					{
					ships_collided = 1;
					}
				}

			if( ships_collided )
				{
//				vobj[ 0 ].is_exploding = 1;
				vobj[ 0 ].col_temp_frame_cnt = vobj[ 0 ].col_temp_frames_total;		//set temp 'hit' colour
//				ships_left_user--;

//				vobj[ 1 ].is_exploding = 1;
				vobj[ 1 ].col_temp_frame_cnt = vobj[ 1 ].col_temp_frames_total;		//set temp 'hit' colour
//				ships_left_cpu--;

//				deaths_combined += 2;

				add_hits( 0, 1 );
				add_hits( 1, 1 );

//				vobj[ 0 ].hits++;
//				vobj[ 1 ].hits++;

				if( vobj[ 0 ].hits >= max_ship_hits )   //check if cpu ship exceeds its max hits
					{
//					if( !disable_deaths )
						{
						vobj[ 0 ].is_exploding = 1;
						ships_left_user--;
						deaths_combined++;
						printf( "user destroyed by ships_collided\n" );
						sdeath = "user ship destroyed by ship to ship collision";
						}
					}

				if( vobj[ 1 ].hits >= max_ship_hits )   //check if cpu ship exceeds its max hits
					{
//					if( !disable_deaths )
						{
						vobj[ 1 ].is_exploding = 1;
						ships_left_cpu--;
						deaths_combined++;
						printf( "cpu destroyed by ships_collided\n" );
						sdeath = "cpu ship destroyed by ship to ship collision";
						}
					}

				}
			}



		if( vobj[ i ].type == en_ot_medic )             //medic collision?
			{
			x = vobj[ i ].x;
			y = vobj[ i ].y;

			if( ( x >= x1 ) && ( x <= x2 ) )
				{
				if( ( y >= y1 ) && ( y <= y2 ) )
					{
					vobj[ i ].mark_to_delete = 1;       //remove medic and clear hits
					vobj[ 1 ].hits = 0;
					}
				}        
			}


/*
		if( have_grav_collisions )
			{
			if( vobj[ i ].type == en_ot_grav_obj )         			//cpu's ship hit by grav obj?
				{
				x = vobj[ i ].x;
				y = vobj[ i ].y;
				
				if( ( x >= x1 ) && ( x <= x2 ) )
					{
					if( ( y >= y1 ) && ( y <= y2 ) )
						{                
//						if( !disable_deaths )
							{
							vobj[ 1 ].is_exploding = 1;
							vobj[ 1 ].col_temp_frame_cnt = vobj[ 1 ].col_temp_frames_total;		//set temp 'hit' colour
							ships_left_cpu--;
							deaths_combined++;
							printf( "cpu death by 'en_ot_grav_obj'\n" );
							sdeath = "cpu ship death by gravity obj hit";
							}
						}
					}
				}
			}
*/

		if( vobj[ i ].type == en_ot_meteor )         				//cpu's ship hit by meteor?
			{
			x = vobj[ i ].x;
			y = vobj[ i ].y;
			
			if( ( x >= x1 ) && ( x <= x2 ) )
				{
				if( ( y >= y1 ) && ( y <= y2 ) )
					{                
//					if( !disable_deaths )
						{
						vobj[ i ].col_temp_frame_cnt = vobj[ i ].col_temp_frames_total;		//set temp 'hit' colour

						vobj[ 1 ].is_exploding = 1;
						vobj[ 1 ].col_temp_frame_cnt = vobj[ 1 ].col_temp_frames_total;		//set temp 'hit' colour

						ships_left_cpu--;
						deaths_combined++;
						
						vobj[ i ].is_exploding = 1;
						printf( "cpu destroyed by 'en_ot_meteor'\n" );
						sdeath = "cpu ship destroyed by meteor hit";
						}
					}
				}
			}

//		if( vobj[ i ].type == en_ot_missile_cpu )          		//user's ship hit by missle?
		if( ( vobj[ i ].type == en_ot_missile_cpu ) || ( vobj[ i ].type == en_ot_missile_user ) )         //user's  ship hit by missle?
			{
			if( !vobj[ i ].is_exploding )
				{
				if( vobj[ i ].arming_delay_frame_cnt == 0 ) 			 //armed?
					{
					x = vobj[ i ].x;
					y = vobj[ i ].y;
					
					if( ( x >= xx1 ) && ( x <= xx2 ) )
						{
						if( ( y >= yy1 ) && ( y <= yy2 ) )
							{
//							if( !disable_deaths )
								{
								vobj[ i ].is_exploding = 1;
								
								vobj[ 0 ].col_temp_frame_cnt = vobj[ 0 ].col_temp_frames_total;		//set temp 'hit' colour
								
								vobj[ 0 ].is_exploding = 1;
								ships_left_user--;
								deaths_combined++;
								printf( "user destroyed by missile\n" );
								sdeath = "user ship destroyed by missile hit";
								}
							}
						}
					}
				}
			}


		if( ( vobj[ i ].type == en_ot_missile_cpu ) || ( vobj[ i ].type == en_ot_missile_user ) )         //cpu's ship hit by missle?
			{
			if( !vobj[ i ].is_exploding )
				{
				if( vobj[ i ].arming_delay_frame_cnt == 0 ) 			 //armed?
					{
					x = vobj[ i ].x;
					y = vobj[ i ].y;
					
					if( ( x >= x1 ) && ( x <= x2 ) )
						{
						if( ( y >= y1 ) && ( y <= y2 ) )
							{					                
//							if( !disable_deaths )
								{
								vobj[ 1 ].is_exploding = 1;
								vobj[ 1 ].col_temp_frame_cnt = vobj[ 1 ].col_temp_frames_total;		//set temp 'hit' colour
								ships_left_cpu--;
								deaths_combined++;
								
								vobj[ i ].is_exploding = 1;
								printf( "cpu destroyed by missile\n" );
								sdeath = "cpu ship destroyed by missile hit";
								}
							}
						}
					}
				}
			}


		if( vobj[ i ].type == en_ot_magmine_fragment )          		//user's ship hit by magmine fragment?
			{
			x = vobj[ i ].x;
			y = vobj[ i ].y;
			
			if( ( x >= x1 ) && ( x <= x2 ) )
				{
				if( ( y >= y1 ) && ( y <= y2 ) )
					{
					vobj[ i ].mark_to_delete = 1;           //remove magmine fragment to show ship took the blow
					add_hits( 1, 10 );

					vobj[ 1 ].col_temp_frame_cnt = vobj[ 1 ].col_temp_frames_total;		//set temp 'hit' colour
					if( vobj[ 1 ].hits >= max_ship_hits )   //check if user ship exceeds its max hits
						{
//						if( !disable_deaths )
							{
							vobj[ 1 ].is_exploding = 1;
							ships_left_cpu--;
							deaths_combined++;
							printf( "cpu destroyed by 'en_ot_magmine_fragment'\n" );
							sdeath = "cpu ship destroyed by magnetic mine fragment hit";
							}
						}
					}
				}
			}

		}
	}

   
if( !disable_deaths )
	{
	//check if user ship has any collisions
	for( int i = 0; i < vobj.size(); i++ )
		{
		double x, y;

		if( vobj[ i ].is_exploding ) continue;
	   
		if( vobj[ i ].type == en_ot_bul_cpu )          		//cpu bullet?
			{
			x = vobj[ i ].x;
			y = vobj[ i ].y;
			
			if( ( x >= xx1 ) && ( x <= xx2 ) )
				{
				if( ( y >= yy1 ) && ( y <= yy2 ) )
					{
					vobj[ i ].mark_to_delete = 1;           //remove bullet to show ship took the blow
					add_hits( 0, 10 );

					vobj[ 0 ].col_temp_frame_cnt = vobj[ 0 ].col_temp_frames_total;		//set temp 'hit' colour

					if( vobj[ 0 ].hits >= max_ship_hits )   //check if user ship exceeds its max hits
						{
//						if( !disable_deaths )
							{
							vobj[ 0 ].is_exploding = 1;
							ships_left_user--;
							deaths_combined++;
							printf( "user destroyed by 'en_ot_bul_cpu'\n" );
							sdeath = "user ship destroyed by bullet hit";
							}
						}
					}
				}
			}

		if( vobj[ i ].type == en_ot_medic )             		//medic collision?
			{
			x = vobj[ i ].x;
			y = vobj[ i ].y;

			if( ( x >= xx1 ) && ( x <= xx2 ) )
				{
				if( ( y >= yy1 ) && ( y <= yy2 ) )
					{
					vobj[ i ].mark_to_delete = 1;       		//remove medic and clear hits
					vobj[ 0 ].hits = 0;
					}
				}        
			}



/*
		if( have_grav_collisions )
			{
			if( vobj[ i ].type == en_ot_grav_obj )          	//user's ship hit grav obj?
				{
				x = vobj[ i ].x;
				y = vobj[ i ].y;
				
				if( ( x >= xx1 ) && ( x <= xx2 ) )
					{
					if( ( y >= yy1 ) && ( y <= yy2 ) )
						{                
//						if( !disable_deaths )
							{
							vobj[ 0 ].is_exploding = 1;
							vobj[ 0 ].col_temp_frame_cnt = vobj[ 0 ].col_temp_frames_total;		//set temp 'hit' colour
							ships_left_user--;
							deaths_combined++;
							printf( "user death by 'en_ot_grav_obj'\n" );
							sdeath = "user ship death by gravity obj hit";
							}
						}
					}
				}
			}
*/

		if( vobj[ i ].type == en_ot_meteor )          			//user's ship hit by meteor?
			{
			x = vobj[ i ].x;
			y = vobj[ i ].y;
			
			if( ( x >= xx1 ) && ( x <= xx2 ) )
				{
				if( ( y >= yy1 ) && ( y <= yy2 ) )
					{                
//					if( !disable_deaths )
						{
						vobj[ i ].col_temp_frame_cnt = vobj[ i ].col_temp_frames_total;		//set temp 'hit' colour

						vobj[ 0 ].is_exploding = 1;
						vobj[ 0 ].col_temp_frame_cnt = vobj[ 0 ].col_temp_frames_total;		//set temp 'hit' colour
						ships_left_user--;
						deaths_combined++;
						
						vobj[ i ].is_exploding = 1;
						printf( "user destroyed by 'en_ot_meteor'\n" );
						sdeath = "user ship destroyed by meteor hit";
						}
					}
				}
			}



		if( vobj[ i ].type == en_ot_magmine_fragment )          		//user's ship hit by magmine fragment?
			{
			x = vobj[ i ].x;
			y = vobj[ i ].y;
			
			if( ( x >= xx1 ) && ( x <= xx2 ) )
				{
				if( ( y >= yy1 ) && ( y <= yy2 ) )
					{
					vobj[ i ].mark_to_delete = 1;           //remove magmine fragment to show ship took the blow
					add_hits( 0, 10 );

					vobj[ 0 ].col_temp_frame_cnt = vobj[ 0 ].col_temp_frames_total;		//set temp 'hit' colour

					if( vobj[ 0 ].hits >= max_ship_hits )   //check if user ship exceeds its max hits
						{
//						if( !disable_deaths )
							{
							vobj[ 0 ].is_exploding = 1;
							ships_left_user--;
							deaths_combined++;
							printf( "user destroyed by 'en_ot_magmine_fragment'\n" );
							sdeath = "user ship destroyed by magnetic mine fragment hit";
							}
						}
					}
				}
			}

		}
	}



//check if each meteor has any collisions
for( int j = 0; j < vobj.size(); j++ )
    {
    double x, y;
	
//if( once_off_collision_dbg ) continue;				//remove this its just for debugging


	if( vobj[ j ].type != en_ot_meteor ) continue;
	if( vobj[ j ].is_exploding ) continue;
	
	//make meteor rect
	x1 = vobj[ j ].x + vobj[ j ].bound_x1;
	x2 = vobj[ j ].x + vobj[ j ].bound_x2;
	y1 = vobj[ j ].y + vobj[ j ].bound_y1;
	y2 = vobj[ j ].y + vobj[ j ].bound_y2;

//if( x_key ) add_new_meteors = 1;

	for( int i = 0; i < vobj.size(); i++ )
		{
		double x, y;
		
		if( vobj[ i ].is_exploding ) continue;
	   	if( i == j ) continue;			//don't compare obj with itself
	   	
	   	
		x = vobj[ i ].x;
		y = vobj[ i ].y;
		
		if( ( x >= x1 ) && ( x <= x2 ) )
			{
			if( ( y >= y1 ) && ( y <= y2 ) )
				{
				bool add_new_meteors = 0;

				if( ( vobj[ i ].type == en_ot_bul_cpu ) || ( vobj[ i ].type == en_ot_bul_user ) || ( vobj[ i ].type == en_ot_magmine_fragment ) )   //bullet hit?
					{

					vobj[ i ].mark_to_delete = 1;           	//remove obj to show meteor took the blow
					add_hits( j, 1 );

//					vobj[ j ].hits++;
					vobj[ j ].col_temp_frame_cnt = vobj[ j ].col_temp_frames_total;		//set temp 'hit' colour

					if( vobj[ j ].hits >= max_meteor_hits )   	//check if meteor exceeds its max hits
						{
						vobj[ j ].is_exploding = 1; 			//explode meteor
						add_new_meteors = 1;
						}
					}

				if( ( vobj[ i ].type == en_ot_missile_user ) || ( vobj[ i ].type == en_ot_missile_cpu ) )         //missile hit?
					{
					vobj[ i ].is_exploding = 1; 				//explode missile

					vobj[ j ].col_temp_frame_cnt = vobj[ j ].col_temp_frames_total;		//set temp 'hit' colour
					vobj[ j ].is_exploding = 1; 				//explode meteor
					add_new_meteors = 1;
					}




				if( add_new_meteors )
					{
					if( vobj[ j ].scale > 0.2 )
						{
						bool affected_by_grav = 0;
						bool apply_dxdy = 1;
						
						if( rnd() > 0.0 )
							{
							affected_by_grav = 1;
//							apply_dxdy = 1;
							}
						
						double xx1, yy1, dx1, dy1;
						double xx2, yy2, dx2, dy2;
						
						int rn = rand_range(0, 3);
						if( rn == 0 )
							{
							xx1 = vobj[ j ].x - 10;
							yy1 = vobj[ j ].y + 10;
							dx1 = vobj[ j ].dx + -3 + rnd() * 1.0;
							dy1 = vobj[ j ].dy + 0.7 + rnd() * 0.3;

							xx2 = vobj[ j ].x + 10;
							yy2 = vobj[ j ].y - 10;
							dx2 = vobj[ j ].dx + 3 + rnd() * 1.0;
							dy2 = vobj[ j ].dy + -0.7 + rnd() * 0.3;
							}

						if( rn == 1 )
							{
							xx1 = vobj[ j ].x + 10;
							yy1 = vobj[ j ].y + 10;
							dx1 = 3 + rnd() * 1.0;
							dy1 = 0.7 + rnd() * 0.3;

							xx2 = vobj[ j ].x - 10;
							yy2 = vobj[ j ].y - 10;
							dx2 = vobj[ j ].dx + -3 + rnd() * 1.0;
							dy2 = vobj[ j ].dy + -0.7 + rnd() * 0.6;
							}

						if( rn == 2 )
							{
							xx1 = vobj[ j ].x + 10;
							yy1 = vobj[ j ].y + 10;
							dx1 = vobj[ j ].dx + 3 + rnd() * 1.0;
							dy1 = vobj[ j ].dy + 0.7 + rnd() * 0.3;

							xx2 = vobj[ j ].x + 10;
							yy2 = vobj[ j ].y - 10;
							dx2 = vobj[ j ].dx + 3 + rnd() * 1.0;
							dy2 = vobj[ j ].dy + -1.4 + rnd() * 0.8;
							}

						if( rn == 3 )
							{
							xx1 = vobj[ j ].x - 10;
							yy1 = vobj[ j ].y + 10;
							dx1 = vobj[ j ].dx + -3 + rnd() * 1.0;
							dy1 = vobj[ j ].dy + 0.7 + rnd() * 0.3;

							xx2 = vobj[ j ].x - 10;
							yy2 = vobj[ j ].y - 10;
							dx2 = vobj[ j ].dx + -0.9 + rnd() * 0.5;
							dy2 = vobj[ j ].dy + -3 + rnd() * 1.0;
							}
						
						add_meteor( rand_range(0, 2),    affected_by_grav,  apply_dxdy, vobj[ j ].scale * 0.7,    xx1,   yy1,   dx1,  dy1 );
						add_meteor( rand_range(0, 2),    affected_by_grav,  apply_dxdy, vobj[ j ].scale * 0.6,    xx2,   yy2,   dx2,  dy2 );
						}
					}

				
				if( vobj[ i ].type == en_ot_meteor  )         	//another meteor hit?
					{
					bool sign;
					
					double dx = vobj[ j ].dx + vobj[ i ].dx;
					double dy = vobj[ j ].dy + vobj[ i ].dy;
					
					double angle, dist;
					angle=find_angle_between_objs( j, i, dist );
					
//					printf("angle: %.3f, dist: %.3f\n", angle, dist );
					
//once_off_collision_dbg = 1;
					
					//cater for simple collision energy transference and deflection
					//collisions in 2 dimensions see -- http://farside.ph.utexas.edu/teaching/301/lectures/node77.html
					//here j is obj1, and i is obj2
					
					double vi1 = -vobj[ i ].mass / ( vobj[ j ].mass + vobj[ i ].mass ) * ( vobj[ i ].dx - vobj[ j ].dx );			//  vi1=-m2/(m1+m2)*(v2-v1)
					double vi2 = vobj[ j ].mass / ( vobj[ j ].mass + vobj[ i ].mass ) * ( vobj[ i ].dx - vobj[ j ].dx );			//  vi2=m1/(m1+m2)*(v2-v1)
					
					double vf1 = ( ( vobj[ j ].mass - vobj[ i ].mass ) / ( vobj[ j ].mass + vobj[ i ].mass ) ) * vi1 + ( 2.0 * vobj[ i ].mass / ( vobj[ j ].mass + vobj[ i ].mass ) ) * vi2;	//vf1=(m1-m2)/(m1+m2)*vi1+(2*m2)/(m1+m2)*vi2
					double vf2 = (  2.0 * vobj[ j ].mass / ( vobj[ j ].mass + vobj[ i ].mass ) ) * vi1 -  ( ( vobj[ j ].mass - vobj[ i ].mass ) / ( vobj[ j ].mass + vobj[ i ].mass ) ) * vi2;	//vf2=(2*m1)/(m1+m2)*vi1+(m1-m2)/(m1+m2)*vi2

					vobj[ j ].dx = vf1 + 0.4 * rnd();			//rnd helps break apart meteors that repeatedly bound off each other and become stuck together, due to simple bounce coding
					vobj[ i ].dx = vf2 + 0.2 * rnd();

					vi1 = -vobj[ i ].mass / ( vobj[ j ].mass + vobj[ i ].mass ) * ( vobj[ i ].dy - vobj[ j ].dy );			//  vi1=-m2/(m1+m2)*(v2-v1)
					vi2 = vobj[ j ].mass / ( vobj[ j ].mass + vobj[ i ].mass ) * ( vobj[ i ].dy - vobj[ j ].dy );			//  vi2=m1/(m1+m2)*(v2-v1)
					
					vf1 = ( ( vobj[ j ].mass - vobj[ i ].mass ) / ( vobj[ j ].mass + vobj[ i ].mass ) ) * vi1 + ( 2.0 * vobj[ i ].mass / ( vobj[ j ].mass + vobj[ i ].mass ) ) * vi2;		//vf1=(m1-m2)/(m1+m2)*vi1+(2*m2)/(m1+m2)*vi2
					vf2 = (  2.0 * vobj[ j ].mass / ( vobj[ j ].mass + vobj[ i ].mass ) ) * vi1 -  ( ( vobj[ j ].mass - vobj[ i ].mass ) / ( vobj[ j ].mass + vobj[ i ].mass ) ) * vi2;		//vf2=(2*m1)/(m1+m2)*vi1+(m1-m2)/(m1+m2)*vi2

//printf("collision %f %f %f %f\n", vobj[ j ].dx, vobj[ j ].dy, vobj[ i ].dx, vobj[ i ].dy);
					vobj[ j ].dy = vf1 + 0.4 * rnd();			//rnd helps break apart meteors that repeatedly bound off each other and get stuck together, due to simple bounce coding
					vobj[ i ].dy = vf2 + 0.1 * rnd();
					
//					vobj[ j ].x += vobj[ j ].dx * 10;
//					vobj[ j ].y += vobj[ j ].dy * 10;
					
//					vobj[ i ].x += vobj[ i ].dx * 10;
//					vobj[ i ].y += vobj[ i ].dy * 10;
					}

				}
			}


		}
	}
}














bool mywnd::add_hits( int ii, int amount )
{


if( ii >= vobj.size() ) return 0;

if( have_a_ship_explosion ) return 0;

vobj[ ii ].hits += amount;

if( vobj[ ii ].hits < 0 ) vobj[ ii ].hits = 0;
	
return 1;
}



void mywnd::do_any_explosions()
{

//check objs
for( int i = 0; i < vobj.size(); i++ )
    {
    if( vobj[ i ].is_exploding )
        {
//        if( ( vobj[ i ].type != en_ot_missile_cpu ) && ( vobj[ i ].type != en_ot_meteor  ) ) have_a_ship_explosion = 1;
        if( ( vobj[ i ].type == en_ot_user_ship ) || ( vobj[ i ].type == en_ot_cpu_ship  ) ) 
			{
			have_a_ship_explosion = 1;
			}
        
        vector<st_line_tag> vo;
        vo = vobj[ i ].vline;
    
        double dir = 1;                             //this set which way the lines explode away spherically
        if( vobj[ i ].explode_shape ) dir = -1;

        for( int j = 0; j < vo.size(); j++ )
            {
            double x1 = vo[ j ].x1;
            double y1 = vo[ j ].y1;
            double x2 = vo[ j ].x2;
            double y2 = vo[ j ].y2;

            //this rotates each line around various point and at various angle change rates
            
            rotate( x1 + ( x1 - x2 ) / 2.0, y1 + ( y1 - y2 ) / 2.0, x1, y1, x2, y2,  dir * M_PI / ( 50.0 * ( j + 5 ) ) );
            
            if( vobj[ i ].type == en_ot_meteor )
				{
				vobj[ i ].delta_spin = 0;
	
//				bool heads_tails = rand_range( 0, 1 );     //just uses set or reset

//				if( !(j % 2) )
//				if( heads_tails )
					{
					x1 += vo[ j ].dx;
					y1 += vo[ j ].dy;
					x2 += vo[ j ].dx;
					y2 += vo[ j ].dy;
					}
				}

            vobj[ i ].vline[ j ].x1 = x1;
            vobj[ i ].vline[ j ].y1 = y1;
            vobj[ i ].vline[ j ].x2 = x2;
            vobj[ i ].vline[ j ].y2 = y2;
            }

        vobj[ i ].explode_frame_cnt--;
        
        if( vobj[ i ].explode_frame_cnt <= 0 )
            {
            if( ( vobj[ i ].type == en_ot_missile_user ) || ( vobj[ i ].type == en_ot_missile_cpu ) || ( vobj[ i ].type == en_ot_meteor  ) ) vobj[ i ].mark_to_delete = 1;

			if( have_a_ship_explosion )
				{
				if( !paused) keyhit_disable_cnt = cn_keyhit_disable_time / tframe;		//allow sufficient time for text messages to be read before keybrd hit starts next play

				if( ships_left_user == 0 ) { game_ended = 1; paused = 1; }
				if( ships_left_cpu == 0 ) { game_ended = 1; paused = 1; }
				
				if( ( !game_ended ) )
					{
					pause_ship_explosion = 1;  
					paused = 1;
	//                init( 0 );
					}
				}
            }
        }
    }

}














/*
void mywnd::do_any_explosions()
{

//check objs
for( int i = 0; i < vobj.size(); i++ )
    {
    if( vobj[ i ].is_exploding )
        {
//        if( ( vobj[ i ].type != en_ot_missile_cpu ) && ( vobj[ i ].type != en_ot_meteor  ) ) have_a_ship_explosion = 1;
        if( ( vobj[ i ].type == en_ot_user_ship ) || ( vobj[ i ].type == en_ot_cpu_ship  ) ) have_a_ship_explosion = 1;
        
        vector<st_line_tag> vo;
        vo = vobj[ i ].vline;
    
        double dir = 1;                             //this set which way the lines explode away spherically
        if( vobj[ i ].explode_shape ) dir = -1;

        for( int j = 0; j < vo.size(); j++ )
            {
            double x1 = vo[ j ].x1;
            double y1 = vo[ j ].y1;
            double x2 = vo[ j ].x2;
            double y2 = vo[ j ].y2;

            //this rotates each line around various point and at various angle change rates
            
            rotate( x1 + ( x1 - x2 ) / 2.0, y1 + ( y1 - y2 ) / 2.0, x1, y1, x2, y2,  dir * M_PI / ( 50.0 * ( j + 5 ) ) );
            
            if( vobj[ i ].type == en_ot_meteor )
				{
				vobj[ i ].delta_spin = 0;
	
//				bool heads_tails = rand_range( 0, 1 );     //just uses set or reset

//				if( !(j % 2) )
//				if( heads_tails )
					{
					x1 += vo[ j ].dx;
					y1 += vo[ j ].dy;
					x2 += vo[ j ].dx;
					y2 += vo[ j ].dy;
					}
				}

            vobj[ i ].vline[ j ].x1 = x1;
            vobj[ i ].vline[ j ].y1 = y1;
            vobj[ i ].vline[ j ].x2 = x2;
            vobj[ i ].vline[ j ].y2 = y2;
            }

        vobj[ i ].explode_frame_cnt--;
        
        if( vobj[ i ].explode_frame_cnt <= 0 )
            {
            if( ( vobj[ i ].type == en_ot_missile_user ) || ( vobj[ i ].type == en_ot_missile_cpu ) || ( vobj[ i ].type == en_ot_meteor  ) ) vobj[ i ].mark_to_delete = 1;

            if( ships_left_user == 0 ) { game_ended = 1; paused = 1; }
            if( ships_left_cpu == 0 ) { game_ended = 1; paused = 1; }
            
            if( ( !game_ended ) && have_a_ship_explosion )
                {
				pause_ship_explosion = 1;
				paused = 1;
//                init( 0 );
                }
            }
        }
    }

}
*/










bool mywnd::is_neg( double d )
{
if( d < 0.0 ) return 1;
return 0;
}









//adjusts an obj's theta to approach 'theta_req', uses obj's 'delta_theta_per_frame' to provide a smoothly change, 
//needs to be called multiple time to reach 'theta_req',
//handles +M_PI to -M_PI transition correctly, without having to going around almost 2PI
void mywnd::track_theta_smoothly( st_obj_tag &o, double theta_req )
{
if( o.is_exploding ) return;

if( ( is_neg( o.theta ) != is_neg( o.theta_req ) && ( fabs( o.theta > ( M_PI / 2.0 ) ) ) ) )
	{
	o.theta += o.delta_theta_per_frame;
	}
else{
	if( o.theta < theta_req ) o.theta += o.delta_theta_per_frame;
	if( o.theta > theta_req ) o.theta -= o.delta_theta_per_frame;
	}

if( o.theta > M_PI ) o.theta = -M_PI + ( o.theta - M_PI ); 
if( o.theta < -M_PI ) o.theta = M_PI + ( o.theta + M_PI ); 


}











//find angle and distance between 'spec' obj
double mywnd::find_angle_between_objs( int i1, int i2, double &dist )
{
if( ( i1 >= vobj.size() ) || ( i2 >= vobj.size() ) ) return 0;

if( ( i1 < 0 ) || ( i2 < 0 ) ) return 0;

double dx, dy;
dx = vobj[ i1 ].x - vobj[ i2 ].x;
dy = vobj[ i1 ].y - vobj[ i2 ].y;


dist = sqrt( pow( vobj[ i1 ].x - vobj[ i2 ].x, 2 ) + pow( vobj[ i1 ].y - vobj[ i2 ].y, 2 ) );

return atan2( dy , dx );

}









//find angle of supplied vector
double mywnd::vector_angle( double dx, double dy )
{
return atan2( dy , dx );

}






void mywnd::get_magmine_capture_count( int &user_cnt, vector<int> &vuser, int &cpu_cnt, vector<int> &vcpu )
{

user_cnt = cpu_cnt = 0;

vuser.clear();
vcpu.clear();


for( int i = 0; i < vobj.size(); i++ )
	{
	if( vobj[ i ].type == en_ot_magmine )
		{
		float x0 = vobj[ i ].x;
		float y0 = vobj[ i ].y;
		
		float x1 = vobj[ 0 ].x;											//user
		float y1 = vobj[ 0 ].y;

		float x2 = vobj[ 1 ].x;											//cpu
		float y2 = vobj[ 1 ].y;

		vobj[ i ].possible_owner = -1;

		if( fabs(x0 - x1) < 10 ) 
			{
			if( fabs(y0 - y1) < 10 ) 
				{
				vuser.push_back( i );
				vobj[ i ].possible_owner = 0;							//flag possession
				user_cnt++;
				}
			}

		if( fabs(x0 - x2) < 10 ) 
			{
			if( fabs(y0 - y2) < 10 ) 
				{
				vcpu.push_back( i );
				vobj[ i ].possible_owner = 1;							//flag possesion
				cpu_cnt++;
				}
			}
		}
	}

}










void mywnd::fire_magmine( int ship_idx, int magmine_idx )
{

//change magmine to move like a bullet
int idx = magmine_idx;


vobj[ idx ].magmine_in_flight_frame_cnt = 1.3 / tframe;		//set a time for magmide to be in flight before it fragments

float dx = vobj[ ship_idx ].dx + 500.0 * cos( vobj[ ship_idx ].theta ) * measured_tframe;
float dy = vobj[ ship_idx ].dy + 500.0 * sin( vobj[ ship_idx ].theta ) * measured_tframe;

vobj[ idx ].dx = dx;
vobj[ idx ].dy = dy;
vobj[ idx ].dv = sqrt( pow( dx, 2 ) + pow( dy, 2 ) );

vobj[ idx ].drag = 0.008;

vobj[ idx ].col_temp_frames_total = vobj[ idx ].magmine_in_flight_frame_cnt;
vobj[ idx ].col_temp_frame_cnt = vobj[ idx ].col_temp_frames_total;
}





//do calculations for next update here, called by cb_timer1() for each frame
//calcs actual time per frame in 'measured_tframe'
void mywnd::tick( double &measured_tframe )
{
int rr;
double local_tframe;

local_tframe = tframe * 1;

frame_counter++;
measured_tframe_avg_cnt++;

//calc frame rate
measured_tframe = m1_timer.time_passed( m1_timer.ns_tim_start );
m1_timer.time_start( m1_timer.ns_tim_start );

if( keyhit_disable_cnt > 0 ) keyhit_disable_cnt--;

if( game_to_start )
	{
	if( attract_mode_cnt > 0 ) attract_mode_cnt--;

	if( attract_mode_cnt == 0 )
		{
		if( !attract_mode_active )
			{
			attract_mode_active = 1;
			option_key1 = 0;
			option_key2 = 0;
			option_key3 = 0;
			option_key4 = 0;
			option_key5 = 0;

			init( 1 );
			game_to_start = 0;
			game_ended = 0;
			paused = 0;
			}
		}
	}
else{
	attract_mode_cnt = cn_attract_mode_delaystart / tframe;
//	attract_mode_active = 0;
	}


if( ( paused ) && ( attract_mode_active ) )
	{
	init( 1 );
	paused = 0;
	pause_ship_explosion = 0;
	
	}


//if( keyhit_disable_cnt > 0 ) printf( "keyhit_disable_cnt: %d\n", keyhit_disable_cnt );

int magmine_cpu_cnt, magmine_user_cnt;
vector<int> vmagmine_cpu;
vector<int> vmagmine_user;

get_magmine_capture_count( magmine_user_cnt, vmagmine_user, magmine_cpu_cnt, vmagmine_cpu );

//printf( "magmine_user_cnt: %d %d %d %d\n", magmine_user_cnt, vmagmine_user.size(), magmine_cpu_cnt, vmagmine_cpu.size() );


//provide a slow repair of ships
if( !(frame_counter % 50) )
	{
	add_hits( 0, -1 );
//	vobj[ 0 ].hits--;
//	if( vobj[ 0 ].hits < 0 ) vobj[ 0 ].hits = 0;

	add_hits( 1, -1 );
//	vobj[ 1 ].hits--;
//	if( vobj[ 1 ].hits < 0 ) vobj[ 1 ].hits = 0;
	}



//show frame rate
if( !( frame_counter % 20 ) )
    {
    
//    int idx = missile_in_flight( 0 );
//    printf( "idx: %d\n", idx );
    
//    printf( "cum: %.5f, cnt: %u, avg: %.5f (secs), time_factor: %f\n", measured_tframe_cumm, measured_tframe_avg_cnt, measured_tframe_avg, frame_factor );
//    printf( "measured_tframe: %.5f (secs), %0.2f (fps)\n", measured_tframe, 1.0 / measured_tframe );

    }


if ( paused  ) 
	{
	redraw();	
	return;
	}
	
if( vobj.size() == 0 ) return;

vector<int> vgrav;
get_grav_objs( vgrav );


delete_marked_objs();


do_any_explosions();                   //animate any explosions


exploding_ship_user = 0;
exploding_ship_cpu = 0;
if( vobj[ 0 ].is_exploding )  exploding_ship_user = 1;
if( vobj[ 1 ].is_exploding )  exploding_ship_cpu = 1;


if( frame_counter != frame_counter )
    {
    vobj[ 1 ].x = -100;
    vobj[ 1 ].y = -100;
    vobj[ 1 ].dx = 2;
    vobj[ 1 ].dy = 0;
    }

bool cpu_being_hit = 0;
int cpu_hits_now = vobj[ 1 ].hits;

if( !have_a_ship_explosion )
    {
    check_obj_collisions();
    }


vector<int> vmeteor_idx;
int meteors = get_meteor_objs( vmeteor_idx );
if( have_meteor_objs )
	{
	if ( meteors < 5 )
		{
		bool affected_by_grav = 0;
		bool apply_dxdy = 1;
		
		if( rnd() > 0.0 )
			{
			affected_by_grav = 1;
			}

		double scale =  0.6 + rnd() * 0.4;
		if( scale < 0.3 ) scale = 0.3;
		
		add_meteor( rand_range(0, 2),    affected_by_grav,  apply_dxdy,  scale,    -midx / 1.1 + 20 * rnd(),    midy / 1.1 + 20 * rnd(),   2.0 + 0.8 * rnd(),  -2.0 - 0.8 * rnd()  );
		}
	}




//apply cpu's ship steering
if( ( !have_a_ship_explosion ) && ( missle_in_flight_user == -1 ) )
    {
	ship_to_ship_distance = sqrt( pow( vobj[ 0 ].x - vobj[ 1 ].x, 2 ) + pow( vobj[ 0 ].y - vobj[ 1 ].y, 2 ) );

	//make a point ahead of user's ship movement, rem. this may be behind ship if going backwards
	double d1, d2;

	if( ship_to_ship_distance > 200.0 )         //ships far apart?
		{
		d1 = vobj[ 0 ].x + vobj[ 0 ].dx;        //make a point anticipating motion of ship only
		d2 = vobj[ 0 ].y + vobj[ 0 ].dy;
		}
	else{
		d1 = vobj[ 0 ].x + ( vobj[ 0 ].dx * ship_to_ship_distance / 2.0 );        //make a point anticipating motion of ship plus distance between ships
		d2 = vobj[ 0 ].y + ( vobj[ 0 ].dy * ship_to_ship_distance / 2.0 );
	//    d1 = vobj[ 0 ].x + vobj[ 0 ].dx;        //make a point anticipating motion of ship only
	//    d2 = vobj[ 0 ].y + vobj[ 0 ].dy;
		}

	vobj[ 0 ].dbgx = d1;
	vobj[ 0 ].dbgy = d2;


	//angle cpu ship to above calc'd point
	double dx, dy;
	dx = d1 - vobj[ 1 ].x;
	dy = d2 - vobj[ 1 ].y;

    vobj[ 1 ].theta_req = atan2( dy , dx );     //make angle to above calc'd point
    }

//if missile in flight, just slightly turn ship, high thrust will be applied further below to help evade
if( ( missle_in_flight_user >= 0 ) || ( vobj[ 1 ].hits >= evade_hits ) )
	{
	if ( evade_angle ) vobj[ 1 ].theta_req = vobj[ 1 ].theta += 0.005;
	else vobj[ 1 ].theta_req = vobj[ 1 ].theta -= 0.005;
	}



//printf("cpu theta: %f, theta_req: %f\n", vobj[ 1 ].theta, vobj[ 1 ].theta_req ); 


track_theta_smoothly( vobj[ 1 ], vobj[ 1 ].theta_req );



//time for cpu ship's to fire weapons?
if( !have_a_ship_explosion )
	{
	bool cpu_to_fire_bullet = 0;
	bool cpu_to_fire_magmine = 0;
	if( vobj[ 1 ].time_bullet_remaining <= 0.0 )						//time to fire?
		{
        if( ship_to_ship_distance < 400 ) cpu_to_fire_bullet = 1;     	//only shoot when close
		if( ( ship_to_ship_distance >= 400 ) && ( ship_to_ship_distance < 900 ) ) { cpu_to_fire_magmine = 1; }

		//---- cpu to fire bullets if near a meteor ----
		for( int j = 0; j < vmeteor_idx.size(); j++ )
			{
			double dist;
			double theta_req = find_angle_between_objs( 1,  vmeteor_idx[j], dist );	//find distance between cpu and all meteors
			if( dist < 250 ) 
				{
				cpu_to_fire_bullet = 1;
				break;
				}
			}
		//----------------------------------------------
		}

    if( (cpu_to_fire_bullet) || (cpu_to_fire_magmine) )
        {
        //randomize/dither cpu ship's angle around its set angle to disperse bullet patterns
        double dr = ( rand() % 50 ) - 25.0;
        vobj[ 1 ].theta += dr / 100.0;
        
		if( cpu_to_fire_bullet ) add_bullet( 1 ); 
 
			if( cpu_to_fire_magmine )
				{
				if( vmagmine_cpu.size() > 0 )
					{
					int idx = vmagmine_cpu[0];
					fire_magmine( 1, idx );
					}
				}
 
        vobj[ 1 ].theta -= dr / 100.0;
        }
    }

//update cpu ship's bullet frame count
if( vobj[ 1 ].time_bullet_remaining <= 0.0 )
    {
    vobj[ 1 ].time_bullet_remaining = vobj[ 1 ].time_per_bullet;
    }
else{
    vobj[ 1 ].time_bullet_remaining -= measured_tframe;
    }



//if( vobj[ 1 ].bullet_frame_cnt <= 0 )
//    {
//    vobj[ 1 ].bullet_frame_cnt = vobj[ 1 ].frames_per_bullet;
//    }
//else{
//    vobj[ 1 ].bullet_frame_cnt--;
//    }



 


//time for user ship's bullet fire?
if( ctrl_key )
    {
//    if( vobj[ 0 ].bullet_frame_cnt == 0 )
    if( vobj[ 0 ].time_bullet_remaining <= 0.0 )
        {
        //randomize/dither user ship's angle around its set angle to disperse bullet patterns
        double dr = ( rand() % 30 ) - 15.0;
        vobj[ 0 ].theta += dr / 100.0;
        
        if( !have_a_ship_explosion ) add_bullet( 0 );
        
        vobj[ 0 ].theta -= dr / 100.0;
        
        
        //change magmine to move like a bullet
        if( vmagmine_user.size() > 0 )
			{
//			int idx = vmagmine_user[0];
//			fire_magmine( 0, idx );
			
/*			
			vobj[ idx ].magmine_in_flight_frame_cnt = 3 / tframe;		//set a time
			
			float dx = vobj[ 0 ].dx + 500.0 * cos( vobj[ 0 ].theta ) * measured_tframe;
			float dy = vobj[ 0 ].dy + 500.0 * sin( vobj[ 0 ].theta ) * measured_tframe;
			
			vobj[ idx ].dx = dx;
			vobj[ idx ].dy = dy;
			vobj[ idx ].dv = sqrt( pow( dx, 2 ) + pow( dy, 2 ) );

			vobj[ idx ].drag = 0.008;
			
			vobj[ idx ].col_temp_frames_total = vobj[ idx ].magmine_in_flight_frame_cnt;
			vobj[ idx ].col_temp_frame_cnt = vobj[ idx ].col_temp_frames_total;

//			exit(0);
//			getchar();
*/
			}
        }
    }




if( ctrl_key )
    {
    if( vobj[ 0 ].time_magmine_fire_remaining <= 0.0 )
        {

         //change magmine to move like a bullet
       if( vmagmine_user.size() > 0 )
			{
			int idx = vmagmine_user[0];
			fire_magmine( 0, idx );
			vobj[ 0 ].time_magmine_fire_remaining = vobj[ 0 ].time_per_magmine_fire;
			}
		}
	}




//update user ship's bullet frame count
if( vobj[ 0 ].time_bullet_remaining <= 0.0 )
    {
    vobj[ 0 ].time_bullet_remaining = vobj[ 0 ].time_per_bullet;
    }
else{
    vobj[ 0 ].time_bullet_remaining -= measured_tframe;
    }


if( vobj[ 0 ].time_magmine_fire_remaining < 0.0 )
    {
//    vobj[ 0 ].time_magmine_fire_remaining = vobj[ 0 ].time_per_magmine_fire;
    }
else{
    vobj[ 0 ].time_magmine_fire_remaining -= measured_tframe;
    }


//if( vobj[ 0 ].bullet_frame_cnt <= 0 )
//    {
//    vobj[ 0 ].bullet_frame_cnt = vobj[ 0 ].frames_per_bullet;
//    }
//else{
//    vobj[ 0 ].bullet_frame_cnt--;
//    }



//get a grav obj idx
//int grav_idx = -1;
//double gaccel = 0;
//double grav_dx = 0;
//double grav_dy = 0;
//double grav_x = 0;
//double grav_y = 0;

//for( int i = 0; i < vobj.size(); i++ )
    {
//    if( vobj[ i ].type == en_ot_grav_obj )
        {
//        grav_idx = i;
//        grav_x = vobj[ i ].x;
//        grav_x = vobj[ i ].y;
        }
    }







/*
//apply cpu ship thrust
vobj[ 1 ].thrust = vobj[ 1 ].thrust_max;
add_exaust( 1, vobj[ 1 ].thrust );


vobj[ 1 ].thrust = vobj[ 1 ].thrust_max;
*/

//do user's ship rotate
if( !have_a_ship_explosion )
    {
    if( left_key ) vobj[ 0 ].theta += ( 0.1 * frame_factor );
    if( right_key ) vobj[ 0 ].theta -= ( 0.1 * frame_factor );
    }












bool medic_exists = 0;
bool at_vel_limitx = 0;
bool at_vel_limity = 0;


vector<int> vint;

missle_in_flight_user = missile_in_flight( 0, vint );
missle_in_flight_cpu = missile_in_flight( 1, vint );

chaff_user = get_chaff_objs( 0, vchaff_user );
chaff_cpu = get_chaff_objs( 1, vchaff_cpu );


//add cpu chaff if missle in flight
if( ( missle_in_flight_user > -1 ) && ( !exploding_ship_cpu ) )
	{
	if( chaff_cpu < missle_in_flight_user )							//match chaff count to missile count
		{
		if( ( (double)vobj[ 1 ].hits / max_ship_hits ) < 0.95  )	//only if resources allow
			{
			add_chaff( 1 );
			add_hits( 1,  max_ship_hits * chaff_cost );				//pay cost

//			vobj[ 1 ].hits += max_ship_hits * chaff_cost;			//pay cost
			}
		}
	}



//add cpu missile if user has a missle in flight
if( ( missle_in_flight_user > -1 ) && ( !exploding_ship_cpu ) && (!attract_mode_active) )
	{
	if( missle_in_flight_cpu == -1 )
		{
		if( ( (double)vobj[ 1 ].hits / max_ship_hits ) < 0.75  )	//only if resources allow
			{
			add_missile( 1 );
			next_missle_frame_cnt = frames_per_missle_launch;		//reset next missle period cnt
			
			add_hits( 1,  max_ship_hits * missile_cost );			//pay cost
			
//			vobj[ 1 ].hits += max_ship_hits * missile_cost;	//pay cost
			}
		}
	}



//printf( "chaff_user: %d, chaff_cpu: %d\n", chaff_user, chaff_cpu );

//move objs
for( int i = 0; i < vobj.size(); i++ )
    {
    int obj_type = vobj[ i ].type;
    vobj[ i ].accelx = 0;
    vobj[ i ].accely = 0;

//    if( vobj[ i ].dx > vobj[ i ].max_dx ) vobj[ i ].dx = vobj[ i ].max_dx;              //limit vel
//    if( vobj[ i ].dx < -vobj[ i ].max_dx ) vobj[ i ].dx = -vobj[ i ].max_dx;

//    if( vobj[ i ].dy > vobj[ i ].max_dy ) vobj[ i ].dy = vobj[ i ].max_dy;              //limit vel
//    if( vobj[ i ].dy < -vobj[ i ].max_dy ) vobj[ i ].dy = -vobj[ i ].max_dy;


	vobj[ i ].theta += vobj[ i ].delta_spin;


    //apply user's ship thrust
    if( obj_type == en_ot_user_ship )
        {
        vobj[ i ].thrust = 0;
        if( !have_a_ship_explosion )
            {
			//-------- attract mode, basic automation of user ship ------
			bool attract_mode_thrust = 0;
			bool attract_mode_thrust_fast = 0;
			
            if( ( attract_mode_active ) )
				{
				if( ( fabs( vobj[ i ].dx ) < 1 ) && ( fabs( vobj[ i ].dy ) < 1 ) ) attract_mode_thrust = 1;				//minor thrust
				
				if( missle_in_flight_cpu >= 1 )
						{
						if( ( fabs( vobj[ i ].dx ) < 5 ) && ( fabs( vobj[ i ].dy ) < 5 ) ) attract_mode_thrust_fast = 1; //major thrust
						}
					
//				if( fabs( vobj[ i ].dy ) < 4 ) attract_mode_thrust = 1;
					
				if( rnd() > 0.8 ) vobj[ i ].theta += 0.03;
				if( rnd() < -0.8 ) vobj[ i ].theta -= 0.03;
				
				if( missle_in_flight_cpu >= 1 ) attract_mode_thrust_fast;
				else attract_mode_thrust_fast = 0;
				if( ( rnd() > 0.95 ) && ( rnd() > 0.95 ) ) if( missle_in_flight_user < 2 ) add_missile( 0 );

				if( rnd() > 0.8 ) 
					{
					//change magmine to move like a bullet
				   if( vmagmine_user.size() > 0 )
						{
						int idx = vmagmine_user[0];
						fire_magmine( 0, idx );
						vobj[ 0 ].time_magmine_fire_remaining = vobj[ 0 ].time_per_magmine_fire;
						}
					add_bullet( 0 );
					}

				}
			//----------------------------------------------------------
			
            if( ( up_key ) || ( attract_mode_thrust ) || ( attract_mode_thrust_fast ) )
                {
        //        if( vobj[ 0 ].thrust < vobj[ 0 ].thrust_max )               //not at max thrust yet?
        //            {
        //            vobj[ 0 ].thrust += vobj[ 0 ].thrust_inc_per_frame;
        //            }
        //        if( vobj[ 0 ].thrust > vobj[ 0 ].thrust_max ) vobj[ 0 ].thrust = vobj[ 0 ].thrust_max;
                
                vobj[ i ].thrust = vobj[ i ].thrust_max;
                
                add_exhaust( i, vobj[ i ].thrust / vobj[ i ].thrust_max );
                }
            
            
        //    vobj[ 0 ].accelx = vobj[ 0 ].thrust / vobj[ 0 ].mass * cos( vobj[ 0 ].theta );        //a = f/m
        //    vobj[ 0 ].accely = vobj[ 0 ].thrust / vobj[ 0 ].mass * sin( vobj[ 0 ].theta );        //a = f/m
            }

        vobj[ i ].accel = vobj[ i].thrust / vobj[ i ].mass;
        vobj[ i ].accelx = vobj[ i ].accel * cos( vobj[ i ].theta );                //a = f/m       i.e: accel = force / mass
        vobj[ i ].accely = vobj[ i ].accel * sin( vobj[ i ].theta );                //a = f/m
        }
    


    double dist_grav = 1e300;					//pick any large num
    
    //apply cpu ship's thrust
    if( ( obj_type == en_ot_cpu_ship ) )
        {
        vobj[ i ].thrust = 0;
        
        bool apply_thrust = 1;
        
        if( ( !have_a_ship_explosion )  )
            {
			for( int j = 0; j < vgrav.size(); j++ )				//check if poss stuck in grav obj
				{
				double dist;
                double theta_req = find_angle_between_objs( i, vgrav[j], dist );

                if( dist < dist_grav ) dist_grav = dist;		//find smallest distance
                }
			}

        vobj[ i ].dv = sqrt( pow( vobj[ i ].dx, 2 ) + pow( vobj[ i ].dy, 2 ) );         //calc vector magnitude, i.e. distance displacement

        double av = vector_angle( vobj[ i ].dx, vobj[ i ].dy );                         //calc angle of distance displacement
        double delta_ang = av - vobj[ i ].theta;

        double dist;
        double theta = find_angle_between_objs( i,  0, dist );                          //get distance between cpu and user ships

        double thrust = vobj[ i ].thrust_max;
        if( dist < 600 ) thrust = vobj[ i ].thrust_max * 0.5;
        if( dist < 400 ) thrust = vobj[ i ].thrust_max * 0.3;
        if( dist < 100 ) thrust = vobj[ i ].thrust_max * 0.05;
        if( dist < 50 ) thrust = vobj[ i ].thrust_max * 0.01;


        if( vobj[ i ].dv > vobj[ i ].max_dx )
            {

            if( ( delta_ang >= -( M_PI / 2.5 ) ) && ( delta_ang <= ( M_PI / 2.5 ) ) )
                {
//                thrust = thrust * fabs( sin( delta_ang ) );
                thrust = thrust * 0.001;
                }
            else{
                }
//thrust = 0;
            }


		if( dist_grav < 10.0 )													//accel away from grav obj
			{
			apply_thrust = 1;
			vobj[ i ].timed_burn = 1;
			vobj[ i ].timed_burn_thrust = 5000.0;
			}

		if( !(rand() % 100 ) )
			{
//			printf( "rand()\n" );
			apply_thrust = 1;
			vobj[ i ].timed_burn = 1.0;
			vobj[ i ].timed_burn_thrust = 2000.0;
			}

//		printf( "timed_burn: %f\n", vobj[ i ].timed_burn );



        if( vobj[ i ].timed_burn > 0.0 )
			{
			vobj[ i ].timed_burn -= measured_tframe;
			apply_thrust = 1;
			thrust = vobj[ i ].timed_burn_thrust;
			}

        if( ( missle_in_flight_user != -1 ) || ( vobj[ 1 ].hits >= evade_hits ) )
			{
			apply_thrust = 1;
			thrust = 2600.0;
			}


//            printf( "Dang: %.3f, thrust: %.3f\n", delta_ang * cn_rad2deg, thrust );
//apply_thrust = 0;


            if( apply_thrust )
                {

                vobj[ i ].thrust = thrust;
            
                add_exhaust( i, 2.0 * vobj[ i ].thrust / vobj[ i ].thrust_max );		//multiply by 2.0 make exhaust look 
                }

        vobj[ i ].accel = vobj[ i].thrust / vobj[ i ].mass;
        vobj[ i ].accelx = vobj[ i ].accel * cos( vobj[ i ].theta );                //a = f/m       i.e: accel = force / mass
        vobj[ i ].accely = vobj[ i ].accel * sin( vobj[ i ].theta );                //a = f/m
        }


    if( obj_type == en_ot_medic ) medic_exists = 1;


    //apply missle steering and thrust
    if( obj_type == en_ot_missile_cpu ) 
        {        
		double d1, d2;

		if( chaff_user > -1 )									//any user chaff about?
			{
			double dist, smallest_dist;
			int idx = 0;										//default to user ship firstly
			find_angle_between_objs( i, 0, smallest_dist );		//find the smallest distance between the missile and user ship and its chaff 

			for( int j = 0; j < vchaff_user.size(); j++ )
				{
				find_angle_between_objs( i, vchaff_user[j], dist );
				
				if( dist < smallest_dist )
					{
					smallest_dist = dist;						//find obj that is closest
					idx = vchaff_user[j];
					}
				}

//printf("idx: %d\n", idx);
			d1 = vobj[ idx ].x + ( vobj[ idx ].dx );
			d2 = vobj[ idx ].y + ( vobj[ idx ].dy );
			}
		else{
			d1 = vobj[ 0 ].x + ( vobj[ 0 ].dx );				//no chaff, just aim for user's ship
			d2 = vobj[ 0 ].y + ( vobj[ 0 ].dy );
			}
        //angle cpu ship to above calc'd point
        double dx, dy;
        dx = d1 - vobj[ i ].x;
        dy = d2 - vobj[ i ].y;

        
        double theta_req = atan2( dy , dx );		//make angle to above calc'd point
        vobj[ i ].theta_req = theta_req;

//dbg_framecntr++;

//if( vobj[ i ].x < (-midx + 10) )
//{
//printf("dbg_framecntr: %d, local_tframe: %f\n", dbg_framecntr, local_tframe );
//}
        if ( vobj[ i ].time_to_live > 1.0 )         //still not near expiration, keep steering and apply thrust
            {
            add_exhaust( i, vobj[ i ].thrust / vobj[ i ].thrust_max );

            if( vobj[ i ].missle_steer_delay == 0 ) track_theta_smoothly( vobj[ i ], theta_req );       //this delays steering, so missle tracks a strainght path straight after launch 
            vobj[ i ].missle_steer_delay--;
            if( vobj[ i ].missle_steer_delay < 0 ) vobj[ i ].missle_steer_delay = 0;

            //apply missle thrust        
            if( vobj[ i ].thrust < vobj[ i ].thrust_max )                  //not at max thrust yet?
                {
                vobj[ i ].thrust += vobj[ i ].thrust_inc_per_frame;
                }
            if( vobj[ i ].thrust > vobj[ i ].thrust_max ) vobj[ i ].thrust = vobj[ i ].thrust_max;
            }

        vobj[ i ].accel = vobj[ i].thrust / vobj[ i ].mass * 1.0;
        vobj[ i ].accelx = vobj[ i ].accel * cos( vobj[ i ].theta );                //a = f/m       i.e: accel = force / mass
        vobj[ i ].accely = vobj[ i ].accel * sin( vobj[ i ].theta );                //a = f/m


		if( vobj[ i ].arming_delay_frame_cnt > 0 ) vobj[ i ].arming_delay_frame_cnt--;					//dec arming count, if not yet armed
		
        }





    //apply missle steering and thrust
    if( obj_type == en_ot_missile_user ) 
        {
        
		double d1, d2;
		if( chaff_cpu > -1 )									//any cpu chaff about?
			{
			double dist, smallest_dist;
			int idx = 1;										//default to cpu ship firstly
			find_angle_between_objs( i, 1, smallest_dist );		//find the smallest distance between the missile and cpu ship and its chaff 

			for( int j = 0; j < vchaff_cpu.size(); j++ )
				{
				find_angle_between_objs( i, vchaff_cpu[j], dist );
				
				if( dist < smallest_dist )
					{
					smallest_dist = dist;						//find obj that is closest
					idx = vchaff_cpu[j];
					}
				}

//printf("idx: %d\n", idx);

			d1 = vobj[ idx ].x + ( vobj[ idx ].dx );
			d2 = vobj[ idx ].y + ( vobj[ idx ].dy );
			}
		else{
			d1 = vobj[ 1 ].x + ( vobj[ 1 ].dx );
			d2 = vobj[ 1 ].y + ( vobj[ 1 ].dy );
			}
        //angle cpu ship to above calc'd point
        double dx, dy;
        dx = d1 - vobj[ i ].x;
        dy = d2 - vobj[ i ].y;

        
        double theta_req = atan2( dy , dx );		//make angle to above calc'd point
        vobj[ i ].theta_req = theta_req;

        if ( vobj[ i ].time_to_live > 1.0 )         //still not near expiration, keep steering and apply thrust
            {
            add_exhaust( i, vobj[ i ].thrust / vobj[ i ].thrust_max );

            if( vobj[ i ].missle_steer_delay == 0 ) track_theta_smoothly( vobj[ i ], theta_req );       //this delays steering, so missle tracks a strainght path straight after launch 
            vobj[ i ].missle_steer_delay--;
            if( vobj[ i ].missle_steer_delay < 0 ) vobj[ i ].missle_steer_delay = 0;

            //apply missle thrust        
            if( vobj[ i ].thrust < vobj[ i ].thrust_max )                  //not at max thrust yet?
                {
                vobj[ i ].thrust += vobj[ i ].thrust_inc_per_frame;
                }
            if( vobj[ i ].thrust > vobj[ i ].thrust_max ) vobj[ i ].thrust = vobj[ i ].thrust_max;
            }

        vobj[ i ].accel = vobj[ i].thrust / vobj[ i ].mass * 1.0;
        vobj[ i ].accelx = vobj[ i ].accel * cos( vobj[ i ].theta );                //a = f/m       i.e: accel = force / mass
        vobj[ i ].accely = vobj[ i ].accel * sin( vobj[ i ].theta );                //a = f/m


		if( vobj[ i ].arming_delay_frame_cnt > 0 ) vobj[ i ].arming_delay_frame_cnt--;					//dec arming count, if not yet armed
		
        }




//apply gravity accel
	for( int kk = 0; kk < vobj.size(); kk++ )
		{
		if( vobj[ kk ].type == en_ot_grav_obj )
			{
			if( vobj[ i ].grav_affected )
				{
				double dist;
				double theta_req = find_angle_between_objs( i,  kk, dist );
				if( dist < 50 ) dist = 50;
				
	//            dist *= 5000;
				double fg = cn_G * ( vobj[ kk ].mass * vobj[ i ].mass ) / dist;    //gravitational acceleration for point masses
				
	//            if( fg >= 5.0 ) fg = 5.0;
				
				
	//            grav_dx = -fg * tframe * cos( theta_req );                    //v = u + at
	//            grav_dy = -fg * tframe * sin( theta_req );

	//            printf( "g_theta: %.03f, g_dist, %.03f, fg: %.3f\n",theta_req, dist, fg );
				vobj[ i ].accelx -= fg / vobj[ i ].mass * cos( theta_req );        //a = f/m
				vobj[ i ].accely -= fg / vobj[ i ].mass * sin( theta_req );        //a = f/m
				
//				if( vobj[ i ].accelx > 20.0 ) vobj[ i ].accelx = 20.0;
//				if( vobj[ i ].accely > 20.0 ) vobj[ i ].accely = 20.0;

//				if( vobj[ i ].accelx < -20.0 ) vobj[ i ].accelx = -20.0;
//				if( vobj[ i ].accely < -20.0 ) vobj[ i ].accely = -20.0;
				
	//            printf( "thrust: %.03f, accelx, %.03f, accely: %.03f --%0.3f\n", vobj[ 0 ].thrust, vobj[ 0 ].accelx, vobj[ 0 ].accely, sin( theta_req ) );

		//        vobj[ i ].dy += vobj[ i ].thrust * sin( vobj[ i ].theta );

				}
			}
		}




//calc magnetic mine attraction/accel
	if( ( !have_a_ship_explosion ) && ( vobj[ i ].type == en_ot_magmine  ) )
		{
		for( int kk = 0; kk < vobj.size(); kk++ )
			{
			if( vobj[ i ].magmine_in_flight_frame_cnt > 0 ) continue;

			if( vobj[ kk ].type == en_ot_user_ship )
				{
				double dist;
				double theta_req = find_angle_between_objs( i,  kk, dist );
				if( dist < 5 ) dist = 5;

				double fg = cn_G * ( vobj[ kk ].mass * vobj[ i ].mass ) / dist;    //acceleration for point masses

				vobj[ i ].drag = 0.08;
				
				float rn = 1.0 + fabs(rnd()) / 10.0f;
				fg *= 1000000000 * rn;
				vobj[ i ].accelx -= fg / ship_mass_user * cos( theta_req );        //a = f/m
				vobj[ i ].accely -= fg / ship_mass_user * sin( theta_req );        //a = f/m
//printf( "vobj[ i ].accelx %f %f %f %f\n", vobj[ i ].accelx,  vobj[ i ].mass,  vobj[ kk ].mass, fg );
//printf( "rn %f\n", rn );
				}

			if( vobj[ kk ].type == en_ot_cpu_ship )
				{
				double dist;
				double theta_req = find_angle_between_objs( i,  kk, dist );
				if( dist < 5 ) dist = 5;

				double fg = cn_G * ( vobj[ kk ].mass*7 * vobj[ i ].mass ) / dist;    //acceleration for point masses

				vobj[ i ].drag = 0.08;
				
				float rn = 1.0 + fabs(rnd()) / 10.0f;
				fg *= 1000000000 * rn;
				vobj[ i ].accelx -= fg / ship_mass_user * cos( theta_req );        //a = f/m
				vobj[ i ].accely -= fg / ship_mass_user * sin( theta_req );        //a = f/m
//printf( "vobj[ i ].accelx %f %f %f %f\n", vobj[ i ].accelx,  vobj[ i ].mass,  vobj[ kk ].mass, fg );
//printf( "rn %f\n", rn );
				}
			}


		//check if magmine is in flight after being fired, when it times out add explosion with fragments, but keep magmine avail for another capture
		if( vobj[ i ].magmine_in_flight_frame_cnt > 0 )
			{
			vobj[ i ].magmine_in_flight_frame_cnt--;
			if( vobj[ i ].magmine_in_flight_frame_cnt == 0  )
				{
				int xx1, yy1;
				double dx1, dy1, scle;
				bool affected_by_grav = 1;
				bool apply_dxdy = 1;
				xx1 = vobj[ i ].x;
				yy1 = vobj[ i ].y;
				
				scle = 1;
				
				int fragments = 40;
				float theta = 0;
				float theta_inc = twopi/fragments;
				
				float src_dx = vobj[ i ].dx;
				float src_dy = vobj[ i ].dy;



				float elongate = rnd();								//add some explosion fragment scatter variety
				
				float felongatex = 1;
				float felongatey = 1;
				if( elongate > 0.5 )
					{
					felongatex = 1.3;	
					}
					
				if( elongate < -0.5 )
					{
					felongatey = 1.3;	
					}

				if( elongate > 0.8 )
					{
					felongatex = 2;	
					}

				if( elongate < -0.8 )
					{
					felongatey = 2;	
					}



				for( int j = 0; j < fragments; j++ )
					{
					float time_to_live = 2+0.5*rnd();
					float velx = 3 + 2*rnd();
					float vely = 3 + 2*rnd();
					

					velx /= felongatex;
					vely /= felongatey;
					
					dx1 = src_dx*0.4 + velx*0.4 * cos( theta + 0.01*rnd() );
					dy1 = src_dy*0.4 + vely*0.4 * sin( theta + 0.01*rnd() );
					
					float delta_spin = 0.3*rnd();
					add_magmine_fragment( i, affected_by_grav,  apply_dxdy, scle,    xx1,   yy1,   dx1,  dy1, time_to_live, delta_spin );
					
					theta += theta_inc;
					}				
				
				}
			}
		}



    //apply obj accel
    double hold_dx = vobj[ i ].dx;
    double hold_dy = vobj[ i ].dy;
    
    vobj[ i ].dx += vobj[ i ].accelx * local_tframe;                      //v = u + at    i.e: vel = init_vel + accel * dt
    vobj[ i ].dy += vobj[ i ].accely * local_tframe;
    
    vobj[ i ].dv = sqrt( pow( vobj[ i ].dx, 2 ) + pow( vobj[ i ].dy, 2 ) );


double av = vector_angle( vobj[ i ].dx, vobj[ i ].dy );

//if( obj_type == en_ot_cpu_ship ) printf( "dx: %.3f dy: %.3f, dv: %.3f, ang: %.3f\n", vobj[ i ].dx, vobj[ i ].dy, vobj[ i ].dv, av * cn_rad2deg );


    if( vobj[ i ].dv > vobj[ i ].max_dx ) at_vel_limitx = 1;
//    if( vobj[ i ].dx > vobj[ i ].max_dx ) at_vel_limitx = 1;                             //limit vel
//    if( vobj[ i ].dx < -vobj[ i ].max_dx ) at_vel_limitx = 1;

//    if( vobj[ i ].dy > vobj[ i ].max_dy ) at_vel_limity = 1;
//    if( vobj[ i ].dy < -vobj[ i ].max_dy ) at_vel_limity = 1;

    if( at_vel_limitx | at_vel_limity )
        {
//        vobj[ i ].dx = hold_dx;
//        vobj[ i ].dy = hold_dy;
//        vobj[ i ].dv = sqrt( pow( vobj[ i ].dx, 2 ) + pow( vobj[ i ].dy, 2 ) );
        }





    //check of objs at end of life
    if( vobj[ i ].finite_life )                                 //obj has a finite life?
        {
        if( vobj[ i ].time_to_live != 0.0  )        
            {
            vobj[ i ].time_to_live -= tframe;                   //dec life left

            if( vobj[ i ].time_to_live < 0.0  )                 //end of life
                {
                if( ( obj_type == en_ot_missile_user) | ( obj_type == en_ot_missile_cpu ) )
					{
					vobj[ i ].is_exploding = 1;
					}
				else{
					vobj[ i ].mark_to_delete = 1;                      //make obj for deletion
					}
                vobj[ i ].time_to_live = 0;
                }
            }
        }


    vobj[ i ].x += vobj[ i ].dx;
    vobj[ i ].y += vobj[ i ].dy;

    //wrap any offscreens
    if( vobj[ i ].can_wrap )
        {
        if( vobj[ i ].x > midx ) vobj[ i ].x = -midx;
        if( vobj[ i ].x < -midx ) vobj[ i ].x = midx;

        if( vobj[ i ].y > midy ) vobj[ i ].y = -midy;
        if( vobj[ i ].y < -midy ) vobj[ i ].y = midy;
        }

    //do obj moves
    if( vobj[ i ].dx != 0 ) vobj[ i ].dx *= 1.0 - vobj[ i ].drag ;
    if( vobj[ i ].dy != 0 ) vobj[ i ].dy *= 1.0 - vobj[ i ].drag ;




	if( vobj[ i ].col_temp_frame_cnt > 0 )
		{
		vobj[ i ].col_temp_frame_cnt--;
		}
    }




//count down to next medic insertion
if( !medic_exists )
    {
    frames_till_next_medic -= 1;
    if( frames_till_next_medic <= 0 )
        {
        double dr1 = ( rand() % 10 ) - 5.0;
        double dr2 = ( rand() % 10 ) - 5.0;
        
        double xx, yy, dx, dy;
        
        xx = 0 + midx*rnd();
        yy = 0 + midy*rnd();
        dx = 10*rnd();
        dy = 10*rnd();
       
//        add_medic( 0, midy / 1.2, dr1 / 10.0, dr2 / 10.0 );
 
		add_medic( xx, yy, dx, dy );
 
        
        dr1 = rand() % 30;                      //add some random time to next medic insertion
        frames_till_next_medic = ( 30.0 + dr1 ) / tframe; 
        }
    }





//spawn a missile?
next_missle_frame_cnt--;

if( ( missle_in_flight_cpu == -1 ) && ( next_missle_frame_cnt == 0 ) )
    {
    next_missle_frame_cnt = frames_per_missle_launch;
    if( !have_a_ship_explosion ) 
		{
		if( ( (double)vobj[ 1 ].hits / max_ship_hits ) < 0.75  )
			{
			add_missile( 1 );
			add_hits( 1,  max_ship_hits * missile_cost );				//pay cost
//			vobj[ 1 ].hits += max_ship_hits * missile_cost;

			}		
		}
    }


if( disable_deaths )
{
vobj[0].hits = 0;
vobj[1].hits = 0;
}



if( z_key ) 
	{
//	if( missle_in_flight_cpu < 5 ) add_missile( 1 );
	}


if( x_key ) 
	{
//	if( missle_in_flight_user < 5 ) add_missile( 0 );
	}




//vobj[1].x = 100;
//vobj[1].y = 100;

redraw();
}
















//0xaabbggrr
void mywnd::set_col( unsigned int col )
{
colref cr; 

cr.r = col & 0xff; 
cr.g = ( col >> 8 ) & 0xff; 
cr.b = ( col >> 16 ) & 0xff; 

setcolref( cr );
}





void mywnd::line( double x1, double y1, double x2, double y2 )
{

double xx1 = midx + x1 * zoomx;
double yy1 = midy - y1 * zoomy;

double xx2 = midx + x2 * zoomx;
double yy2 = midy - y2 * zoomy;

fl_line( nearbyint( xx1 ), nearbyint( yy1 ), nearbyint( xx2 ), nearbyint( yy2 ) );

}







//translate rotate translate
void mywnd::rotate( double xx, double yy, double &x1, double &y1, double &x2, double &y2, double theta )
{

//xx = ( x1 - x2 );
//yy = ( y1 - y2 );


double tx1 = x1 - xx;
double ty1 = y1 - yy;

double tx2 = x2 - xx;
double ty2 = y2 - yy;


double rx1 = tx1 * cos( theta ) - ty1 * sin( theta );
double ry1 = tx1 * sin( theta ) + ty1 * cos( theta );

double rx2 = tx2 * cos( theta ) - ty2 * sin( theta );
double ry2 = tx2 * sin( theta ) + ty2 * cos( theta );

x1 = rx1 + xx;
y1 = ry1 + yy;

x2 = rx2 + xx;
y2 = ry2 + yy;

}






void mywnd::draw_death_reason( int posy )
{
string s1;

if( exploding_ship_user )
	{
	set_col( 0x0000ffff );
	strpf( s1, "-- Your ship was destroyed --" );
	
//	fl_draw( s1.c_str(), midx - 140, posy + 20  );
	fl_draw( s1.c_str(), midx, posy + 20, 1, 1, FL_ALIGN_CENTER );

	set_col( 0x00009000 );
	strpf( s1, "(cause:  %s)", sdeath.c_str() );
//	fl_draw( s1.c_str(), midx - 190, posy + 40  );
	fl_draw( s1.c_str(), midx, posy + 40, 1, 1, FL_ALIGN_CENTER );
	}

if( exploding_ship_cpu )
	{
	set_col( 0x0000ff00 );
	strpf( s1, "-- cpu ship was destroyed --" );
//	fl_draw( s1.c_str(), midx - 140, posy + 20  );
	fl_draw( s1.c_str(), midx, posy + 20, 1, 1, FL_ALIGN_CENTER );

	set_col( 0x00009000 );
	strpf( s1, "(cause:  %s)", sdeath.c_str() );
//	fl_draw( s1.c_str(), midx - 190, posy + 40  );
	fl_draw( s1.c_str(), midx, posy + 40, 1, 1, FL_ALIGN_CENTER );
	}
}












void mywnd::draw_offscr( bool menu_showing )
{
string s1;

int iF = fl_font();
int iS = fl_size();


fl_font( 4, 13 );

//int img_w = jpg->w();
//int img_h = jpg->h();


/*
if ( offscr == 0 )
	{
	offscr = fl_create_offscreen( sizex, sizey );
	}


if( offscr == 0 )
	{
	printf( "\nFailed to allocate offscr_dest\n" );
	exit(0);
	}
*/


//	fl_begin_offscreen( offscr );

//clear wnd
colref cr; 
setcolref( col_bkgd );

if( menu_showing ) fl_rectf( 0, menu_hei, w() , h() );
else fl_rectf( 0, 0, w() , h() );

int half_secs = (double) frame_counter * tframe * 2.0;

if( ( !disable_deaths ) && ( !menu_showing ) )
	{
	set_col( 0x0000A000 );
	strpf( s1, "cpu ships: %d", ships_left_cpu );
	fl_draw( s1.c_str(), 5, 13 );


	set_col( 0x00808080 );
	if( ( vobj[ 1 ].hits >= damage_warning_threshold ) && ( half_secs % 2  ) ) set_col( 0x008080ff );  //flash cpu ship's damage warning?
	strpf( s1, "damage: %.0f%%", 100.0 * (double)vobj[ 1 ].hits / (double)max_ship_hits );
	fl_draw( s1.c_str(), 5, 26 );


	set_col( 0x00008080 );
	strpf( s1, "your ships: %d", ships_left_user );
	fl_draw( s1.c_str(), sizex - 130, 13 );

	set_col( 0x00808080 );
	if( ( vobj[ 0 ].hits >= damage_warning_threshold ) && ( half_secs % 2  ) ) set_col( 0x008080ff );  //flash user ship's damage warning?
	strpf( s1, "damage: %.0f%%", 100.0 * (double)vobj[ 0 ].hits / (double)max_ship_hits );
	fl_draw( s1.c_str(), sizex - 130, 26 );
	}

set_col( 0x00808080 );
if( ( pref_frames_per_second ) && (!menu_showing) )
	{
	strpf( s1, "fps: %.1f", 1.0 / a_wnd->measured_tframe_avg );
	fl_draw( s1.c_str(), sizex / 2.2, 13 );
	}

//set_col( 0x00808080 );
//strpf( s1, "ship_to_ship_distance: %f", ship_to_ship_distance );
//fl_draw( s1.c_str(), 30, 50 );




//if( ships_left_user == 0 ) game_ended = 1;
//if( ships_left_cpu == 0 ) game_ended = 1;



for( int i = 0; i < vobj.size(); i++ )
    {
    double xx = vobj[ i ].x;
    double yy = vobj[ i ].y;
    double theta = vobj[ i ].theta;

    vector<st_line_tag> vo;
    vo = vobj[ i ].vline;
    
    
    
    for( int j = 0; j < vo.size(); j++ )
        {
        double x1 = vo[ j ].x1;
        double y1 = vo[ j ].y1;
        double x2 = vo[ j ].x2;
        double y2 = vo[ j ].y2;

        rotate( 0, 0, x1, y1, x2, y2, theta - M_PI / 2 );


		if( vobj[ i ].col_temp_frame_cnt > 0 ) set_col( vobj[ i ].col_temp );	//show temp col if req
        else set_col( vobj[ i ].col );

        line( xx + x1, yy + y1, xx + x2, yy + y2 );
        }



	bool show_collision_rect = 0;
	if( show_collision_rect )
        {
        if( ( vobj[ i ].type != en_ot_bul_user ) && ( vobj[ i ].type != en_ot_bul_cpu ) && ( vobj[ i ].type != en_ot_exhaust ) )
			{
			double x1;
			double y1;
			double x2;
			double y2;
			x1 = vobj[ i ].x + vobj[ i ].bound_x1;
			x2 = vobj[ i ].x + vobj[ i ].bound_x2;
			y1 = vobj[ i ].y + vobj[ i ].bound_y1;
			y2 = vobj[ i ].y + vobj[ i ].bound_y2;

			set_col( 0x00909090 );
			line( x1, y1, x2, y1 );
			line( x2, y1, x2, y2 );
			line( x2, y2, x1, y2 );
			line( x1, y2, x1, y1 );
			}
        }

    }




//draw line at bottom of wnd to show user's damage
if( !disable_deaths )
	{
	double xx1 = 0;
	double yy1 = h() - 3;
	double xx2 = 0;
	double yy2 = h() - 3;

	xx2 = w() * (double)vobj[ 0 ].hits / (double)max_ship_hits;

	if( ( vobj[ 0 ].hits > damage_warning_threshold ) && ( half_secs % 2  ) ) 
		{
		}
	else{
		set_col( 0x008080ff );
		fl_line_style( FL_SOLID, 3 );
		fl_line( xx1, yy1, xx2, yy2 );
		fl_line_style( FL_SOLID, 0 );
		}
	}


//exploding_ship_cpu = 1;
//game_ended = 1;
if( ( game_ended ) )
    {
//    set_col( 0x00ffffff );
//    strpf( s1, "Game finished - you won, new game (y/n)?" );
//    if( ships_left_user == 0 ) strpf( s1, "Game finished - you lost, new game (y/n)?" );
//    if( ships_left_user == ships_left_cpu ) strpf( s1, "Game finished - with no winner, new game (y/n)?" );
//    fl_draw( s1.c_str(), midx - 190, midy - 30 );

    set_col( 0x00ffffff );
	fl_font( cnFontEditor, cnSizeEditor * 1.2 );
    strpf( s1, "Game finished - you won" );
    if( ships_left_user == 0 ) strpf( s1, "Game finished - you lost" );
    if( ships_left_user == ships_left_cpu ) strpf( s1, "Game finished - with no winner" );
    fl_draw( s1.c_str(), midx, midy - 140, 1,1, FL_ALIGN_CENTER );
	fl_font( cnFontEditor, cnSizeEditor );
 
	draw_death_reason( midy - 250 );
 
    game_to_start = 1;
    }




if( ( paused ) && ( !game_to_start ) && ( !game_ended ) ) 
    {
//    set_col( 0x00ffffff );
//    strpf( s1, "GravStar %s", cns_version );
    
 //   fl_font( cnFontEditor, cnSizeEditor * 2 );

 //   fl_draw( s1.c_str(), midx - 150, midy  );

    fl_font( cnFontEditor, cnSizeEditor );
   
 /*   
    if( exploding_ship_user )
		{
		set_col( 0x0000ffff );
		strpf( s1, "-- Your ship was destroyed --" );
		
		fl_draw( s1.c_str(), midx - 140, midy + 20  );

		set_col( 0x00009000 );
		strpf( s1, "(cause:  %s)", sdeath.c_str() );
		fl_draw( s1.c_str(), midx - 190, midy + 40  );
		}

    if( exploding_ship_cpu )
		{
		set_col( 0x0000ff00 );
		strpf( s1, "-- cpu ship was destroyed --" );
		fl_draw( s1.c_str(), midx - 140, midy + 20  );

		set_col( 0x00009000 );
		strpf( s1, "(cause:  %s)", sdeath.c_str() );
		fl_draw( s1.c_str(), midx - 190, midy + 40  );
		}
*/
	draw_death_reason( midy );

    set_col( 0x0000d0d0 );
    if(  keyhit_disable_cnt == 0 ) 
		{
		strpf( s1, "Game is paused, press a key to continue..." );
		fl_draw( s1.c_str(), midx, midy + 86, 1, 1, FL_ALIGN_CENTER );
		}
	else{
//		set_col( 0x00ffffff );
//		fl_font( cnFontEditor, cnSizeEditor * 1.5 );
//		strpf( s1, "%d", (int)(keyhit_disable_cnt * tframe) );			//show a countdown
//		fl_draw( s1.c_str(), midx, midy + 86, 1, 1, FL_ALIGN_CENTER );
		
		//draw a progress bar
//		set_col( 0x0000d0d0 );
		set_col( 0x00c430cb );
		float xxx = 1.0f - keyhit_disable_cnt / (cn_keyhit_disable_time / tframe);
		xxx *= 400;
		fl_line_style ( FL_SOLID, 6 );
		fl_line( midx - 190, midy + 70, (midx - 190 + xxx), midy + 70 );
		fl_line_style ( FL_SOLID, 0 );
		

//		string st;
//		for( int i = 0; i < ( cn_keyhit_disable_time / tframe - keyhit_disable_cnt )/5; i++ )
//			{
//			strpf( s1, "_" );			//show a countdown
//			st += s1;
//			}		
//		fl_draw( st.c_str(), midx - 190, midy + 100, 1, 1, FL_ALIGN_LEFT );
		fl_font( cnFontEditor, cnSizeEditor );
		}
    }


if( ( paused ) && ( game_to_start ) && ( keyhit_disable_cnt == 0 ) ) 
    {
    set_col( 0x00ffffff );
    strpf( s1, "GravStar %s", cns_version );
    
    fl_font( cnFontEditor, cnSizeEditor * 2 );

//    fl_draw( s1.c_str(), midx - 150, midy  );
	fl_draw( s1.c_str(), midx, midy, 1, 1, FL_ALIGN_CENTER );

    set_col( 0x0000d0d0 );
    fl_font( cnFontEditor, cnSizeEditor );

	int px = 230;
	int py = midy;
    strpf( s1, "'1' for occasional meteors, magnetic mines and grav objs" );
    fl_draw( s1.c_str(), midx - px, py + 56  );

    strpf( s1, "'2' for occasional meteors" );
    fl_draw( s1.c_str(), midx - px, py + 69  );

    strpf( s1, "'3' for occasional magnetic mines" );
    fl_draw( s1.c_str(), midx - px, py + 82  );

    strpf( s1, "'4' for occasional grav objs" );
    fl_draw( s1.c_str(), midx - px, py + 95  );

    strpf( s1, "'5' for ships only" );
    fl_draw( s1.c_str(), midx - px, py + 108  );

    strpf( s1, "'q' to quit" );
    fl_draw( s1.c_str(), midx - px, py + 136  );

    strpf( s1, "'h' key to open 'help.txt'" );
    fl_draw( s1.c_str(), midx - px, py + 166  );
	}


if( attract_mode_active )
	{
    set_col( 0x00808080 );
	strpf( s1, "Press a key to Play..." );
	fl_draw( s1.c_str(), midx, midy - 20, 1, 1, FL_ALIGN_CENTER );
	}

//	fl_read_image( buf, 0, 0, sizex, sizey, 0 );	//read offscreen obj pixels into buffer

//	fl_end_offscreen( );


fl_font( iF, iS );

}
















void mywnd::draw()
{
int rght,bot;
string s1;
fl_font( cnFontEditor, cnSizeEditor );

sizex = w();
sizey = h();

midx = sizex / 2;
midy = sizey / 2;

bool menu_showing = 0;
if( mousey < menu_hei ) menu_showing = 1;
if( !is_inside ) menu_showing = 0;






if( menu_showing ) 
	{
	Fl_Double_Window::draw();
	menu2->redraw();
	}

draw_offscr( menu_showing );              //do some fltk drawing calls

//write buffer to offscreen obj
//fl_draw_image( ( const uchar* ) buf, 0, 0, sizex, sizey, cn_bytes_per_pixel, cn_bytes_per_pixel * sizex );


//fl_copy_offscreen( 0, 0, sizex, sizey, offscr, 0, 0 );


 
/*
//clear wnd
setcolref( col_bkgd );
fl_rectf( 0 , 0 , w() , h() - 50 );

setcolref( col_yel );

strpf( s1, "Dropped something here: %s", dropped_str.c_str() );
fl_draw( s1.c_str(), 5, 14 );


if ( ctrl_key == 1 ) s1 ="ControlKey: down";
else s1 ="ControlKey: up";
fl_draw( s1.c_str(), 5, 42 );


if ( left_button == 1 ) s1 ="LeftButton: down";
else s1 ="LeftButton: up";
setcolref( col_yel );
fl_draw( s1.c_str(), 5, 56 );

strpf( s1,"MouseWheel Val = %d" , mousewheel );
fl_draw( s1.c_str(), 5, 70 );

strpf( s1,"You should see a map below... ");
fl_draw( s1.c_str(), 5, 98 );

setcolref( col_mag );
fl_line( 50, 70, 80 , h() );


//bx_image->draw( 0, 0 );
*/
}
















void mywnd::add_bullet( int owner_idx )
{

int ii = owner_idx;
if( ii >= vobj.size() ) return;

//build obj
st_obj_tag o;
st_line_tag oo;

if( owner_idx == 0 )
    {
    o.type = en_ot_bul_user;				//user's
	o.col = 0x0000ffff;
    oo.col = 0x0000ffff;
    }

if( owner_idx == 1 )						//cpu's
    {
    o.type = en_ot_bul_cpu;
	o.col = 0x0000ff00;
    oo.col = 0x0000ff00;
    }

o.col_temp = 0x000000ff;
o.col_temp_frames_total = 10;
o.col_temp_frame_cnt = 0;

o.x = vobj[ ii ].x;
o.y = vobj[ ii ].y;
o.dx = vobj[ ii ].dx + 100.0 * cos( vobj[ ii ].theta ) * measured_tframe;
o.dy = vobj[ ii ].dy + 100.0 * sin( vobj[ ii ].theta ) * measured_tframe;
o.dv = sqrt( pow( o.dx, 2 ) + pow( o.dy, 2 ) );

o.max_dx = 10;
o.max_dy = 10;

o.theta = vobj[ ii ].theta;
o.delta_spin = 0;
o.thrust = 0;
o.drag = 0.001;						//make it relative to tframe: 0.995 / 0.02, was: 0.995
o.mass = 1;
o.accel = 0;
o.accelx = 0;
o.accely = 0;
o.finite_life = 1;
o.time_to_live = 5;
o.mark_to_delete = 0;
o.has_thrust = 0;
o.can_wrap = 0;
o.is_exploding = 0;
o.explode_frames_total = 0;
o.explode_frame_cnt = 0;
o.grav_affected = 1;

oo.x1 = 0;
oo.y1 = 0;
oo.x2 = 0;
oo.y2 = 0;

o.vline.push_back( oo );

vobj.push_back( o );
}









void mywnd::add_missile( int owner_idx )
{
//return;

int ii = owner_idx;
if( ii >= vobj.size() ) return;



//build obj
st_obj_tag o;
st_line_tag oo;

if( owner_idx == 0 )
    {
    o.type = en_ot_missile_user;				//user's
	o.col = 0x0000ffff;
    oo.col = 0x0000ffff;
	o.time_to_live = 16;
    }

if( owner_idx == 1 )							//cpu's
    {
    o.type = en_ot_missile_cpu;
	o.col = 0x0000ff00;
    oo.col = 0x0000ff00;
	o.time_to_live = 19;
    }

o.col_temp = 0x000000ff;
o.col_temp_frames_total = 10;
o.col_temp_frame_cnt = 0;

o.x = vobj[ ii ].x + 10.0 * cos( vobj[ ii ].theta );
o.y = vobj[ ii ].y + 10.0 * sin( vobj[ ii ].theta );
o.dx = vobj[ ii ].dx;
o.dy = vobj[ ii ].dy;
o.dv = sqrt( pow( o.dx, 2 ) + pow( o.dy, 2 ) );
o.max_dx = 2 * 1;
o.max_dy = 2 * 1;

o.theta = vobj[ ii ].theta;
o.theta_req = 0;
o.delta_theta_per_frame = 0.1;
o.delta_spin = 0;


o.thrust_max = 18000;
o.thrust = 0.0;
o.thrust_inc_per_frame = 80 * 1;							//make it relative to tframe: 80 / 0.02, was: 80
o.drag = 0.08;															//make it relative to tframe: 10 / 0.02, was: 4
o.mass = 900;
o.accel = 0;
o.accelx = 0;
o.accely = 0;
o.finite_life = 1;
o.mark_to_delete = 0;
o.has_thrust = 1;
o.can_wrap = 1;
o.is_exploding = 0;
o.explode_frames_total = 2.0 / tframe;
o.explode_frame_cnt = o.explode_frames_total;
o.explode_shape = 0;
o.missle_steer_delay = 2.0 / tframe;
o.grav_affected = 1;

o.arming_delay_frame_cnt = 3.0 / tframe;

oo.x1 = 0;
oo.y1 = 4;
oo.x2 = 0;
oo.y2 = 4;

o.vline.push_back( oo );


oo.x1 = 0;
oo.y1 = 4;
oo.x2 = 1;
oo.y2 = -4;

o.vline.push_back( oo );

oo.x1 = 1;
oo.y1 = -4;
oo.x2 = -1;
oo.y2 = -4;

o.vline.push_back( oo );

oo.x1 = -1;
oo.y1 = -4;
oo.x2 = 0;
oo.y2 = 4;

o.vline.push_back( oo );

vobj.push_back( o );
set_bounding_rect( vobj.size() - 1, bounding_scale_down, bounding_scale_down );
}











void mywnd::add_magmine( int shape, bool affected_by_grav, bool apply_dxdy, double scale, double xx, double yy, double dx, double dy, float time_to_live )
{
//return;




//build obj
st_obj_tag o;
st_line_tag oo;

o.type = en_ot_magmine;
o.col = 0x00ffff9a;
oo.col = 0x0ff0ff9a;

o.time_to_live = time_to_live;

if( time_to_live != 0.0 ) 
	{
	o.finite_life = 1;
	}
else{
	o.finite_life = 0;
	}

o.col_temp = 0x008080ff;
o.col_temp_frames_total = 10;
o.col_temp_frame_cnt = 0;

o.x = xx;
o.y = yy;
o.dx = dx;
o.dy = dy;
o.dv = sqrt( pow( o.dx, 2 ) + pow( o.dy, 2 ) );
o.max_dx = 2 * 1;
o.max_dy = 2 * 1;

o.theta = pi2;
o.theta_req = pi;
o.delta_theta_per_frame = 0.1;
o.delta_spin = 0.05*rnd();


o.thrust_max = 1800;
o.thrust = 0.0;
o.thrust_inc_per_frame = 80 * 1;							//make it relative to tframe: 80 / 0.02, was: 80
o.drag = 0.001;															//make it relative to tframe: 10 / 0.02, was: 4
o.mass = 9000;
o.accel = 0;
o.accelx = 0;
o.accely = 0;
//o.finite_life = 1;
o.mark_to_delete = 0;
o.has_thrust = 1;
o.can_wrap = 1;
o.is_exploding = 0;
o.explode_frames_total = 2.0 / tframe;
o.explode_frame_cnt = o.explode_frames_total;
o.explode_shape = 0;
o.missle_steer_delay = 2.0 / tframe;
o.grav_affected = 1;
o.magmine_in_flight_frame_cnt = 0;

o.arming_delay_frame_cnt = 3.0 / tframe;

oo.x1 = -1;
oo.y1 = 1;
oo.x2 = 1;
oo.y2 = 1;

o.vline.push_back( oo );


oo.x1 = 1;
oo.y1 = 1;
oo.x2 = 1;
oo.y2 = -1;

o.vline.push_back( oo );

oo.x1 = 1;
oo.y1 = -1;
oo.x2 = -1;
oo.y2 = -1;

o.vline.push_back( oo );

oo.x1 = -1;
oo.y1 = -1;
oo.x2 = -1;
oo.y2 = 1;

o.vline.push_back( oo );


oo.x1 = -2;
oo.y1 = 2;
oo.x2 = -2;
oo.y2 = 2;

o.vline.push_back( oo );


oo.x1 = 2;
oo.y1 = 2;
oo.x2 = 2;
oo.y2 = 2;

o.vline.push_back( oo );


oo.x1 = 2;
oo.y1 = -2;
oo.x2 = 2;
oo.y2 = -2;

o.vline.push_back( oo );


oo.x1 = -2;
oo.y1 = -2;
oo.x2 = -2;
oo.y2 = -2;

o.vline.push_back( oo );

vobj.push_back( o );
set_bounding_rect( vobj.size() - 1, bounding_scale_down, bounding_scale_down );
}














void mywnd::add_magmine_fragment( int shape, bool affected_by_grav, bool apply_dxdy, double scale, double xx, double yy, double dx, double dy, float time_to_live, float spin )
{
//return;

//build obj
st_obj_tag o;
st_line_tag oo;

o.type = en_ot_magmine_fragment;
o.col = 0x00ffff9a;
oo.col = 0x0ff0ff9a;

o.time_to_live = time_to_live;

if( time_to_live != 0.0 ) 
	{
	o.finite_life = 1;
	}
else{
	o.finite_life = 0;
	}

o.col_temp = 0x008080ff;
o.col_temp_frames_total = 10;
o.col_temp_frame_cnt = 0;

o.x = xx;
o.y = yy;
o.dx = dx;
o.dy = dy;
o.dv = sqrt( pow( o.dx, 2 ) + pow( o.dy, 2 ) );
o.max_dx = 2 * 1;
o.max_dy = 2 * 1;

o.theta = pi2;
o.theta_req = pi;
o.delta_theta_per_frame = 0.1;
o.delta_spin = spin;


o.thrust_max = 1800;
o.thrust = 0.0;
o.thrust_inc_per_frame = 80 * 1;							//make it relative to tframe: 80 / 0.02, was: 80
o.drag = 0.001;															//make it relative to tframe: 10 / 0.02, was: 4
o.mass = 9000;
o.accel = 0;
o.accelx = 0;
o.accely = 0;
//o.finite_life = 1;
o.mark_to_delete = 0;
o.has_thrust = 1;
o.can_wrap = 1;
o.is_exploding = 0;
o.explode_frames_total = 2.0 / tframe;
o.explode_frame_cnt = o.explode_frames_total;
o.explode_shape = 0;
o.missle_steer_delay = 2.0 / tframe;
o.grav_affected = 1;
o.magmine_in_flight_frame_cnt = 0;

o.arming_delay_frame_cnt = 3.0 / tframe;

/*
oo.x1 = -1;
oo.y1 = 1;
oo.x2 = 1;
oo.y2 = 1;

o.vline.push_back( oo );


oo.x1 = 1;
oo.y1 = 1;
oo.x2 = 1;
oo.y2 = -1;

o.vline.push_back( oo );

oo.x1 = 1;
oo.y1 = -1;
oo.x2 = -1;
oo.y2 = -1;

o.vline.push_back( oo );

oo.x1 = -1;
oo.y1 = -1;
oo.x2 = -1;
oo.y2 = 1;

o.vline.push_back( oo );
*/

oo.x1 = -2;
oo.y1 = 0;
oo.x2 = 2;
oo.y2 = 0;

o.vline.push_back( oo );

/*
oo.x1 = 2;
oo.y1 = 2;
oo.x2 = 2;
oo.y2 = 2;

o.vline.push_back( oo );


oo.x1 = 2;
oo.y1 = -2;
oo.x2 = 2;
oo.y2 = -2;

o.vline.push_back( oo );


oo.x1 = -2;
oo.y1 = -2;
oo.x2 = -2;
oo.y2 = -2;

o.vline.push_back( oo );
*/
vobj.push_back( o );
set_bounding_rect( vobj.size() - 1, bounding_scale_down, bounding_scale_down );
}














//level is 0->1.0
void mywnd::add_exhaust( int owner_idx, double level )
{
if( level < 0.05 )  return;

int ii = owner_idx;
if( ii >= vobj.size() ) return;

if( vobj[ ii ].is_exploding ) return;

//build obj
st_obj_tag o;
st_line_tag oo;

o.type = en_ot_exhaust;
o.col = 0x00a0a0a0;
o.col_temp = 0x00000000;
o.col_temp = 0x000000ff;							//red
int gamble = rand_range( 0, 2 );
if( gamble == 1 ) o.col_temp = 0x0000ffff;			//yell
if( gamble == 2 ) o.col_temp = 0x00009dff;			//orange

o.col_temp_frames_total = 20.0 * 1.0 / frame_factor;
o.col_temp_frame_cnt = 20.0 * 1.0 / frame_factor;

oo.col = 0x00a0a0a0;


int rr = rand_range( -25, 25 );

double dr = rr / 100.0;

o.x = vobj[ ii ].x;
o.y = vobj[ ii ].y;
o.dx = vobj[ ii ].dx - 1.0 * ( cos( vobj[ ii ].theta + dr ) ) * frame_factor;
o.dy = vobj[ ii ].dy - 1.0 * ( sin( vobj[ ii ].theta + dr ) ) * frame_factor;
o.dv = sqrt( pow( o.dx, 2 ) + pow( o.dy, 2 ) );

o.max_dx = 50;
o.max_dy = 50;

o.theta = vobj[ ii ].theta;
o.delta_spin = 0;
o.thrust = 0;
o.drag = 0.02 * frame_factor;
o.mass = 0;
o.accel = 0;
o.accelx = 0;
o.accely = 0;
o.finite_life = 1;
o.time_to_live = ( 0.6 + 1.0 * fabs( dr ) ) * level;
o.mark_to_delete = 0;
o.has_thrust = 0;
o.can_wrap = 0;
o.is_exploding = 0;
o.explode_frames_total = 0;
o.explode_frame_cnt = 0;
o.grav_affected = 0;


oo.x1 = 0;
oo.y1 = -5;
oo.x2 = 0;
oo.y2 = -5;

o.vline.push_back( oo );

vobj.push_back( o );
}












void mywnd::add_chaff( int owner_idx )
{
//return;

int ii = owner_idx;
if( ii >= vobj.size() ) return;



//build obj
st_obj_tag o;
st_line_tag oo;

if( owner_idx == 0 )
    {
    o.type = en_ot_chaff_user;				//user's
	o.col = 0x0000d0d0;
    oo.col = 0x0000d0d0;
    }

if( owner_idx == 1 )							//cpu's
    {
    o.type = en_ot_chaff_cpu;
	o.col = 0x0050d050;
    oo.col = 0x0050d050;
    }

o.col_temp = 0x000000ff;
o.col_temp_frames_total = 10;
o.col_temp_frame_cnt = 0;

o.x = vobj[ ii ].x + -10.0 * cos( vobj[ ii ].theta );
o.y = vobj[ ii ].y + -10.0 * sin( vobj[ ii ].theta );
o.dx = vobj[ ii ].dx + 0.5 * ( rand_range( 0, 10 ) / 10.0 );
o.dy = vobj[ ii ].dy + 0.7 * ( rand_range( 0, 10 ) / 10.0 );
o.dv = 0;//sqrt( pow( o.dx, 2 ) + pow( o.dy, 2 ) );
o.max_dx = 1e99;
o.max_dy = 1e99;

o.theta = vobj[ ii ].theta;
o.theta_req = 0;
o.delta_theta_per_frame = 0.1;
o.delta_spin = 0;


o.thrust_max = 0;
o.thrust = 0.0;
o.thrust_inc_per_frame = 0;							//make it relative to tframe: 80 / 0.02, was: 80
o.drag =  + 0.001 + 0.007 * ( rand_range( 0, 10 ) / 10.0 );															//make it relative to tframe: 10 / 0.02, was: 4
o.mass = 900;
o.accel = 0;
o.accelx = 0;
o.accely = 0;
o.finite_life = 1;
o.time_to_live = 3 + 2 * ( rand_range( 0, 10 ) / 10.0 );
o.mark_to_delete = 0;
o.has_thrust = 1;
o.can_wrap = 1;
o.is_exploding = 0;
o.explode_frames_total = 2.0 / tframe;
o.explode_frame_cnt = o.explode_frames_total;
o.explode_shape = 0;
o.missle_steer_delay = 0;
o.grav_affected = 1;

o.arming_delay_frame_cnt = 0;

oo.x1 = -2;
oo.y1 = -2;
oo.x2 = 1;
oo.y2 = 2;

o.vline.push_back( oo );


oo.x1 = 1;
oo.y1 = 2;
oo.x2 = 2;
oo.y2 = -2;

o.vline.push_back( oo );

oo.x1 = 2;
oo.y1 = -2;
oo.x2 = -2;
oo.y2 = -2;


o.vline.push_back( oo );

vobj.push_back( o );
set_bounding_rect( vobj.size() - 1, bounding_scale_down, bounding_scale_down );
}














//add an arc of using line coords
bool mywnd::append_arcseg( st_obj_tag &o, int segments, double radius, double start_ang, double stop_ang, double ofx, double ofy )
{
if( segments < 1 ) return 0;

double rad = radius;

double theta_span = ( start_ang - stop_ang );

double theta_inc = theta_span / segments;

double theta = start_ang;

st_line_tag oo;

oo.x2 = rad * cos( theta ) + ofx;				//initial point
oo.y2 = rad * sin( theta ) + ofy;


for( int i = 0; i < segments + 1; i++ )
	{
	oo.x1 = rad * cos( theta ) + ofx;
	oo.y1 = rad * sin( theta ) + ofy;

	o.vline.push_back( oo );

//printf( "[%d] fx1: %f, fy1: %f\n", i, fx1, fy1 );

	oo.x2 = oo.x1;
	oo.y2 = oo.y1;

	theta += theta_inc;
	}
//getchar();
return 1;
}











void mywnd::add_grav_obj( double xx, double yy, double dx, double dy )
{

//build obj
st_obj_tag o;
st_line_tag oo;

o.type = en_ot_grav_obj;
o.col = 0x006060A0;
oo.col = o.col;

o.col_temp = 0x000000ff;
o.col_temp_frames_total = 10;
o.col_temp_frame_cnt = 0;

o.x = xx;
o.y = yy;
o.dx = dx;
o.dy = dy;
o.dv = sqrt( pow( o.dx, 2 ) + pow( o.dy, 2 ) );

o.max_dx = 50;
o.max_dy = 50;

o.theta = 0;
o.delta_spin = 0;
o.thrust = 0;
//o.mass = 2000000000000;//5.97219e24;
o.mass = 2000000000000;//5.97219e24;
o.accel = 0;
o.accelx = 0;
o.accely = 0;
o.drag = 1;
o.finite_life = 0;
o.time_to_live = 60;
o.mark_to_delete = 0;
o.has_thrust = 0;
o.can_wrap = 0;
o.is_exploding = 0;
o.explode_frames_total = 0;
o.explode_frame_cnt = 0;
o.grav_affected = 0;

/*
oo.x1 = -5;
oo.y1 = 5;
oo.x2 = 5;
oo.y2 = 5;
o.vline.push_back( oo );


oo.x1 = 5;
oo.y1 = 5;
oo.x2 = 5;
oo.y2 = -5;
o.vline.push_back( oo );


oo.x1 = 5;
oo.y1 = -5;
oo.x2 = -5;
oo.y2 = -5;
o.vline.push_back( oo );

oo.x1 = -5;
oo.y1 = -5;
oo.x2 = -5;
oo.y2 = 5;
o.vline.push_back( oo );
*/

append_arcseg( o, 20, 8, 0, twopi, 0, 0 );


vobj.push_back( o );

int grav_idx = vobj.size() - 1;
set_bounding_rect( grav_idx, bounding_scale_down, bounding_scale_down );

double ang, dist;
ang = find_angle_between_objs( 1, grav_idx, dist );

vobj[ 0 ].dx = ( 1.0 + rnd() ) * cos( ang );		//give user's ship some movement
vobj[ 0 ].dy = ( 0.5 + rnd() ) * sin( ang );


vobj[ 1 ].dx = ( -1.0 + rnd() ) * cos( ang );		//give cpu's ship some movement
vobj[ 1 ].dy = ( 0.5 + rnd() ) * sin( ang );




/*
o.type = en_ot_grav_obj;
o.col = 0x00000000;
oo.col = 0x00000000;


o.x = -100;
o.y = 100;
o.dx = -0.05;
o.dy = 0.1;
o.dv = sqrt( pow( o.dx, 2 ) + pow( o.dy, 2 ) );

o.max_dx = 50;
o.max_dy = 50;

o.theta = 0;
o.thrust = 0;
o.mass = 2000000000000;//5.97219e24;
o.accel = 0;
o.accelx = 0;
o.accely = 0;
o.drag = 1;
o.finite_life = 1;
o.time_to_live = 60;
o.mark_to_delete = 0;
o.has_thrust = 0;
o.can_wrap = 0;
o.is_exploding = 0;
o.explode_frames_total = 0;
o.explode_frame_cnt = 0;
o.grav_affected = 0;


oo.x1 = -5;
oo.y1 = 5;
oo.x2 = 5;
oo.y2 = 5;
o.vline.push_back( oo );


oo.x1 = 5;
oo.y1 = 5;
oo.x2 = 5;
oo.y2 = -5;
o.vline.push_back( oo );


oo.x1 = 5;
oo.y1 = -5;
oo.x2 = -5;
oo.y2 = -5;
o.vline.push_back( oo );

oo.x1 = -5;
oo.y1 = -5;
oo.x2 = -5;
oo.y2 = 5;
o.vline.push_back( oo );

vobj.push_back( o );

grav_idx = vobj.size() - 1;

ang, dist;
ang = find_angle_between_objs( 1, grav_idx, dist );







o.type = en_ot_grav_obj;
o.col = 0x00000000;
oo.col = 0x00000000;


o.x = 200;
o.y = -200;
o.dx = 0.05;
o.dy = -0.05;
o.dv = sqrt( pow( o.dx, 2 ) + pow( o.dy, 2 ) );

o.max_dx = 50;
o.max_dy = 50;

o.theta = 0;
o.thrust = 0;
o.mass = 2000000000000;//5.97219e24;
o.accel = 0;
o.accelx = 0;
o.accely = 0;
o.drag = 1;
o.finite_life = 1;
o.time_to_live = 60;
o.mark_to_delete = 0;
o.has_thrust = 0;
o.can_wrap = 0;
o.is_exploding = 0;
o.explode_frames_total = 0;
o.explode_frame_cnt = 0;
o.grav_affected = 0;


oo.x1 = -5;
oo.y1 = 5;
oo.x2 = 5;
oo.y2 = 5;
o.vline.push_back( oo );


oo.x1 = 5;
oo.y1 = 5;
oo.x2 = 5;
oo.y2 = -5;
o.vline.push_back( oo );


oo.x1 = 5;
oo.y1 = -5;
oo.x2 = -5;
oo.y2 = -5;
o.vline.push_back( oo );

oo.x1 = -5;
oo.y1 = -5;
oo.x2 = -5;
oo.y2 = 5;
o.vline.push_back( oo );

vobj.push_back( o );

grav_idx = vobj.size() - 1;

ang, dist;
*/

}









/*
//if 'game_ended' is set will init() a new game and reset 'paused'
//if 'toggle' set will change 'paused' state
//if 'toggle' not set will reset 'paused'
void mywnd::change_pause( bool toggle )
{

if ( toggle ) paused = !paused;
else paused = 0;

}
*/














int bDonePaste = 0;		//need this to be 0 to make dragging from web browser in linux work


int mywnd::handle( int e )
{
bool need_redraw = 0;
bool dont_pass_on = 0;
bool game_running;


#ifdef compile_for_windows 
bDonePaste = 1;					//does not seem to be required for Windows, so nullify by setting to 1
#endif

if ( e == FL_PASTE )	//needed below code because on drag release the first FL_PASTE call does not have valid text as yet,
	{					//possibly because of a delay in X windows, so have used Fl:paste(..) to send another paste event and 
	if( bDonePaste == 0)	//this seems to work ok (does not seem to happen in Windows)
		{
		Fl::paste( *this, 0 );					//passing second var as 0 uses currently selected text, (not prev cut text)
//		printf("\nDropped1\n" );
		bDonePaste = 1;
		}
	else{
		bDonePaste = 0;
		string s = Fl::event_text();
		int len = Fl::event_length();
		printf("\nDropped Len=%d, Str=%s\n", len, s.c_str() );
		if( len )								//anything dropped/pasted?
			{
			dropped_str = s;
			need_redraw = 1;
			}
		}
//	return 1;
	dont_pass_on = 1;
	}


if (e == FL_DND_DRAG)
	{
	printf("\nDrag\n");
	dont_pass_on = 1;
	}

if (e == FL_DND_ENTER)
	{
	printf("\nDrag Enter\n");
	dont_pass_on = 1;
	}

if (e == FL_DND_RELEASE)
	{
	printf("\nDrag Release\n");
	dont_pass_on = 1;
	}

if ( e == FL_ENTER )
	{
	is_inside = 1;
	need_redraw = 1;
	dont_pass_on = 1;
	}
if ( e == FL_LEAVE )
	{
	is_inside = 0;
	need_redraw = 1;
	dont_pass_on = 1;
	}

if ( e == FL_MOVE )
	{
	mousex = Fl::event_x();
	mousey = Fl::event_y();
	need_redraw = 1;
	}

if ( e == FL_PUSH )
	{
	if(Fl::event_button()==1)
		{
		left_button = 1;
		need_redraw = 1;
		}
//	dont_pass_on = 1;
	}

if ( e == FL_RELEASE )
	{
	
	if(Fl::event_button()==1)
		{
		left_button = 0;
		need_redraw = 1;
		}
//	dont_pass_on = 1;
	}


if ( ( e == FL_KEYDOWN ) || ( e == FL_SHORTCUT ) )					//key pressed?
	{
	
	if( attract_mode_active )
		{
		init( 1 );
		
		game_to_start = 1;
		paused = 1;
		
		attract_mode_active = 0;
		}
	
	attract_mode_cnt = cn_attract_mode_delaystart / tframe;
//	attract_mode_active = 0;

	int key = Fl::event_key();

	if( key == 'h' )
		{
		if( paused ) cb_bt_help(0, 0);
		goto skip_key;
		}

   
	game_running = ( ( !game_to_start ) && ( !game_ended ) && ( !paused ) );

    
	if( ( key == FL_Control_L ) || (  key == FL_Control_R ) ) ctrl_key = 1;


	if( ( key == FL_Shift_L ) || (  key == FL_Shift_R ) )
		{
		if( ( !shift_key ) && ( game_running ) && ( !have_a_ship_explosion ) ) 
			{
			if( chaff_user < max_chaff )
				{
				if( ( (double)vobj[ 0 ].hits / max_ship_hits ) < 0.97  )	//only if resources allow
					{
					add_chaff( 0 );
					add_hits( 0,  max_ship_hits * chaff_cost );				//pay cost

//					vobj[ 0 ].hits += max_ship_hits * chaff_cost;			//pay cost
					}
				}

			shift_key = 1;
			}
		}

	
	if( key == FL_Up ) up_key = 1;
	if( key == FL_Down ) down_key = 1;
	if( key == FL_Left ) left_key = 1;
	if( key == FL_Right ) right_key = 1;

	if( ( paused ) && ( !game_to_start ) && ( !game_ended ) && ( keyhit_disable_cnt == 0 ) )
        {
		if( ( pause_ship_explosion )  )
			{
			init( 0 );
			}
        paused = 0;
		goto skip_key;
        }


	if( ( key == 'q' ) & ( ( game_to_start ) | ( game_ended ) ) )
        {
		if( paused ) DoQuit();
        }


	if( key == 'p' )
        {
		p_key = 1;
		}

	if( ( key == 'p' ) && ( !game_to_start ) && ( !game_ended ) )
        {
        paused = !paused;
        }


	if(  key == '0' )
		{
		disable_deaths = !disable_deaths;
		}

	if( ( key >= '1' ) & ( key  <= '5' ) )
		{
		if( ( game_to_start || game_ended ) && ( keyhit_disable_cnt == 0 ) )
			{
			option_key1 = 0;
			option_key2 = 0;
			option_key3 = 0;
			option_key4 = 0;
			option_key5 = 0;
			
			if( key == '1' ) option_key1 = 1;
			if( key == '2' ) option_key2 = 1;
			if( key == '3' ) option_key3 = 1;
			if( key == '4' ) option_key4 = 1;
			if( key == '5' ) option_key5 = 1;
			
			init( 1 );
			game_to_start = 0;
			game_ended = 0;
			paused = 0;
			goto skip_key;
			}
		}




	if( ( key == ' ' ) && (!space_key) && ( game_running ) && ( !have_a_ship_explosion ) && ( keyhit_disable_cnt == 0 ) )
		{
		space_key = 1;
		if( missle_in_flight_user < max_missles )
			{
			if( ( (double)vobj[ 0 ].hits / max_ship_hits ) < 0.75  )
				{
				evade_angle = rand_range( 0, 1 );						//assign a rnd evade angle flag for cpu ship to use
				add_missile( 0 );
				add_hits( 0,  max_ship_hits * missile_cost );			//pay cost
//				vobj[ 0 ].hits += max_ship_hits * missile_cost;
				}
			}
		}

	if( key == 'x' ) x_key = 1;

	if( ( key == 'z' ) && ( game_running ) )
		{
		z_key = 1;
		if( missle_in_flight_cpu < max_missles )
			{
			add_missile( 1 );
//			vobj[ 1 ].hits += max_ship_hits * 0.1;
			}
		}

	if( ( key == 'm' ) && ( game_running ) && ( !have_a_ship_explosion ) )
		{
		int rn = rand_range( 0, 3 );
		
		int xx1, yy1;
		double dx1, dy1, scle;
		bool affected_by_grav = 1;
		bool apply_dxdy = 1;
		if( rn == 0 )
			{
			xx1 = -midx;
			yy1 = -midy;
			dx1 = 3 + rnd() * 1.0;
			dy1 = 0.7 + rnd() * 0.3;
			scle = 0.5 + 0.8 * fabs( rnd() );
			}

		if( rn == 1 )
			{
			xx1 = -midx;
			yy1 = midy;
			dx1 = 2 + rnd() * 0.8;
			dy1 = -0.8 + rnd() * 0.4;
			scle = 0.5 + 0.8 * fabs( rnd() );
			}
		
		if( rn == 2 )
			{
			xx1 = midx;
			yy1 = midy;
			dx1 = -2 + rnd() * 0.8;
			dy1 = -0.8 + rnd() * 0.4;
			scle = 0.5 + 0.8 * fabs( rnd() );
			}

		if( rn == 3 )
			{
			xx1 = midx;
			yy1 = -midy;
			dx1 = -2 + rnd() * 0.8;
			dy1 = 0.8 + rnd() * 0.4;
			scle = 0.5 + 0.8 * fabs( rnd() );
			}

		add_meteor( rand_range(0, 2),    affected_by_grav,  apply_dxdy, scle,    xx1,   yy1,   dx1,  dy1 );
		}


	if( ( key == 'b' ) && ( game_running ) && ( !have_a_ship_explosion ) )
		{
		int xx1, yy1;
		double dx1, dy1, scle;
		bool affected_by_grav = 1;
		bool apply_dxdy = 1;
		float time_to_live = 0;
		xx1 = rnd() * 400.0;
		yy1 = rnd() * 400.0;
		dx1 = 0.0;
		dy1 = 0.0;
		scle = 1;
		add_magmine( 0, affected_by_grav,  apply_dxdy, scle,    xx1,   yy1,   dx1,  dy1, time_to_live );
		}

	if( ( key == 'g' ) && ( game_running ) && ( !have_a_ship_explosion ) )
		{
		int x = -w() / 2  + rand_range( 0, w() - 100 );
		int y = -h() / 2 + rand_range( 0, h() - 100 );
		add_grav_obj( x, y, 0.1, 0.1 );
		}

skip_key:
	need_redraw = 1;
	dont_pass_on = 1;
	}


if ( e == FL_KEYUP )												//key release?
	{
	int key = Fl::event_key();
	
	if( ( key == FL_Control_L ) || ( key == FL_Control_R ) ) ctrl_key = 0;
	if( ( key == FL_Shift_L ) || (  key == FL_Shift_R ) ) shift_key = 0;
	if( key == FL_Up ) up_key = 0;
	if( key == FL_Down ) down_key = 0;
	if( key == FL_Left ) left_key = 0;
	if( key == FL_Right ) right_key = 0;
	if( key == ' ' ) space_key = 0;
	if( key == 'n' ) n_key = 0;
	if( key == 'q' ) q_key = 0;
	if( key == 'p' ) p_key = 0;
	if( key == 'x' ) x_key = 0;
	if( key == 'z' ) z_key = 0;
	
	need_redraw = 1;
	dont_pass_on = 1;
	}


if ( e == FL_MOUSEWHEEL )
	{
	mousewheel = Fl::event_dy();
	
    if( mousewheel >= 1 ) vobj[ 0 ].theta += 0.05;
    if( mousewheel <= -1 ) vobj[ 0 ].theta -= 0.05;
   
	need_redraw = 1;
	dont_pass_on = 1;
	}
	
if ( need_redraw ) redraw();

if( dont_pass_on ) return 1;

return Fl_Double_Window::handle(e);
}


//----------------------------------------------------------














//----------------------------------------------------------







dble_wnd::dble_wnd( int xx, int yy, int wid, int hei, const char *label ) : Fl_Double_Window( xx, yy, wid ,hei, label )
{
dble_wnd_verbose = 0;
maximize_boundary_x = 5;                //assume this window's title bar and borders, see dble_wnd::resize()
maximize_boundary_y = 20;
maximize_boundary_w = 10;
maximize_boundary_h = 25;

}







dble_wnd::~dble_wnd()
{

}












//try to determine if window maximize was pressed, store previous size so we can save final un-maximized values,
//makes assumption on when a window has been maximized using window title bar, border widths and actual screen resolution,
//you need to change maximize_boundary_x, maximize_boundary_y, maximize_boundary_w, maximize_boundary_h, if your window title bar/borders are different

void dble_wnd::resize( int xx, int yy, int ww, int hh )
{

//assuming screen: 0, 0, 1920, 1080
//got this resize when maximize button was pressed: 5, y: 20, w: 1910, h: 1055
if( dble_wnd_verbose ) printf( "dble_wnd::resize() - x: %d, y: %d, w: %d, h: %d\n", xx, yy, ww, hh );


int xxx, yyy, www, hhh;

Fl::screen_xywh( xxx, yyy, www, hhh );
if( dble_wnd_verbose ) printf( "dble_wnd::resize() - screen_xywh() is x: %d, y: %d, w: %d, h: %d\n", xxx, yyy, www, hhh );

if( xx >= xxx + maximize_boundary_x )                                 //detect a possible the maximize button press happened
    {
    if( yy >= ( yyy + maximize_boundary_y ) )
        {
        if( ww >= ( www - maximize_boundary_w ) )
            {
            if( hh >= ( hhh - maximize_boundary_h ) )
                {
                if( dble_wnd_verbose ) printf( "dble_wnd::resize() - possibly maximize was pressed, storing original pos/size for later\n" );

                goto probably_maximized;
                }
            }
        }
   
    }


restore_size_x = xx;                    
restore_size_y = yy;
restore_size_w = ww;
restore_size_h = hh;

Fl_Double_Window::resize( xx, yy, ww, hh );

if( dble_wnd_verbose ) printf( "dble_wnd::resize() - actually resizing to x: %d, y: %d, w: %d, h: %d\n", xx, yy, www, hh );


probably_maximized:

return;
}

//----------------------------------------------------------








void clear_csl()
{
if ( tb_csl ==0 ) return;
int len = tb_csl->length();
if( len > 0 ) tb_csl->replace(0,len,"");		//clear text buf if anything there
}




void outcsl(string s)
{

if ( te_csl ==0 ) return;

te_csl->insert( s.c_str() );
te_csl->show_insert_position();

}








 


//console printf(...) like function
//e.g. cslpf( "\nTesing int=%05d, float=%f str=%s\n",z,(float)4.567,"George was Here" );

void cslpf( const char *fmt,... )
{
string s1;

int buf_size =  10 * 1024 * 1024;
char* buf = new char[ buf_size ];				//create a data buffer


if( buf == 0 )
	{
	printf("cslpf() - no memory allocated!!!!!!!!!!!!!!!!!!!!!!!\n");
	return;
	}

string s;
va_list ap;

va_start( ap, fmt );
vsnprintf( buf, buf_size - 1, fmt, ap );
if( my_verbose ) outcsl( buf );


//prevent string growing anywhere near 'max_size()' and causing a std::__throw_length_error(char const*)
unsigned int len = sdebug.length();
unsigned int max = sdebug.max_size();
unsigned int limit = (double)max / 1.25;

//printf("cslpf() - culling %d\n", (int)limit );

if( len > limit )
	{
	s1 = sdebug.substr( max - limit, len - ( max - limit ) );	//cut off begining section of str
	sdebug = s1;
	printf("cslpf() - culling begining of str to avoid hitting string::max_size()\n");
	}
sdebug += buf;								//for diag

va_end(ap);

delete buf;

}























void cb_bt_clear(Fl_Widget *, void *)
{
clear_csl();
}













void cb_btRunAnyway(Fl_Widget *w, void* v)
{
Fl_Widget* wnd=(Fl_Widget*)w;
Fl_Window *win;
win=(Fl_Window*)wnd->parent();

iAppExistDontRun = 0;
win->~Fl_Window();
}






void cb_btDontRunExit(Fl_Widget* w, void* v)
{
Fl_Widget* wnd=(Fl_Widget*)w;
Fl_Window *win;
win=(Fl_Window*)wnd->parent();

win->~Fl_Window();
}









//linux code
#ifndef compile_for_windows 

//gets its ID, -- fixed memory leak using XFetchName (used XFree) 01-10-10
int FindWindowID(string csName,Display *display,Window &wid)
{
Window root, parent, *children;
unsigned int numWindows = 0;
int iRet=0;

//*display = XOpenDisplay(NULL);
//gDisp = XOpenDisplay(NULL);

//if(cnShowFindResults) printf("\nDispIn %x\n",display);

XQueryTree(gDisp, RootWindow(gDisp,0), &root, &parent, &children, &numWindows);

int i = 0;
for(i=0; i < numWindows ; i++)
	{
//	char *name;
	Window root2, parent2, *children2;
//	XFetchName(display, children[i], &name);

	
	unsigned int numWindows2 = 0;

//	if(cnShowFindResults) if(name) printf("Window name: %s\n", name);

	XQueryTree(display, children[i], &root2, &parent2, &children2, &numWindows2);
	for(int j=0; j < numWindows2 ; j++)
		{
		char *name;
		XFetchName(display, children2[j], &name);

		
//		unsigned int numWindows2 = 0;
//		Window root2, parent2, *children2;
//		XQueryTree(display, RootWindow(display,0), children[i], &parent2, &children2, &numWindows2);
		 
		if(name) 
			{
//		if(cnShowFindResults) printf("    Window2 name: %s  Id=%x\n", name2,children2[j]);

			if(strcmp(csName.c_str(),name)==0)
				{
//				if(cnShowFindResults) printf("Found It................\n");
//				XMoveWindow(display, children2[j], -100, -100);
//				XMoveWindow(display, children2[j], -100, -100);
//				XMoveWindow(display, children2[j], -100, -100);
//				XResizeWindow(display, children2[j], 1088, 612+22);
//				XMoveWindow(*display, children2[j], 1100, 22);
				wid=children2[j];
//				gw=children2[j];
				iRet=1;
//				return 0;
//				if(iRet)
//					{
//					printf("\n\nTrying to Move %x  %x\n\n",gDisp, gw);
//					XMoveWindow(gDisp, gw, 700, 22);
//					return 1;
//					}
				}
			XFree(name);
			}
		}
	 if(children2) XFree(children2);
	}

if(children) XFree(children);
return  iRet;
}

#endif




void BringWindowToFront(string csTitle)
{

//linux code
#ifndef compile_for_windows
Window wid;
if(FindWindowID(csTitle,gDisp,wid))
	{
	XUnmapWindow(gDisp, wid);
	XMapWindow(gDisp, wid);
	XFlush(gDisp);
	}
#endif


//windows code
#ifdef compile_for_windows
HWND hWnd;
//csAppName.LoadString(IDS_APPNAME);

hWnd = FindWindow( 0, cnsAppName );

if( hWnd )
	{
	::BringWindowToTop( hWnd );
//	::SetForegroundWindow( hWnd );
//	::PostMessage(hWnd,WM_MAXIMIZE,0,0);
	::ShowWindow( hWnd, SW_RESTORE );
	}
#endif

}







//linux code
#ifndef compile_for_windows 

//test if window with csAppName already exists, if so create inital main window with
//two options to either run app, or to exit.
//if no window with same name exists returns 0
//if 'exit' option chosen, exit(0) is called and no return happens
//if 'run anyway' option is chosen, returns 1
int CheckInstanceExists(string csAppName)
{
string csTmp;

gDisp = XOpenDisplay(NULL);

Window wid;


if(FindWindowID(csAppName,gDisp,wid))		//a window with same name exists?
	{
	BringWindowToFront( csAppName );

	XCloseDisplay(gDisp);		//added this to see if valgrind showed improvement - it didn't

	Fl_Window *wndInitial = new Fl_Window(50,50,330,90);
	wndInitial->label("Possible Instance Already Running");
	
	Fl_Box *bxHeading = new Fl_Box(10,10,200, 15, "Another Window With Same Name Was Found.");
	bxHeading->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);

	strpf(csTmp,"App Name: '%s'",csAppName.c_str()); 
	Fl_Box *bxAppName = new Fl_Box(10,30,200, 15,csTmp.c_str());
	bxAppName->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);

	Fl_Button *btRunAnyway = new Fl_Button(25,55,130,25,"Run App Anyway");
	btRunAnyway->labelsize(12);
	btRunAnyway->callback(cb_btRunAnyway,0);

	Fl_Button *btDontRunExit = new Fl_Button(btRunAnyway->x()+btRunAnyway->w()+15,55,130,25,"Don't Run App, Exit");
	btDontRunExit->labelsize(12);
	btDontRunExit->callback(cb_btDontRunExit,0);

	wndInitial->end();
	wndInitial->show();
	
	Fl::run();

	return iAppExistDontRun;
	}
else iAppExistDontRun=0;

XCloseDisplay(gDisp);		//added this to see if valgrind showed improvement - it didn't

return iAppExistDontRun;

}

#endif














//windows code
#ifdef compile_for_windows 

//test if window with csAppName already exists, if so create inital main window with
//two options to either run app, or to exit.
//if no window with same name exists returns 0
//if 'exit' option chosen, exit(0) is called and no return happens
//if 'run anyway' option is chosen, returns 1
int CheckInstanceExists( string csAppName )
{
string csTmp;

HWND hWnd;
//csAppName.LoadString(IDS_APPNAME);

hWnd = FindWindow( 0, csAppName.c_str() );

if( hWnd )
	{
	BringWindowToFront( csAppName );
//	::BringWindowToTop( hWnd );
//::SetForegroundWindow( hWnd );
//::PostMessage(hWnd,WM_MAXIMIZE,0,0);
//	::ShowWindow( hWnd, SW_RESTORE );
Sleep(1000);

	Fl_Window *wndInitial = new Fl_Window(50,50,330,90);
	wndInitial->label("Possible Instance Already Running");
	
	Fl_Box *bxHeading = new Fl_Box(10,10,200, 15, "Another Window With Same Name Was Found.");
	bxHeading->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);

	strpf(csTmp,"App Name: '%s'",csAppName.c_str()); 
	Fl_Box *bxAppName = new Fl_Box(10,30,200, 15,csTmp.c_str());
	bxAppName->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);

	Fl_Button *btRunAnyway = new Fl_Button(25,55,130,25,"Run App Anyway");
	btRunAnyway->labelsize(12);
	btRunAnyway->callback(cb_btRunAnyway,0);

	Fl_Button *btDontRunExit = new Fl_Button(btRunAnyway->x()+btRunAnyway->w()+15,55,130,25,"Don't Run App, Exit");
	btDontRunExit->labelsize(12);
	btDontRunExit->callback(cb_btDontRunExit,0);

	wndInitial->end();
	wndInitial->show();
	wndInitial->hide();
	wndInitial->show();
	Fl::run();

	return iAppExistDontRun;
	}
else iAppExistDontRun = 0;

return iAppExistDontRun;
}

#endif















/*
//find this apps path and prefix it to supplied filename
void MakeIniPathFilename(string csFilename,string &csPathFilename)
{

//linux code
#ifndef compile_for_windows

//get the actual path this app lives in
#define MAXPATHLEN 1025   // make this larger if you need to

int length;
char fullpath[MAXPATHLEN];

// /proc/self is a symbolic link to the process-ID subdir
// of /proc, e.g. /proc/4323 when the pid of the process
// of this program is 4323.
//
// Inside /proc/<pid> there is a symbolic link to the
// executable that is running as this <pid>.  This symbolic
// link is called "exe".
//
// So if we read the path where the symlink /proc/self/exe
// points to we have the full path of the executable.


length = readlink("/proc/self/exe", fullpath, sizeof(fullpath));
 
// Catch some errors:
if (length < 0)
	{
 
	syslog(LOG_ALERT,"Error resolving symlink /proc/self/exe.\n");
	fprintf(stderr, "Error resolving symlink /proc/self/exe.\n");
	exit(0);
	}

if (length >= MAXPATHLEN)
	{
	syslog(LOG_ALERT, "Path too long. Truncated.\n");
	fprintf(stderr, "Path too long. Truncated.\n");
	exit(0);
	}

//I don't know why, but the string this readlink() function 
//returns is appended with a '@'

fullpath[length] = '\0';      // Strip '@' off the end


//printf("Full path is: %s\n", fullpath);
//syslog(LOG_ALERT,"Full path is: %s\n", fullpath);

string csTmp;

csTmp=fullpath;
size_t found=csTmp.rfind("/");
if (found!=string::npos) csPathFilename=csTmp.substr(0,found);
//syslog(LOG_ALERT,"Path only is: %s\n", csPathFilename.c_str());

csPathFilename+='/';
csPathFilename+=csFilename;	
#endif



//windows code
#ifdef compile_for_windows 
csTmp = GetCommandLine();

size_t found = csTmp.rfind( dir_seperator );
if ( found != string::npos ) csPathFilename  =csTmp.substr( 0, found );

csPathFilename += dir_seperator;
csPathFilename+=csFilename;	


csPathFilename = csFilename;
#endif

}

*/






//extract command line details from windud

//from GCCmdLine::GetAppName()
//eg: prog.com											//no path
//eg. "c:\dos\edit prog.com"							//with path\prog in quotes
//eg. "c:\dos\edit prog.com" c:\dos\junk.txt			//with path\prog in quotes and path\file
//eg. c\dos\edit.com /p /c		(as in screen-savers)	//path\prog and params no quotes

void get_cmd_line( string cmdline, string &path, string &appname, vector<string> &vparams )
{
string stmp;
char ch;

path = "";
appname = "";
vparams.clear();
bool in_str = 0;
bool in_quote = 0;
bool beyond_app_name = 0;
//bool app_name_in_quotes = 0;

//cmdline = "c:/dos/edit prog.com";
//cmdline = "\"c:/dos/edit prog.com\" hello 123";
//cmdline = "c:/dos/edit.com hello 123";

int len =  cmdline.length();

if( len == 0 ) return;

for( int i = 0; i < len; i++ )
	{
	ch = cmdline[ i ];
	
	if( ch == '\"' )									//quote?
		{
		if( in_quote )
			{
			in_quote = 0;								//if here found closing quote
			goto got_param;
			}
		else{
			in_quote = 1;
			}
		}
	else{
		if( ch == ' ' )									//space?
			{
			if( !in_quote )				//if not in quote and space must be end of param
				{
				if( in_str ) goto got_param;
				}
			}
		else{
			in_str = 1;
			}

		if( in_str ) stmp += ch;
		}

	continue;

	got_param:

	in_str = 0;
	if ( beyond_app_name == 0 )					//store where approp
		{
		path = stmp;
		beyond_app_name = 1;
		}
	else{
		//store if not just a space
		if( stmp.compare( " " ) != 0 ) vparams.push_back( stmp );
		}

	stmp = "";
	}


//if here end of params reached, store where approp
if ( beyond_app_name == 0 )
	{
	path = stmp;
	}
else{
	vparams.push_back( stmp );
	}




appname = path;

len = path.length();
if( len == 0 ) return;

int pos = path.rfind( dir_seperator );

if( pos == string::npos )					//no directory path found?
	{
	path = "";	
	return;
	}

if( ( pos + 1 ) < len ) appname = path.substr( pos + 1,  pos + 1 - len );	//extract appname
path = path.substr( 0,  pos );								//extract path


//windows code
#ifdef compile_for_windows 
#endif

}









//find this apps path
void get_app_path( string &path_out )
{
string s1, path;
mystr m1;


//linux code
#ifndef compile_for_windows

//get the actual path this app lives in
#define MAXPATHLEN 1025   // make this larger if you need to

int length;
char fullpath[MAXPATHLEN];

// /proc/self is a symbolic link to the process-ID subdir
// of /proc, e.g. /proc/4323 when the pid of the process
// of this program is 4323.
//
// Inside /proc/<pid> there is a symbolic link to the
// executable that is running as this <pid>.  This symbolic
// link is called "exe".
//
// So if we read the path where the symlink /proc/self/exe
// points to we have the full path of the executable.


length = readlink("/proc/self/exe", fullpath, sizeof(fullpath));
 
// Catch some errors:
if (length < 0)
	{
	syslog(LOG_ALERT,"Error resolving symlink /proc/self/exe.\n");
	fprintf(stderr, "Error resolving symlink /proc/self/exe.\n");
	exit(0);
	}

if (length >= MAXPATHLEN)
	{
	syslog(LOG_ALERT, "Path too long. Truncated.\n");
	fprintf(stderr, "Path too long. Truncated.\n");
	exit(0);
	}

//I don't know why, but the string this readlink() function 
//returns is appended with a '@'

fullpath[length] = '\0';      // Strip '@' off the end


//printf("Full path is: %s\n", fullpath);
//syslog(LOG_ALERT,"Full path is: %s\n", fullpath);

path = fullpath;
size_t found = path.rfind( "/" );
if ( found != string::npos ) path_out = path.substr( 0, found );
//syslog(LOG_ALERT,"Path only is: %s\n", csPathFilename.c_str());

#endif



//windows code
#ifdef compile_for_windows 
UINT i,uiLen;                    //eg. c\dos\edit.com /p /c		(as in screen-savers)
bool bQuotes;
string csCmdLineStr;


//----------------------------
//from GCCmdLine::GetAppName()
//eg: prog.com											//no path
//eg. "c:\dos\edit prog.com"							//with path\prog in quotes
//eg. "c:\dos\edit prog.com" c:\dos\junk.txt			//with path\prog in quotes and path\file
//eg. c\dos\edit.com /p /c		(as in screen-savers)	//path\prog and params no quotes
csCmdLineStr = GetCommandLine();

//csCmdLineStr = "skeleton.exe abc";
//printf("csCmdLineStr= '%s'\n", csCmdLineStr.c_str() );


string appname;
vector<string> vparams;
get_cmd_line( csCmdLineStr, path_out, appname, vparams  );

#endif


printf( "csPathFilename= %s\n", path_out.c_str() );

}




















void LoadSettings(string csIniFilename)
{
string csTmp;
int x,y,cx,cy;

GCProfile p(csIniFilename);
x=p.GetPrivateProfileLONG("Settings","WinX",100);
y=p.GetPrivateProfileLONG("Settings","WinY",100);
cx=p.GetPrivateProfileLONG("Settings","WinCX",750);
cy=p.GetPrivateProfileLONG("Settings","WinCY",550);

wndMain->position( x , y );	
wndMain->size( cx , cy );	

if( pref_wnd2 != 0 ) pref_wnd2->Load( p );

}









void SaveSettings(string csIniFilename)
{
GCProfile p(csIniFilename);


//p.WritePrivateProfileLONG("Settings","WinX", wndMain->x()-iBorderWidth);
//p.WritePrivateProfileLONG("Settings","WinY", wndMain->y()-iBorderHeight);
//p.WritePrivateProfileLONG("Settings","WinCX", wndMain->w());
//p.WritePrivateProfileLONG("Settings","WinCY", wndMain->h());



//this uses previously saved window sizing value, hopefully grabbed before window was maximized by user see: dble_wnd::resize()
//remove window border offset when saving window pos settings
p.WritePrivateProfileLONG("Settings","WinX", wndMain->restore_size_x - iBorderWidth );
p.WritePrivateProfileLONG("Settings","WinY", wndMain->restore_size_y - iBorderHeight);
p.WritePrivateProfileLONG("Settings","WinCX", wndMain->restore_size_w );
p.WritePrivateProfileLONG("Settings","WinCY", wndMain->restore_size_h );


if( pref_wnd2 != 0 ) pref_wnd2->Save( p );

}




void DoQuit()
{

SaveSettings(csIniFilename);

if( a_wnd != 0 ) delete a_wnd;

printf( "Daisy...Daisy....\n" );
exit(0);
}









void cb_wndmain(Fl_Widget*, void* v)
{
Fl_Window* wnd = (Fl_Window*)v;

//wndMain->iconize();
//wndMain->hide();
//wndMain->show();

//do_quit_via_timer = 1;
DoQuit();

}




void cb_a_wnd(Fl_Widget*, void* v)
{
DoQuit();
}






void cb_btAbout(Fl_Widget *, void *)
{
string s1, st;

Fl_Window *wnd = new Fl_Window(wndMain->x()+20,wndMain->y()+20,500,100);
wnd->label("About");
Fl_Input *teText = new Fl_Input(10,10,wnd->w()-20,wnd->h()-20,"");
teText->type(FL_MULTILINE_OUTPUT);
teText->textsize(12);


strpf( s1, "%s,  %s,  Built: %s\n", cnsAppWndName, cns_version, cns_build_date );
st += s1;

strpf( s1, "\nEnjoy..." );
st += s1;


teText->value(st.c_str());
wnd->end();

#ifndef compile_for_windows
wnd->set_modal();
#endif

wnd->show();

}








void cb_btQuit(Fl_Widget *, void *)
{
DoQuit();
}






void cb_bt_help( Fl_Widget *, void *)
{
mystr m1;
string pathname, s1;
pathname += '"';
pathname += app_path;
pathname += dir_seperator;
pathname += cnsOpenEditor;
pathname += '"';
pathname += " ";
pathname += '"';
pathname += app_path;
pathname += dir_seperator;
pathname += cnsHelpFilename;
pathname += '"';

s1 = pathname;
//m1 = pathname;

//m1.FindReplace( s1, " ", "\\ ", 0 );

printf( "cb_bt_help() - '%s'\n", s1.c_str() );

RunShell( s1 );					//do both shell cmds

tm tt;
string s2, s3, s4, s5, s6, s7;


m1.get_time_now( tt );
m1.make_time_str( tt, s1, s2, s3, s4 );
outcsl( "   Time is: " );
outcsl( s4 );

//void make_date_str( struct tm tn, string &dow, string &dom, string &mon_num, string &mon_name, string &year, string &year_short );

m1.make_date_str( tt, s1, s2, s3, s4, s5, s6 );
strpf( s7, "%s-%s-%s, ", s2.c_str(), s3.c_str(), s6.c_str() );
outcsl( "   Date is: " );
outcsl( s7 );

strpf( s7, "   %s %s-%s-%s", s1.c_str(), s2.c_str(), s4.c_str(), s5.c_str() );
outcsl( s7 );

}




void cb_bt_new_game(Fl_Widget*w, void* v)
{
a_wnd->new_game();
}






void cb_pref(Fl_Widget*w, void* v)
{
if(pref_wnd2) pref_wnd2->Show(1);
}




//this is the callback that is called by buttons that specified is in - 
//definition ....pref_wnd->sc.cb=(void*)cb_user;
//'*o' is the PrefWnd* ptr 
//'row' is the row the control lives on, 0 is first row
//'ctrl' is the number of the controlon that row, 0 is first control
void cb_user2(void *o, int row,int ctrl)
{

printf("cb_user2() - Ping by Control on Row=%d at control count Ctrl=%d on this row.\n",row,ctrl);
}





void make_pref2_wnd()
{
sControl sc;

if(pref_wnd2==0)
	{
	pref_wnd2 = new PrefWnd( a_wnd->x()+50, a_wnd->y()+50, 300, 160,"Preferences","Settings","PrefWnd2Pos");
	}
else{
	pref_wnd2->Show(1);
	return;
	}


// -- dont need to do the below manual default load as "ClearToDefCtrl()" does this for you --

pref_wnd2->ClearToDefCtrl();					//this will clear sc struct to safe default values

pref_wnd2->sc.type=cnNone;						//blank gap from top of window
pref_wnd2->sc.x=105;
pref_wnd2->sc.y=0;
pref_wnd2->sc.w=150;
pref_wnd2->sc.h=20;
pref_wnd2->sc.label="";
pref_wnd2->sc.label_type=FL_ALIGN_CENTER;
pref_wnd2->sc.tooltip="";						//tool tip
pref_wnd2->sc.options="";	//menu button drop down options
pref_wnd2->sc.labelfont=-1;					//-1 means use fltk default
pref_wnd2->sc.labelsize=-1;					//-1 means use fltk default
pref_wnd2->sc.textfont=fl_font();
pref_wnd2->sc.textsize=fl_size();
pref_wnd2->sc.section="";
pref_wnd2->sc.key="";
pref_wnd2->sc.keypostfix=-1;					//ini file Key post fix
pref_wnd2->sc.def="";							//default to use if ini value not avail
pref_wnd2->sc.iretval=(int*)-1;					//address of int to be modified, -1 means none
pref_wnd2->sc.sretval=(string*)-1;				//address of string to be modified, -1 means none
pref_wnd2->sc.uniq_id = 1;                      //allows identification of an actual control, rather that using its row and column values, don't use 0xffffffff
pref_wnd2->sc.cb=0;								//address of a callback if any, 0 means none


pref_wnd2->AddControl();
pref_wnd2->CreateRow(10);			//specify optional row height



/*
pref_wnd2->ClearToDefCtrl();			//this will clear sc struct to safe default values

pref_wnd2->sc.type=cnMenuChoicePref;
pref_wnd2->sc.x=120;
pref_wnd2->sc.y=0;
pref_wnd2->sc.w=150;
pref_wnd2->sc.h=20;
pref_wnd2->sc.label="Initial Execution:";
pref_wnd2->sc.label_type = FL_NORMAL_LABEL;      //label effects such a FL_EMBOSSED_LABEL, FL_ENGRAVED_LABEL, FL_SHADOW_LABEL
pref_wnd2->sc.label_align = FL_ALIGN_LEFT;
pref_wnd2->sc.tooltip="";						//tool tip
pref_wnd2->sc.options="&None,&Step into main(...),&Run";	//menu button drop down options
pref_wnd2->sc.labelfont=-1;						//-1 means use fltk default
pref_wnd2->sc.labelsize=-1;						//-1 means use fltk default
pref_wnd2->sc.textfont=-1;//fl_font();
pref_wnd2->sc.textsize=-1;//fl_size();
pref_wnd2->sc.section="MyPref";
pref_wnd2->sc.key="InitExec";
pref_wnd2->sc.keypostfix=-1;					//ini file Key post fix
pref_wnd2->sc.def="0";							//default to use if ini value not avail
pref_wnd2->sc.iretval=&gi0;						//address of int to be modified, -1 means none
pref_wnd2->sc.dretval=(double*)-1;				//address of double to be modified, -1 means none
pref_wnd2->sc.sretval=(string*)-1;				//address of string to be modified, -1 means none
pref_wnd2->sc.cb = cb_user2;					//address of a callback if any, 0 means none
pref_wnd2->sc.dynamic = 1;						//allow immediate dynamic change of user var
pref_wnd2->sc.uniq_id = 0;                      //allows identification of an actual control, rather that using its row and column values, don't use 0xffffffff
pref_wnd2->AddControl();

pref_wnd2->CreateRow(25);			//specify optional row height
*/

pref_wnd2->ClearToDefCtrl();			//this will clear sc struct to safe default values

pref_wnd2->sc.type=cnCheckPref;
pref_wnd2->sc.x=20;
pref_wnd2->sc.y=0;
pref_wnd2->sc.w=20;
pref_wnd2->sc.h=20;
pref_wnd2->sc.label="Meteors";
pref_wnd2->sc.label_type = FL_NORMAL_LABEL;      //label effects such a FL_EMBOSSED_LABEL, FL_ENGRAVED_LABEL, FL_SHADOW_LABEL
pref_wnd2->sc.label_align = FL_ALIGN_RIGHT;
pref_wnd2->sc.tooltip="Always have meteors";	//tool tip
pref_wnd2->sc.options="";						//menu button drop down options
pref_wnd2->sc.labelfont=-1;						//-1 means use fltk default
pref_wnd2->sc.labelsize=11;						//-1 means use fltk default
pref_wnd2->sc.textfont=4;
pref_wnd2->sc.textsize=11;
pref_wnd2->sc.section="Preferences";
pref_wnd2->sc.key="pref_always_have_meteors";
pref_wnd2->sc.keypostfix=-1;					//ini file Key post fix
pref_wnd2->sc.def="0";							//default to use if ini value not avail
pref_wnd2->sc.iretval=&pref_always_have_meteors;						//address of int to be modified, -1 means none
pref_wnd2->sc.dretval=(double*)-1;				//address of double to be modified, -1 means none
pref_wnd2->sc.sretval=(string*)-1;				//address of string to be modified, -1 means none
pref_wnd2->sc.cb = cb_user2;					//address of a callback if any, 0 means none
pref_wnd2->sc.dynamic = 1;						//allow immediate dynamic change of user var
pref_wnd2->sc.uniq_id = 1;                      //allows identification of an actual control, rather that using its row and column values, don't use 0xffffffff
pref_wnd2->AddControl();

pref_wnd2->CreateRow( 25 );					//specify optional row height



pref_wnd2->ClearToDefCtrl();			//this will clear sc struct to safe default values

pref_wnd2->sc.type=cnCheckPref;
pref_wnd2->sc.x=20;
pref_wnd2->sc.y=0;
pref_wnd2->sc.w=20;
pref_wnd2->sc.h=20;
pref_wnd2->sc.label="Magentic Mines";
pref_wnd2->sc.label_type = FL_NORMAL_LABEL;      //label effects such a FL_EMBOSSED_LABEL, FL_ENGRAVED_LABEL, FL_SHADOW_LABEL
pref_wnd2->sc.label_align = FL_ALIGN_RIGHT;
pref_wnd2->sc.tooltip="Always have magnetic mines";	//tool tip
pref_wnd2->sc.options="";						//menu button drop down options
pref_wnd2->sc.labelfont=-1;						//-1 means use fltk default
pref_wnd2->sc.labelsize=11;						//-1 means use fltk default
pref_wnd2->sc.textfont=4;
pref_wnd2->sc.textsize=11;
pref_wnd2->sc.section="Preferences";
pref_wnd2->sc.key="pref_always_have_magmines";
pref_wnd2->sc.keypostfix=-1;					//ini file Key post fix
pref_wnd2->sc.def="0";							//default to use if ini value not avail
pref_wnd2->sc.iretval=&pref_always_have_magmines;						//address of int to be modified, -1 means none
pref_wnd2->sc.dretval=(double*)-1;				//address of double to be modified, -1 means none
pref_wnd2->sc.sretval=(string*)-1;				//address of string to be modified, -1 means none
pref_wnd2->sc.cb = cb_user2;					//address of a callback if any, 0 means none
pref_wnd2->sc.dynamic = 1;						//allow immediate dynamic change of user var
pref_wnd2->sc.uniq_id = 1;                      //allows identification of an actual control, rather that using its row and column values, don't use 0xffffffff
pref_wnd2->AddControl();

pref_wnd2->CreateRow( 25 );					//specify optional row height



pref_wnd2->ClearToDefCtrl();			//this will clear sc struct to safe default values

pref_wnd2->sc.type=cnCheckPref;
pref_wnd2->sc.x=20;
pref_wnd2->sc.y=0;
pref_wnd2->sc.w=20;
pref_wnd2->sc.h=20;
pref_wnd2->sc.label="Gravity Objects";
pref_wnd2->sc.label_type = FL_NORMAL_LABEL;      //label effects such a FL_EMBOSSED_LABEL, FL_ENGRAVED_LABEL, FL_SHADOW_LABEL
pref_wnd2->sc.label_align = FL_ALIGN_RIGHT;
pref_wnd2->sc.tooltip="Always have gravity objects";	//tool tip
pref_wnd2->sc.options="";						//menu button drop down options
pref_wnd2->sc.labelfont=-1;						//-1 means use fltk default
pref_wnd2->sc.labelsize=11;						//-1 means use fltk default
pref_wnd2->sc.textfont=4;
pref_wnd2->sc.textsize=11;
pref_wnd2->sc.section="Preferences";
pref_wnd2->sc.key="pref_always_have_grav_objs";
pref_wnd2->sc.keypostfix=-1;					//ini file Key post fix
pref_wnd2->sc.def="0";							//default to use if ini value not avail
pref_wnd2->sc.iretval=&pref_always_have_grav_objs;						//address of int to be modified, -1 means none
pref_wnd2->sc.dretval=(double*)-1;				//address of double to be modified, -1 means none
pref_wnd2->sc.sretval=(string*)-1;				//address of string to be modified, -1 means none
pref_wnd2->sc.cb = cb_user2;					//address of a callback if any, 0 means none
pref_wnd2->sc.dynamic = 1;						//allow immediate dynamic change of user var
pref_wnd2->sc.uniq_id = 1;                      //allows identification of an actual control, rather that using its row and column values, don't use 0xffffffff
pref_wnd2->AddControl();

pref_wnd2->CreateRow( 25 );					//specify optional row height



pref_wnd2->ClearToDefCtrl();			//this will clear sc struct to safe default values

pref_wnd2->sc.type=cnCheckPref;
pref_wnd2->sc.x=20;
pref_wnd2->sc.y=0;
pref_wnd2->sc.w=20;
pref_wnd2->sc.h=20;
pref_wnd2->sc.label="FPS";
pref_wnd2->sc.label_type = FL_NORMAL_LABEL;      //label effects such a FL_EMBOSSED_LABEL, FL_ENGRAVED_LABEL, FL_SHADOW_LABEL
pref_wnd2->sc.label_align = FL_ALIGN_RIGHT;
pref_wnd2->sc.tooltip="Show frames per second";	//tool tip
pref_wnd2->sc.options="";						//menu button drop down options
pref_wnd2->sc.labelfont=-1;						//-1 means use fltk default
pref_wnd2->sc.labelsize=11;						//-1 means use fltk default
pref_wnd2->sc.textfont=4;
pref_wnd2->sc.textsize=11;
pref_wnd2->sc.section="Preferences";
pref_wnd2->sc.key="pref_frames_per_second";
pref_wnd2->sc.keypostfix=-1;					//ini file Key post fix
pref_wnd2->sc.def="0";							//default to use if ini value not avail
pref_wnd2->sc.iretval=&pref_frames_per_second;						//address of int to be modified, -1 means none
pref_wnd2->sc.dretval=(double*)-1;				//address of double to be modified, -1 means none
pref_wnd2->sc.sretval=(string*)-1;				//address of string to be modified, -1 means none
pref_wnd2->sc.cb = cb_user2;					//address of a callback if any, 0 means none
pref_wnd2->sc.dynamic = 1;						//allow immediate dynamic change of user var
pref_wnd2->sc.uniq_id = 1;                      //allows identification of an actual control, rather that using its row and column values, don't use 0xffffffff
pref_wnd2->AddControl();

pref_wnd2->CreateRow( 25 );		

/*
pref_wnd2->ClearToDefCtrl();					//this will clear sc struct to safe default values

pref_wnd2->sc.type=cnCheckPref;
pref_wnd2->sc.x=0;
pref_wnd2->sc.y=0;
pref_wnd2->sc.w=300;
pref_wnd2->sc.h=20;
pref_wnd2->sc.label="Prompt to Reload, if Executable Datestamp Changes";
pref_wnd2->sc.label_type = FL_NORMAL_LABEL;      //label effects such a FL_EMBOSSED_LABEL, FL_ENGRAVED_LABEL, FL_SHADOW_LABEL
pref_wnd2->sc.label_align = FL_ALIGN_LEFT;
pref_wnd2->sc.tooltip="";						//tool tip
pref_wnd2->sc.options="";						//menu button drop down options
pref_wnd2->sc.labelfont=-1;						//-1 means use fltk default
pref_wnd2->sc.labelsize=-1;						//-1 means use fltk default
pref_wnd2->sc.textfont=-1;
pref_wnd2->sc.textsize=-1;
pref_wnd2->sc.section="MyPref";
pref_wnd2->sc.key="DateStampReload";
pref_wnd2->sc.keypostfix=-1;					//ini file Key post fix
pref_wnd2->sc.def="0";							//default to use if ini value not avail
pref_wnd2->sc.iretval=&gi2;						//address of int to be modified, -1 means none
pref_wnd2->sc.dretval=(double*)-1;				//address of double to be modified, -1 means none
pref_wnd2->sc.sretval=(string*)-1;				//address of string to be modified, -1 means none
pref_wnd2->sc.cb = cb_user2;					//address of a callback if any, 0 means none
pref_wnd2->sc.dynamic = 1;						//allow immediate dynamic change of user var
pref_wnd2->sc.uniq_id = 2;                      //allows identification of an actual control, rather that using its row and column values, don't use 0xffffffff
pref_wnd2->AddControl();

pref_wnd2->CreateRow(25);						//specify optional row height


pref_wnd2->ClearToDefCtrl();					//this will clear sc struct to safe default values

pref_wnd2->sc.type=cnInputIntPref;
pref_wnd2->sc.x=190;
pref_wnd2->sc.y=0;
pref_wnd2->sc.w=150;
pref_wnd2->sc.h=20;
pref_wnd2->sc.label="Enter a Integer Num:";
pref_wnd2->sc.label_type = FL_NORMAL_LABEL;      //label effects such a FL_EMBOSSED_LABEL, FL_ENGRAVED_LABEL, FL_SHADOW_LABEL
pref_wnd2->sc.label_align = FL_ALIGN_LEFT;
pref_wnd2->sc.tooltip="";						//tool tip
pref_wnd2->sc.options="";						//menu button drop down options
pref_wnd2->sc.labelfont=-1;						//-1 means use fltk default
pref_wnd2->sc.labelsize=-1;						//-1 means use fltk default
pref_wnd2->sc.textfont=-1;
pref_wnd2->sc.textsize=-1;
pref_wnd2->sc.section="MyPref";
pref_wnd2->sc.key="Integer1";
pref_wnd2->sc.keypostfix=-1;					//ini file Key post fix
pref_wnd2->sc.def="0";							//default to use if ini value not avail
pref_wnd2->sc.iretval=&gi3;						//address of int to be modified, -1 means none
pref_wnd2->sc.dretval=(double*)-1;				//address of double to be modified, -1 means none
pref_wnd2->sc.sretval=(string*)-1;				//address of string to be modified, -1 means none
pref_wnd2->sc.cb = cb_user2;					//address of a callback if any, 0 means none
pref_wnd2->sc.dynamic = 1;						//allow immediate dynamic change of user var
pref_wnd2->sc.uniq_id = 3;                      //allows identification of an actual control, rather that using its row and column values, don't use 0xffffffff
pref_wnd2->AddControl();

pref_wnd2->CreateRow(25);						//specify optional row height


pref_wnd2->ClearToDefCtrl();					//this will clear sc struct to safe default values

pref_wnd2->sc.type=cnInputDoublePref;
pref_wnd2->sc.x=200;
pref_wnd2->sc.y=0;
pref_wnd2->sc.w=150;
pref_wnd2->sc.h=20;
pref_wnd2->sc.label="Enter a Floating Point Num:";
pref_wnd2->sc.label_type = FL_NORMAL_LABEL;      //label effects such a FL_EMBOSSED_LABEL, FL_ENGRAVED_LABEL, FL_SHADOW_LABEL
pref_wnd2->sc.label_align = FL_ALIGN_LEFT;
pref_wnd2->sc.tooltip="";						//tool tip
pref_wnd2->sc.options="";						//menu button drop down options
pref_wnd2->sc.labelfont=-1;						//-1 means use fltk default
pref_wnd2->sc.labelsize=-1;						//-1 means use fltk default
pref_wnd2->sc.textfont=-1;
pref_wnd2->sc.textsize=-1;
pref_wnd2->sc.section="MyPref";
pref_wnd2->sc.key="Float1";
pref_wnd2->sc.keypostfix=-1;					//ini file Key post fix
pref_wnd2->sc.def="0";							//default to use if ini value not avail
pref_wnd2->sc.iretval=(int*)-1;					//address of int to be modified, -1 means none
pref_wnd2->sc.dretval=&gd0;						//address of double to be modified, -1 means none
pref_wnd2->sc.sretval=(string*)-1;				//address of string to be modified, -1 means none
pref_wnd2->sc.cb = cb_user2;					//address of a callback if any, 0 means none
pref_wnd2->sc.dynamic = 1;						//allow immediate dynamic change of user var
pref_wnd2->sc.uniq_id = 4;                      //allows identification of an actual control, rather that using its row and column values, don't use 0xffffffff
pref_wnd2->AddControl();

pref_wnd2->CreateRow(25);						//specify optional row height


pref_wnd2->ClearToDefCtrl();					//this will clear sc struct to safe default values

pref_wnd2->sc.type=cnInputHexPref;
pref_wnd2->sc.x=190;
pref_wnd2->sc.y=0;
pref_wnd2->sc.w=150;
pref_wnd2->sc.h=20;
pref_wnd2->sc.label="Enter a Hex Num:";
pref_wnd2->sc.label_type = FL_NORMAL_LABEL;      //label effects such a FL_EMBOSSED_LABEL, FL_ENGRAVED_LABEL, FL_SHADOW_LABEL
pref_wnd2->sc.label_align = FL_ALIGN_LEFT;
pref_wnd2->sc.tooltip="";						//tool tip
pref_wnd2->sc.options="";						//menu button drop down options
pref_wnd2->sc.labelfont=-1;						//-1 means use fltk default
pref_wnd2->sc.labelsize=-1;						//-1 means use fltk default
pref_wnd2->sc.textfont=-1;
pref_wnd2->sc.textsize=-1;
pref_wnd2->sc.section="MyPref";
pref_wnd2->sc.key="Hex1";
pref_wnd2->sc.keypostfix=-1;					//ini file Key post fix
pref_wnd2->sc.def="0";							//default to use if ini value not avail
pref_wnd2->sc.iretval=&gi4;						//address of int to be modified, -1 means none
pref_wnd2->sc.dretval=(double*)-1;				//address of double to be modified, -1 means none
pref_wnd2->sc.sretval=(string*)-1;				//address of string to be modified, -1 means none
pref_wnd2->sc.cb = cb_user2;					//address of a callback if any, 0 means none
pref_wnd2->sc.dynamic = 1;						//allow immediate dynamic change of user var
pref_wnd2->sc.uniq_id = 5;                      //allows identification of an actual control, rather that using its row and column values, don't use 0xffffffff
pref_wnd2->AddControl();

pref_wnd2->CreateRow(25);						//specify optional row height



pref_wnd2->ClearToDefCtrl();					//this will clear sc struct to safe default values

pref_wnd2->sc.type=cnInputPref;
pref_wnd2->sc.x=100;
pref_wnd2->sc.y=0;
pref_wnd2->sc.w=570;
pref_wnd2->sc.h=25;
pref_wnd2->sc.label="FavEditor:";
pref_wnd2->sc.label_type = FL_NORMAL_LABEL;      //label effects such a FL_EMBOSSED_LABEL, FL_ENGRAVED_LABEL, FL_SHADOW_LABEL
pref_wnd2->sc.label_align = FL_ALIGN_LEFT;
pref_wnd2->sc.tooltip="Enter Appname of your favorite text editor"; //tool tip
pref_wnd2->sc.options="";						//menu button drop down options
pref_wnd2->sc.labelfont=-1;						//-1 means use fltk default
pref_wnd2->sc.labelsize=-1;						//-1 means use fltk default
pref_wnd2->sc.textfont=4;
pref_wnd2->sc.textsize=12;
pref_wnd2->sc.section="Pref";
pref_wnd2->sc.key="FavEditor";
pref_wnd2->sc.keypostfix=-1;						//ini file Key post fix
pref_wnd2->sc.def="";							//default to use if ini value not avail
pref_wnd2->sc.iretval=(int*)-1;			       	//address of int to be modified, -1 means none
pref_wnd2->sc.sretval=&fav_editor;				//address of string to be modified, -1 means none
pref_wnd2->sc.cb = cb_user2;					//address of a callback if any, 0 means none
pref_wnd2->sc.dynamic = 1;						//allow immediate dynamic change of user var
pref_wnd2->sc.uniq_id = 6;                      //allows identification of an actual control, rather that using its row and column values, don't use 0xffffffff
pref_wnd2->AddControl();

pref_wnd2->CreateRow( 25 );							//specify optional row height




pref_wnd2->sc.type=cnGCColColour;
pref_wnd2->sc.x=70;
pref_wnd2->sc.y=2;
pref_wnd2->sc.w=84;
pref_wnd2->sc.h=20;
pref_wnd2->sc.label="Colour1";
pref_wnd2->sc.label_type = FL_NORMAL_LABEL;      //label effects such a FL_EMBOSSED_LABEL, FL_ENGRAVED_LABEL, FL_SHADOW_LABEL
pref_wnd2->sc.label_align = FL_ALIGN_LEFT;
pref_wnd2->sc.tooltip="Set r,g,b colour value"; //tool tip
pref_wnd2->sc.options="";						//menu button drop down options
pref_wnd2->sc.labelfont=-1;						//-1 means use fltk default
pref_wnd2->sc.labelsize=-1;						//-1 means use fltk default
pref_wnd2->sc.textfont=4;
pref_wnd2->sc.textsize=12;
pref_wnd2->sc.section="Pref";
pref_wnd2->sc.key="Colour1";
pref_wnd2->sc.keypostfix=-1;					//ini file Key post fix
pref_wnd2->sc.def="255,255,0";						//default to use if ini value not avail
pref_wnd2->sc.iretval=(int*)-1;			       	//address of int to be modified, -1 means none
pref_wnd2->sc.sretval=&sg_col1;				//address of string to be modified, -1 means none
pref_wnd2->sc.cb = cb_user2;					//address of a callback if any, 0 means none
pref_wnd2->sc.dynamic = 1;						//allow immediate dynamic change of user var
pref_wnd2->sc.uniq_id = 7;                      //allows identification of an actual control, rather that using its row and column values, don't use 0xffffffff
pref_wnd2->AddControl();

pref_wnd2->CreateRow(25);					//specify optional row height


pref_wnd2->sc.type=cnGCColColour;
pref_wnd2->sc.x=70;
pref_wnd2->sc.y=2;
pref_wnd2->sc.w=84;
pref_wnd2->sc.h=20;
pref_wnd2->sc.label="Colour2";
pref_wnd2->sc.label_type = FL_NORMAL_LABEL;      //label effects such a FL_EMBOSSED_LABEL, FL_ENGRAVED_LABEL, FL_SHADOW_LABEL
pref_wnd2->sc.label_align = FL_ALIGN_LEFT;
pref_wnd2->sc.tooltip="Set r,g,b colour value"; //tool tip
pref_wnd2->sc.options="";						//menu button drop down options
pref_wnd2->sc.labelfont=-1;						//-1 means use fltk default
pref_wnd2->sc.labelsize=-1;						//-1 means use fltk default
pref_wnd2->sc.textfont=4;
pref_wnd2->sc.textsize=12;
pref_wnd2->sc.section="Pref";
pref_wnd2->sc.key="Colour2";
pref_wnd2->sc.keypostfix=-1;					//ini file Key post fix
pref_wnd2->sc.def="0,255,255";						//default to use if ini value not avail
pref_wnd2->sc.iretval=(int*)-1;			       	//address of int to be modified, -1 means none
pref_wnd2->sc.sretval=&sg_col2;				//address of string to be modified, -1 means none
pref_wnd2->sc.cb = cb_user2;					//address of a callback if any, 0 means none
pref_wnd2->sc.dynamic = 1;						//allow immediate dynamic change of user var
pref_wnd2->sc.uniq_id = 8;                      //allows identification of an actual control, rather that using its row and column values, don't use 0xffffffff
pref_wnd2->AddControl();

pref_wnd2->CreateRow(25);					//specify optional row height
*/

pref_wnd2->End();								//do end for all windows

}










void cb_timer1(void *)
{
string s1;
mystr m1;


if( a_wnd )
    {
    a_wnd->tick( a_wnd->measured_tframe );						//drive game fwd


	a_wnd->measured_tframe_cumm += a_wnd->measured_tframe;		//keep cummulative tally of each measured time per frame

	a_wnd->measured_tframe_avg = a_wnd->measured_tframe_cumm / (double)a_wnd->measured_tframe_avg_cnt;	//make an avg of tally to create a true indication of time per frame

	//don't let timing numbers get too big as the resolvable frame time average will be lost
	if( a_wnd->measured_tframe_avg_cnt >= 5000 )
		{
		a_wnd->measured_tframe_avg_cnt /= 100;
		a_wnd->measured_tframe_cumm /= 100.0;
		}

	 //slew the timer1 value to keep frame time around desired value held in: a_wnd->tframe
	if( a_wnd->measured_tframe_avg > a_wnd->tframe )
		{
		a_wnd->corrected_timer1_trame -= 0.00001;
		}
	else{
		if( a_wnd->measured_tframe_avg < a_wnd->tframe )
			{
			a_wnd->corrected_timer1_trame += 0.00001;
			}
		}

	a_wnd->frame_factor = a_wnd->measured_tframe_avg / 0.02;				//this will be 1.0 if frame time is 0.02 (50fps), or 0.5 when frame time is 0.01 (100fps), its used to scale values

	Fl::repeat_timeout( a_wnd->corrected_timer1_trame, cb_timer1 );
	}
else{
	Fl::repeat_timeout( 0.02, cb_timer1 );		//should never get here
	}
}













//linux code
#ifndef compile_for_windows

//execute shell cmd
int RunShell(string sin)
{

if ( sin.length() == 0 ) return 0;

//make command to cd working dir to app's dir and execute app (params in "", incase of spaces)
//strpf(csCmd,"cd \"%s\";\"%s\" \"%s\"",csPath.c_str(),sEntry[iEntryNum].csStartFunct.c_str(),csFile.c_str());

pid_t child_pid;

child_pid=fork();		//create a child process	

if(child_pid==-1)		//failed to fork?
	{
	printf("\nRunShell() failed to fork\n");
	return 0;
	}

if(child_pid!=0)		//parent fork? i.e. child pid is avail
	{
	int status;
	printf("\nwaitpid: %d, RunShell start\n",child_pid);	

	while(1)
		{
		waitpid(child_pid,&status,0);		//wait for return val from child so a zombie process is not left in system
		printf("\nwaitpid %d RunShell stop\n",child_pid);
		if(WIFEXITED(status)) break;		//confirm status returned shows the child terminated
		}	
	}
else{					//child fork (0) ?
//	printf("\nRunning Shell: %s\n",csCmd.c_str());
	printf("\nRunShell system cmd started: %s\n",sin.c_str());	
	system(sin.c_str());
	printf("\nRunShell system cmd finished \n");	
	exit(1);
	}
return 1;
}

#endif











//windows code
#ifdef compile_for_windows

//execute shell cmd as a process that can be monitored
int RunShell( string sin )
{
BOOL result;
wstring ws1;

if ( sin.length() == 0 ) return 0;


mystr m1 = sin;

m1.mbcstr_wcstr( ws1 );	//convert utf8 string to windows wchar string array


memset(&processInformation, 0, sizeof(processInformation));


STARTUPINFOW StartInfoW; 							// name structure
memset(&StartInfoW, 0, sizeof(StartInfoW));
StartInfoW.cb = sizeof(StartInfoW);

StartInfoW.wShowWindow = SW_HIDE;

result = CreateProcessW( NULL, (WCHAR*)ws1.c_str(), NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &StartInfoW, &processInformation);

if ( result == 0)
	{
	
	return 0;
	}

return 1;



//bkup_filelist_SEI[ 0 ].cbSize = sizeof( bkup_filelist_SEI[ 0 ] ); 
//bkup_filelist_SEI[ 0 ].lpVerb = "open"; 
//bkup_filelist_SEI[ 0 ].lpFile = sin.c_str(); 
//bkup_filelist_SEI[ 0 ].lpParameters= 0; 
//bkup_filelist_SEI[ 0 ].nShow = SW_HIDE; 
//bkup_filelist_SEI[ 0 ].fMask = SEE_MASK_NOCLOSEPROCESS; 

//ShellExecuteEx( &bkup_filelist_SEI[ 0 ] );     //execute batch file



//WCHAR cmd[] = L"cmd.exe /c pause";
//LPCWSTR dir = L"c:\\";
//STARTUPINFOW si = { 0 };
//si.cb = sizeof(si);
//PROCESS_INFORMATION pi;

//STARTUPINFO StartInfo; 							// name structure
//PROCESS_INFORMATION ProcInfo; 						// name structure
//memset(&ProcInfo, 0, sizeof(ProcInfo));				// Set up memory block
//memset(&StartInfo, 0 , sizeof(StartInfo)); 			// Set up memory block
//StartInfo.cb = sizeof(StartInfo); 					// Set structure size

//int res = CreateProcess( NULL, (char*)sin.c_str(), 0, 0, TRUE, 0, NULL, NULL, &StartInfo, &ProcInfo );

}

#endif











int main(int argc, char **argv)
{
Fl::visual( FL_RGB );


Fl::scheme("plastic");								//optional
string s, fname, dir_sep;
bool add_ini = 1;									//assume need to add ini extension	

//Fl::set_font( FL_HELVETICA, "Helvetica bold");
//Fl::set_font( FL_COURIER, "");
//Fl::set_font( FL_COURIER, "Courier bold italic");

//fl_font( (Fl_Font) FL_COURIER, 12 );

fname = cnsAppName;							//assume ini file will have same name as app
dir_sep = "";								//assume no path specified, so no / or \ (dos\windows)



//test if window with same name found and ask if still to run this -
// will return 1 if user presses 'Don't Run, Exit' button
if( CheckInstanceExists( cnsAppWndName ) ) return 0;

//linux code
#ifndef compile_for_windows
dir_seperator = "/";									//use unix folder directory seperator
#endif


dir_sep = dir_seperator;



//windows code
//attach a command line console, so printf works
#ifdef compile_for_windows
int hCrt;
FILE *hf;

AllocConsole();
hCrt = _open_osfhandle( (long) GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT);
hf = _fdopen( hCrt, "w" );
*stdout = *hf;
setvbuf( stdout, NULL, _IONBF, 0 );
#endif





get_app_path( app_path );				//get app's path



//handle command line params
printf("\n\nGravStar App\n");
    printf("~~~~~~~~~~~~");


printf("\nSpecify no switches to use config with app's filename in app's dir\n");
printf("\nSpecify '--cf ffff' to use config with filename 'ffff'\n");


if( argc == 1 )
	{
	printf( "-- Using app's dir for config storage.\n" );
	}


if( argc == 3 )
	{
	if( strcmp( argv[ 1 ], "--cf" ) == 0 )
		{
		printf( "-- Using specified filename for config storage.\n" );
		fname = argv[ 2 ];
		app_path = "";
		dir_sep = "";								//no directory seperator
		add_ini = 0;								//user spec fname don't add ini ext
		}
	}




if( app_path.length() == 0 )
	{
	csIniFilename = app_path + fname;					//make config file pathfilename
	}
else{
	csIniFilename = app_path + dir_sep + fname;			//make config file pathfilename
	}
if( add_ini ) csIniFilename += ".ini";                  //need ini if no user spcified fname




printf("\n\n-> Config pathfilename is:'%s', this will be used for config recall and saving.\n",csIniFilename.c_str());





wndMain = new dble_wnd( 50, 50, 780, 550 );
wndMain->label( cnsAppWndName );





//calc offset window border, for removal when saving settings
iBorderWidth=wndMain->x();
iBorderHeight=wndMain->y();
wndMain->border(1);
iBorderWidth=wndMain->x()-iBorderWidth;
iBorderHeight=wndMain->y()-iBorderHeight;

//menu bar
meMain = new Fl_Menu_Bar(0, 0, wndMain->w(), 25);
meMain->textsize(12);
meMain->copy(menuitems, wndMain);


Fl_Box *bx1 = new Fl_Box( 0, meMain->h(), 700, 150, "");
bx1->box( FL_BORDER_BOX );


//this box is used by resizable() function, it limits window resizing
//notice buttons that cross this box in a the x dimension are resized in that dimension on user resizing the main wnd
Fl_Box *bx2 = new Fl_Box( 0, meMain->h()+155, 95, 50, " Resizable(Box)");
bx2->box( FL_BORDER_BOX );

Fl_Button *btClear = new Fl_Button( 370,50,55,20,"Clear");
btClear->labelsize(12);
btClear->callback( cb_bt_clear, 0 );


Fl_Button *btHelp = new Fl_Button( 50,80,55,20,"Help");
btHelp->labelsize(12);
btHelp->callback( cb_bt_help, 0 );




tb_csl = new Fl_Text_Buffer;
te_csl = new Fl_Text_Editor( cnGap,  wndMain->h() - cnCslHei - cnGap, wndMain->w() - 2 * cnGap , cnCslHei );
te_csl->buffer( tb_csl );
te_csl->textsize(12);
te_csl->textfont(4);




wndMain->end();
wndMain->callback((Fl_Callback *)cb_wndmain, wndMain);


a_wnd = new mywnd( 100, 100, 1500, 900, "GravStar" );
a_wnd->end();
a_wnd->callback((Fl_Callback *)cb_a_wnd, a_wnd);
a_wnd->resizable( a_wnd );	



wndMain->size_range( 700, 450 );
wndMain->resizable( bx2 );	//note this must be before LoadSettings(), where the window is resized


make_pref2_wnd();


LoadSettings(csIniFilename); 


//wndMain->show(argc, argv);


Fl_RGB_Image *theapp_icon;

//add icon to app window, this will also appear in taskbar
	{

	//Fl_Pixmap *yourapp_16x16_icon = new Fl_Pixmap( x16music01_xpm );
	//Fl_Pixmap *yourapp_16x16_icon = new Fl_Pixmap( audacity16_xpm );
	Fl_Pixmap *theapp_16x16_icon = new Fl_Pixmap( gravstar_xpm );		//see 'gravstar.xpm'

	theapp_icon = new Fl_RGB_Image( theapp_16x16_icon, Fl_Color(0) );
	a_wnd->icon( theapp_icon );
//	a_wnd->xclass("GravStarClass");

	}


menu2 = new Fl_Menu_Bar(0, 0, a_wnd->w(), 25);
menu2->textsize(12);
menu2->copy( menuitems, a_wnd );

a_wnd->add( menu2 );
a_wnd->show();


fl_font( (Fl_Font) font_num, font_size );


//cb_bt_help( 0, 0);

Fl::add_timeout( 0.5, cb_timer1 );		//update controls, post queued messages


fl_message_font( (Fl_Font) font_num, font_size );

 
int iAppRet=Fl::run();

return iAppRet;

}









