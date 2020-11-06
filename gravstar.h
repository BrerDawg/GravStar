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

//gravstar.h
//v1.04			

#define _FILE_OFFSET_BITS 64			//large file handling, must be before all #include...
//#define _LARGE_FILES

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <locale.h>
#include <math.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <wchar.h>
#include <algorithm>

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Enumerations.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_JPEG_Image.H>
#include <FL/Fl_File_Input.H>
#include <FL/Fl_Input_Choice.H>

#include "pref.h"
#include "GCProfile.h"
#include "GCLed.h"

//linux code
#ifndef compile_for_windows

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
//#include <X11/Xaw/Form.h>
//#include <X11/Xaw/Command.h>

#include <dirent.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <syslog.h>		//MakeIniPathFilename(..) needs this
#endif


//windows code
#ifdef compile_for_windows
#include <windows.h>
#include <process.h>
#include <winnls.h>
#include <share.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <fstream>
#include <conio.h>

#define WC_ERR_INVALID_CHARS 0x0080		//missing from gcc's winnls.h
#endif


using namespace std;

#define cnsAppName "gravstar"
#define cnsAppWndName "GravStar"
#define cns_version "v1.04"
#define cnsHelpFilename "help.txt"

#define cns_build_date build_date       //this holds the build date e.g: 2016 Mar 23, obtained automatically by a shell cmd and sets '-Dbuild_date' option in Makefile, you could manually use e.g: -Dbuild_date="\"2016 Mar 23\""


#define cnFontEditor 4
#define cnSizeEditor 15

#define cnGap 2
#define cnCslHei 270


//linux code
#ifndef compile_for_windows
#define cnsOpenEditor "open_editor.sh"
#define LLU "llu"
#endif


//windows code
#ifdef compile_for_windows
#define LLU "I64u"
#define cnsOpenEditor "open_editor.bat"
#endif


#define cn_bytes_per_pixel 3


#define cn_rad2deg 180.0 / M_PI

#define cn_G 6.674e-11

#define max_missles 200
#define max_chaff 5

#define missile_cost 0.2
#define chaff_cost 0.03

#define cn_attract_mode_delaystart 20
#define cn_time_per_missle_launch 45.0

#define cn_keyhit_disable_time 1.8

struct colref
{
int r, g, b;
};

colref col_bkgd =   { 64,    64,    64 	};
colref col_yel = 	{ 255,   255,   0 	};
colref col_red =	{ 255,   0,     0	};
colref col_mag =	{ 255,   0,     255	};
colref col_wht =	{ 255,   255,   255	};






struct st_line_tag
{
double x1;
double y1;
double x2;
double y2;

double dx;								//for explosion
double dy;

unsigned int col;

};





enum en_obj_type_tag
{
en_ot_user_ship,
en_ot_cpu_ship,
en_ot_bul_user,
en_ot_bul_cpu,
en_ot_medic,
en_ot_missile_user,
en_ot_missile_cpu,
en_ot_exhaust,
en_ot_grav_obj,
en_ot_meteor,
en_ot_chaff_user,
en_ot_chaff_cpu,
en_ot_magmine,
en_ot_magmine_fragment,
};




struct st_obj_tag
{
double x;
double y;

double bound_x1;						//hold a rectangle bouning the obj's vertices
double bound_y1;
double bound_x2;
double bound_y2;

int idbg;
double dbgx;
double dbgy;
double dbgx2;
double dbgy2;


unsigned int col;
unsigned int col_temp;					//temp col
int col_temp_frames_total;   			//when reaches zero col_temp no longer used                          
int col_temp_frame_cnt;                 //when non zero col_temp is used instead of col                          

en_obj_type_tag type;
double dv;                              //directionless velocity
double dx;                              //vectored velocity
double dy;
double max_dx;                          //limit dx to this max and min
double max_dy;

int has_thrust;
double thrust;                          //cur thrust
double thrust_inc_per_frame;            //rate thrust can be inc'd per frame
double thrust_max;

double theta;                           //this is the current theta angle 'on its way' to 'theta_req'
double theta_req;                       //this is the required theta
double delta_theta_per_frame;           //this gives the rate of theta change
double delta_spin;						//this is used to give an obj spin


bool finite_life;
double time_to_live;                    //gives objs a finite life
bool mark_to_delete;                    //set this if obj needs to be deleted
double mass;
double accel;                           //directionless acceleration
double accelx;                          //vectored acceleration
double accely;
double drag;
int can_wrap;
int hits;                               //bullet hits
float time_per_bullet;                  //time between bullet firings
float time_bullet_remaining;			//time left before next bullet can be fired
float time_per_magmine_fire;            //time between magmine firings
float time_magmine_fire_remaining;		//time left before next magmine fire can be fired
int frames_per_bullet;                  //number of frame before next bullet can be fired
int bullet_frame_cnt;                   //when reaches zero next bullet can be fired                          
int is_exploding;                       //set if obj us exploding
int explode_frames_total;
int explode_frame_cnt;
int explode_shape;                      //set or reset to make 2 different looking explosions
int missle_steer_delay;                 //a delay in frames before missle steering in allowed, this helps missle get away from cpu ship
int grav_affected;                      //gravitational acceleration has an effect
int arming_delay_frame_cnt;				//used to stop a missle that's still coming up to speed being able to kill cpu's ship

double timed_burn;                    	//fixed burn
double timed_burn_thrust;
double scale;

vector<st_line_tag> vline;              //hold obj outline


int possible_owner;						//-1 = none, 0 = user,  1 = cpu
int magmine_in_flight_frame_cnt;		//

};











//use this class as it trys to hold un-maximised window size values by detecting a
//large window resize that is close to sreen resolution, see dble_wnd::resise()
class dble_wnd : public Fl_Double_Window
{
private:										//private var



public:											//public var
bool dble_wnd_verbose;
int maximize_boundary_x, maximize_boundary_y, maximize_boundary_w, maximize_boundary_h;     //see dble_wnd::resize()
int restore_size_x, restore_size_y, restore_size_w, restore_size_h;                         //see dble_wnd::resize()

public:											//public functions
dble_wnd( int xx, int yy, int wid, int hei, const char *label = 0 );
~dble_wnd();

private:										//private functions
void resize( int xx, int yy, int ww, int hh );

};









class mywnd : public dble_wnd
{
private:										//private var
mystr m1_timer;
int ctrl_key;
int shift_key;
int left_button;
int mousex, mousey;
int up_key;
int down_key;
int left_key;
int right_key;
int space_key;
int n_key;
int y_key;
int p_key;
int q_key;
int z_key;
int x_key;
bool option_key1;
bool option_key2;
bool option_key3;
bool option_key4;
bool option_key5;
int ships_left_user;
int ships_left_cpu;
bool have_a_ship_explosion;                     //true if cpu or user ship is exploding
bool exploding_ship_user;
bool exploding_ship_cpu;
bool pause_ship_explosion;
int max_ship_hits;                              //when this is exceeded a ship is lost
int max_meteor_hits;
int deaths_combined;							//total deaths regardless of cpu or user's side			
int missle_in_flight_cpu;
int missle_in_flight_user;
bool evade_angle;

double ship_to_ship_distance;
//double cpu_ship_hard_turn_angle;                //used to avoid user's ship bullets
//double cpu_ship_thrust_jump;                    //used to avoid user's ship bullets
//int cpu_ship_bullet_avoid_cnt;               //used to avoid user's ship bullets
double pi, pi2, twopi;

string dropped_str;
int midx, midy;

double zoomx, zoomy;

unsigned int frames_per_missle_launch;          //how oftern missles are launched
unsigned int next_missle_frame_cnt;

int damage_warning_threshold;                   //flag user in peril                 

int frames_till_next_medic;

int mousewheel;
int sizex;
int sizey;


bool have_grav_objs;
bool have_grav_collisions;
bool have_meteor_objs;
bool have_magmine_objs;

bool disable_deaths;							//set this to stop any ships being destroyed, useful for debugging

vector<st_obj_tag> vobj;

public:											//public var
unsigned int frame_counter;

double time_factor;								//set this to 1.0 for normal time
double ref_tframe;								//0.02 (50fps) - this is the frame rate all obj values are referenced with, newtonian accel, etc
double frame_factor;							//this will be 1.0 if frame time is 0.02 (50fps), or 0.5 when frame time is 0.01 (100fps), its used to scale values
 
double tframe;                                  //desired time between frames
double measured_tframe;							//actual measured time between frames
double corrected_timer1_trame;					//variable timer1 val used from Windows timer1 inaccuracies
double measured_tframe_cumm;					//used to calc actual frame rate using long term averaging
double measured_tframe_avg;						//used to calc actual frame rate using long term averaging
unsigned int measured_tframe_avg_cnt;			//used to calc actual frame rate using long term averaging

double bounding_scale_down;						//see set_bounding_rect(..), applies a scale(fudge) to bounding rect to cater for poss of obj being rotate
int evade_hits;


int chaff_user;
int chaff_cpu;
vector<int> vchaff_user;
vector<int> vchaff_cpu;


float ship_mass_user;
float ship_mass_cpu;


string sdeath;
bool game_ended;
bool game_to_start;
bool paused;
int keyhit_disable_cnt;
int attract_mode_cnt;							//start play automatically
int attract_mode_active;

int menu_hei;
bool is_inside;

//int a_public_var;
//Fl_Offscreen offscr;

public:											//public functions
mywnd( int xx, int yy, int wid, int hei, const char *label );
~mywnd();
void init( bool new_game );
void tick( double &measured_tframe );
void new_game();

private:										//private functions
void draw();
int handle( int );
void setcolref( colref col );
void draw_offscr( bool menu_showing );
void draw_death_reason( int posy );
void set_col( unsigned int col );
void line( double x1, double y1, double x2, double y2 );
void rotate( double xx, double yy, double &x1, double &y1, double &x2, double &y2, double theta );
void add_bullet( int owner_idx );
void delete_marked_objs();
void check_obj_collisions();
void add_medic( double xx, double yy, double dx, double dy );
void add_meteor( int shape, bool affected_by_grav, bool apply_dxdy, double scale, double xx, double yy, double dx, double dy );
void do_any_explosions();
//void change_pause( bool toggle );
void add_missile( int owner_idx );
void add_chaff( int owner_idx );
int rand_range( int low, int high );
double rnd();
bool is_neg( double d );
void add_exhaust( int owner_idx, double level );
void track_theta_smoothly( st_obj_tag &o, double theta_req );
void add_grav_obj( double xx, double yy, double dx, double dy );
double find_angle_between_objs( int i1, int i2, double &dist );
double vector_angle( double dx, double dy );
void obj_vertex_offset( int idx, double offx, double offy );
void obj_vertex_scale( int idx, double scalex, double scaley );
void set_bounding_rect( int idx, double scalex, double scaley );
bool append_arcseg( st_obj_tag &o, int segments, double radius, double start_ang, double stop_ang, double ofx, double ofy );
int missile_in_flight( int owner_idx, vector<int> &vint );
int get_grav_objs( vector<int> &vint );
int get_chaff_objs(  int owner_idx, vector<int> &vint );
int get_meteor_objs( vector<int> &vint );
bool add_hits( int ii, int amount );
void add_magmine( int shape, bool affected_by_grav, bool apply_dxdy, double scale, double xx, double yy, double dx, double dy, float time_to_live );
void get_magmine_capture_count( int &user_cnt, vector<int> &vuser, int &cpu_cnt, vector<int> &vcpu );
void fire_magmine( int ship_idx, int magmine_idx );
void add_magmine_fragment( int shape, bool affected_by_grav, bool apply_dxdy, double scale, double xx, double yy, double dx, double dy, float time_to_live, float spin );

};




