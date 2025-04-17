#include <Arduino.h>

// row pins
int rows[8] = {9, 8, 7, 6, 5, 4, 3, 2};
// col pins
int cols[8] = {10, 11, 12, 13, A3, A2, A1, A0};

// // original col pins
// int colsOrig[8] = {10, 11, 12, 13, A0, A1, A2, A3};
// // original row pins
// int rowsOrig[8] = {2, 3, 4, 5, 6, 7, 8, 9};

// Forward declaration of lightLed function
void lightLed(int row, int col);


void setup()
{
  // set the row pins as OUTPUT
  for (int i = 0; i < 8; i++)
  {
    pinMode(rows[i], OUTPUT);
  }

  // set the column pins as OUTPUT
  for (int i = 0; i < 8; i++)
  {
    pinMode(cols[i], OUTPUT);
  }

}

void lightLed(int row, int col)
{
    // set the row pin to HIGH
    digitalWrite(rows[row], 1);

    // set the column pin to LOW
    digitalWrite(cols[col], 0);

    // set all other row pins to LOW
    for (int i = 0; i < 8; i++)
    {
        if (i != row)
        {
            digitalWrite(rows[i], LOW);
        }
    }

    // set all other column pins to HIGH
    for (int i = 0; i < 8; i++)
    {
        if (i != col)
        {
            digitalWrite(cols[i], HIGH);
        }
    }

}

void loop()
{
  // light every LED in the matrix one by one
  for (int i = 0; i < 8; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      lightLed(i, j);
      delay(100); // wait for 100 milliseconds
    }
  }
}
