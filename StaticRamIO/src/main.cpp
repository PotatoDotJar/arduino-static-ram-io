#include <Arduino.h>

#define SHIFT_DATA 2
#define SHIFT_CLOCK 3
#define SHIFT_LATCH 4
#define WRITE_PIN 5
#define OUTPUT_EN_PIN 6

void setAddress(int address);
void clearRam();
byte readByte(byte address);
void writeByte(byte data, byte address);
void writeString(char* data);

int addressMax = 0;

void setup()
{
	Serial.begin(9600);
	pinMode(SHIFT_DATA, OUTPUT);
	pinMode(SHIFT_CLOCK, OUTPUT);
	pinMode(SHIFT_LATCH, OUTPUT);
	pinMode(WRITE_PIN, OUTPUT);
	digitalWrite(WRITE_PIN, HIGH); // Disabled
	pinMode(OUTPUT_EN_PIN, OUTPUT);
	digitalWrite(OUTPUT_EN_PIN, LOW); // Enabled Default

	DDRL = 0; // Init data register

	//clearRam();

	char message[92] = "I love Jalee so much! She is the love of my life and I can't explain how much I love her!!!";
	//writeString(message);
	addressMax = sizeof(message);
}

int selected_add = 0;
void loop()
{

	Serial.print("Address: ");
	Serial.print(selected_add);
	Serial.print(" Data: ");
	Serial.print((char)readByte(selected_add));
	Serial.print("\t");
	Serial.print(readByte(selected_add), BIN);
	Serial.print("\t");
	Serial.println(readByte(selected_add), HEX);
	delay(100);

	selected_add++;

	if (selected_add > addressMax)
	{
		selected_add = 0;
	}
}

void setAddress(int address)
{
	if (address > 255)
	{
		address = 255;
	}
	else if (address < 0)
	{
		address = 0;
	}

	digitalWrite(SHIFT_LATCH, LOW);
	shiftOut(SHIFT_DATA, SHIFT_CLOCK, MSBFIRST, address);
	digitalWrite(SHIFT_LATCH, HIGH);
}

void clearRam()
{
	digitalWrite(OUTPUT_EN_PIN, HIGH);	// Disable output
	DDRL = 0xFF;	// Set data pins to outputs

	// Loop through all memory addresses
	for (byte i = 0; i < 255; i++)
	{
		setAddress(i);	// Set memory address
		PORTL = 0x00;	// Set registers to data value on bus

		// Pulse write pin to commit data
		delayMicroseconds(10);
		digitalWrite(WRITE_PIN, LOW);
		delayMicroseconds(10);
		digitalWrite(WRITE_PIN, HIGH);
	}

	DDRL = 0x00;	// Data pins inputs
	PORTL = 0x00;	// Data pins no pullup

	digitalWrite(WRITE_PIN, HIGH);	// Reset write pin to default
	digitalWrite(OUTPUT_EN_PIN, LOW);	// Re-enable output

	// Logging
	Serial.println("Ram Cleared");
}

byte readByte(byte address)
{
	setAddress(selected_add);	// Set address
	return PINL;	// Return data in register
}

void writeByte(byte data, byte address)
{
	digitalWrite(OUTPUT_EN_PIN, HIGH); // Disable output
	DDRL = 0xFF;					   // Set data pins to outputs
	setAddress(address);			   // Set memory address
	PORTL = data;					   // Set registers to data value on bus

	// Pulse write pin to commit data
	delayMicroseconds(10);
	digitalWrite(WRITE_PIN, LOW);
	delayMicroseconds(10);
	digitalWrite(WRITE_PIN, HIGH);

	DDRL = 0x00;  // Set data pins back to inputs
	PORTL = 0x00; // Set data pins to not use pullup

	digitalWrite(OUTPUT_EN_PIN, LOW); // Re-enable output

	// Logging
	Serial.print("Wrote Data: ");
	Serial.print((char)data);
	Serial.print("\t");
	Serial.print(data, BIN);
	Serial.print("\t");
	Serial.println(data, HEX);
	Serial.print(data, HEX);
	Serial.print(" to address ");
	Serial.println(address, HEX);
}

void writeString(char* data) {
	for (byte i = 0; i < strlen(data); i++)
	{
		writeByte(data[i], i);
	}
}