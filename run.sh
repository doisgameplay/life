#!/bin/bash

# Verifica se um arquivo .cpp foi passado como argumento
if [ -z "$1" ]; then
    echo "Uso: $0 <arquivo.cpp>"
    exit 1
fi

# Remove a extensão .cpp para obter o nome do executável
filename=$(basename -- "$1")
filename="${filename%.*}"

# Verifica se a pasta 'programs' existe, se não, cria
if [ ! -d "programs" ]; then
    mkdir programs
fi

# Compila o arquivo .cpp e coloca o executável na pasta 'programs'
g++ -o "programs/$filename" "$1" -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio

# Verifica se a compilação foi bem-sucedida
if [ $? -eq 0 ]; then
    # Executa o arquivo compilado da pasta 'programs'
    ./programs/"$filename"
else
    echo "Erro na compilação."
    exit 1
fi
