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
  # Удаляем \r для совместимости с Windows-редакторами
  line = line.chomp("\r\n").chomp("\n")
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
      # Удаляем кавычки вокруг литерала
      lit = lit.gsub(/\A['"]|['"]\z/, '')
      terminals_map[lit] = name.to_sym
    end
  else
    if line.include?('->')
      lhs, rhs = line.split('->', 2).map(&:strip)
      rules << [lhs, rhs.split]
    end
  end
end

# Нетерминалы
nonterminals = rules.map(&:first).uniq
nonterminals << start_symbol unless start_symbol.nil? || nonterminals.include?(start_symbol)
nonterminals.sort!
terminal_enums = terminals_map.values

if start_symbol.nil?
  start_symbol = nonterminals.first
  warn "Не указан %start — используем: #{start_symbol}"
end

all_symbols = nonterminals.map(&:upcase) + terminal_enums + [:END_MARKER]
symbol_count = all_symbols.size
nt_count = nonterminals.size
t_count = terminal_enums.size + 1

# --- symbol_enums.hpp ---
names_str = all_symbols.map { |s| "\"#{s}\"" }.join(", ")
File.write("#{GEN_DIR}/symbol_enums.hpp", [
  "// Сгенерировано из #{GRAMMAR_FILE}",
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
].join("\n"))

# --- grammar_rules.cpp ---
rhs_arrays = []
rule_inits = []

rules.each_with_index do |(lhs, rhs_tokens), idx|
  rhs_symbols = rhs_tokens.map do |tok|
    clean_tok = tok.gsub(/\A['"]|['"]\z/, '')
    if terminals_map.key?(clean_tok)
      terminals_map[clean_tok]
    elsif nonterminals.include?(tok)
      tok.upcase.to_sym
    else
      raise "Неизвестный символ в правиле: '#{tok}' (очищено: '#{clean_tok}')\n" \
            "Правило: #{lhs} -> #{rhs_tokens.join(' ')}"
    end
  end

  array_name = "RHS_#{idx}"
  rhs_arrays << "static constexpr Symbol #{array_name}[] = { #{rhs_symbols.map { |s| "Symbol::#{s}" }.join(', ')} };"
  rule_inits << "  {#{idx}, Symbol::#{lhs.upcase}, #{array_name}, #{rhs_symbols.size}}"
end

File.write("#{GEN_DIR}/grammar_rules.cpp", [
  "// Сгенерировано из #{GRAMMAR_FILE}",
  '#include "grammar/rule.hpp"',
  "",
  *rhs_arrays,
  "",
  "const Rule GRAMMAR_RULES_ARRAY[] = {",
  rule_inits.join(",\n"),
  "};",
  "",
  "const Rule* GRAMMAR_RULES = GRAMMAR_RULES_ARRAY;",
  "const std::size_t GRAMMAR_RULES_COUNT = #{rules.size};",
  ""
].join("\n"))

puts "Сгенерировано:"
puts "  #{GEN_DIR}/symbol_enums.hpp"
puts "  #{GEN_DIR}/grammar_rules.cpp"
