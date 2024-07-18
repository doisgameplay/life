#!/bin/bash

# Verifica se um arquivo .cpp foi passado como argumento
if [ -z "$1" ]; then
    echo "Uso: $0 <arquivo.cpp>"
    exit 1
fi

# Remove a extensão .cpp para obter o nome do executável
filename=$(basename -- "$1")
filename="${filename%.*}"

# Compila o arquivo .cpp
g++ -o "$filename" "$1" -lsfml-graphics -lsfml-window -lsfml-system

# Verifica se a compilação foi bem-sucedida
if [ $? -eq 0 ]; then
    # Executa o arquivo compilado
    ./"$filename"
else
    echo "Erro na compilação."
    exit 1
fi
