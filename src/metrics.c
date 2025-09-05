/**
 * @file metrics.c
 * @brief Implementación de funciones para recolectar métricas del sistema
 */

#include "cJSON.h"
#include "monitoreo.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

// Variable global para testing
char* test_log_dir = NULL;

/**
 * @brief Crea directorios recursivamente
 * @param path Ruta del directorio a crear
 * @return 0 en caso de éxito, -1 en caso de error
 */
static int create_directory_recursive(const char* path)
{
    char tmp[256];
    char* p = NULL;
    size_t len;

    snprintf(tmp, sizeof(tmp), "%s", path);
    len = strlen(tmp);

    // Asegurarse de que la ruta termina con '/'
    if (tmp[len - 1] == '/')
    {
        tmp[len - 1] = 0;
    }

    // Crear directorios recursivamente
    for (p = tmp + 1; *p; p++)
    {
        if (*p == '/')
        {
            *p = 0;
            if (mkdir(tmp, 0755) != 0 && errno != EEXIST)
            {
                return -1;
            }
            *p = '/';
        }
    }

    // Crear el directorio final
    if (mkdir(tmp, 0755) != 0 && errno != EEXIST)
    {
        return -1;
    }

    return 0;
}

/**
 * @brief Lee las métricas de CPU desde /proc/stat
 * @param cpu Puntero a la estructura donde se guardarán las métricas
 * @return 0 en caso de éxito, -1 en caso de error
 */
int read_cpu_metrics(cpu_metrics_t* cpu)
{
    FILE* file = fopen("/proc/stat", "r");
    if (file == NULL)
    {
        perror("Error al abrir /proc/stat");
        return -1;
    }

    // Variables para almacenar los valores leídos de /proc/stat
    unsigned long user, nice, system, idle, iowait, irq, softirq, steal;

    // Leer la primera línea que contiene los datos de CPU
    int result = fscanf(file, "cpu %lu %lu %lu %lu %lu %lu %lu %lu", &user, &nice, &system, &idle, &iowait, &irq,
                        &softirq, &steal);
    fclose(file);

    if (result != 8)
    {
        fprintf(stderr, "Error al leer datos de CPU desde /proc/stat\n");
        return -1;
    }

    // Calcular el tiempo total y el tiempo activo
    unsigned long total_time = user + nice + system + idle + iowait + irq + softirq + steal;
    unsigned long active_time = user + nice + system + irq + softirq + steal;

    // Almacenar los valores en la estructura
    cpu->user = (double)user / (double)total_time * 100.0;
    cpu->system = (double)system / (double)total_time * 100.0;
    cpu->idle = (double)idle / (double)total_time * 100.0;
    cpu->usage_percent = (double)active_time / (double)total_time * 100.0;

    return 0;
}

/**
 * @brief Lee las métricas de memoria desde /proc/meminfo
 * @param memory Puntero a la estructura donde se guardarán las métricas
 * @return 0 en caso de éxito, -1 en caso de error
 */
int read_memory_metrics(memory_metrics_t* memory)
{
    FILE* file = fopen("/proc/meminfo", "r");
    if (file == NULL)
    {
        perror("Error al abrir /proc/meminfo");
        return -1;
    }

    char line[256];
    long mem_total = 0;
    long mem_free = 0;
    long buffers = 0;
    long cached = 0;

    // Leer línea por línea buscando los valores necesarios
    while (fgets(line, sizeof(line), file))
    {
        if (strncmp(line, "MemTotal:", 9) == 0)
        {
            sscanf(line, "MemTotal: %ld", &mem_total);
        }
        else if (strncmp(line, "MemFree:", 8) == 0)
        {
            sscanf(line, "MemFree: %ld", &mem_free);
        }
        else if (strncmp(line, "Buffers:", 8) == 0)
        {
            sscanf(line, "Buffers: %ld", &buffers);
        }
        else if (strncmp(line, "Cached:", 7) == 0 && strncmp(line, "SwapCached:", 11) != 0)
        {
            sscanf(line, "Cached: %ld", &cached);
        }
    }

    fclose(file);

    // Verificar que se hayan leído todos los valores
    if (mem_total == 0)
    {
        fprintf(stderr, "Error al leer datos de memoria desde /proc/meminfo\n");
        return -1;
    }

    // Almacenar los valores en la estructura
    memory->total = mem_total;
    memory->free = mem_free;
    memory->buffers = buffers;
    memory->cached = cached;
    memory->used = mem_total - mem_free - buffers - cached;

    return 0;
}

/**
 * @brief Lee las métricas de carga desde /proc/loadavg
 * @param load Puntero a la estructura donde se guardarán las métricas
 * @return 0 en caso de éxito, -1 en caso de error
 */
int read_load_metrics(load_metrics_t* load)
{
    FILE* file = fopen("/proc/loadavg", "r");
    if (file == NULL)
    {
        perror("Error al abrir /proc/loadavg");
        return -1;
    }

    // Leer los tres valores de carga
    int result = fscanf(file, "%lf %lf %lf", &load->load_1m, &load->load_5m, &load->load_15m);
    fclose(file);

    if (result != 3)
    {
        fprintf(stderr, "Error al leer datos de carga desde /proc/loadavg\n");
        return -1;
    }

    return 0;
}

/**
 * @brief Inicializa el sistema de monitoreo
 * @return 0 en caso de éxito, -1 en caso de error
 */
int init_monitoring_system(void)
{
    // Usar directorio de test si está definido
    const char* log_dir = test_log_dir != NULL ? test_log_dir : DEFAULT_LOG_DIR;

    // Crear el directorio de logs si no existe
    struct stat st = {0};
    if (stat(log_dir, &st) == -1)
    {
        // Intentar crear el directorio recursivamente
        if (create_directory_recursive(log_dir) != 0)
        {
            perror("Error al crear directorio de logs");
            return -1;
        }
    }
    return 0;
}

/**
 * @brief Recolecta todas las métricas del sistema
 * @param metrics Puntero a la estructura donde se guardarán las métricas
 * @return 0 en caso de éxito, -1 en caso de error
 */
int collect_metrics(system_metrics_t* metrics)
{
    if (metrics == NULL)
    {
        return -1;
    }

    // Registrar el timestamp actual
    metrics->timestamp = time(NULL);

    // Recolectar métricas de CPU, memoria y carga
    if (read_cpu_metrics(&metrics->cpu) != 0)
    {
        return -1;
    }

    if (read_memory_metrics(&metrics->memory) != 0)
    {
        return -1;
    }

    if (read_load_metrics(&metrics->load) != 0)
    {
        return -1;
    }

    return 0;
}

/**
 * @brief Guarda las métricas en formato JSON en un archivo
 * @param metrics Puntero a la estructura con las métricas a guardar
 * @param filepath Ruta del archivo donde se guardarán las métricas
 * @return 0 en caso de éxito, -1 en caso de error
 */
int save_metrics_to_json(const system_metrics_t* metrics, const char* filepath)
{
    if (metrics == NULL || filepath == NULL)
    {
        return -1;
    }

    // Crear el objeto JSON
    cJSON* json = cJSON_CreateObject();
    if (json == NULL)
    {
        return -1;
    }

    // Agregar timestamp
    cJSON_AddNumberToObject(json, "timestamp", (double)metrics->timestamp);

    // Agregar métricas de CPU
    cJSON_AddNumberToObject(json, "cpu_user", metrics->cpu.user);
    cJSON_AddNumberToObject(json, "cpu_system", metrics->cpu.system);
    cJSON_AddNumberToObject(json, "cpu_usage", metrics->cpu.usage_percent);

    // Agregar métricas de memoria - convertir explícitamente a double para evitar warnings
    cJSON_AddNumberToObject(json, "mem_total", (double)metrics->memory.total);
    cJSON_AddNumberToObject(json, "mem_free", (double)metrics->memory.free);
    cJSON_AddNumberToObject(json, "mem_used", (double)metrics->memory.used);

    // Agregar métricas de carga
    cJSON_AddNumberToObject(json, "load_1m", metrics->load.load_1m);
    cJSON_AddNumberToObject(json, "load_5m", metrics->load.load_5m);
    cJSON_AddNumberToObject(json, "load_15m", metrics->load.load_15m);

    // Convertir a string
    char* json_string = cJSON_Print(json);
    cJSON_Delete(json);

    if (json_string == NULL)
    {
        return -1;
    }

    // Abrir el archivo en modo append
    FILE* file = fopen(filepath, "a");
    if (file == NULL)
    {
        free(json_string);
        return -1;
    }

    // Escribir el JSON y un salto de línea
    fprintf(file, "%s\n", json_string);
    fclose(file);
    free(json_string);

    return 0;
}

/**
 * @brief Limpia los recursos utilizados por el sistema de monitoreo
 */
void cleanup_monitoring_system(void)
{
    // Por ahora no hay recursos que limpiar
}
