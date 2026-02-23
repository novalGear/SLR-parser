add_executable(slr_parser
    main.cpp
    slr_parser.cpp
    slr_parser_dump.cpp
    table_builder.cpp
)

# Линкуем библиотеки
# Порядок важен: сначала зависимые, потом зависимости (хотя cmake обычно сам разбирается)
target_link_libraries(slr_parser PRIVATE
    grammar
    lexer_lib
)

# Если будете использовать nlohmann/json, раскомментируйте блок ниже:
# include(FetchContent)
# FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
# FetchContent_MakeAvailable(json)
# target_link_libraries(slr_parser PRIVATE nlohmann_json::nlohmann_json)
