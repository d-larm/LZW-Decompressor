CC=g++
BINDIR=./bin
SRCDIR=./src
OBJDIR=./obj

DEPS=$(OBJDIR)/LZW_Decompressor.o $(OBJDIR)/File_Loader.o

$(BINDIR)/LZW: $(DEPS) $(SRCDIR)/LZW.cpp
	$(CC) -o $@ $^

$(OBJDIR)/LZW_Decompressor.o: $(SRCDIR)/LZW_Decompressor.cpp
	$(CC) -c $^ -o  $@

$(OBJDIR)/File_Loader.o: $(SRCDIR)/File_Loader.cpp
	$(CC) -c $^ -o $@

clean:
	rm -f $(OBJDIR)/*.o core && make
