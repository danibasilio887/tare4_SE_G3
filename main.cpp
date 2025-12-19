/*
codigo del main.cpp utilizado para la esp32 feather en el entorno de Platformio en Visual Studio Code
*/

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DFRobotDFPlayerMini.h"
#include "sprites.h" // <--- IMPORTANTE: Carga las imagenes

// --- 1. CONFIGURACIÓN PANTALLA ---
#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 
#define PIN_SDA 23
#define PIN_SCL 22

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// --- 2. CONFIGURACIÓN AUDIO ---
HardwareSerial myHardwareSerial(2); 
DFRobotDFPlayerMini player;

// --- 3. PINES Y VARIABLES ---
const int btnSiguiente = 32; 
const int btnAnterior = 33;  
const int pinPotenciometro = 26; 

int pokemonActual = 1;
const int totalPokemon = 151; 
unsigned long ultimaAccion = 0;
const int tiempoEspera = 400;   // Un poco más rápido
unsigned long ultimoCheckVolumen = 0;
int volumenAnterior = -1;

// Prototipos
String obtenerNombre(int id);
void actualizarSistema();
void verificarVolumen();

void setup() {
  Serial.begin(115200);

  // A. PANTALLA
  Wire.begin(PIN_SDA, PIN_SCL); 
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("OLED Falla"));
    for(;;); 
  }
  
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 20);
  display.println(F("Cargando Pokedex..."));
  display.setCursor(0, 40);
  display.print(F("Audio: "));
  display.display();

  // B. AUDIO (Modo Bloqueo hasta que responda)
  myHardwareSerial.begin(9600, SERIAL_8N1, 16, 17);
  
  int intentos = 0;
  while (!player.begin(myHardwareSerial)) {
    display.print(F("."));
    display.display();
    intentos++;
    if (intentos > 14) {
      display.fillRect(40, 40, 80, 10, BLACK); 
      display.setCursor(40, 40);
      intentos = 0;
    }
    delay(800); 
  }

  // Éxito
  display.println(F(" OK!"));
  display.display();
  delay(500);
  player.volume(20); 

  // C. BOTONES
  pinMode(btnSiguiente, INPUT_PULLUP);
  pinMode(btnAnterior, INPUT_PULLUP);
  
  actualizarSistema();
}

void loop() {
  // Lógica Botones
  if (millis() - ultimaAccion > tiempoEspera) {
    if (digitalRead(btnSiguiente) == LOW) {
      pokemonActual++;
      if (pokemonActual > totalPokemon) pokemonActual = 1;
      actualizarSistema();
      ultimaAccion = millis();
    }
    if (digitalRead(btnAnterior) == LOW) {
      pokemonActual--;
      if (pokemonActual < 1) pokemonActual = totalPokemon;
      actualizarSistema();
      ultimaAccion = millis();
    }
  }

  // Lógica Volumen
  if (millis() - ultimoCheckVolumen > 100) {
    verificarVolumen();
    ultimoCheckVolumen = millis();
  }
}

void verificarVolumen() {
  int lectura = analogRead(pinPotenciometro); 
  int volumenDeseado = map(lectura, 0, 4095, 0, 30);
  if (abs(volumenDeseado - volumenAnterior) > 1) {
    player.volume(volumenDeseado);
    volumenAnterior = volumenDeseado;
  }
}

void actualizarSistema() {
  display.clearDisplay();
  display.setTextColor(WHITE);
  
  // -- CABECERA --
  display.setCursor(118,0); 
  display.write(14); // Nota musical
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(F("POKEDEX ESP32"));
  
  // -- NUMERO --
  display.setTextSize(2);
  display.setCursor(0, 20);
  display.print("#"); 
  if(pokemonActual < 10) display.print("00");
  else if(pokemonActual < 100) display.print("0");
  display.println(pokemonActual);
  
  // -- NOMBRE --
  display.setTextSize(1);
  display.setCursor(0, 45);
  display.println(obtenerNombre(pokemonActual));

  // -- IMAGEN (Lógica Inteligente) --
  // Buscamos en el array sprites.h si existe imagen para este ID
  const unsigned char* imagenActual = NULL;
  if (pokemonActual < 152) {
      imagenActual = listaSprites[pokemonActual];
  }

  if (imagenActual != NULL) {
    // Si hay imagen en la lista, la dibuja
    display.drawBitmap(80, 15, imagenActual, 32, 32, WHITE);
  } else {
    // Si es NULL (0), dibuja interrogación
    display.drawRect(80, 15, 32, 32, WHITE);
    display.setCursor(89, 24); // Centrado a ojo
    display.setTextSize(2);
    display.print("?");
  }
  
  display.display();

  player.play(pokemonActual); 
}

// --- BASE DE DATOS COMPLETA ---
String obtenerNombre(int id) {
  switch (id) {
    case 1: return F("Bulbasaur");
    case 2: return F("Ivysaur");
    case 3: return F("Venusaur");
    case 4: return F("Charmander");
    case 5: return F("Charmeleon");
    case 6: return F("Charizard");
    case 7: return F("Squirtle");
    case 8: return F("Wartortle");
    case 9: return F("Blastoise");
    case 10: return F("Caterpie");
    case 11: return F("Metapod");
    case 12: return F("Butterfree");
    case 13: return F("Weedle");
    case 14: return F("Kakuna");
    case 15: return F("Beedrill");
    case 16: return F("Pidgey");
    case 17: return F("Pidgeotto");
    case 18: return F("Pidgeot");
    case 19: return F("Rattata");
    case 20: return F("Raticate");
    case 21: return F("Spearow");
    case 22: return F("Fearow");
    case 23: return F("Ekans");
    case 24: return F("Arbok");
    case 25: return F("Pikachu");
    case 26: return F("Raichu");
    case 27: return F("Sandshrew");
    case 28: return F("Sandslash");
    case 29: return F("Nidoran F");
    case 30: return F("Nidorina");
    case 31: return F("Nidoqueen");
    case 32: return F("Nidoran M");
    case 33: return F("Nidorino");
    case 34: return F("Nidoking");
    case 35: return F("Clefairy");
    case 36: return F("Clefable");
    case 37: return F("Vulpix");
    case 38: return F("Ninetales");
    case 39: return F("Jigglypuff");
    case 40: return F("Wigglytuff");
    case 41: return F("Zubat");
    case 42: return F("Golbat");
    case 43: return F("Oddish");
    case 44: return F("Gloom");
    case 45: return F("Vileplume");
    case 46: return F("Paras");
    case 47: return F("Parasect");
    case 48: return F("Venonat");
    case 49: return F("Venomoth");
    case 50: return F("Diglett");
    case 51: return F("Dugtrio");
    case 52: return F("Meowth");
    case 53: return F("Persian");
    case 54: return F("Psyduck");
    case 55: return F("Golduck");
    case 56: return F("Mankey");
    case 57: return F("Primeape");
    case 58: return F("Growlithe");
    case 59: return F("Arcanine");
    case 60: return F("Poliwag");
    case 61: return F("Poliwhirl");
    case 62: return F("Poliwrath");
    case 63: return F("Abra");
    case 64: return F("Kadabra");
    case 65: return F("Alakazam");
    case 66: return F("Machop");
    case 67: return F("Machoke");
    case 68: return F("Machamp");
    case 69: return F("Bellsprout");
    case 70: return F("Weepinbell");
    case 71: return F("Victreebel");
    case 72: return F("Tentacool");
    case 73: return F("Tentacruel");
    case 74: return F("Geodude");
    case 75: return F("Graveler");
    case 76: return F("Golem");
    case 77: return F("Ponyta");
    case 78: return F("Rapidash");
    case 79: return F("Slowpoke");
    case 80: return F("Slowbro");
    case 81: return F("Magnemite");
    case 82: return F("Magneton");
    case 83: return F("Farfetch'd");
    case 84: return F("Doduo");
    case 85: return F("Dodrio");
    case 86: return F("Seel");
    case 87: return F("Dewgong");
    case 88: return F("Grimer");
    case 89: return F("Muk");
    case 90: return F("Shellder");
    case 91: return F("Cloyster");
    case 92: return F("Gastly");
    case 93: return F("Haunter");
    case 94: return F("Gengar");
    case 95: return F("Onix");
    case 96: return F("Drowzee");
    case 97: return F("Hypno");
    case 98: return F("Krabby");
    case 99: return F("Kingler");
    case 100: return F("Voltorb");
    case 101: return F("Electrode");
    case 102: return F("Exeggcute");
    case 103: return F("Exeggutor");
    case 104: return F("Cubone");
    case 105: return F("Marowak");
    case 106: return F("Hitmonlee");
    case 107: return F("Hitmonchan");
    case 108: return F("Lickitung");
    case 109: return F("Koffing");
    case 110: return F("Weezing");
    case 111: return F("Rhyhorn");
    case 112: return F("Rhydon");
    case 113: return F("Chansey");
    case 114: return F("Tangela");
    case 115: return F("Kangaskhan");
    case 116: return F("Horsea");
    case 117: return F("Seadra");
    case 118: return F("Goldeen");
    case 119: return F("Seaking");
    case 120: return F("Staryu");
    case 121: return F("Starmie");
    case 122: return F("Mr. Mime");
    case 123: return F("Scyther");
    case 124: return F("Jynx");
    case 125: return F("Electabuzz");
    case 126: return F("Magmar");
    case 127: return F("Pinsir");
    case 128: return F("Tauros");
    case 129: return F("Magikarp");
    case 130: return F("Gyarados");
    case 131: return F("Lapras");
    case 132: return F("Ditto");
    case 133: return F("Eevee");
    case 134: return F("Vaporeon");
    case 135: return F("Jolteon");
    case 136: return F("Flareon");
    case 137: return F("Porygon");
    case 138: return F("Omanyte");
    case 139: return F("Omastar");
    case 140: return F("Kabuto");
    case 141: return F("Kabutops");
    case 142: return F("Aerodactyl");
    case 143: return F("Snorlax");
    case 144: return F("Articuno");
    case 145: return F("Zapdos");
    case 146: return F("Moltres");
    case 147: return F("Dratini");
    case 148: return F("Dragonair");
    case 149: return F("Dragonite");
    case 150: return F("Mewtwo");
    case 151: return F("Mew");
    default: return F("Desconocido");
  }
}
