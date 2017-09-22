# Kevin the blacksmith
# This file is called whenever a :playerent: occurs
# we need to check whether it's applicable to us first
# because its caused by a player then $player will be set
# $me will be set to Kevin in this case
# $mob,$obj will be unset
:playerent:
CMP: $me.room, $player.room
JE: "isme"
EXIT:
LBL: "isme"
SAY: "Hello ", $player.name, ", how can I help you?"
#SAY: "Oh, and I appear to be mob number: ", $me
EXIT:
