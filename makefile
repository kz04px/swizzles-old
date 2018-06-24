CC         = g++
CFLAGS     = -std=c++14 -Wall -Wextra -O3 -flto

LINKER     = g++ -o
LFLAGS     = -pthread

TARGET     = swizzles
SRCDIR     = src
OBJDIR     = obj
BINDIR     = bin

SOURCES  := $(wildcard $(SRCDIR)/*.cpp)
INCLUDES := $(wildcard $(SRCDIR)/*.hpp)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

$(BINDIR)/$(TARGET): $(BINDIR) $(OBJDIR) $(OBJECTS)
	@$(LINKER) $@ $(OBJECTS) $(LFLAGS)

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	@$(CC) $(CFLAGS) -c $< -o $@

release:
	$(MAKE) CFLAGS="$(CFLAGS) -DNDEBUG"

debug:
	$(MAKE) TARGET="$(TARGET)-debug"

bin:
	mkdir -p $(BINDIR)

obj:
	mkdir -p $(OBJDIR)

clean:
	rm -r $(OBJDIR)

.PHONY: clean
