# Monitoreo básico y observabilidad – Detalles resumidos

## Propósito
El objetivo de este primer trabajo es implementar un módulo de observabilidad para el nodo local de monitoreo sanitario.  
El módulo deberá **capturar, procesar y registrar métricas clave del sistema operativo**, garantizando que la información sea precisa, estructurada y accesible para su consulta local y futura integración con herramientas de visualización como Grafana.

---

## Alcance funcional
El sistema debe:

- Obtener métricas en tiempo real desde el pseudo-sistema de archivos `/proc`.
- Registrar la información en formato **JSON** en un directorio predefinido: `/var/lib/monitoreo/`.
- Capturar métricas en intervalos regulares (mínimo cada 5 segundos, configurable).
- Permitir la consulta inmediata de métricas actuales mediante Grafana u otra herramienta de observabilidad que lea directamente los registros.
- Evitar dependencias externas complejas: desarrollo con **CMake** y **Conan**, utilizando funciones estándar y llamadas al sistema.

---

## Observabilidad en el contexto del proyecto
La **observabilidad** es la capacidad de un sistema para exponer información relevante sobre su estado interno a partir de sus salidas externas.

En este caso, el nodo debe describir:

- Qué recursos utiliza (CPU, memoria, disco).
- En qué condiciones está operando (carga, procesos activos).
- Cómo evoluciona su estado a lo largo del tiempo.

**Beneficios:**
- Detectar problemas antes de que se conviertan en fallas críticas.
- Analizar el rendimiento y la capacidad del sistema.
- Proporcionar datos confiables para la toma de decisiones de mantenimiento o ajuste.

---

## Directorios y archivos relevantes
- **CPU:** `/proc/stat` → estadísticas generales de CPU (usuario, sistema, idle, etc.).
- **Memoria:** `/proc/meminfo` → memoria física total, memoria libre, buffers y cachés.
- **Carga del sistema:** `/proc/loadavg` → carga media en intervalos de 1, 5 y 15 minutos.
- **Procesos:** `/proc/[pid]/status` → información detallada de un proceso en particular *(opcional para futuras extensiones)*.

⚠️ **Nota:** No depender de utilitarios externos como `top` o `free`; se debe parsear directamente la información en `/proc`.

---

## Métricas mínimas a capturar

### CPU
- Tiempo total de CPU usado por usuario.
- Tiempo total de CPU usado por el sistema.
- Porcentaje de utilización de CPU.

### Memoria
- Memoria total.
- Memoria libre.
- Memoria usada (`total - libre - buffers - caché`).

### Carga
- Promedio de carga en 1, 5 y 15 minutos.

### Tiempo
- Timestamp de la medición (formato **UNIX epoch** o **ISO8601**).

---

## Estructura de almacenamiento de métricas
- **Directorio principal:** `/var/lib/monitoreo/`
- **Nombre sugerido del archivo:** `metrics-YYYYMMDD.log`
- **Formato:** **NDJSON** (un objeto JSON por línea)

**Ejemplo:**
```json
{"timestamp":1693262400,"cpu_user":23.5,"cpu_system":12.1,"mem_total":8048580,"mem_free":1456292,"load_1m":0.42,"load_5m":0.35,"load_15m":0.30}
