# Makefile,v 1.2 2004/01/07 22:40:16 shuston Exp
#
# Copyright 2003 Addison-Wesley Inc. All Rights Reserved.

MAKEFILE=Makefile

MKLIST = Makefile.static \
	 Makefile.dynamic

## Ensure that Makefiles in MKLIST are executed in sequence during a
## parallel build because we share some files between multiple libraries.
.NOTPARALLEL:

## Makefile.Reactive_Logging_Server.mkfile is a dummy target which will cause
## $(MAKE) -f Makefile.Reactive_Logging_Server to be invoked, then it cleans
## up tempinc when needed for AIX Visual Age C++.
%.mkfile: %
	@echo $(MAKE) -f $< $(MKFILE_TARGET)
	-@$(RM) -rf tempinc
	@$(MAKE) -f $< $(MKFILE_TARGET)

# This rule invokes make again with the list of .mkfile targets as a
# parameter.  For example, if the all target is being made, make is invoked
# as follows:
#
# make -f Makefile MKFILE_TARGET=all Makefile.static

all clean depend realclean html:
ifneq ($(MKLIST),)
	@echo $(MAKE) -f $(MAKEFILE) MKFILE_TARGET=$@ $(addsuffix .mkfile, $(MKLIST))
	@$(MAKE) -f $(MAKEFILE) MKFILE_TARGET=$@ $(addsuffix .mkfile, $(MKLIST))
endif

# DO NOT DELETE THIS LINE -- g++dep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.
