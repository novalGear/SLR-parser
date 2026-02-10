
APP_DIR  = ./app
DATA_DIR = ./data
SRC_DIR  = ./src

TOKEN_DEF = $(SRC_DIR)/tokens_def.rb
TOKEN_GEN = $(SRC_DIR)/generate_tokens.rb

$(SRC_DIR)/tokens.h $(SRC_DIR)/tokens.hpp $(SRC_DIR)/token_utils.hpp: $(TOKEN_DEF) $(TOKEN_GEN)
	ruby $(TOKEN_GEN)

lexer:
	mkdir -p $(APP_DIR)
	flex -o $(APP_DIR)/lex.yy.c $(DATA_DIR)/lexer.l
	gcc -c $(APP_DIR)/lex.yy.c -I$(SRC_DIR) -o $(APP_DIR)/lex.yy.o
	g++ -c $(SRC_DIR)/lexer.cpp -I$(SRC_DIR) -o $(APP_DIR)/lexer.o
	g++ -c $(SRC_DIR)/main.cpp -I$(SRC_DIR) -o $(APP_DIR)/main.o
	g++ $(APP_DIR)/main.o $(APP_DIR)/lexer.o $(APP_DIR)/lex.yy.o -o $(APP_DIR)/lexer

clean:
	rm -rf $(APP_DIR)/ $(SRC_DIR)/tokens.h $(SRC_DIR)/tokens.hpp $(SRC_DIR)/token_utils.hpp lexer
