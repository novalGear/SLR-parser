# Исполняемый файл парсера
add_executable(slr_test
    main.cpp
    slr_parser.cpp
    table_builder.cpp
)

# Зависимость от грамматики
target_link_libraries(slr_test PRIVATE grammar)

# Для JSON-экспорта (если используете nlohmann/json)
# include(FetchContent)
# FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
# FetchContent_MakeAvailable(json)
# target_link_libraries(slr_test PRIVATE nlohmann_json::nlohmann_json)
