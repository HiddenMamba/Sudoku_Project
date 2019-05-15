#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

#define SUDOKU_SIZE 9
#define PIN_RST 3 // 3
#define PIN_UP 7
#define PIN_DOWN 4 // 4
#define PIN_RIGHT 2 // 5
#define PIN_LEFT 5 // 2
#define PIN_OK 6 //6
#define LED_PIN 13

#define TRUE 1
#define FALSE 0

#define MOVE_MODE 0
#define INSERT_MODE 1

#define BOARD_START_X 10
#define BOARD_START_Y 5
#define BOARD_HEIGHT 126
#define BOARD_WIDTH 126

//
//  Type & Data Definitions
//

typedef struct _POINT {
  uint8_t x;
  uint8_t y;
} POINTXD, *PPOINTXD;

typedef struct _STATE {
  uint8_t buttonState;
  uint8_t gameState;
  int8_t insertSelection;
  uint8_t sudoku[SUDOKU_SIZE * SUDOKU_SIZE];

  uint8_t cSwap[SUDOKU_SIZE];
  uint8_t rSwap[SUDOKU_SIZE];

  uint8_t insertedElements;

  POINTXD cursorPosition;
} STATE, *PSTATE;

STATE state = {
  0,
  MOVE_MODE,
  0,
  {
    1, 3, 9, 0, 0, 0, 0, 6, 0,
    0, 0, 0, 0, 6, 1, 0, 8, 0,
    0, 0, 8, 0, 9, 0, 0, 0, 7,
    3, 0, 0, 0, 0, 2, 5, 0, 0,
    2, 0, 7, 0, 3, 0, 6, 0, 8,
    0, 0, 6, 4, 0, 0, 0, 0, 3,
    9, 0, 0, 0, 4, 0, 8, 0, 0,
    0, 4, 0, 1, 5, 0, 0, 0, 0,
    0, 6, 0, 0, 0, 0, 7, 5, 4
  },
  {0, 1, 2, 3, 4, 5, 6, 7, 8},
  {0, 1, 2, 3, 4, 5, 6, 7, 8},
  32,
  { 0, 0 },
};

const uint8_t const_sudoku[] =     {
  1, 3, 9, 0, 0, 0, 0, 6, 0,
  0, 0, 0, 0, 6, 1, 0, 8, 0,
  0, 0, 8, 0, 9, 0, 0, 0, 7,
  3, 0, 0, 0, 0, 2, 5, 0, 0,
  2, 0, 7, 0, 3, 0, 6, 0, 8,
  0, 0, 6, 4, 0, 0, 0, 0, 3,
  9, 0, 0, 0, 4, 0, 8, 0, 0,
  0, 4, 0, 1, 5, 0, 0, 0, 0,
  0, 6, 0, 0, 0, 0, 7, 5, 4
};

#define TFT_CS     9         // TFT CS pin is connected to arduino pin 9
#define TFT_DC     10        // TFT DC pin is connected to arduino pin 10
#define rst        8

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, rst);


#define BUTTON_UP       (state.buttonState & HIGH)
#define BUTTON_DOWN     (state.buttonState & (HIGH << 1))
#define BUTTON_RIGHT    (state.buttonState & (HIGH << 2))
#define BUTTON_LEFT     (state.buttonState & (HIGH << 3))
#define BUTTON_OK       (state.buttonState & (HIGH << 4))
#define BUTTON_RST      (state.buttonState & (HIGH << 5))

//
//  Declarations
//

void
print_sudoku(
  void
);

uint8_t
validate_cursor(
  uint8_t x,
  uint8_t y
);

uint8_t
handle_input(
  void
);

uint8_t
swap_columns(
  uint8_t a,
  uint8_t b
);

uint8_t
swap_rows(
  uint8_t a,
  uint8_t b
);

uint8_t
insert_value(
  uint8_t x,
  uint8_t y,
  uint8_t value
);

uint8_t
random_sudoku (
  void
);

uint8_t
validate_sudoku(
  void
);

uint8_t
read_buttons(
  void
);

uint8_t
handle_move(
  void
);

uint8_t
handle_insert(
  void
);

//
//  Definitions
//

uint8_t
get_value (
  uint8_t x,
  uint8_t y
)
{
  return state.sudoku[state.rSwap[x] * SUDOKU_SIZE + state.cSwap[y]];
}

uint8_t
insert_value(
  uint8_t x,
  uint8_t y,
  uint8_t value
)
{
  return state.sudoku[state.rSwap[x] * SUDOKU_SIZE + state.cSwap[y]] = value;
}

uint8_t
swap_columns(
  uint8_t a,
  uint8_t b
)
{
  state.cSwap[a] ^= state.cSwap[b];
  state.cSwap[b] ^= state.cSwap[a];
  state.cSwap[a] ^= state.cSwap[b];

  return 0;
}

uint8_t
swap_rows(
  uint8_t a,
  uint8_t b
)
{
  state.rSwap[a] ^= state.rSwap[b];
  state.rSwap[b] ^= state.rSwap[a];
  state.rSwap[a] ^= state.rSwap[b];

  return 0;
}

uint8_t
validate_cursor(
  uint8_t x,
  uint8_t y
)
{
  if (
    x < SUDOKU_SIZE &&
    y < SUDOKU_SIZE &&
    x >= 0 &&
    y >= 0
  )
  {
    return 1;
  }

  return 0;
}
uint8_t
validate_right(
  uint8_t x,
  uint8_t y
)
{
  if (
    x == 9
  )
  {
    return 1;
  }

  return 0;
}
uint8_t
validate_top(
  uint8_t x,
  uint8_t y
)
{
  if (
    y == 0
  )
  {
    return 1;
  }

  return 0;
}
uint8_t
validate_bot(
  uint8_t x,
  uint8_t y
)
{
  if (
    y == 9
  )
  {
    return 1;
  }

  return 0;
}
uint8_t
validate_left(
  uint8_t x,
  uint8_t y
)
{
  if (
    x == 0
  )
  {
    return 1;
  }

  return 0;
}
uint8_t
read_buttons(
  void
)
{
  state.buttonState = 0;
  state.buttonState |= (digitalRead(PIN_UP) );
  state.buttonState |= (digitalRead(PIN_DOWN) << 1);
  state.buttonState |= (digitalRead(PIN_LEFT) << 2);
  state.buttonState |= (digitalRead(PIN_RIGHT) << 3);
  state.buttonState |= (digitalRead(PIN_OK) << 4);
  state.buttonState |= (digitalRead(PIN_RST) << 5);

  return state.buttonState;
}

uint8_t
handle_move(
  void
)
{
  uint8_t valid = 1;
  uint8_t overflw = 1;
  if (BUTTON_UP)
  {
overflw &= validate_top(
               state.cursorPosition.x,
               state.cursorPosition.y
             );

    valid &= validate_cursor(
               state.cursorPosition.x,
               state.cursorPosition.y - 1
             );
 if (overflw)
    {
    state.cursorPosition.y++;
          state.cursorPosition.y++;
                state.cursorPosition.y++;
                      state.cursorPosition.y++;
                            state.cursorPosition.y++;
                                  state.cursorPosition.y++;
                                        state.cursorPosition.y++;      
                                     state.cursorPosition.y++; }
    else if (valid)
    {
     if (valid)
    {
      state.cursorPosition.y--;
    }
  }
  }

  if (BUTTON_DOWN)
  {
   overflw &= validate_bot(
               state.cursorPosition.x,
               state.cursorPosition.y + 1
             );
    valid &= validate_cursor(
               state.cursorPosition.x,
               state.cursorPosition.y + 1
             );
    if (overflw)
    {
      state.cursorPosition.y = 0;
    }
    else if (valid)
    {
      state.cursorPosition.y++;
    }
  }

  if (BUTTON_RIGHT)
  {
    overflw &= validate_right( 
               state.cursorPosition.x + 1,
               state.cursorPosition.y
             );
    valid &= validate_cursor(
               state.cursorPosition.x + 1,
               state.cursorPosition.y
             );

    if (overflw)
    {
    state.cursorPosition.x--;
          state.cursorPosition.x--;
                state.cursorPosition.x--;
                      state.cursorPosition.x--;
                            state.cursorPosition.x--;
                                  state.cursorPosition.x--;
                                        state.cursorPosition.x--;       
        state.cursorPosition.x--; }
    else if (valid)
    {
      state.cursorPosition.x++;
    }
  }

  if (BUTTON_LEFT)
  {
    overflw &= validate_left( 
               state.cursorPosition.x ,
               state.cursorPosition.y
             );
    valid &= validate_cursor(
               state.cursorPosition.x - 1,
               state.cursorPosition.y
             );
    if (overflw) 
    {
      state.cursorPosition.x =8;
    }
   else if (valid)
    {
      state.cursorPosition.x--;
    }
  }

  if (BUTTON_OK)
  {
    state.gameState = INSERT_MODE;
  }

  Serial.print(state.cursorPosition.x, HEX);
  Serial.print(state.cursorPosition.y, HEX);

  return state.buttonState;
}

uint8_t handle_insert(
  void
)
{
  uint8_t value;

  if (BUTTON_RIGHT)
  {
    state.insertSelection = ++(state.insertSelection) < 10 ? state.insertSelection : 0;
  }

  if (BUTTON_LEFT)
  {
    state.insertSelection = --(state.insertSelection) >= 0 ? state.insertSelection : 9;
  }

  if (BUTTON_OK)
  {
    value = get_value(
              state.cursorPosition.y,
              state.cursorPosition.x
            );

    if (!value && state.insertSelection)
    {
      state.insertedElements++;
    }

    if (value && !state.insertSelection)
    {
      state.insertedElements--;
    }

    insert_value(
      state.cursorPosition.y,
      state.cursorPosition.x,
      state.insertSelection
    );

    state.gameState = MOVE_MODE;
  }

  return state.buttonState;
}

uint8_t
handle_input(
  void
)
{
  if (BUTTON_RST)
  {
    memcpy(state.sudoku, const_sudoku, SUDOKU_SIZE * SUDOKU_SIZE);
    random_sudoku();
    state.insertedElements = 32;
    return;
  }

  if (state.gameState == MOVE_MODE) {
    handle_move();
    return state.buttonState;
  }

  handle_insert();
  return state.buttonState;
}

void
print_char(
  uint8_t i,
  uint8_t j,
  uint16_t fg,
  uint16_t bg
)
{
  tft.drawChar(
    10 + j * 12 + 4,
    10 + i * 12 + 3,
    get_value(i, j) + 48,
    fg,
    bg,
    1
  );
}

void
clear_char(
  uint8_t i,
  uint8_t j,
  uint16_t fg,
  uint16_t bg
)
{
  tft.drawChar(
    10 + j * 12 + 4,
    10 + i * 12 + 3,
    ' ',
    fg,
    bg,
    1
  );
}

uint8_t
random_sudoku (
  void
)
{
  randomSeed(analogRead(A0));
  SwapWiersz(random(0, 15), 3);
  SwapKolumn(random(0, 15), 3);

  SwapWiersz(random(0, 15), 1);
  SwapKolumn(random(0, 15), 1);

  return 0;
}

void
SwapWiersz(
  uint8_t ilosc,
  uint8_t wiersze
)
{
  uint8_t radek = random(1, 4);
  uint8_t darek = random(1, 4);

  while (radek == darek)
  {
    darek = random(1, 4);
  }

  uint8_t warek = random(3);
  uint8_t wurek = random(3);

  while (warek == wurek)
  {
    wurek = random(3);
  }

  if (wiersze > 1)
  {
    for (uint8_t i = 0; i < ilosc; i++)
    {
      for (uint8_t j = 0; j < wiersze; j++)
      {
        swap_rows(j + 3 * (radek - 1), j + 3 * (darek - 1));
      }
    }
  }
  else
  {
    swap_rows((3 * (radek - 1) + warek), (3 * (radek - 1) + wurek));
  }
}
void SwapKolumn(
  uint8_t ilosc,
  uint8_t kolumny
)
{
  uint8_t radek = random(1, 4);
  uint8_t warek = random(3);
  uint8_t darek = random(1, 4);

  while (radek == darek)
  {
    darek = random(1, 4);
  }

  uint8_t wurek = random(3);

  while (warek == wurek)
  {
    wurek = random(3);
  }

  if (kolumny > 1)
  {
    for (uint8_t i = 0; i < ilosc; i++)
    {
      for (uint8_t j = 0; j < kolumny; j++)
      {
        swap_columns(j + 3 * (radek - 1), j + 3 * (darek - 1));
      }
    }
  }
  else {
    swap_columns((3 * (radek - 1) + warek), (3 * (radek - 1) + wurek));
  }
}

uint8_t
is_cursor(
  uint8_t i,
  uint8_t j
)
{
  return state.cursorPosition.x == i && state.cursorPosition.y == j;
}

void
print_cursor(
  uint16_t fg,
  uint16_t bg
)
{
  tft.drawChar(
    10 + state.cursorPosition.x * 12 + 4,
    10 + state.cursorPosition.y * 12 + 3,
    state.gameState == MOVE_MODE ?
    get_value(state.cursorPosition.y, state.cursorPosition.x) + 48 :
    state.insertSelection + 48,
    fg,
    bg,
    1
  );
}

void
print_sudoku(
  void
)
{

  for (int i = 0; i < 9; ++i)
  {
    for (int j = 0; j < 9; ++j)
    {
      if (is_cursor(j, i))
      {
        continue;
      }

      if (get_value(i, j)) {
        print_char(i, j, ST7735_BLACK, ST7735_WHITE);
      }
      else
      {
        clear_char(i, j, ST7735_BLACK, ST7735_WHITE);
      }
    }
  }

  print_cursor(
    ST7735_WHITE,
    ST7735_RED
  );
}

uint8_t
validate_sudoku(
  void
)
{
  uint8_t check[] = "000000000";

  for (int j = 0; j < SUDOKU_SIZE; ++j)
  {
    for (int i = 0; i < SUDOKU_SIZE; ++i)
    {
      check[get_value(j, i) - 1]++;
    }

    Serial.println((char*)check);

    if (strcmp(check, "111111111"))
    {
      return 0;
    }

    strcpy(check, "000000000");
  }


  for (int j = 0; j < SUDOKU_SIZE; ++j)
  {
    for (int i = 0; i < SUDOKU_SIZE; ++i)
    {
      check[get_value(i, j) - 1]++;
    }

    if (strcmp(check, "111111111"))
    {
      return 0;
    }

    strcpy(check, "000000000");
  }

#define GOD_HELP_ME(C) get_value(((i + C) - ((i + C) % 9)) / 9, (i + C)% 9)

  for (int i = 0; i < 61; i += ((i % 9) == 6 ? 21 : 3))
  {
    check[GOD_HELP_ME(0) - 1]++;
    check[GOD_HELP_ME(1) - 1]++;
    check[GOD_HELP_ME(2) - 1]++;
    check[GOD_HELP_ME(9) - 1]++;
    check[GOD_HELP_ME(10) - 1]++;
    check[GOD_HELP_ME(11) - 1]++;
    check[GOD_HELP_ME(18) - 1]++;
    check[GOD_HELP_ME(19) - 1]++;
    check[GOD_HELP_ME(20) - 1]++;

    if (strcmp(check, "111111111"))
    {
      return 0;
    }

    strcpy(check, "000000000");
  }

  return 1;
}

void setup(
)
{
  pinMode(BUTTON_UP, INPUT);
  pinMode(BUTTON_DOWN, INPUT);
  pinMode(BUTTON_LEFT, INPUT);
  pinMode(BUTTON_RIGHT, INPUT);
  pinMode(BUTTON_OK, INPUT);

  Serial.begin(9600);

  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  SPI.begin();

  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(ST7735_WHITE);

  random_sudoku();

  tft.drawRect(10, 10, 108, 108, ST7735_BLACK);
  for (int i = 1; i < 9; ++i)
  {
    tft.drawFastHLine(10, 10 + i * 12, 108, ST7735_BLACK);
    tft.drawFastVLine(10 + i * 12, 10, 108, ST7735_BLACK);
  }
}

void loop() {

  if (read_buttons())
  {
    handle_input();
  }

  print_sudoku();

  if (state.insertedElements == SUDOKU_SIZE * SUDOKU_SIZE)
  {
    if (validate_sudoku()) {
      memcpy(state.sudoku, const_sudoku, SUDOKU_SIZE * SUDOKU_SIZE);
      random_sudoku();
      state.insertedElements = 32;
    }
  }


  delay(50);
}
