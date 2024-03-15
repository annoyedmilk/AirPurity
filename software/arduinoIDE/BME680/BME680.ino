#include <bsec2.h>

#define SDA_PIN 20
#define SCL_PIN 21

void checkBsecStatus(Bsec2 bsec);
void newDataCallback(const bme68xData data, const bsecOutputs outputs, Bsec2 bsec);

Bsec2 envSensor;

void setup() {
    bsecSensor sensorList[] = {
        BSEC_OUTPUT_IAQ,
        BSEC_OUTPUT_STATIC_IAQ,
        BSEC_OUTPUT_CO2_EQUIVALENT,
        BSEC_OUTPUT_BREATH_VOC_EQUIVALENT,
        BSEC_OUTPUT_RAW_TEMPERATURE,
        BSEC_OUTPUT_RAW_PRESSURE,
        BSEC_OUTPUT_RAW_HUMIDITY,
        BSEC_OUTPUT_RAW_GAS,
        BSEC_OUTPUT_STABILIZATION_STATUS,
        BSEC_OUTPUT_RUN_IN_STATUS,
        BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE,
        BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY,
        BSEC_OUTPUT_GAS_PERCENTAGE
    };

    Serial.begin(115200);
    Wire.begin(SDA_PIN, SCL_PIN);

    while(!Serial) delay(10);

    if (!envSensor.begin(0x76, Wire)) { // Adjusted to use the specified I2C address
        checkBsecStatus(envSensor);
    }

    if (!envSensor.updateSubscription(sensorList, sizeof(sensorList) / sizeof(sensorList[0]), BSEC_SAMPLE_RATE_LP)) {
        checkBsecStatus(envSensor);
    }

    envSensor.attachCallback(newDataCallback);

    Serial.println("BSEC library version " + String(envSensor.version.major) + "." + String(envSensor.version.minor) + "." + String(envSensor.version.major_bugfix) + "." + String(envSensor.version.minor_bugfix));
}

void loop() {
    if (!envSensor.run()) {
        checkBsecStatus(envSensor);
    }
}

void newDataCallback(const bme68xData data, const bsecOutputs outputs, Bsec2 bsec) {
    if (!outputs.nOutputs) {
        return;
    }

    Serial.println("BSEC outputs:\n\ttimestamp = " + String((int)(outputs.output[0].time_stamp / INT64_C(1000000))));
    for (uint8_t i = 0; i < outputs.nOutputs; i++) {
        const bsecData output = outputs.output[i];
        switch (output.sensor_id) {
        case BSEC_OUTPUT_IAQ:
            Serial.println("\tIAQ = " + String(output.signal));
            Serial.println("\tIAQ Accuracy = " + String((int)output.accuracy));
            break;
        case BSEC_OUTPUT_STATIC_IAQ:
            Serial.println("\tStatic IAQ = " + String(output.signal));
            Serial.println("\tStatic IAQ Accuracy = " + String((int)output.accuracy));
            break;
        case BSEC_OUTPUT_CO2_EQUIVALENT:
            Serial.println("\tCO2 Equivalent = " + String(output.signal) + " ppm");
            Serial.println("\tCO2 Accuracy = " + String((int)output.accuracy));
            break;
        case BSEC_OUTPUT_BREATH_VOC_EQUIVALENT:
            Serial.println("\tBreath VOC Equivalent = " + String(output.signal) + " ppm");
            break;
        case BSEC_OUTPUT_RAW_TEMPERATURE:
            Serial.println("\tRaw Temperature = " + String(output.signal) + " °C");
            break;
        case BSEC_OUTPUT_RAW_PRESSURE:
            Serial.println("\tRaw Pressure = " + String(output.signal) + " Pa");
            break;
        case BSEC_OUTPUT_RAW_HUMIDITY:
            Serial.println("\tRaw Humidity = " + String(output.signal) + " %");
            break;
        case BSEC_OUTPUT_RAW_GAS:
            Serial.println("\tRaw Gas Resistance = " + String(output.signal) + " Ohms");
            break;
        case BSEC_OUTPUT_STABILIZATION_STATUS:
            Serial.println("\tStabilization Status = " + String(output.signal));
            break;
        case BSEC_OUTPUT_RUN_IN_STATUS:
            Serial.println("\tRun-In Status = " + String(output.signal));
            break;
        case BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE:
            Serial.println("\tHeat Compensated Temperature = " + String(output.signal) + " °C");
            break;
        case BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY:
            Serial.println("\tHeat Compensated Humidity = " + String(output.signal) + " %");
            break;
        case BSEC_OUTPUT_GAS_PERCENTAGE:
            Serial.println("\tGas Percentage = " + String(output.signal) + " %");
            break;
        default:
            Serial.println("\tUnknown Sensor ID");
            break;
        }
    }
}

void checkBsecStatus(Bsec2 bsec) {
    if (bsec.status < BSEC_OK) {
        Serial.println("BSEC error code : " + String(bsec.status));
    } else if (bsec.status > BSEC_OK) {
        Serial.println("BSEC warning code : " + String(bsec.status));
    }

    if (bsec.sensor.status < BME68X_OK) {
        Serial.println("BME68X error code : " + String(bsec.sensor.status));
    } else if (bsec.sensor.status > BME68X_OK) {
        Serial.println("BME68X warning code : " + String(bsec.sensor.status));
    }
}