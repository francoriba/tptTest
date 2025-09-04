#!/bin/bash -e

echo "=== Build Script para Sistema de Monitoreo ==="
echo ""

# Ir al directorio raíz del proyecto (un nivel arriba de scripts)
cd "$(dirname "$0")/.."

echo "Trabajando en directorio: $(pwd)"

# Verificar y corregir el CMakeLists.txt de Unity para evitar conflictos de nombres
UNITY_CMAKE="lib/unity/CMakeLists.txt"
if [ -f "$UNITY_CMAKE" ]; then
    echo "## Verificando configuración de Unity..."

    # Comprobar si ya está configurado con el nombre correcto
    if grep -q "unity_framework" "$UNITY_CMAKE"; then
        echo "## Unity ya está configurado correctamente."
    else
        echo "## Corrigiendo configuración de Unity para evitar conflictos de nombres..."

        # Hacer una copia de seguridad
        cp "$UNITY_CMAKE" "${UNITY_CMAKE}.bak"

        # Reemplazar el nombre del proyecto y del target
        sed -i 's/project(\s*"unity"/project("unity_framework"/g' "$UNITY_CMAKE"
        sed -i 's/add_library(${PROJECT_NAME}/add_library(unity_framework/g' "$UNITY_CMAKE"

        echo "## Configuración de Unity corregida."
    fi
fi

# Buscar el directorio 'build'
if [ -d "build" ]; then
    echo "## Encontrado directorio 'build' en $(pwd)"
    echo "## Eliminando contenido del directorio build..."
    rm -rf build/*
else
    echo "## Directorio 'build' no encontrado en $(pwd)"
    echo "## Creando directorio build"
    mkdir build
fi

echo "Compilando proyecto..."
cd build && cmake -GNinja -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON ..

echo "Construyendo proyecto..."
ninja

echo "✅ Build completado exitosamente"