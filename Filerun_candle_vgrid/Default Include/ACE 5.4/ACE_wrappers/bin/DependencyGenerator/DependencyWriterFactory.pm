package DependencyWriterFactory;

# ************************************************************
# Description   : Create DependencyWriter objects.
# Author        : Chad Elliott
# Create Date   : 5/23/2003
# ************************************************************

# ************************************************************
# Pragmas
# ************************************************************

use strict;

use GNUDependencyWriter;
use NMakeDependencyWriter;

# ************************************************************
# Subroutine Section
# ************************************************************

sub create {
  switch: {
    $_[0] eq 'gnu'   && do { return new GNUDependencyWriter(); };
    $_[0] eq 'nmake' && do { return new NMakeDependencyWriter(); };
    print STDERR "WARNING: Invalid dependency writer type: $_[0]\n";
  }

  return new DependencyWriter();
}


1;
