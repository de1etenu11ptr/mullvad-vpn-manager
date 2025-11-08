CXX				= gcc
CXXFLAGS			= -Wall -D_XOPEN_SOURCE=700 -Wextra -Werror -std=c11 -MMD -MT

LDFLAGS				= -lncursesw

SRCDIR				= src
OBJDIR				= obj

# SOURCES				= $(wildcard $(SRCDIR)/**/*.c) $(wildcard $(SRCDIR)/*.c)
SOURCES				= $(shell find $(SRCDIR) -type f -name '*.c')
OBJECTS				= $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SOURCES))
# DEPENDENCIES			= $(patsubst $(OBJDIR)/%.o, $(OBJDIR)/%.d, $(OBJECTS))
DEPENDENCIES			= $(OBJECTS:.o=.d)

TARGET				= mvm

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(LDFLAGS) $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -c $< -o $@

-include $(DEPENDENCIES)

clean:
	rm -rf $(OBJDIR) $(TARGET)
