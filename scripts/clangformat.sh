#!/bin/bash

echo "=== Verificando estilo de código con clang-format ==="

# Ir al directorio raíz del proyecto
cd "$(dirname "$0")/.."
echo "Directorio actual: $(pwd)"

# Verificar que clang-format esté instalado
if ! command -v clang-format &> /dev/null; then
    echo "Error: clang-format no está instalado"
    echo "Por favor, instálalo con: sudo apt-get install clang-format"
    exit 1
else
    echo "clang-format encontrado: $(which clang-format)"
    echo "Versión: $(clang-format --version)"
fi

# Si se pasa el parámetro --fix, activar modo de corrección
FIX_MODE=0
if [ "$1" = "--fix" ]; then
    FIX_MODE=1
    echo "Modo de corrección activado"
fi

# Encontrar todos los archivos .c y .h
echo "Buscando archivos .c y .h..."
FILES=$(find src include tests -type f \( -name "*.c" -o -name "*.h" \) 2>/dev/null)
echo "Archivos encontrados: $FILES"

if [ -z "$FILES" ]; then
    echo "No se encontraron archivos .c o .h para verificar"
    exit 0
fi

# Verificar que el archivo .clang-format existe
if [ ! -f ".clang-format" ]; then
    echo "Error: No se encontró el archivo .clang-format en el directorio actual"
    exit 1
else
    echo "Archivo .clang-format encontrado"
fi

# Verificar el estilo
STYLE_ISSUES=0

for FILE in $FILES; do
    echo "Verificando $FILE..."
    
    if [ $FIX_MODE -eq 1 ]; then
        # Corregir el estilo
        echo "Ejecutando: clang-format -style=file -i \"$FILE\""
        clang-format -style=file -i "$FILE"
        echo "✅ Estilo corregido en $FILE"
    else
        # Comprobar si hay diferencias con clang-format
        echo "Ejecutando: clang-format -style=file \"$FILE\" | diff \"$FILE\" -"
        DIFF=$(clang-format -style=file "$FILE" | diff "$FILE" -)
        
        if [ -n "$DIFF" ]; then
            echo "❌ $FILE tiene problemas de estilo"
            echo "$DIFF"
            STYLE_ISSUES=1
        else
            echo "✅ $FILE cumple con el estilo"
        fi
    fi
done

if [ $FIX_MODE -eq 1 ]; then
    echo "✅ Estilo corregido en todos los archivos"
    exit 0
elif [ $STYLE_ISSUES -eq 1 ]; then
    echo "❌ Se encontraron problemas de estilo. Por favor, ejecuta:"
    echo "    ./scripts/clangformat.sh --fix"
    exit 1
else
    echo "✅ Todos los archivos cumplen con el estilo definido"
    exit 0
fi
