#include <iostream>
#include <vector>
#include <bitset>
#include <random>
#include <functional>
#include <cmath>
#include <algorithm>
#include <string>
#include <sstream>
#include <memory>
#include <chrono>
#include <map>
#include <iomanip>

using namespace std;
using namespace std::chrono;

// Configuración
const int NUM_FUNCIONES = 4;
const int NUM_LLAVES_PRUEBA = 1000; // Número de llaves a generar para pruebas

// [Todas las funciones existentes: esPrimo, funcionScrambled, funcionGeneracion, funcionMutacion, generarLlaves]

// Función para verificar si un número es primo
bool esPrimo(uint64_t n) {
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;
    
    for (uint64_t i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0)
            return false;
    }
    return true;
}

// Función scrambled para crear la llave embrión
uint64_t funcionScrambled(uint64_t S, uint64_t P) {
    uint64_t resultado = S;
    resultado = resultado ^ (P << 17);
    resultado = (resultado >> 13) | (resultado << (64 - 13));
    resultado = resultado ^ (P >> 5);
    resultado = resultado ^ (P << 29);
    resultado = (resultado << 7) | (resultado >> (64 - 7));
    resultado = resultado ^ (P >> 11);
    resultado = resultado ^ (P << 19);
    resultado = (resultado >> 23) | (resultado << (64 - 23));
    resultado = resultado ^ (P >> 3);
    return resultado;
}

// Función de generación que usa la llave embrión y Q
uint64_t funcionGeneracion(uint64_t embrion, uint64_t Q) {
    uint64_t llave = embrion;
    llave = llave ^ Q;
    llave = (llave << 19) | (llave >> (64 - 19));
    llave = llave + Q;
    llave = llave ^ (Q << 31);
    llave = (llave >> 11) | (llave << (64 - 11));
    llave = llave * (Q | 1);
    llave = llave | 0x1;
    return llave;
}

// Función de mutación que modifica S usando Q
uint64_t funcionMutacion(uint64_t S, uint64_t Q) {
    uint64_t nuevoS = S;
    nuevoS = nuevoS ^ (Q << 17);
    nuevoS = (nuevoS << 13) | (nuevoS >> (64 - 13));
    nuevoS = nuevoS + Q;
    nuevoS = nuevoS ^ (Q >> 7);
    nuevoS = (nuevoS >> 19) | (nuevoS << (64 - 19));
    nuevoS = nuevoS * (Q | 1);
    if (nuevoS == 0) nuevoS = Q ^ 0xFFFFFFFFFFFFFFFF;
    return nuevoS;
}

// Generar N llaves diferentes 
vector<uint64_t> generarLlaves(uint64_t P, uint64_t Q, uint64_t S, int num_llaves) {
    vector<uint64_t> llaves;
    uint64_t semilla = S;
    
    for (int i = 0; i < num_llaves; i++) {
        uint64_t embrion = funcionScrambled(semilla, P);
        uint64_t llave = funcionGeneracion(embrion, Q);
        semilla = funcionMutacion(semilla, Q);
        llaves.push_back(llave);
    }
    return llaves;
}

// Función para calcular la distribución de bits
void analizarDistribucionBits(const vector<uint64_t>& llaves) {
    vector<int> conteoBits(64, 0);
    int totalLlaves = llaves.size();
    
    for (uint64_t llave : llaves) {
        for (int i = 0; i < 64; i++) {
            if (llave & (1ULL << i)) {
                conteoBits[i]++;
            }
        }
    }
    
    cout << "\nDistribución de bits (0-63, donde 0 es el LSB):" << endl;
    for (int i = 0; i < 64; i++) {
        double porcentaje = (conteoBits[i] * 100.0) / totalLlaves;
        cout << "Bit " << setw(2) << i << ": " << setw(5) << conteoBits[i] 
             << " (" << fixed << setprecision(1) << porcentaje << "%)" 
             << (abs(porcentaje - 50.0) > 5.0 ? " *" : "") << endl;
    }
}

// Función para verificar colisiones
void verificarColisiones(const vector<uint64_t>& llaves) {
    map<uint64_t, int> frecuencia;
    int colisiones = 0;
    
    for (uint64_t llave : llaves) {
        if (frecuencia.find(llave) != frecuencia.end()) {
            colisiones++;
        }
        frecuencia[llave]++;
    }
    
    cout << "\nVerificación de colisiones:" << endl;
    cout << "Total de llaves únicas: " << frecuencia.size() << endl;
    cout << "Colisiones encontradas: " << colisiones << endl;
    cout << "Porcentaje de colisiones: " 
         << (colisiones * 100.0 / llaves.size()) << "%" << endl;
}

// Función para calcular el tiempo promedio de generación
double medirTiempoGeneracion(int numPruebas, int numLlaves) {
    uint64_t P = 19;
    uint64_t Q = 18446744073709551533ULL;
    uint64_t S = 12345678901234567890ULL;
    
    double tiempoTotal = 0;
    
    for (int i = 0; i < numPruebas; i++) {
        auto inicio = high_resolution_clock::now();
        
        vector<uint64_t> llaves = generarLlaves(P, Q, S, numLlaves);
        
        auto fin = high_resolution_clock::now();
        tiempoTotal += duration_cast<microseconds>(fin - inicio).count();
    }
    
    return tiempoTotal / numPruebas;
}

// Función principal para evaluación
void evaluarGeneracionLlaves() {
    cout << "=== EVALUACIÓN DE GENERACIÓN DE LLAVES ===" << endl;
    
    uint64_t P = 19;
    uint64_t Q = 18446744073709551533ULL;
    uint64_t S = 12345678901234567890ULL;
    
    // Medir tiempo de generación
    cout << "\n1. TIEMPO DE GENERACIÓN:" << endl;
    
    auto inicio = high_resolution_clock::now();
    vector<uint64_t> llaves = generarLlaves(P, Q, S, NUM_LLAVES_PRUEBA);
    auto fin = high_resolution_clock::now();
    
    auto duracion = duration_cast<microseconds>(fin - inicio);
    cout << "Tiempo para generar " << NUM_LLAVES_PRUEBA << " llaves: " 
         << duracion.count() << " μs" << endl;
    cout << "Tiempo promedio por llave: " 
         << (duracion.count() * 1000.0 / NUM_LLAVES_PRUEBA) << " ns" << endl;
    
    // Tiempo promedio con múltiples pruebas
    int numPruebasTiempo = 10;
    double tiempoPromedio = medirTiempoGeneracion(numPruebasTiempo, 100);
    cout << "Tiempo promedio (10 pruebas, 100 llaves): " 
         << tiempoPromedio << " μs" << endl;
    
    // Análisis estadístico
    cout << "\n2. ANÁLISIS ESTADÍSTICO:" << endl;
    
    // Distribución de bits
    analizarDistribucionBits(llaves);
    
    // Verificación de colisiones
    verificarColisiones(llaves);
    
    // Entropía aproximada (bits distintos)
    cout << "\n3. ENTROPÍA Y VARIABILIDAD:" << endl;
    
    // Generar segundo conjunto con semilla diferente
    uint64_t S2 = S + 1;
    vector<uint64_t> llaves2 = generarLlaves(P, Q, S2, NUM_LLAVES_PRUEBA);
    
    int llavesDiferentes = 0;
    for (int i = 0; i < NUM_LLAVES_PRUEBA; i++) {
        if (llaves[i] != llaves2[i]) {
            llavesDiferentes++;
        }
    }
    
    cout << "Llaves diferentes con cambio de semilla: " << llavesDiferentes 
         << " de " << NUM_LLAVES_PRUEBA << " (" 
         << (llavesDiferentes * 100.0 / NUM_LLAVES_PRUEBA) << "%)" << endl;
    
    // Mostrar algunas llaves de ejemplo
    cout << "\n4. EJEMPLOS DE LLAVES:" << endl;
    for (int i = 0; i < min(5, NUM_LLAVES_PRUEBA); i++) {
        cout << "Llave " << i+1 << ": 0x" << hex << llaves[i] << dec << endl;
    }
    
    cout << "\n=== EVALUACIÓN COMPLETADA ===" << endl;
}

int main() {
    try {
        // Ejecutar evaluación
        evaluarGeneracionLlaves();
        
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}