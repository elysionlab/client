package DependencyEditor;

# ************************************************************
# Description   : Edits existing dependencies.
# Author        : Chad Elliott
# Create Date   : 2/10/2002
# ************************************************************

# ************************************************************
# Pragmas
# ************************************************************

use strict;
use FileHandle;
use File::Basename;

use DependencyGenerator;
use ObjectGeneratorFactory;

# ************************************************************
# Subroutine Section
# ************************************************************

sub new {
  return bless {
               }, $_[0];
}


sub process {
  my($self)     = shift;
  my($output)   = shift;
  my($type)     = shift;
  my($noinline) = shift;
  my($macros)   = shift;
  my($ipaths)   = shift;
  my($replace)  = shift;
  my($files)    = shift;

  ## Back up the original file and receive the contents
  my($contents) = undef;
  if (-s $output) {
    $contents = [];
    if (!$self->backup($output, $contents)) {
      print STDERR "ERROR: Unable to backup $output\n";
      return 1;
    }
  }

  ## Write out the new file
  my($fh) = new FileHandle();
  if (open($fh, ">$output")) {
    if (defined $contents) {
      foreach my $line (@$contents) {
        if ($line =~ /DO NOT DELETE/) {
          last;
        }
        print $fh $line;
      }
    }

    print $fh "# DO NOT DELETE THIS LINE -- " . basename($0) . " uses it.\n" .
              "# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.\n\n";

    my($dep) = new DependencyGenerator($macros, $ipaths,
                                       $replace, $type, $noinline);
    my($objgen) = ObjectGeneratorFactory::create($type);
    ## Sort the files so the dependencies are reproducible
    foreach my $file (sort @$files) {
      my(@objects) = $objgen->process($file);
      print $fh $dep->process($file, \@objects) . "\n";
    }

    print $fh "# IF YOU PUT ANYTHING HERE IT WILL GO AWAY\n";
    close($fh);
  }
  else {
    print STDERR "ERROR: Unable to open $output for output\n";
    return 1;
  }

  return 0;
}


sub backup {
  my($self)     = shift;
  my($source)   = shift;
  my($contents) = shift;
  my($status)   = 0;
  my($fh)       = new FileHandle();
  my($backup)   = "$source.bak";

  if (open($fh, $source)) {
    my($oh) = new FileHandle();
    if (open($oh, ">$backup")) {
      $status = 1;
      while(<$fh>) {
        print $oh $_;
        push(@$contents, $_);
      }
      close($oh);

      ## Set file permission
      my(@buf) = stat($source);
      if (defined $buf[8] && defined $buf[9]) {
        utime($buf[8], $buf[9], $backup);
      }
    }
    close($fh);
  }
  return $status;
}


1;
