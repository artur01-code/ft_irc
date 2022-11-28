NAME	= ircserv

CC		=	c++
FLAGS	=	-Wall -Wextra -Werror -std=c++98 -pedantic
MAIN	=	main.cpp
SRCS	=	Channel.cpp \
			Client.cpp \
			Commands.cpp \
			Message.cpp \
			Server.cpp \
			ErrorMessages.cpp \
			Replies.cpp \
			Mode.cpp
INC = Channel.hpp \
	  Client.hpp \
	  Colors.hpp \
	  Message.hpp \
	  Server.hpp

DFLAGS	=	-g -D M_DEBUG=1

all:	${NAME}

${NAME}: ${SRCS} ${INC}
	${CC} ${MAIN} ${SRCS} -o ${NAME}

docker:
	docker run -ti -v $(PWD):/test memory-test:0.1 bash -c "cd /test/; make re && valgrind --leak-check=full ./${NAME}"

debug:
	${CC} ${MAIN} ${SRCS} ${FLAGS} ${DFLAGS} -o ${NAME}

test_messages:
	${CC} ${SRCS} tests/test_messages.cpp -o messages.test

test_errors:
	${CC} ${SRCS} tests/test_errors.cpp -o errors.test

fclean:
	rm -f ${NAME}
	rm -f *.test
	rm -rf ircserv.dSYM/

re: fclean all

.PHONY: re fclean all

