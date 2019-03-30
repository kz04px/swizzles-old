CC         = g++
CFLAGS     = -std=c++17 -Wall -Wextra
RFLAGS     = -O3 -flto -march=native -DNDEBUG
DFLAGS     = -g

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
	$(MAKE) CFLAGS="$(CFLAGS) $(RFLAGS)"

debug:
	$(MAKE) CFLAGS="$(CFLAGS) $(DFLAGS)" TARGET="$(TARGET)-debug"

bin:
	mkdir -p $(BINDIR)

obj:
	mkdir -p $(OBJDIR)

clean:
	rm -r $(OBJDIR) $(BINDIR)/$(TARGET) $(BINDIR)/$(TARGET)-debug

.PHONY: clean
