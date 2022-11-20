CC = $(CROSS_COMPILE)gcc
NAME = aesdsocket
CFLAGS = -g

.PHONY: all
all: $(NAME)

$(NAME):
	$(CC) $(CFLAGS) -o $(NAME) $(NAME).c 

.PHONY: clean

clean:
	rm -f $(NAME)
