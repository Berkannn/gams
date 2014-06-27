#!/usr/bin/perl
$gams_root=$ENV{'GAMS_ROOT'};
require "$gams_root/scripts/simulation/launch.pl";

$time=3600;
$madara_debug=0;
$period=1;
$num=10;
$sim="prioritized_min_time";
$area="small";

run($num, $time, $period, $sim, $area, $madara_debug);
