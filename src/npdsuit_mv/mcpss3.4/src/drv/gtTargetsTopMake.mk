####################################################################
#   Common Targets
####################################################################
ifeq (visual,$(TOOL))
 MDFLAGS0 = $(subst /D,-D, $(CFLAGS))
 MDFLAGS1 = $(subst /W3,, $(MDFLAGS0))
 MDFLAGS2 = $(subst /Od,, $(MDFLAGS1))
 MDFLAGS3 = $(subst /Gm,, $(MDFLAGS2))
 MDFLAGS4 = $(subst /GX,, $(MDFLAGS3))
 MDFLAGS5 = $(subst /ZI,, $(MDFLAGS4))
 MDFLAGS6 = $(subst /nologo,, $(MDFLAGS5))
 MDFLAGS7 = $(subst /GZ,, $(MDFLAGS6))
 MDFLAGS8 = $(subst /RTC1,, $(MDFLAGS7))
 MDFLAGS9 = $(subst /EHsc,, $(MDFLAGS8))
 MDFLAGS = $(subst /MTd,, $(MDFLAGS9))
endif

ifeq (D_ON,$(DEBUG_INFO))
   CFLAGS += -g 
endif
  
all: dep touch compile
	set -e;$(DO_LOOP);$(DO_KERNELLOOP)

# removed Level0 conditionned directories create
# build_dir dependence added instead 
dep: build_dir $(DEP_FILE_LIST)
	set -e;$(DO_LOOP);$(DO_KERNELLOOP)

lib: localToLib
	$(DO_LOOP);$(DO_KERNELLOOP)
	
localToLib:
	$(DO_LOOP);$(DO_KERNELLOOP)
ifneq (0, $(words $(OBJ_FILES)))
ifeq (visual,$(TOOL))
	$(AR) /nologo /out:$(LIB_DIR)/$(LIB_NAME) $(wildcard $(LIB_DIR)/$(LIB_NAME)) $(OBJ_FILES) /IGNORE:4006
else
	$(AR) $(ARFLAGS) $(LIB_DIR)/$(LIB_NAME) $(OBJ_FILES)
endif
endif

onelib: localToOneLib
	$(DO_LOOP);$(DO_KERNELLOOP)

localToOneLib:
ifneq (0, $(words $(OBJ_FILES)))
ifeq (visual,$(TOOL))
	$(AR) /nologo /out:$(LIB_DIR)/$(PROJ_NAME).lib $(wildcard $(LIB_DIR)/$(PROJ_NAME).lib) $(OBJ_FILES) /IGNORE:4006
else
	$(AR) $(ARFLAGS) $(LIB_DIR)/$(PROJ_NAME).lib $(OBJ_FILES)
endif
endif

compile: $(OBJ_FILE_LIST)
	set -e;$(DO_LOOP)

local: $(OBJ_FILE_LIST) localToLib localToOneLib


qac_full: qac_dir dep qac qac_rep

qac: $(QAC_FILE_LIST)
	set -e;$(DO_LOOP)

qac_rep: $(QAC_REP_FILE_LIST)
	set -e;$(DO_LOOP)


touch:
ifneq (0,$(words $(TOUCH_FILE_LIST)))
ifeq (visual,$(TOOL))
	$(TOUCH) $(addprefix $(SCRAP_DIR)/,$(notdir $(TOUCH_FILE_LIST:.c=.obj)))
	$(RM) $(addprefix $(SCRAP_DIR)/,$(notdir $(TOUCH_FILE_LIST:.c=.obj)))
else
	$(TOUCH) $(addprefix $(SCRAP_DIR)/,$(notdir $(TOUCH_FILE_LIST:.c=.o)))
	$(RM) $(addprefix $(SCRAP_DIR)/,$(notdir $(TOUCH_FILE_LIST:.c=.o)))
endif
endif

# gmake -f $(SW_ROOT)/$(PROJ_NAME)/gtTopMake mkld
# is wrond fore Dune drivers directory, PROJ_MAKE defined in gtDefsTopMake.mk
# as $(SW_ROOT)/$(PROJ_NAME)/gtTopMake and overridden in Dune's gtTopMake
elf:
	gmake -f $(PROJ_MAKE) mkld

mkld:
ifneq (0,$(words $(EXE_OBJ_LIST)))
ifeq (visual,$(TOOL))
	$(AR) /nologo /out:$(LIB_DIR)/$(ARCH_NAME).lib $(EXE_OBJ_LIST) /IGNORE:4006 
else
	$(LD) $(LDFLAGS) $(BUILD_MAP_FILE) $(LIB_DIR)/$(ARCH_NAME).map $(FO) $(LIB_DIR)/$(ARCH_NAME).o $(EXE_OBJ_LIST)
endif
endif

ifeq (visual,$(TOOL))
$(OBJ_FILE_LIST) : %.obj : %.c %.dep
	$(CC) $(CFLAGS) $(INCLUDE_PATH) $(PROJECT_PATH) $(FC) /Fo$(SCRAP_DIR)/$@ /Fd$(SCRAP_DIR)/vc60.pdb $(filter %/$(notdir $<),$(C_FILE_LIST_AFTER_EXCLUDE))
else
$(OBJ_FILE_LIST) : %.o : %.c %.dep
	$(CC) $(CFLAGS) $(INCLUDE_PATH) $(PROJECT_PATH) $(FC) $(FO) $(SCRAP_DIR)/$@ $(filter %/$(notdir $<),$(C_FILE_LIST_AFTER_EXCLUDE))
endif

$(DEP_FILE_LIST) : %.dep : %.c
ifeq  ($(findstring gnu,$(TOOL)),gnu)
	$(CC) $(CFLAGS) $(INCLUDE_PATH) $(PROJECT_PATH) -M -o $(SCRAP_DIR)/$@ $(filter %/$(notdir $<),$(C_FILE_LIST_AFTER_EXCLUDE))
endif
ifeq  ($(findstring diab,$(TOOL)),diab)
	$(CC) $(OPTION_DEPEND_C) $(CFLAGS) $(PROJECT_PATH) $(filter %/$(notdir $<),$(C_FILE_LIST_AFTER_EXCLUDE)) > $(SCRAP_DIR)/$@ 
endif

ifeq (visual,$(TOOL))
	$(MD) -f- $(MDFLAGS) $(subst /I ,-I,$(PROJECT_PATH)) $(filter %/$(notdir $<),$(C_FILE_LIST_AFTER_EXCLUDE)) > $(SCRAP_DIR)/$@ 
endif

ifeq ($(TOOL),visual)
 QAC_PROJECT_PATH = $(subst /I ,-i , $(PROJECT_PATH))
# QAC_FLAGS = $(subst /D,-d , $(AD_CFLAGS))
 QAC_FLAGS_ = $(filter /D%, $(CFLAGS)) 
 QAC_FLAGS = $(subst /D,-d , $(QAC_FLAGS_)) 
 QAC_PERSONALITIES = $(subst \,/,$(USER_BASE))/tools/Prqa/personalities.via 
else
 QAC_PROJECT_PATH = $(subst -I,-i , $(PROJECT_PATH))
 QAC_FLAGS_ = $(filter -D%, $(CFLAGS)) 
 QAC_FLAGS  = $(subst -D,-d , $(QAC_FLAGS_))
 QAC_PERSONALITIES = $(USER_BASE)\tools\Prqa\personalities.via  
endif

$(QAC_FILE_LIST) : %.c.err : %.c %.dep
	qac $(QAC_PROJECT_PATH) $(QAC_FLAGS) -via $(QAC_PERSONALITIES) -op $(QAC_OUT_DIR) $(filter %/$(notdir $<),$(C_FILE_LIST_AFTER_EXCLUDE))


$(QAC_REP_FILE_LIST) : %.rep : %.c %.c.err
#	prjdsp qac $(QAC_PROJECT_PATH)  -via $(USER_BASE)\tools\Prqa\personalities.via  -op $(QAC_OUT_DIR) -file $(QAC_OUT_DIR)/$@ $(filter %/$(notdir $<),$(C_FILE_LIST_AFTER_EXCLUDE))
	errdsp qac -via $(QAC_PERSONALITIES) -op $(QAC_OUT_DIR) -file $(QAC_OUT_DIR)/$@ ./$<
	prjdsp qac -via $(QAC_PERSONALITIES) -op $(QAC_OUT_DIR) -file $(QAC_OUT_DIR)/$@.lst ./$<

clean_loop:
	$(DO_LOOP);$(DO_KERNELLOOP)
ifneq (0,$(words $(DEP_FILES)))
	$(RM) $(DEP_FILES)
endif
ifneq (0,$(words $(OBJ_FILES)))
	$(RM) $(OBJ_FILES)
endif
ifneq (0,$(words $(wildcard $(LIB_DIR)/$(LIB_NAME))))
	$(RM) $(LIB_DIR)/$(LIB_NAME)
endif

.PHONY: clean_p
clean_p: clean_loop
ifneq (0,$(words $(wildcard $(LIB_DIR)/$(ARCH_NAME).map)))
	$(RM) $(LIB_DIR)/$(ARCH_NAME).map
endif

# used "-p" flag: create all the path without "alredy exist" warning
build_dir: $(SCRAP_DIR) $(LIB_DIR)

$(SCRAP_DIR):
	- @ $(MKDIR) -p $(SCRAP_DIR)

$(LIB_DIR):
	- @ $(MKDIR) -p $(LIB_DIR)

qac_dir: $(QAC_OUT_DIR)

$(QAC_OUT_DIR):
	- @ $(MKDIR) -p $(QAC_OUT_DIR)


