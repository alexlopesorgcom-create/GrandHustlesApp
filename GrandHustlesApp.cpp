#ifdef _WIN32
#define NOMINMAX
#include <windows.h> // Necesario para SetConsoleOutputCP y habilitar VT en Windows
// Prevent Windows' min/max macros from colliding with std::min/std::max
#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif
#endif
#include <iostream>
#include <string>
#include <limits>
#include <vector>
#include <algorithm>
#include <cstdio>
#include <fstream>
#include <sstream>

// Colores ANSI para la consola
static constexpr const char* RED = "\x1b[1;31m";
static constexpr const char* GREEN = "\x1b[1;32m";
static constexpr const char* YELLOW = "\x1b[1;33m";
static constexpr const char* CYAN = "\x1b[1;36m";
static constexpr const char* RESET = "\x1b[0m";
static constexpr const char* BOLD = "\x1b[1m";

// Prototipos de funciones
void showCatalog();
void showDropStatus();
void exitProgram();
void adminPanel();
bool authenticateAdmin();
void viewOrders();
void addProduct();
void removeProduct();
void loadData();
void saveData();

// Ruta del archivo de datos
static const std::string DATA_FILE = "data.json";

// Datos simples en memoria (simulación)
static std::vector<std::string> productList = {
    "Hellstar Tee - $120",
    "BAPE Sta Tee - $110",
    "Cap Collection (Barbas/Chucky) - $45",
    "Limited Drop Hoodie - $150"
};

static std::vector<std::string> orderList = {
    "Orden #1001 - Hellstar Tee - Usuario: user1",
    "Orden #1002 - Limited Drop Hoodie - Usuario: user2"
};

// --- Utilidades JSON mínimas ---
static std::string escapeJsonString(const std::string &input) {
    std::string s;
    for (char c : input) {
        switch (c) {
            case '"': s += "\\\""; break;
            case '\\': s += "\\\\"; break;
            case '\b': s += "\\b"; break;
            case '\f': s += "\\f"; break;
            case '\n': s += "\\n"; break;
            case '\r': s += "\\r"; break;
            case '\t': s += "\\t"; break;
            default:
                s += c;
        }
    }
    return s;
}

static std::string unescapeJsonString(const std::string &input) {
    std::string out;
    out.reserve(input.size());
    for (size_t i = 0; i < input.size(); ++i) {
        char c = input[i];
        if (c == '\\' && i + 1 < input.size()) {
            char n = input[++i];
            switch (n) {
                case '"': out += '"'; break;
                case '\\': out += '\\'; break;
                case 'b': out += '\b'; break;
                case 'f': out += '\f'; break;
                case 'n': out += '\n'; break;
                case 'r': out += '\r'; break;
                case 't': out += '\t'; break;
                default: out += n; break;
            }
        } else {
            out += c;
        }
    }
    return out;
}

static std::vector<std::string> parseJsonArray(const std::string &json, const std::string &key) {
    std::vector<std::string> result;
    std::string needle = "\"" + key + "\"";
    size_t kpos = json.find(needle);
    if (kpos == std::string::npos) return result;
    size_t arrStart = json.find('[', kpos);
    if (arrStart == std::string::npos) return result;
    size_t pos = arrStart + 1;
    while (pos < json.size()) {
        // skip whitespace
        while (pos < json.size() && isspace((unsigned char)json[pos])) pos++;
        if (pos >= json.size() || json[pos] == ']') break;
        if (json[pos] == '"') {
            pos++;
            std::string cur;
            while (pos < json.size()) {
                char c = json[pos++];
                if (c == '\\' && pos < json.size()) {
                    char esc = json[pos++];
                    switch (esc) {
                        case '"': cur += '"'; break;
                        case '\\': cur += '\\'; break;
                        case 'b': cur += '\b'; break;
                        case 'f': cur += '\f'; break;
                        case 'n': cur += '\n'; break;
                        case 'r': cur += '\r'; break;
                        case 't': cur += '\t'; break;
                        default: cur += esc; break;
                    }
                } else if (c == '"') {
                    break;
                } else {
                    cur += c;
                }
            }
            result.push_back(cur);
            // move to comma or end
            while (pos < json.size() && json[pos] != ',' && json[pos] != ']') pos++;
            if (pos < json.size() && json[pos] == ',') pos++;
            continue;
        }
        pos++;
    }
    return result;
}

void loadData() {
    std::ifstream in(DATA_FILE);
    if (!in) return; // no data file, keep defaults
    std::stringstream ss;
    ss << in.rdbuf();
    std::string content = ss.str();
    auto p = parseJsonArray(content, "products");
    if (!p.empty()) productList = p;
    auto o = parseJsonArray(content, "orders");
    if (!o.empty()) orderList = o;
}

void saveData() {
    std::ofstream out(DATA_FILE, std::ios::trunc);
    if (!out) return;
    out << "{\n";
    out << "  \"products\": [\n";
    for (size_t i = 0; i < productList.size(); ++i) {
        out << "    \"" << escapeJsonString(productList[i]) << "\"";
        if (i + 1 < productList.size()) out << ",\n"; else out << "\n";
    }
    out << "  ],\n";
    out << "  \"orders\": [\n";
    for (size_t i = 0; i < orderList.size(); ++i) {
        out << "    \"" << escapeJsonString(orderList[i]) << "\"";
        if (i + 1 < orderList.size()) out << ",\n"; else out << "\n";
    }
    out << "  ]\n";
    out << "}\n";
}

int main()
{
    // Cargar datos desde archivo JSON (si existe)
    loadData();

    // Configurar la consola para soportar caracteres UTF-8 (acentos y bordes)
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    // Intentar habilitar VT (secuencias ANSI) en la consola de Windows
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode)) {
            dwMode |= 0x0004; // ENABLE_VIRTUAL_TERMINAL_PROCESSING
            SetConsoleMode(hOut, dwMode);
        }
    }

    // Banner ASCII estilo Streetwear
    std::cout << RED << R"(  ___ ___    _   _  _ ___    _  _ _  _ ___ _____ _    ___ ___ 
 / __| _ \  /_\ | \| |   \  | || | || | __|_   _| |  | __/ __|
| (_ |   / / _ \| .` | |) | | __ | || |__ \ | | | |__| _|\__ \
 \___|_|_\/_/ \_\_|_|_|___/  |_||_\\_/|___/ |_| |____|___|___/
)" << RESET << std::endl;

    std::cout << CYAN << "================================================================" << RESET << std::endl;
    std::cout << "          BIENVENIDO A LA CONSOLA OFICIAL DE GRAND HUSTLES        " << std::endl;
    std::cout << CYAN << "================================================================" << RESET << std::endl << std::endl;

    std::cout << "Por favor, ingresa tu nombre: ";
    std::string name;
    std::getline(std::cin, name);
    if (name.empty()) {
        name = "Invitado";
    }

    std::cout << GREEN << "\n¡Hola, " << name << "! Bienvenido al sistema de Grand Hustles." << RESET << std::endl << std::endl;

    int choice = 0;
    do {
        std::cout << BOLD << "╔════════════════════════════════════════╗" << RESET << std::endl;
        std::cout << BOLD << "║           MENÚ PRINCIPAL               ║" << RESET << std::endl;
        std::cout << BOLD << "╠════════════════════════════════════════╣" << RESET << std::endl;
        std::cout << "║  1. " << YELLOW << "Ver catálogo de productos" << RESET << "                     ║" << std::endl;
        std::cout << "║  2. " << RED << "Estado del lanzamiento (Drop)" << RESET << "               ║" << std::endl;
        std::cout << "║  3. " << CYAN << "Panel Admin" << RESET << "                              ║" << std::endl;
        std::cout << "║  4. " << CYAN << "Salir" << RESET << "                                   ║" << std::endl;
        std::cout << BOLD << "╚════════════════════════════════════════╝" << RESET << std::endl;
        std::cout << "Selecciona una opción (1-4): ";

        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
            std::cout << RED << "\nEntrada inválida. Ingresa un número entre 1 y 4.\n" << RESET << std::endl;
            continue;
        }

        std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

        switch (choice) {
        case 1:
            showCatalog();
            break;
        case 2:
            showDropStatus();
            break;
        case 3:
            adminPanel();
            break;
        case 4:
            exitProgram();
            break;
        default:
            std::cout << RED << "\nOpción no válida. Intenta nuevamente.\n" << RESET << std::endl;
            break;
        }

        std::cout << std::endl;
    } while (choice != 4);

    // Guardar datos antes de salir
    saveData();

    return 0;
}

void showCatalog()
{
    std::cout << std::endl << YELLOW << "┌────────────────────────────────────────┐" << RESET << std::endl;
    std::cout << YELLOW << "│        CATÁLOGO - GRAND HUSTLES        │" << RESET << std::endl;
    std::cout << YELLOW << "├────────────────────────────────────────┤" << RESET << std::endl;
    for (size_t i = 0; i < productList.size(); ++i) {
        char buf[256];
        // Imprime cada producto con índice
        std::snprintf(buf, sizeof(buf), "│ %2zu) %-34s │", i + 1, productList[i].c_str());
        std::cout << buf << std::endl;
    }
    std::cout << YELLOW << "└────────────────────────────────────────┘" << RESET << std::endl;
    std::cout << "Pedidos vía Instagram DM: " << GREEN << "@grand_hustles" << RESET << std::endl;
}

void showDropStatus()
{
    std::cout << std::endl << RED << "┌────────────────────────────────────────┐" << RESET << std::endl;
    std::cout << RED << "│       PRÓXIMO DROP / LANZAMIENTO       │" << RESET << std::endl;
    std::cout << RED << "├────────────────────────────────────────┤" << RESET << std::endl;
    std::cout << "│ Estado: " << GREEN << "EN PREPARACIÓN" << RESET << "                 │" << std::endl;
    std::cout << "│ Unidades disponibles: 350              │" << std::endl;
    std::cout << "│ Exclusividad: Edición Limitada         │" << std::endl;
    std::cout << RED << "└────────────────────────────────────────┘" << RESET << std::endl;
}

void exitProgram()
{
    // Guardar antes de salir
    saveData();
    std::cout << std::endl << GREEN << "¡Gracias por visitar Grand Hustles! Nos vemos en el próximo Drop." << RESET << std::endl;
}

// ---------------- Admin Panel y utilidades ----------------
bool authenticateAdmin()
{
    const std::string adminPassword = "admin123"; // Cambia esto en producción
    std::string input;
    std::cout << CYAN << "Ingresa la contraseña de administrador: " << RESET;
    std::getline(std::cin, input);
    return (input == adminPassword);
}

void viewOrders()
{
    std::cout << std::endl << YELLOW << "--- Pedidos registrados (simulación) ---" << RESET << std::endl;
    if (orderList.empty()) {
        std::cout << "No hay pedidos por mostrar." << std::endl;
    } else {
        for (const auto &o : orderList) {
            std::cout << "- " << o << std::endl;
        }
    }
}

void addProduct()
{
    std::cout << CYAN << "Nombre del nuevo producto (ej: " << YELLOW << "Cool Tee - $99" << CYAN << "): " << RESET;
    std::string p;
    std::getline(std::cin, p);
    if (!p.empty()) {
        productList.push_back(p);
        saveData();
        std::cout << GREEN << "Producto agregado." << RESET << std::endl;
    } else {
        std::cout << RED << "Entrada vacía. Operación cancelada." << RESET << std::endl;
    }
}

void removeProduct()
{
    showCatalog();
    std::cout << CYAN << "Ingresa el número del producto a eliminar: " << RESET;
    int idx = 0;
    if (!(std::cin >> idx)) {
        std::cin.clear();
        std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
        std::cout << RED << "Entrada inválida." << RESET << std::endl;
        return;
    }
    std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
    if (idx >= 1 && static_cast<size_t>(idx) <= productList.size()) {
        productList.erase(productList.begin() + (idx - 1));
        saveData();
        std::cout << GREEN << "Producto eliminado." << RESET << std::endl;
    } else {
        std::cout << RED << "Índice fuera de rango." << RESET << std::endl;
    }
}

void adminPanel()
{
    // Autenticación
    std::string dummy;
    std::cout << std::endl;
    // no need to clear buffer here; authenticateAdmin reads a line
    if (!authenticateAdmin()) {
        std::cout << RED << "Autenticación fallida. Acceso denegado." << RESET << std::endl;
        return;
    }

    std::cout << GREEN << "Acceso de administrador concedido." << RESET << std::endl;
    int aChoice = 0;
    do {
        std::cout << BOLD << "\n╔══════════════ Panel Admin ══════════════╗" << RESET << std::endl;
        std::cout << "║ 1. Ver productos                       ║" << std::endl;
        std::cout << "║ 2. Agregar producto                    ║" << std::endl;
        std::cout << "║ 3. Eliminar producto                   ║" << std::endl;
        std::cout << "║ 4. Ver pedidos                         ║" << std::endl;
        std::cout << "║ 5. Salir admin                         ║" << std::endl;
        std::cout << BOLD << "╚════════════════════════════════════════╝" << RESET << std::endl;
        std::cout << "Selecciona una opción (1-5): ";

        if (!(std::cin >> aChoice)) {
            std::cin.clear();
            std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
            std::cout << RED << "Entrada inválida." << RESET << std::endl;
            continue;
        }
        std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

        switch (aChoice) {
        case 1:
            showCatalog();
            break;
        case 2:
            addProduct();
            break;
        case 3:
            removeProduct();
            break;
        case 4:
            viewOrders();
            break;
        case 5:
            std::cout << GREEN << "Saliendo del panel admin." << RESET << std::endl;
            break;
        default:
            std::cout << RED << "Opción no válida." << RESET << std::endl;
            break;
        }
    } while (aChoice != 5);
}