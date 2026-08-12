The exe file is used to generate the ameba_wifi_power_table_usrcfg.c by "WR-TX_Power by Rate&Limit Table-RTLXXXX_RXX.xls" file

args:
	-h/--help						show help message
	--limit_table <TABLE_NAME> 		specified the excel to generate TxPowerTable
	--suffix <SUFFIX>      			specified suffix for tx power table/functions

E.g.
	AmebaTxPowerTool.exe --limit_table "WR-TX_Power by Rate&Limit Table-RTLXXXX_R0X.xls"  [--suffix  test]

Output :
	.\target\ameba_wifi_power_table_usrcfg.c

