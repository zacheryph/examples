# eggdrop tcl to send phone pages <via email> for people on a verified list
# must go: /msg <bot> <valid xr_clients #/string/id/etc> message here
# settings
set xr_sender  "<sender email addr>"
set xr_recieve "<reciever email addr>"
set xr_clients {
  VALID
}

bind msg -|- page xr_checkmsg
#bind dcc n|- +client xr_addclient
#bind dcc n|- -client xr_delclient
#bind dcc n|- client xr_listclients

proc xr_checkmsg { nick hostmask handle msg } {
  set client_id [lindex $msg 0]
  set msg [lreplace $msg 0 0]
  if {[lsearch -exact $xr_clients [string toupper $client_id]]} {
    # valid client id. send the email
    set email ["| /usr/sbin/sendmail -f $xr_sender -t" "w"]
      puts $email "To: $xr_recieve"
      puts $email "From: $xr_sender"
      puts $email "Subject: $client_id"
      puts $email ""
      puts $email "$msg"
    close $email
    putserv "PRIVMSG $handle :emergency page sent!"
  }
}
