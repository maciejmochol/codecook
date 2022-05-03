#!/usr/bin/wishx -f 
# Vooyeck 1999-10

# Set window title
wm title . "Codecook client - raw protocol"

# Create for overall managing
frame .top -borderwidth 10
pack .top -side top -fill x

set socket_no 0
set connected 0

button .top.quit -text Quit -command exit
set but_con [button .top.connect -text \
"Connect (to localhost 2930)" -command Connect]
pack .top.quit .top.connect -side left



# PARAMS LIST (pl)
frame .paramsList -borderwidth 0
pack .paramsList -side top -fill x
set pl_but [button .paramsList.run -text "Send" -command pl_Run]
pack .paramsList.run -side right
label .paramsList.l -text "PARAMS LIST" -padx 0
pack .paramsList.l -side left


	# PARAMS SHOWCHANNELS <PATH> (psp)
	frame .params -borderwidth 0
	pack .params -side top -fill x
	set psp_but [button .params.run -text "Send" -command psp_Run]
	pack .params.run -side right
	label .params.l -text "PARAMS SHOWCHANNELS " -padx 0
	entry .params.cmd -width 2 -relief sunken -textvariable psp_var
	pack .params.l -side left
	pack .params.cmd -side left -fill x -expand true

	# PARAMS VALUE <FULLPATH> (pvf)
	frame .paramsVal -borderwidth 0
	pack .paramsVal -side top -fill x
	set pvf_but [button .paramsVal.run -text "Send" -command pvf_Run]
	pack .paramsVal.run -side right
	label .paramsVal.l -text "PARAMS VALUE" -padx 0
	entry .paramsVal.cmd -width 2 -relief sunken -textvariable pvf_var
	pack .paramsVal.l -side left
	pack .paramsVal.cmd -side left -fill x -expand true


# ALARM SHOW NAMES (al)
frame .alList -borderwidth 0
pack .alList -side top -fill x
set al_but [button .alList.run -text "Send" -command {al_Run "NAMES"}]
pack .alList.run -side right
label .alList.l -text "ALARM SHOW NAMES" -padx 0
pack .alList.l -side left

# ALARM SHOW DESC (al)
frame .alListDescr -borderwidth 0
pack .alListDescr -side top -fill x
set alDescr_but [button .alListDescr.run -text "Send" -command {al_Run "DESCR"}]
pack .alListDescr.run -side right
label .alListDescr.l -text "ALARM SHOW DESCR" -padx 0
pack .alListDescr.l -side left

# ALARM SHOW ON (al)
frame .alListOn -borderwidth 0
pack .alListOn -side top -fill x
set alOn_but [button .alListOn.run -text "Send" -command {al_Run "ON"}]
pack .alListOn.run -side right
label .alListOn.l -text "ALARM SHOW ON" -padx 0
pack .alListOn.l -side left

# ALARM SHOW OFF (al)
frame .alListOff -borderwidth 0
pack .alListOff -side top -fill x
set alOff_but [button .alListOff.run -text "Send" -command {al_Run "OFF"}]
pack .alListOff.run -side right
label .alListOff.l -text "ALARM SHOW OFF" -padx 0
pack .alListOff.l -side left


# ALARM SAVE (al)
frame .alSave -borderwidth 0
pack .alSave -side top -fill x
set alSave_but [button .alSave.run -text "Send" -command {al_Run "SAVE"}]
pack .alSave.run -side right
label .alSave.l -text "ALARM SAVE" -padx 0
pack .alSave.l -side left


	# ALARM ADD <ALARM_NAME> [<CONDITION>] (aac)
	frame .alAdd -borderwidth 0
	pack .alAdd -side top -fill x
	set aac_but [button .alAdd.run -text "Send" -command aac_Run]
	pack .alAdd.run -side right
	label .alAdd.l -text "ALARM ADD" -padx 0
	entry .alAdd.cmd -width 2 -relief sunken -textvariable aac_var
	pack .alAdd.l -side left
	pack .alAdd.cmd -side left -fill x -expand true

# DEVICE LIST (dl)
frame .deviceList -borderwidth 0
pack .deviceList -side top -fill x
set dl_but [button .deviceList.run -text "Send" -command dl_Run]
pack .deviceList.run -side right
label .deviceList.l -text "DEVICE LIST" -padx 0
pack .deviceList.l -side left

# Commented, 'cause of too few devices written

#	# DEVICE ADD <DEVICE> (dad)
#	frame .deviceAdd -borderwidth 0
#	pack .deviceAdd -side top -fill x
#	set dad_but [button .deviceAdd.run -text "Send" -command dad_Run]
#	pack .deviceAdd.run -side right
#	label .deviceAdd.l -text "DEVICE ADD" -padx 0
#	entry .deviceAdd.cmd -width 2 -relief sunken -textvariable dad_var
#	pack .deviceAdd.l -side left
#	pack .deviceAdd.cmd -side left -fill x -expand true
#
#
## DEVICE DEL (dd)
#frame .deviceDel -borderwidth 0
#pack .deviceDel -side top -fill x
#set dd_but [button .deviceDel.run -text "Send" -command dd_Run]
#pack .deviceDel.run -side right
#label .deviceDel.l -text "DEVICE DEL" -padx 0
#entry .deviceDel.cmd -width 2 -relief sunken -textvariable dd_var
#pack .deviceDel.l -side left
#pack .deviceDel.cmd -side left -fill x -expand true
#

# OTHER COMMANDS
frame .o_command -borderwidth 0
pack .o_command -side top -fill x
set oc_but [button .o_command.run -text "Send" -command oc_Run]
pack .o_command.run -side right
label .o_command.l -text "Type full command here:" -padx 0 
entry .o_command.cmd -width 6 -relief sunken -textvariable oc_var
pack .o_command.l -side left
pack .o_command.cmd -side left -fill x -expand true



# SERVER-CLIENT DIALOG LOGGER
frame .t
set log [text .t.log -width 80 -height 10 \
-borderwidth 2 -relief raised -setgrid true \
-yscrollcommand {.t.scroll set}]
scrollbar .t.scroll -command {.t.log yview}

pack .t.scroll -side right -fill y
pack .t.log -side left -fill both -expand true
pack .t -side top -fill both -expand true


# CONNECTION STATE INFORMATOR (csi)
frame .state -borderwidth 6
set csi [label .state.l -text "Not connected" -padx 0]
pack .state -side top
pack .state.l -side top -fill y


############################################################################
# PROCEDURES
############################################################################

# Just Connect +
proc Connect {} { 
	global connected socket_no csi but_con

	$csi config -text "Cannot connect.."

	set socket_no [socket localhost 2930]	
	fconfigure $socket_no -blocking 0

	set connected 1
	$csi config -text "Connected!"

	$but_con config -text "Disconnect" -command DisConnect
}

proc DisConnect {} {
	global connected socket_no csi but_con

	close $socket_no
	set connected 0
	$csi config -text "Not connected"
	$but_con config -text "Connect (to localhost 2930)" -command Connect

}


#PARAMS LIST PROC (pl) +
proc pl_Run {} {
	global socket_no connected log

	if { $connected == 0 } {
		tk_messageBox -message "You're NOT connected to server"\
		-type ok -title "Cannot send ORDER"
	} else {
		puts $socket_no "PARAMS LIST"
		flush $socket_no
		$log insert end "CLIENT: {PARAMS LIST}\n"

		select $socket_no

		set ret_string [read $socket_no]

			set lnum [string range $ret_string 0 2]
			set lret [analNum $lnum]
			set lretNoNum [string range $ret_string 4 end]

		$log insert end "SERVER (answers: $lret)\n{\n$lretNoNum}\n"
	}
}

# PARAMS SHOWCHANNELS <PATH> (psp) +
proc psp_Run {} {
	global socket_no connected log psp_var

	if { [string length $psp_var] == 0} {
		tk_messageBox -message "You haven't specified Path Name!"\
		-type ok -title "Cannot send ORDER"
		return
	}

	if { $connected == 0 } {
		tk_messageBox -message "You're NOT connected to server"\
		-type ok -title "Cannot send ORDER"
	} else {
		puts $socket_no "PARAMS SHOWCHANNELS $psp_var"
		flush $socket_no

		$log insert end "CLIENT: {PARAMS SHOWCHANNELS $psp_var}\n"

		select $socket_no

		set ret_string [read $socket_no ]

			set lnum [string range $ret_string 0 2]
			set lret [analNum $lnum]
			set lretNoNum [string range $ret_string 4 end]

		$log insert end "SERVER (answers: $lret)\n{\n$lretNoNum}\n"
	}
}

# PARAMS VALUE <FULLPATH> (pvf) +
proc pvf_Run {} {
	global socket_no connected log pvf_var

	if { [string length $pvf_var] == 0} {
		tk_messageBox -message "You haven't specified Path & Channel!"\
		-type ok -title "Cannot send ORDER"
		return
	}

	if { $connected == 0 } {
		tk_messageBox -message "You're NOT connected to server"\
		-type ok -title "Cannot send ORDER"
	} else {
		puts $socket_no "PARAMS VALUE $pvf_var"
		flush $socket_no

		$log insert end "CLIENT: {PARAMS VALUE $pvf_var}\n"

		select $socket_no

		set ret_string [read $socket_no ]

			set lnum [string range $ret_string 0 2]
			set lret [analNum $lnum]
			set lretNoNum [string range $ret_string 4 end]

		$log insert end "SERVER (answers: $lret)\n{\n$lretNoNum}\n"
	}
}


# DEVICE ADD PROC (dad) +
proc dad_Run {} {
	global socket_no connected log dad_var

	if { [string length $dad_var] == 0} {
		tk_messageBox -message "You haven't specified Device Name!"\
		-type ok -title "Cannot send ORDER"
		return
	}

	if { $connected == 0 } {
		tk_messageBox -message "You're NOT connected to server"\
		-type ok -title "Cannot send ORDER"
	} else {
		puts $socket_no "DEVICE ADD $dad_var"
		flush $socket_no

		$log insert end "CLIENT: {DEVICE ADD $dad_var}\n"

		select $socket_no

		set ret_string [read $socket_no ]

			set lnum [string range $ret_string 0 2]
			set lret [analNum $lnum]
			set lretNoNum [string range $ret_string 4 end]

		$log insert end "SERVER (answers: $lret)\n{\n$lretNoNum}\n"
	}
}

# DEVICE DEL PROC (dd) +
proc dd_Run {} {
	global socket_no connected log dd_var

	if { [string length $dd_var] == 0} {
		tk_messageBox -message "You haven't specified Device Name!"\
		-type ok -title "Cannot send ORDER"
		return
	}

	if { $connected == 0 } {
		tk_messageBox -message "You're NOT connected to server"\
		-type ok -title "Cannot send ORDER"
	} else {
		puts $socket_no "DEVICE DEL $dd_var"
		flush $socket_no

		$log insert end "CLIENT: {DEVICE DEL $dd_var}\n"

		select $socket_no

		set ret_string [read $socket_no ]

			set lnum [string range $ret_string 0 2]
			set lret [analNum $lnum]
			set lretNoNum [string range $ret_string 4 end]

		$log insert end "SERVER (answers: $lret)\n{\n$lretNoNum}\n"
	}
}

# DEVICE LIST PROC (dl) +
proc dl_Run {} {
	global socket_no connected log 

	if { $connected == 0 } {
		tk_messageBox -message "You're NOT connected to server"\
		-type ok -title "Cannot send ORDER"
	} else {
		puts $socket_no "DEVICE LIST"
		flush $socket_no

		$log insert end "CLIENT: {DEVICE LIST}\n"

		select $socket_no

		set ret_string [read $socket_no ]

			set lnum [string range $ret_string 0 2]
			set lret [analNum $lnum]
			set lretNoNum [string range $ret_string 4 end]

		$log insert end "SERVER (answers: $lret)\n{\n$lretNoNum}\n"
	}
}

# ALARM LIST PROC (al) +
proc al_Run whattodo {
	global socket_no connected log 

	if { $connected == 0 } {
		tk_messageBox -message "You're NOT connected to server"\
		-type ok -title "Cannot send ORDER"
	} else {
		if { $whattodo == "NAMES" } {
			puts $socket_no "ALARM SHOW NAMES"
			$log insert end "CLIENT: {ALARM SHOW NAMES}\n"
		} elseif { $whattodo == "ON" } {
			puts $socket_no "ALARM SHOW ON"
			$log insert end "CLIENT: {ALARM SHOW ON}\n"
		} elseif { $whattodo == "OFF" } {
			puts $socket_no "ALARM SHOW OFF"
			$log insert end "CLIENT: {ALARM SHOW OFF}\n"
		} elseif { $whattodo == "DESCR" } {
			puts $socket_no "ALARM SHOW DESCR"
			$log insert end "CLIENT: {ALARM SHOW DESCR}\n"
		} elseif { $whattodo == "SAVE" } {
			puts $socket_no "ALARM SAVE"
			$log insert end "CLIENT: {ALARM SAVE}\n"
		} else return

			flush $socket_no

			select $socket_no

			set ret_string [read $socket_no ]

			set lnum [string range $ret_string 0 2]
			set lret [analNum $lnum]
			set lretNoNum [string range $ret_string 4 end]

			$log insert end \
				"SERVER (answers: $lret)\n{\n$lretNoNum}\n"
	}
}

# ALARM ADD <ALARM_NAME> [<CONDITONS>} (aac) +
proc aac_Run {} {
	global socket_no connected log aac_var

	if { [string length $aac_var] == 0} {
		tk_messageBox -message "You haven't specified Alarm Name!"\
		-type ok -title "Cannot send ORDER"
		return
	}

	if { $connected == 0 } {
		tk_messageBox -message "You're NOT connected to server"\
		-type ok -title "Cannot send ORDER"
	} else {
		puts $socket_no "ALARM ADD $aac_var"
		flush $socket_no

		$log insert end "CLIENT: {ALARM ADD $aac_var}\n"

		select $socket_no

		set ret_string [read $socket_no ]

			set lnum [string range $ret_string 0 2]
			set lret [analNum $lnum]
			set lretNoNum [string range $ret_string 4 end]

		$log insert end "SERVER (answers: $lret)\n{\n$lretNoNum}\n"
	}
}

# OTHER COMMANDS (oc) +
proc oc_Run {} {
	global socket_no connected log oc_var

	if { [string length $oc_var] == 0} {
		tk_messageBox -message "You haven't specified any command!"\
		-type ok -title "Cannot send ORDER"
		return
	}

	if { $connected == 0 } {
		tk_messageBox -message "You're NOT connected to server"\
		-type ok -title "Cannot send ORDER"
	} else {
		puts $socket_no $oc_var
		flush $socket_no

		$log insert end "CLIENT: {$oc_var}\n"

		select $socket_no

		set ret_string [read $socket_no ]

			set lnum [string range $ret_string 0 2]
			set lret [analNum $lnum]
			set lretNoNum [string range $ret_string 4 end]

			$log insert end \
				"SERVER (answers: $lret)\n{\n$lretNoNum}\n"
	}
}

# Read and log output from the program
proc Log {} {
	global input log
	if [eof $input] {
	Stop

	} else {

	gets $input line
	$log insert end $line\n
	$log see end
	}

}

proc analNum {lnum} {
	
	if { $lnum >= 100 && $lnum < 200 } { return "OK" 
	} elseif { $lnum >= 200 && $lnum < 300 } { return "Empty list" 
	} elseif { $lnum >= 300 && $lnum < 400 } { return "Bad command" 
	} elseif { $lnum >= 400 && $lnum < 500 } { 
	return "Not enough or wrong params" }

}




#### old procedures


proc Run {} {
	global command input log but

	if [catch {open "|$command"} input] {
	$log insert end $input\n

	} else {

	fileevent $input readable Log
	$log insert end $command\n
	$but config -text Stop -command Stop
	}
}


# Stop the program and fix up the button
proc Stop {} {
	global input but

	catch {close $input}
	$but config -text "Run it" -command Run
}
