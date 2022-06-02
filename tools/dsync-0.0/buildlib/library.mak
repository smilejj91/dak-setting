# -*- make -*-

# This creates a shared library.

# Input
# $(SOURCE) - The source code to use
# $(HEADERS) - Exported header files and private header files
# $(LIBRARY) - The name of the library without lib or .so 
# $(MAJOR) - The major version number of this library
# $(MINOR) - The minor version number of this library

# All output is writtin to .opic files in the build directory to
# signify the PIC output.

# See defaults.mak for information about LOCAL

# Some local definitions
LOCAL := lib$(LIBRARY).so.$(MAJOR).$(MINOR)
$(LOCAL)-OBJS := $(addprefix $(OBJ)/,$(addsuffix .opic,$(notdir $(basename $(SOURCE)))))
$(LOCAL)-DEP := $(addprefix $(DEP)/,$(addsuffix .opic.d,$(notdir $(basename $(SOURCE)))))
$(LOCAL)-HEADERS := $(addprefix $(INCLUDE)/,$(HEADERS))
$(LOCAL)-SONAME := lib$(LIBRARY).so.$(MAJOR)
$(LOCAL)-SLIBS := $(SLIBS)
$(LOCAL)-LIBRARY := $(LIBRARY)

# Install the command hooks
headers: $($(LOCAL)-HEADERS)
library: $(LIB)/lib$(LIBRARY).so $(LIB)/lib$(LIBRARY).so.$(MAJOR) $(LIB)/lib$(LIBRARY).a
clean: clean/$(LOCAL)
veryclean: veryclean/$(LOCAL)

# The clean rules
.PHONY: clean/$(LOCAL) veryclean/$(LOCAL)
clean/$(LOCAL):
	-rm -f $($(@F)-OBJS) $($(@F)-DEP)
veryclean/$(LOCAL): clean/$(LOCAL)
	-rm -f $($(@F)-HEADERS) $(LIB)/lib$($(@F)-LIBRARY).so*

# Build rules for the two symlinks
.PHONY: $(LIB)/lib$(LIBRARY).so.$(MAJOR) $(LIB)/lib$(LIBRARY).so
$(LIB)/lib$(LIBRARY).so.$(MAJOR): $(LIB)/lib$(LIBRARY).so.$(MAJOR).$(MINOR)
	ln -sf $(<F) $@
$(LIB)/lib$(LIBRARY).so: $(LIB)/lib$(LIBRARY).so.$(MAJOR).$(MINOR)
	ln -sf $(<F) $@
	
# The binary build rule
$(LIB)/lib$(LIBRARY).so.$(MAJOR).$(MINOR): $($(LOCAL)-HEADERS) $($(LOCAL)-OBJS)
	-rm -f $(LIB)/lib$($(@F)-LIBRARY).so* 2> /dev/null
	echo Building shared library $@
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(PICFLAGS) $(LFLAGS) -o $@ \
	   $(LFLAGS_SO) $(SONAME_MAGIC)$($(@F)-SONAME) -shared \
	   $(filter %.opic,$^) $($(@F)-SLIBS)

$(LIB)/lib$(LIBRARY).a: $($(LOCAL)-HEADERS) $($(LOCAL)-OBJS)
	-rm -f $(LIB)/lib$($(@F)-LIBRARY).a 2> /dev/null
	echo Building static library $@
	$(AR) rc $@ $(filter %.opic,$^)
	ranlib $@

# Compilation rules
vpath %.cc $(SUBDIRS)
$(OBJ)/%.opic: %.cc
	echo Compiling $< to $@
	$(CXX) -c $(INLINEDEPFLAG) $(CPPFLAGS) $(CXXFLAGS) $(PICFLAGS) -o $@ $<
	$(DoDep)

# Include the dependencies that are available
The_DFiles = $(wildcard $($(LOCAL)-DEP))
ifneq ($(words $(The_DFiles)),0)
include $(The_DFiles)
endif 
