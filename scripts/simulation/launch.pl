$term_cmd="xterm -hold -e";

sub run {
  # get arguments
  my ($num, $time, $period, $sim, $area, $debug) = @_;

  # launch drone controllers
  $gams_root = $ENV{'GAMS_ROOT'};
  for (my $i=0; $i < $num; $i++)
  {
    system("$term_cmd \"$gams_root/gams_controller -i $i -n $num -p vrep --loop-time $time --period $period --madara-file $gams_root/scripts/simulation/$sim/madara_init_$i.mf $gams_root/scripts/simulation/areas/$area.mf $gams_root/scripts/simulation/madara_init_common.mf -l $debug\" &");
  }
  
  # launch simulation controller
  system("$gams_root/dynamic_simulation -p -n $num -mf $gams_root/scripts/simulation/areas/$area.mf");
}
