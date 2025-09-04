#!/bin/bash -e

echo "=== Ejecutando Tests ==="

# Ir al directorio raíz del proyecto (un nivel arriba de scripts)
cd "$(dirname "$0")/.."

echo "Trabajando en directorio: $(pwd)"

# Ir al directorio build
cd build

# Ejecutar tests con CTest
echo "Ejecutando tests unitarios..."
ctest --output-on-failure

echo "✅ Tests completados"
