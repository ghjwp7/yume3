#!/usr/bin/wish
##$Id: yume.tcl 2 2009-03-13 17:58:20Z jwpat7 $
#-----------------------------------------------
proc pack_button {id ypad txt act} {
# Add a button to a horizontal row of buttons.
    global lcolor bucolor
    pack [button $id  -foreground $lcolor -background $bucolor -text $txt \
          -command $act -pady $ypad] -expand 1 -fill both -side left
}
#---------------------------------
proc make_group {id ypad icmd} {
# id=window id.  ypad=extra y size.  icmd=initial command text.

# Make a window with do-button, cmd entry area, and EE-button.
# EE button enables/disables entry-activation of the do-bar.
    global lcolor ee_dis
    frame .b.$id
    button .b.$id.do -foreground $lcolor -font lucidasanstypewriter-18 -background $ee_dis -text "do:" -command "do_group $id" -pady $ypad
    checkbutton .b.$id.ee -text "EE" -variable edo$id -relief flat -command "change_group $id"
    entry .b.$id.e -font lucidasanstypewriter-18  -relief raised -width 40
    .b.$id.e insert 0 $icmd
    pack .b.$id.e -side bottom -expand 1 -fill x 
    pack .b.$id.ee -side left 
    pack .b.$id.do -side right  -expand 1 -fill x
    pack .b.$id   -side top  -pady 2m -expand 1 -fill x
}
#---------------------------------
proc do_group id {
# Perform the command string from entry field id; use an xterm if ! 

    set isBang [regsub "^!" [.b.$id.e get] "" cmd]
    if $isBang {
	eval exec [list xterm -e sh -c [concat $cmd \; $shell ]] &
    } else {
	eval [list eval exec [list sh -c $cmd ] &]
    }
}
#---------------------------------
## Reason for ## bind choices in change_group: You can bind to button,
## button group, or frame, for more- or less-restrictive EE button areas.
proc change_group id {
    global  ee_dis ee_enb
    upvar edo$id edo
    if {$edo} {
	.b.$id.do configure -background $ee_enb;  # Enable
	bind .b.$id.do <Enter>  "do_group $id";   #   button
    } else {
	.b.$id.do configure -background $ee_dis;  # Disable
	bind .b.$id.do <Enter> {};                #   button
    }
}
#----------- The main program ------------------
set myname "yume"
set aname -1
set where "-10-70";		# default location of window
set lcolor Pink; set bucolor Darkblue; set bg SeaGreen3
set ee_dis Darkblue; set ee_enb Red
set shell $env(SHELL);   # To execute commands via user-chosen shell
set mode -1;  # Mode is -1 at start , 0 for -do, 1 for -bu
set narg -1;

frame .b;  .b config -bg $bg;  # Create main window
pack .b -side left -expand 1 -fill both

# Each make_group line creates a do-group.  Use as many make_group's
# and as many button-lines as you like, intermingled.  Each group and
# button line must have a unique id, such as p, q, r.
# You can pre-specify text fields, eg:
# make_group p 4m "ls -l | grep stuff"

# Make a do-group or button stuff for each remaining argument

while {$argc > 1+$narg || $narg < 0} {
    set narg [expr $narg+1]; set arg [lindex $argv $narg]
    set opt 1
    if { 0==[string first "-" $arg] } {
	set front [string range $arg 1 2]
	switch -exact "$front" {
	"do" { set mode 0}	# Select do-group mode
	"bu" {	set mode 1;	# Select shell button-group mode
		set f [frame .b.$narg];
		pack $f -expand 1 -fill both -side top }
	"tc" {	set mode 2;	# Select Tcl button-group mode
		set f [frame .b.$narg];
		pack $f -expand 1 -fill both -side top }
	"la" {	set narg [expr $narg+1]; set arg [lindex $argv $narg]
		set aname $arg }	# Set button name
	"at" {  set narg [expr $narg+1];
		eval [concat wm geometry . [lindex $argv $narg]] }
	"ti" {  set narg [expr $narg+1];
		eval [list wm title . [lindex $argv $narg]];
		eval [list wm iconname . [lindex $argv $narg]] }
	"x"  {  set narg [expr $narg+1];
		eval [lindex $argv $narg] }
	}
	set opt 0
    }

    if {$opt>0} {
	if {$mode < 0} {
	    set mode 0;		# Get back into default mode 0
	    set f [frame .b.r];  pack $f -expand 1 -fill both -side top
	    pack_button $f.2 1m  "exit"  "destroy ."}

        if {$aname==-1} then {set aname $arg}
	if {$mode == 0} { make_group $narg 2m $arg }
	if {$mode == 1} then { pack_button $f.$narg 1m $aname \
		[list eval exec [list sh -c $arg] &] }
    	if {$mode == 2} then { pack_button $f.$narg 1m $aname $arg}
	set aname -1
    }
}
#------------------------end---------------------
