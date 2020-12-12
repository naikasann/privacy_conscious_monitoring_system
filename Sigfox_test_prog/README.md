# Sigfox breakout board command list

AT$I=10 : get Device ID
AT$I=11 : get PAC"
AT$T? : get Temperature
AT$V? : get Voltages
AT$P=unit : set Power mode (unit = 0:software reset 1:sleep 2:deep_sleep)
AT$TR=unit : set Transmit repeat (unit = 0..2)
AT$WR : save config
AT$SF=[payload] : SEND SIGFOX MESSAGE
