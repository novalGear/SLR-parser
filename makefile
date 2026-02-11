
APP_DIR  = ./app
DATA_DIR = ./data
SRC_DIR  = ./src

LEXER_DIR = $(SRC_DIR)/lexer

TOKEN_DEF = $(LEXER_DIR)/tokens_def.rb
TOKEN_GEN = $(LEXER_DIR)/generate_tokens.rb

$(LEXER_DIR)/tokens.h $(LEXER_DIR)/tokens.hpp $(LEXER_DIR)/token_utils.hpp: $(TOKEN_DEF) $(TOKEN_GEN)
	ruby $(TOKEN_GEN)

lexer:
	mkdir -p $(APP_DIR)
	flex -o $(APP_DIR)/lex.yy.c $(DATA_DIR)/lexer.l
	gcc -c $(APP_DIR)/lex.yy.c -I$(LEXER_DIR) -o $(APP_DIR)/lex.yy.o
	g++ -c $(LEXER_DIR)/lexer.cpp -I$(LEXER_DIR) -o $(APP_DIR)/lexer.o
	g++ -c $(LEXER_DIR)/main.cpp -I$(LEXER_DIR) -o $(APP_DIR)/main.o
	g++ $(APP_DIR)/main.o $(APP_DIR)/lexer.o $(APP_DIR)/lex.yy.o -o $(APP_DIR)/lexer

clean:
	rm -rf $(APP_DIR)/ $(LEXER_DIR)/tokens.h $(LEXER_DIR)/tokens.hpp $(LEXER_DIR)/token_utils.hpp lexer
