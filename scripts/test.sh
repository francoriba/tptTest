#!/bin/bash -e

echo "=== Ejecutando Tests ==="

# Ir al directorio raíz del proyecto (un nivel arriba de scripts)
cd "$(dirname "$0")/.."

echo "Trabajando en directorio: $(pwd)"

# Asegurarse de que el proyecto esté compilado
if [ ! -d "build" ] || [ ! -f "build/test_main" ] || [ ! -f "build/test_metrics" ]; then
    echo "Compilando el proyecto primero..."
    ./scripts/build.sh
fi

# Ir al directorio build
cd build

# Ejecutar tests con CTest
echo "Ejecutando tests unitarios..."
ctest --output-on-failure

# Si se quiere ejecutar tests individuales, descomentar estas líneas
# echo "Ejecutando test_main individualmente..."
# ./test_main
# echo "Ejecutando test_metrics individualmente..."
# ./test_metrics

# Generar informe de cobertura si gcovr está instalado
if command -v gcovr &> /dev/null; then
    echo "Generando informe de cobertura..."
    cd ..
    gcovr -r . --html --html-details -o build/coverage.html
    echo "Informe de cobertura generado en build/coverage.html"
fi

echo "✅ Tests completados"
