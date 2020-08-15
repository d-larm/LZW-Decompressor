CC=g++
BINDIR=.
SRCDIR=.
OBJDIR=.

DEPS=$(OBJDIR)/LZW_Decompressor.o $(OBJDIR)/File_Loader.o

$(BINDIR)/LZW: $(DEPS) $(SRCDIR)/LZW.cpp
	$(CC) -o $@ $^

$(OBJDIR)/LZW_Decompressor.o: $(SRCDIR)/LZW_Decompressor.cpp $(SRCDIR)/LZW_Decompressor.h
	$(CC) -c $<

$(OBJDIR)/File_Load.o: $(SRCDIR)/File_Loader.cpp $(SRCDIR)/File_Loader.h
	$(CC) -c $<

clean:
	rm -f $(OBJDIR)/*.o core
