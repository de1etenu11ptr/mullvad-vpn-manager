BUILD				?= release
CXX				= gcc
CXXFLAGS			= -Wall \
					-Wno-pointer-sign \
					-Wno-unused-but-set-parameter \
					-Wextra \
					-Werror \
					-D_XOPEN_SOURCE=700 \
					-std=c11 \
					-MMD \
					-MT

LDFLAGS				= -lncursesw -lsodium

SRCDIR				= src
OBJDIR				= obj

# SOURCES				= $(wildcard $(SRCDIR)/**/*.c) $(wildcard $(SRCDIR)/*.c)
SOURCES				= $(shell find $(SRCDIR) -type f -name '*.c')
OBJECTS				= $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SOURCES))
# DEPENDENCIES			= $(patsubst $(OBJDIR)/%.o, $(OBJDIR)/%.d, $(OBJECTS))
DEPENDENCIES			= $(OBJECTS:.o=.d)

TARGET				= mvm

ifeq ($(BUILD),debug)
	CXXFLAGS		+= -g \
					-Wno-unused-label \
					-Wno-unused-parameter \
					-Wno-unused-variable \
					-Wno-unused-but-set-variable
	LDFLAGS			+= -g
else ifeq ($(BUILD),test)
	CXXFLAGS		+= -O0 \
					-Wno-unused-label \
					-Wno-unused-parameter \
					-Wno-unused-variable \
					-Wno-unused-but-set-variable
else ifeq ($(BUILD),release)
	CXXFLAGS		+= -O2
endif

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(LDFLAGS) $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -c $< -o $@

-include $(DEPENDENCIES)

clean:
	rm -rf $(OBJDIR) $(TARGET)

