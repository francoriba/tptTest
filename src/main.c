#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "cJSON.h"
#include "monitoreo.h"

int main() {
    printf("=== Sistema de Monitoreo Básico ===\n");
    printf("Versión 1.0.0\n\n");
    
    // Test básico de cJSON
    cJSON *json = cJSON_CreateObject();
    if (json == NULL) {
        fprintf(stderr, "Error: No se pudo crear objeto JSON\n");
        return EXIT_FAILURE;
    }
    
    // Agregar timestamp actual
    time_t current_time = time(NULL);
    cJSON *timestamp = cJSON_CreateNumber((double)current_time);
    cJSON_AddItemToObject(json, "timestamp", timestamp);
    
    // Agregar mensaje de prueba
    cJSON *message = cJSON_CreateString("Hello World - Sistema funcionando correctamente");
    cJSON_AddItemToObject(json, "message", message);
    
    // Imprimir JSON
    char *json_string = cJSON_Print(json);
    if (json_string != NULL) {
        printf("JSON de prueba:\n%s\n", json_string);
        free(json_string);
    }
    
    // Limpiar memoria
    cJSON_Delete(json);
    
    printf("\n✅ Test básico completado exitosamente\n");
    printf("📋 cJSON está funcionando correctamente\n");
    printf("🔧 El proyecto está listo para desarrollo\n");
    
    return EXIT_SUCCESS;
}
