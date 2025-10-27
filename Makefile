NAME = ircserv

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -I./include
LDFLAGS =

SRC_DIR = src
OBJ_DIR = obj
INC_DIR = include

SRCS = src/main.cpp \
       src/core/IRCServer.cpp \
       src/core/User.cpp \
       src/core/ClientManager.cpp \
       src/core/ChannelManager.cpp \
       src/core/Room.cpp \
       src/network/NetworkManager.cpp \
       src/commands/Commands.cpp \
       src/commands/auth.cpp \
       src/commands/channel.cpp \
       src/commands/messaging.cpp \
       src/commands/moderation.cpp \
       src/utils/Message.cpp \
       src/utils/Utils.cpp

OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME) $(LDFLAGS)
	@echo "Built $(NAME) successfully"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
