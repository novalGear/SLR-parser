# src/SLR/CMakeLists.txt

# 1. Создаем БИБЛИОТЕКУ из исходников парсера
add_library(slr_core STATIC
    slr_parser.cpp
    slr_parser_dump.cpp
    table_builder.cpp
    # Добавьте сюда другие .cpp файлы парсера, если есть
)

# Подключаем заголовки для библиотеки
target_include_directories(slr_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src/lexer   # Чтобы видела токены
    ${CMAKE_SOURCE_DIR}/src/grammar # Чтобы видела грамматику
)

# Линкуем зависимости библиотеки
target_link_libraries(slr_core PUBLIC
    grammar
    lexer_lib
)

# 2. Создаем ИСПОЛНЯЕМЫЙ ФАЙЛ только из main.cpp
add_executable(slr_parser
    main.cpp
)

# Линкуем exe с нашей новой библиотекой
target_link_libraries(slr_parser PRIVATE
    slr_core
)
