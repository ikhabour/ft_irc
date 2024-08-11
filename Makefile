NAME= ircserv
FLAGS= -Wall -Wextra -Werror -std=c++98 -g3 #-fsanitize=address

SRC = Server.cpp Client.cpp Authentication.cpp Channel.cpp cmds.cpp main.cpp utils.cpp \
	  Commands/Join.cpp Commands/Leave.cpp Commands/Mode.cpp Commands/Invite.cpp Commands/Kick.cpp \
	  Commands/Topic.cpp Commands/List.cpp Commands/Privmsg.cpp	

OBJ = $(SRC:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJ)
	@g++ $(FLAGS) $(OBJ) -o $(NAME)

%.o: %.cpp
	@g++ $(FLAGS) -c $< -o $@

clean:
	@rm -f $(OBJ)

fclean: clean
	@rm -f $(NAME)

re: fclean all

