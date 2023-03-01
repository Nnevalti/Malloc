# This is a minimal set of ANSI/VT100 color codes
_END=$'\x1b[0m
_BOLD=$'\x1b[1m
_UNDER=$'\x1b[4m
_REV=$'\x1b[7m

# Colors
_GREY=$'\x1b[30m
_RED=$'\x1b[31m
_GREEN=$'\x1b[32m
_YELLOW=$'\x1b[33m
_BLUE=$'\x1b[34m
_PURPLE=$'\x1b[35m
_CYAN=$'\x1b[36m
_WHITE=$'\x1b[37m

ifeq ($(HOSTTYPE),)
	HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

PATH_INC = include
PATH_LIB = lib
PATH_OBJ = obj
PATH_SRC = src

SRC = malloc.c \
	 realloc.c \
	 free.c \
	 show_alloc_mem.c \
	 utils.c

OBJ = $(SRC:%.c=$(PATH_OBJ)/%.o)

NAME = libft_malloc_$(HOSTTYPE).so
LIB_NAME = libft_malloc.so

CC = gcc

FLAGS_CC = -Wall -Wextra -Werror -fPIC
FLAGS_LIB = -shared

.PHONY: all clean fclean re

all: $(NAME)

$(NAME): $(OBJ)
	@$(CC) $(FLAGS_LIB) -o $@ $(OBJ)
	@rm -f $(LIB_NAME)
	@ln -s $(NAME) $(LIB_NAME)
	@echo "${_GREEN}✓ Shared library $(NAME) created${_END}"

$(PATH_OBJ)/%.o: $(PATH_SRC)/%.c
	@mkdir -p $(@D)
	@$(CC) -c -o $@ $(FLAGS_CC) $^ -O0 -g -I $(PATH_INC)
	@echo "${_GREEN}✓ Compilation of $< done${_END}"

clean:
	@rm -rf $(PATH_OBJ)
	@echo "${_RED}Object files deleted${_END}"

fclean: clean
	@rm -f $(NAME) $(LIB_NAME)
	@echo "${_RED}Shared library deleted${_END}"

re: fclean $(NAME)
