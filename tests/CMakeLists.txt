add_executable(parser_tests
    parser_e2e_test.cpp
)

target_link_libraries(parser_tests PRIVATE
    GTest::gtest
    GTest::gtest_main
    slr_core
    lexer_lib
    grammar
)

target_include_directories(parser_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/src/lexer
    ${CMAKE_SOURCE_DIR}/src/SLR
    ${CMAKE_SOURCE_DIR}/src/grammar
)

# Явно передаем пути к данным в тесты (на всякий случай, хотя наследование должно работать)
target_compile_definitions(parser_tests PRIVATE
    DATA_DIR="${CMAKE_SOURCE_DIR}/data"
)

# === РУЧНАЯ РЕГИСТРАЦИЯ ТЕСТОВ ===

add_test(NAME ParserE2ETest.SimpleAddition
         COMMAND parser_tests --gtest_filter=ParserE2ETest.SimpleAddition
         WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

add_test(NAME ParserE2ETest.MultiplicationPrecedence
         COMMAND parser_tests --gtest_filter=ParserE2ETest.MultiplicationPrecedence
         WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

add_test(NAME ParserE2ETest.SyntaxError
         COMMAND parser_tests --gtest_filter=ParserE2ETest.SyntaxError
         WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

add_test(NAME ParserE2ETest.ErrorRecovery
         COMMAND parser_tests --gtest_filter=ParserE2ETest.ErrorRecovery
         WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

add_test(NAME ParserE2ETest.ParenthesesAndPrecedence
         COMMAND parser_tests --gtest_filter=ParserE2ETest.ParenthesesAndPrecedence
         WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

add_test(NAME ParserE2ETest.ExactSequenceParenthesesPrecedence
         COMMAND parser_tests --gtest_filter=ParserE2ETest.ExactSequenceParenthesesPrecedence
         WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
