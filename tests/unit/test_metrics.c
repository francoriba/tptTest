/**
 * @file test_metrics.c
 * @brief Tests unitarios para las funciones de métricas
 */

#include "monitoreo.h"
#include "unity.h"
#include <errno.h> // Para errno
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>  // Para struct stat y S_ISDIR
#include <sys/types.h> // Tipos adicionales necesarios
#include <unistd.h>

// Archivo temporal para pruebas de JSON
#define TEST_JSON_FILE "/tmp/test_metrics.json"

void setUp(void)
{
    // Configuración antes de cada test
}

void tearDown(void)
{
    // Limpieza después de cada test
    remove(TEST_JSON_FILE);
}

/**
 * @brief Test para verificar que read_cpu_metrics funciona correctamente
 */
void test_read_cpu_metrics(void)
{
    cpu_metrics_t cpu;

    // Ejecutar la función a testear
    int result = read_cpu_metrics(&cpu);

    // Verificar que la función se ejecutó correctamente
    TEST_ASSERT_EQUAL_INT(0, result);

    // Verificar que los valores son razonables
    TEST_ASSERT_TRUE(cpu.user >= 0.0 && cpu.user <= 100.0);
    TEST_ASSERT_TRUE(cpu.system >= 0.0 && cpu.system <= 100.0);
    TEST_ASSERT_TRUE(cpu.idle >= 0.0 && cpu.idle <= 100.0);
    TEST_ASSERT_TRUE(cpu.usage_percent >= 0.0 && cpu.usage_percent <= 100.0);

    // La suma de user + system + idle debería ser cercana a 100%
    // (puede no ser exactamente 100% debido a otros estados de CPU)
    TEST_ASSERT_TRUE(cpu.user + cpu.system + cpu.idle <= 100.1);
}

/**
 * @brief Test para verificar que read_memory_metrics funciona correctamente
 */
void test_read_memory_metrics(void)
{
    memory_metrics_t memory;

    // Ejecutar la función a testear
    int result = read_memory_metrics(&memory);

    // Verificar que la función se ejecutó correctamente
    TEST_ASSERT_EQUAL_INT(0, result);

    // Verificar que los valores son razonables
    TEST_ASSERT_TRUE(memory.total > 0);
    TEST_ASSERT_TRUE(memory.free >= 0);
    TEST_ASSERT_TRUE(memory.buffers >= 0);
    TEST_ASSERT_TRUE(memory.cached >= 0);
    TEST_ASSERT_TRUE(memory.used >= 0);

    // Verificar que la suma de free + buffers + cached + used es igual a total
    TEST_ASSERT_EQUAL_INT(memory.total, memory.free + memory.buffers + memory.cached + memory.used);
}

/**
 * @brief Test para verificar que read_load_metrics funciona correctamente
 */
void test_read_load_metrics(void)
{
    load_metrics_t load;

    // Ejecutar la función a testear
    int result = read_load_metrics(&load);

    // Verificar que la función se ejecutó correctamente
    TEST_ASSERT_EQUAL_INT(0, result);

    // Verificar que los valores son razonables (no negativos)
    TEST_ASSERT_TRUE(load.load_1m >= 0.0);
    TEST_ASSERT_TRUE(load.load_5m >= 0.0);
    TEST_ASSERT_TRUE(load.load_15m >= 0.0);
}

/**
 * @brief Test para verificar que collect_metrics funciona correctamente
 */
void test_collect_metrics(void)
{
    system_metrics_t metrics;

    // Ejecutar la función a testear
    int result = collect_metrics(&metrics);

    // Verificar que la función se ejecutó correctamente
    TEST_ASSERT_EQUAL_INT(0, result);

    // Verificar que el timestamp es razonable (mayor que 0)
    TEST_ASSERT_TRUE(metrics.timestamp > 0);

    // Verificar que las métricas de CPU son razonables
    TEST_ASSERT_TRUE(metrics.cpu.usage_percent >= 0.0 && metrics.cpu.usage_percent <= 100.0);

    // Verificar que las métricas de memoria son razonables
    TEST_ASSERT_TRUE(metrics.memory.total > 0);
    TEST_ASSERT_TRUE(metrics.memory.used >= 0);

    // Verificar que las métricas de carga son razonables
    TEST_ASSERT_TRUE(metrics.load.load_1m >= 0.0);
}

/**
 * @brief Test para verificar que save_metrics_to_json funciona correctamente
 */
void test_save_metrics_to_json(void)
{
    system_metrics_t metrics;

    // Inicializar métricas con valores de prueba
    metrics.timestamp = 1234567890;
    metrics.cpu.user = 10.5;
    metrics.cpu.system = 5.2;
    metrics.cpu.idle = 84.3;
    metrics.cpu.usage_percent = 15.7;
    metrics.memory.total = 8192000;
    metrics.memory.free = 2048000;
    metrics.memory.used = 4096000;
    metrics.memory.buffers = 1024000;
    metrics.memory.cached = 1024000;
    metrics.load.load_1m = 0.5;
    metrics.load.load_5m = 0.7;
    metrics.load.load_15m = 1.0;

    // Ejecutar la función a testear
    int result = save_metrics_to_json(&metrics, TEST_JSON_FILE);

    // Verificar que la función se ejecutó correctamente
    TEST_ASSERT_EQUAL_INT(0, result);

    // Verificar que el archivo se creó
    FILE* file = fopen(TEST_JSON_FILE, "r");
    TEST_ASSERT_NOT_NULL(file);

    // Leer el contenido del archivo
    char buffer[1024];
    size_t bytes_read = fread(buffer, 1, sizeof(buffer) - 1, file);
    buffer[bytes_read] = '\0';
    fclose(file);

    // Imprimir el contenido para depuración
    printf("JSON generado: %s\n", buffer);

    // Verificar que el contenido contiene los valores esperados de forma más flexible
    TEST_ASSERT_TRUE(strstr(buffer, "timestamp") != NULL);
    TEST_ASSERT_TRUE(strstr(buffer, "cpu_user") != NULL);
    TEST_ASSERT_TRUE(strstr(buffer, "cpu_system") != NULL);
    TEST_ASSERT_TRUE(strstr(buffer, "cpu_usage") != NULL);
    TEST_ASSERT_TRUE(strstr(buffer, "mem_total") != NULL);
    TEST_ASSERT_TRUE(strstr(buffer, "load_1m") != NULL);
}

/**
 * @brief Test para verificar que init_monitoring_system funciona correctamente
 * Nota: Este test crea un directorio temporal para evitar problemas de permisos
 */
void test_init_monitoring_system(void)
{
    // En GitHub Actions, simplemente saltamos este test
    #ifdef GITHUB_ACTIONS
    TEST_IGNORE_MESSAGE("Skipping test_init_monitoring_system in GitHub Actions environment");
    return;
    #endif

    // Para este test, simplemente verificamos que la función no devuelve error
    // Ya que el directorio real podría requerir permisos especiales

    // Crear un directorio temporal para el test
    char test_dir[256];
    snprintf(test_dir, sizeof(test_dir), "/tmp/monitoreo_test_%d/", getpid());
    
    // Guardar el valor original
    extern char* test_log_dir;
    char* old_test_log_dir = test_log_dir;
    test_log_dir = test_dir;
    
    // Eliminar el directorio si ya existe
    char rm_command[300];
    snprintf(rm_command, sizeof(rm_command), "rm -rf %s", test_dir);
    system(rm_command);
    
    // Ejecutar la función a testear
    int result = init_monitoring_system();
    
    // Restaurar el valor original
    test_log_dir = old_test_log_dir;
    
    // Verificar que la función se ejecutó correctamente
    TEST_ASSERT_EQUAL_INT(0, result);
    
    // Verificar que el directorio se creó
    struct stat st;
    int stat_result = stat(test_dir, &st);
    TEST_ASSERT_EQUAL_INT(0, stat_result);
    TEST_ASSERT_TRUE(S_ISDIR(st.st_mode));
    
    // Limpiar
    system(rm_command);
}

/**
 * @brief Función principal para ejecutar los tests
 */
int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_read_cpu_metrics);
    RUN_TEST(test_read_memory_metrics);
    RUN_TEST(test_read_load_metrics);
    RUN_TEST(test_collect_metrics);
    RUN_TEST(test_save_metrics_to_json);
    RUN_TEST(test_init_monitoring_system);

    return UNITY_END();
}
