#!/bin/bash -e

echo "=== Build Script para Sistema de Monitoreo ==="
echo ""

# Ir al directorio raíz del proyecto (un nivel arriba de scripts)
cd "$(dirname "$0")/.."

echo "Trabajando en directorio: $(pwd)"

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