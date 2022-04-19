// --- Configuration ---
#define r2 10000        // value of the R2 resistor
#define BOARD_ITEMS 12 // last item is is put aside
#define V_READ A0      // the voltage divider value
#define TOLERANCE 0.10

// - Board address bit -
#define OUT_OF_RNG_LED 2
// X axis
#define BOARD_ADDRX_1 3
#define BOARD_ADDRX_2 4
#define BOARD_ADDRX_3 5
// Y axis
#define BOARD_ADDRY_1 6
#define BOARD_ADDRY_2 7
#define BOARD_ADDRY_3 8
#define BOARD_ADDRY_4 9
// Multiplier address
#define MULT100X 10
#define MULT1KX 11
#define MULT10KX 12
#define MULT100KX 13

// - Valid resistor values -
// Values from 0 - 1000, multipliers are extracted from these
long resistors[BOARD_ITEMS] = {
    100,
    120,
    150,
    180,
    220,
    270,
    330,
    390,
    470,
    560,
    680,
    820,
};

// --- Code ---
#include <LiquidCrystal_I2C.h>
#include <math.h>
#include <inttypes.h>

// Useful Macros
#define ARR_SIZEOF(X) (sizeof(X) / sizeof((X)[0])) // get the size of an array

//  Circuit variables
LiquidCrystal_I2C lcd(0x27, 16, 2);
long r1 = 0;              // resistor to be measured
float voltageRead = 0.0; // voltage divided from r1_r2

// Ohm symbol for the display panel
byte ohms[] = {
    B01110,
    B11011,
    B10001,
    B10001,
    B10001,
    B01010,
    B11011,
    B00000,
};

// - Function declarations
int isValidResistor(long val);
void digitalWriteBoardX(uint8_t x1, uint8_t x2, uint8_t x3);
void digitalWriteBoardY(uint8_t y1, uint8_t y2, uint8_t y3, uint8_t y4);
void digitalWriteMult(uint8_t m1, uint8_t m2, uint8_t m3, uint8_t m4);
void showContainer(int val, long resistor);

void setup()
{
    Serial.begin(9600);
    // initialize display
    lcd.init();
    lcd.createChar(1, ohms);
    lcd.backlight();
    lcd.home();
    // --- TABLE OUTPUTS ---
    // Addressed output
    // X activates on HIGH
    pinMode(BOARD_ADDRX_1, OUTPUT);
    pinMode(BOARD_ADDRX_2, OUTPUT);
    pinMode(BOARD_ADDRX_3, OUTPUT);
    // Y activates on LOW
    pinMode(BOARD_ADDRY_1, OUTPUT);
    pinMode(BOARD_ADDRY_2, OUTPUT);
    pinMode(BOARD_ADDRY_3, OUTPUT);
    pinMode(BOARD_ADDRY_4, OUTPUT);

    // Normal LED output
    pinMode(MULT100X, OUTPUT);
    pinMode(MULT1KX, OUTPUT);
    pinMode(MULT10KX, OUTPUT);
    pinMode(MULT100KX, OUTPUT);
}

void loop()
{
    lcd.clear();
    voltageRead = analogRead(V_READ);
    voltageRead *= (5.0 / 1023.0); // converts arduino's 1024 bit 5v mapping back into volts
    if (voltageRead != 0)          // identify resistor only if value is non-zero
    {
        r1 = (5.0 * r2) / voltageRead - r2; // calculates the value of r1
        lcd.setCursor(5, 0);
        showContainer(isValidResistor(r1), r1); // point to that resistor's container
        lcd.print(r1);
        lcd.write(1); // write ohm symbol
    }

    delay(200);
}

int isValidResistor(long val)
{
	long curr_res = 0;
	for (int i = 0; i < BOARD_ITEMS; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			curr_res = resistors[i] * pow(10, j);
			long min_val = curr_res * (1 - TOLERANCE); // minimum tolerance
			long max_val = curr_res * (1 + TOLERANCE); // maximum tolerance
			if (val > min_val && val < max_val)
			{
				return i;
			}
		}
	}
	return -1;
}

void digitalWriteBoardX(uint8_t x1, uint8_t x2, uint8_t x3)
{
    digitalWrite(BOARD_ADDRX_1, x1);
    digitalWrite(BOARD_ADDRX_2, x2);
    digitalWrite(BOARD_ADDRX_3, x3);
}

void digitalWriteBoardY(uint8_t y1, uint8_t y2, uint8_t y3, uint8_t y4)
{
    digitalWrite(BOARD_ADDRY_1, y1);
    digitalWrite(BOARD_ADDRY_2, y2);
    digitalWrite(BOARD_ADDRY_3, y3);
    digitalWrite(BOARD_ADDRY_4, y4);
}

void digitalWriteMult(uint8_t m1, uint8_t m2, uint8_t m3, uint8_t m4)
{
    digitalWrite(MULT100X, m1);
    digitalWrite(MULT1KX, m2);
    digitalWrite(MULT10KX, m3);
    digitalWrite(MULT100KX, m4);
}

void showContainer(int val, long resistor)
{
    if (val == -1)
    {
        digitalWrite(OUT_OF_RNG_LED, HIGH);
    }
    else
    {
        digitalWrite(OUT_OF_RNG_LED, LOW);
        // Choose the column to be activated
        if (val == 0 || val == 1 || val == 2)
        {
            digitalWriteBoardY(LOW, HIGH, HIGH, HIGH);
        }
        else if (val == 3 || val == 4 || val == 5)
        {
            digitalWriteBoardY(HIGH, LOW, HIGH, HIGH);
        }
        else if (val == 6 || val == 7 || val == 8)
        {
            digitalWriteBoardY(HIGH, HIGH, LOW, HIGH);
        }
        else if (val == 9 || val == 10 || val == 11)
        {
            digitalWriteBoardY(HIGH, HIGH, HIGH, LOW);
        }

        // choose the row to be activated
        switch (val)
        {
        case 0:
        case 3:
        case 6:
        case 9:
            digitalWriteBoardX(HIGH, LOW, LOW);
            break;
        case 1:
        case 4:
        case 7:
        case 10:
            digitalWriteBoardX(LOW, HIGH, LOW);
            break;
        case 2:
        case 5:
        case 8:
        case 11:
            digitalWriteBoardX(LOW, LOW, HIGH);
            break;
        }

        // get resistor range
      Serial.println((int)log10(resistor));
      
        switch ((int)log10(resistor))
        {
        // 1K
        case 3:
            digitalWriteMult(LOW, HIGH, LOW, LOW);
            break;
        // 10K
        case 4:
            digitalWriteMult(LOW, LOW, HIGH, LOW);
            break;
        // 100K
        case 5:
            digitalWriteMult(LOW, LOW, LOW, HIGH);
            break;
        // 100 or less
        default:
            digitalWriteMult(HIGH, LOW, LOW, LOW);
            break;
        }
        
    }
}
