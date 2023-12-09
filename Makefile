CC := gcc
CFLAGS := -Wall
EXEC := out

$(EXEC): src/main.c
	$(CC) src/main.c -o $(EXEC)

run: $(EXEC)
	@./$(EXEC)
