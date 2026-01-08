include config.mk

all: program

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

program: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LIBS)

clean:
	rm -f $(OBJ) program