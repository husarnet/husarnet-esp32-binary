LIBS += rosem emhusarnet husarnet
COMPONENT_ADD_INCLUDEDIRS := .
COMPONENT_ADD_LINKER_DEPS += $(patsubst %,$(COMPONENT_PATH)/lib/lib%.a,$(LIBS))

COMPONENT_ADD_LDFLAGS += $(patsubst %,$(COMPONENT_PATH)/lib/lib%.a,$(LIBS))
