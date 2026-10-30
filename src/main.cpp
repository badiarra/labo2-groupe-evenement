
#include <Arduino.h>
#include <DHT.h>
#include <math.h>

#include "led/Led.h"
#include "motor/Motor.h"


// Configuration des broches

#define DHT_PIN       4
#define DHT_TYPE      DHT22

#define MOTOR_ENA     5
#define MOTOR_IN1     6
#define MOTOR_IN2     7

#define LED_ROUGE_PIN 15
#define LED_BLEUE_PIN 16

#define TEMP_SEUIL    30.0f

// Bit d'evenement
#define VENTILATION_ON (1 << 0)


// Objets des composants

DHT dht(DHT_PIN, DHT_TYPE);

Motor moteur(MOTOR_ENA, MOTOR_IN1, MOTOR_IN2);

Led ledRouge(LED_ROUGE_PIN, "Rouge");
Led ledBleue(LED_BLEUE_PIN, "Bleue");

// Groupe d'evenements FreeRTOS
EventGroupHandle_t groupeEvenements;

// TACHE 1 : Capteur DHT22

void vTaskSensor(void *parametre) {
    
    for (;;) {
        float temperature = dht.readTemperature();

        if (!isnan(temperature)) {
            Serial.print("Temperature : ");
            Serial.print(temperature);
            Serial.println(" C");

            if (temperature > TEMP_SEUIL) {
                xEventGroupSetBits(groupeEvenements,VENTILATION_ON);
            } else {
                xEventGroupClearBits(groupeEvenements, VENTILATION_ON);
            }
        } else {
            Serial.println("Erreur de lecture du DHT22");
        }

        // Le DHT22 est lu toutes les 2 secondes
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

// TACHE 2 : Controle du moteur

void vTaskMotor(void *parametre) {

    for (;;) {
        EventBits_t etat =
            xEventGroupGetBits(groupeEvenements);

        if (etat & VENTILATION_ON) {
            moteur.setSpeed(1.0f);
            moteur.forward();
        } else {
            moteur.stop();
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

// TACHE 3 : Controle des DELs

void vTaskLed(void *parametre) {

    for (;;) {

        EventBits_t etat = xEventGroupGetBits(groupeEvenements);

        if (etat & VENTILATION_ON) {
            ledRouge.on();
            ledBleue.off();
        } else {
            ledRouge.off();
            ledBleue.on();
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}


// Initialisation

void setup() {
    Serial.begin(115200);

    // Initialisation des composants
    dht.begin();

    moteur.init();
    ledRouge.init();
    ledBleue.init();

    // Creation du groupe d'evenements
    groupeEvenements = xEventGroupCreate();

    if (groupeEvenements == nullptr) {
        moteur.stop();
        ledRouge.off();
        ledBleue.off();

        Serial.println("Erreur de creation du groupe");

        while (true) {
            delay(1000);
        }
    }

    // Etat initial : ventilation arretee
    xEventGroupClearBits(groupeEvenements, VENTILATION_ON);

    // Creation des trois taches
    BaseType_t resultat1 = xTaskCreate(
        vTaskSensor,
        "Capteur",
        2048,
        nullptr,
        2,
        nullptr
    );

    BaseType_t resultat2 = xTaskCreate(
        vTaskMotor,
        "Moteur",
        2048,
        nullptr,
        2,
        nullptr
    );

    BaseType_t resultat3 = xTaskCreate(
        vTaskLed,
        "DELs",
        2048,
        nullptr,
        1,
        nullptr
    );

    if (resultat1 != pdPASS || resultat2 != pdPASS || resultat3 != pdPASS) {

        moteur.stop();
        ledRouge.off();
        ledBleue.off();

        Serial.println("Erreur de creation des taches");

        while (true) {
            delay(1000);
        }
    }

    Serial.println("Systeme F1361 demarre");
}


// Boucle Arduino

void loop() {
    vTaskDelay(pdMS_TO_TICKS(1000));
}
