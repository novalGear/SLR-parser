#!/usr/bin/env ruby
# -*- coding: utf-8 -*-

require 'fileutils'

GRAMMAR_FILE = '../data/grammar.txt'
GEN_DIR = 'grammar/generated'

FileUtils.mkdir_p(GEN_DIR)

# --- Парсинг grammar.txt ---
terminals_map = {}
rules = []
start_symbol = nil
in_terminals = false

File.readlines(GRAMMAR_FILE).each do |line|
  line = line.strip
  next if line.empty? || line.start_with?('#')

  if line.start_with?('%terminals')
    in_terminals = true
    next
  elsif line.start_with?('%start')
    start_symbol = line.split[1]
    in_terminals = false
    next
  elsif line.start_with?('%')
    in_terminals = false
    next
  end

  if in_terminals
    line = line.chomp(',')
    next if line.empty?
    if line.include?(':')
      lit, name = line.split(':', 2).map(&:strip)
      lit = lit.gsub(/^['"]|['"]$/, '')
      terminals_map[lit] = name.to_sym
    end
  else
    if line.include?('->')
      lhs, rhs = line.split('->', 2).map(&:strip)
      rules << [lhs, rhs.split]
    end
  end
end

# Определяем нетерминалы
nonterminals = rules.map(&:first).uniq
nonterminals << start_symbol unless start_symbol.nil? || nonterminals.include?(start_symbol)
nonterminals.sort!
terminal_enums = terminals_map.values

# Если %start не задан — берём первый нетерминал
if start_symbol.nil?
  start_symbol = nonterminals.first
  warn "Не указан %start — используем: #{start_symbol}"
end

# Формируем полный список символов: сначала нетерминалы, потом терминалы, потом END_MARKER
all_symbols = nonterminals.map(&:upcase) + terminal_enums + [:END_MARKER]
symbol_count = all_symbols.size
nt_count = nonterminals.size
t_count = terminal_enums.size + 1

# Массив имён для SYMBOL_NAMES
names_str = all_symbols.map { |s| "\"#{s}\"" }.join(", ")

# --- Генерация symbol_enums.hpp ---
hpp_content = [
  "// Автоматически сгенерировано из #{GRAMMAR_FILE}",
  "#ifndef SYMBOL_ENUMS_HPP",
  "#define SYMBOL_ENUMS_HPP",
  "",
  "#include <cstddef>",
  "",
  "enum class Symbol {",
  all_symbols.map { |s| "  #{s}," },
  "};",
  "",
  "constexpr Symbol START_SYMBOL = Symbol::#{start_symbol.upcase};",
  "constexpr Symbol END_MARKER = Symbol::END_MARKER;",
  "constexpr std::size_t SYMBOL_COUNT = #{symbol_count};",
  "constexpr std::size_t NONTERMINAL_COUNT = #{nt_count};",
  "constexpr std::size_t TERMINAL_COUNT = #{t_count};",
  "",
  "inline constexpr const char* SYMBOL_NAMES[SYMBOL_COUNT] = {",
  "  #{names_str}",
  "};",
  "",
  "#endif // SYMBOL_ENUMS_HPP"
].join("\n")

File.write("#{GEN_DIR}/symbol_enums.hpp", hpp_content)
puts "Сгенерировано: #{GEN_DIR}/symbol_enums.hpp"
