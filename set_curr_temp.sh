./bcu ptc_set_target -ptc_temp=110 -ptc_on -ptc_sensor=1 -board=nxp_custom_revB -id=1-3
while true
do
    ./bcu ptc_set_curr -ptc_temp=70  -board=nxp_custom_revB -id=1-3
done