NAME = codexion

SRC_DIR = src
INC_DIR = include

SRC = $(SRC_DIR)/main.c \
	  $(SRC_DIR)/error.c \
	  $(SRC_DIR)/parser.c \
	  $(SRC_DIR)/coder.c \
	  $(SRC_DIR)/dongle.c \
	  $(SRC_DIR)/time.c \
	  $(SRC_DIR)/simulation.c \
	  $(SRC_DIR)/request.c \
	  $(SRC_DIR)/release.c \
	  $(SRC_DIR)/log.c \
	  $(SRC_DIR)/coder_routine.c \
	  $(SRC_DIR)/monitor.c \
	  $(SRC_DIR)/heap.c

OBJ = $(SRC:.c=.o)

HEADER = $(INC_DIR)/codexion.h

CC = gcc-12
CFLAGS = -Wall -Wextra -Werror -pthread -g -O0 -I$(INC_DIR)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ)

fclean: clean
	rm -rf $(NAME)

lint:
	norminette -R CheckDefine $(HEADER)
	norminette $(SRC)

re: fclean all

.PHONY: all clean fclean re lint
