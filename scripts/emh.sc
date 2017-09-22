# Kevin the blacksmith
# This file is called whenever a :playerent: occurs
# we need to check whether it's applicable to us first
# because its caused by a player then $player will be set
# $me will be set to Kevin in this case
# $mob,$obj will be unset
:playerent:
:playerarrive:
CMP: $me.room, $player.room
JE: "isme"
EXIT:
LBL: "isme"
CMP: $player.hp, $player.maxhp
JE: "getout"
SAY: "Well ", $player.name, ", I suppose you want me to patch you up?"
#SAY: "Oh, and I appear to be mob number: ", $me
LBL: "getout"
EXIT:
