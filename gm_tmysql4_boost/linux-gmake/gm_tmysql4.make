# GNU Make project makefile autogenerated by Premake
ifndef config
  config=release
endif

ifndef verbose
  SILENT = @
endif

ifndef CC
  CC = gcc
endif

ifndef CXX
  CXX = g++
endif

ifndef AR
  AR = ar
endif

ifeq ($(config),release)
  OBJDIR     = obj/Release
  TARGETDIR  = ../lib/linux
  TARGET     = $(TARGETDIR)/libgm_tmysql4.so
  DEFINES   += -DNDEBUG -D_LINUX
  INCLUDES  += -I../../include -I/blackawps/boost_1_51_0 -I/blackawps/sourcesdk/gmod13/include/GarrysMod/Lua
  CPPFLAGS  += -MMD -MP $(DEFINES) $(INCLUDES)
  CFLAGS    += $(CPPFLAGS) $(ARCH) -g -msse -O2 -ffast-math -fPIC
  CXXFLAGS  += $(CFLAGS) 
  LDFLAGS   += -shared -L../../lib -L/blackawps/boost_1_51_0/stage/lib
  LIBS      += -llibmysql -llibboost_thread
  RESFLAGS  += $(DEFINES) $(INCLUDES) 
  LDDEPS    += 
  LINKCMD    = $(CXX) -o $(TARGET) $(OBJECTS) $(LDFLAGS) $(RESOURCES) $(ARCH) $(LIBS)
  define PREBUILDCMDS
  endef
  define PRELINKCMDS
  endef
  define POSTBUILDCMDS
  endef
endif

ifeq ($(config),release32)
  OBJDIR     = obj/x32
  TARGETDIR  = ../lib/linux
  TARGET     = $(TARGETDIR)/libgm_tmysql4.so
  DEFINES   += -DNDEBUG -D_LINUX
  INCLUDES  += -I../../include -I/blackawps/boost_1_51_0 -I/blackawps/sourcesdk/gmod13/include/GarrysMod/Lua
  CPPFLAGS  += -MMD -MP $(DEFINES) $(INCLUDES)
  CFLAGS    += $(CPPFLAGS) $(ARCH) -g -msse -O2 -ffast-math -m32 -fPIC
  CXXFLAGS  += $(CFLAGS) 
  LDFLAGS   += -shared -m32 -L/usr/lib32 -L../../lib -lm -ldl /blackawps/boost_1_51_0/stage/lib/libboost_thread.so.1.51.0 /blackawps/gm_tmysql4/lib/libmysql.so.16 /blackawps/gm_tmysql4/lib/libmysqlclient.a
  LIBS      += 
  RESFLAGS  += $(DEFINES) $(INCLUDES) 
  LDDEPS    += 
  LINKCMD    = $(CXX) -o $(TARGET) $(OBJECTS) $(LDFLAGS) $(RESOURCES) $(ARCH) $(LIBS)
  define PREBUILDCMDS
  endef
  define PRELINKCMDS
  endef
  define POSTBUILDCMDS
  endef
endif

OBJECTS := \
	$(OBJDIR)/database.o \
	$(OBJDIR)/gm_tmysql.o \
	$(OBJDIR)/ILuaInterface.o \
	$(OBJDIR)/ILuaModuleManager.o \
	$(OBJDIR)/ILuaObject.o \

RESOURCES := \

SHELLTYPE := msdos
ifeq (,$(ComSpec)$(COMSPEC))
  SHELLTYPE := posix
endif
ifeq (/bin,$(findstring /bin,$(SHELL)))
  SHELLTYPE := posix
endif

.PHONY: clean prebuild prelink

all: $(TARGETDIR) $(OBJDIR) prebuild prelink $(TARGET)
	@:

$(TARGET): $(GCH) $(OBJECTS) $(LDDEPS) $(RESOURCES)
	@echo Linking gm_tmysql4
	$(SILENT) $(LINKCMD)
	$(POSTBUILDCMDS)

$(TARGETDIR):
	@echo Creating $(TARGETDIR)
ifeq (posix,$(SHELLTYPE))
	$(SILENT) mkdir -p $(TARGETDIR)
else
	$(SILENT) mkdir $(subst /,\\,$(TARGETDIR))
endif

$(OBJDIR):
	@echo Creating $(OBJDIR)
ifeq (posix,$(SHELLTYPE))
	$(SILENT) mkdir -p $(OBJDIR)
else
	$(SILENT) mkdir $(subst /,\\,$(OBJDIR))
endif

clean:
	@echo Cleaning gm_tmysql4
ifeq (posix,$(SHELLTYPE))
	$(SILENT) rm -f  $(TARGET)
	$(SILENT) rm -rf $(OBJDIR)
else
	$(SILENT) if exist $(subst /,\\,$(TARGET)) del $(subst /,\\,$(TARGET))
	$(SILENT) if exist $(subst /,\\,$(OBJDIR)) rmdir /s /q $(subst /,\\,$(OBJDIR))
endif

prebuild:
	$(PREBUILDCMDS)

prelink:
	$(PRELINKCMDS)

ifneq (,$(PCH))
$(GCH): $(PCH)
	@echo $(notdir $<)
	-$(SILENT) cp $< $(OBJDIR)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
endif

$(OBJDIR)/database.o: ../gm_tmysql4/database.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/gm_tmysql.o: ../gm_tmysql4/gm_tmysql.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/ILuaInterface.o: ../gm_tmysql4/ILuaInterface.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/ILuaModuleManager.o: ../gm_tmysql4/ILuaModuleManager.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/ILuaObject.o: ../gm_tmysql4/ILuaObject.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"

-include $(OBJECTS:%.o=%.d)
