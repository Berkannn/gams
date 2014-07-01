use strict;
package simulation;

# empty string to represent not set
my $term_prefix = "";
my $term_suffix = "";

sub run {
  # get arguments
  my ($num, $time, $period, $sim, $area, $debug, $plants) = @_;
  my $osname = $^O;

  # launch drone controllers
  my $gams_root = $ENV{"GAMS_ROOT"};
  for (my $i=0; $i < $num; $i++)
  {
    my $cmd = "\"$gams_root/gams_controller -i $i -n $num -p vrep --loop-time $time --period $period --madara-file $gams_root/scripts/simulation/$sim/madara_init_$i.mf $gams_root/scripts/simulation/areas/$area.mf $gams_root/scripts/simulation/madara_init_common.mf -l $debug\"";
    if ($term_prefix)
    {
      system("$term_prefix $cmd $term_suffix");
    }
    elsif ($osname eq "MSWin32") # windows default
    {
      system("cmd \c $cmd");
    }
    elsif ($osname eq "linux") # linux default
    {
      system("xterm -hold -e $cmd &");
    }
    else
    {
      print("OS not recognized and \$term_prefix is not set...exiting");
      exit(1);
    }
  }
 
  # launch simulation controller
  my $cmd = "$gams_root/dynamic_simulation -n $num --madara-file $gams_root/scripts/simulation/areas/$area.mf";
  if ($plants)
  {
    $cmd = "$cmd -p $plants";
  }
  system("$cmd");
}

1;
