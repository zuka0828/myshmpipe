NAME := myshmpipe

LDLIBS = -lrt

$(NAME): $(NAME).o

clean:
	rm -f $(NAME) *.o
