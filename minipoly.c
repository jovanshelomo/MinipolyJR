#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <Windows.h>
#include <stdbool.h>

// mendefinisikan key pada keyboard
#define ARROW_UP 72
#define ARROW_DOWN 80
#define ARROW_LEFT 75
#define ARROW_RIGHT 77
#define KEY_ENTER 13

typedef struct dadu
{
  int dadu1;
  int dadu2;
} dadu;

typedef struct screenCoordinate
{
  int x;
  int y;
} screenCoordinate;

typedef struct danaUmum
{
  char perintah[100];
} danaUmum;

typedef struct deedCard
{
  bool isKota;
  bool isStasion;
  bool isPerusahaan;
  int blockId;
  char code[3];
  char nama[30];
  int hargaBeli; // harga beli property yang ada di papan
  int hargaSewa[6];
  int hargaUpgrade;
  int jumlahRumah;
  int color;
  screenCoordinate tandaKepemilikan; // tempat kepemilikan
  screenCoordinate lokasiRumah;

} deedCard;

typedef struct step
{
  int id;                        // id step (0-39)
  screenCoordinate lokasiPlayer; // lokasi player
  deedCard kartuProperty;
} step;

typedef struct playerProperty
{
  deedCard deed;
  int propertyGroup;
  int jumlahRumah;
} playerProperty;

typedef struct player
{
  char simbol; //ΩΘΔΠΣ
  int uang;
  int posisi; // 0-39
  screenCoordinate posisiLayar;
  bool punyaKartuBebasPenjara;
  int jumlahProperty;
  playerProperty properties[];
} player;

// list semua module yang terdapat pada program
void gotoxy(int x, int y);

void delay(int ms);
void setColor(int color);
void setBold(bool bold);

int rotateIndex(int currentIndex, int maxIndex, bool isPlus); // berfungsi untuk merotasi index jika sudah terakhir atau ke paling atas
void renderLogo();
int startMenuSelection();
void renderStartSelectionMenu();
int selectionMenu(char notSelectedStrings[][30], char selectedStrings[][30], int maxIndex, int x, int y);
int renderSelectionMenu(char notSelectedStrings[][30], char selectedStrings[][30], int selected, int maxIndex, int x, int y);
void printBoard();
int keyboardInputHandler();
dadu kocokDadu();
void resetGame();
void startGame();

// player
void majuNLangkah(player *p, int N);                         // berfungsi untuk memajukan player dari posisi sekarang ke posisi sekarang + N
void lompatKe(player *p, int index);                         // berfungsi untuk melompatkan player tanpa berjalan memutar
void majuHingga(player *p, int index);                       // berfungsi untuk memajukan player hingga posisi N
void renderPosisiPlayer(player *p, int sebelum, int posisi); // menghapus posisi player sebelumnya dan menampilkan yang baru
int setLokasiPlayer(player *p, int indexLokasi);
int cekPlayerDiStepYangSama(player *p, int indexStep);

void removeProperty(playerProperty properties[], int jumlahProperty, int index);

// warna: 112, 159, 160, 176, 192, 208, 224, 143
/*
int id;
bool isProperty;
bool isCompany;
bool isStasion;
int blockId;
char code[3];
int color;
char nama[30];
int hargaBeli;
int hargaSewa[6];
int hargaUpgrade;
coordinate lokasiRumah;
coordinate tandaKepemilikan;
coordinate lokasiPlayer;
  */
step listStep[40] = {
    {0, false, false, false, 0, "\0", 0, "GO", 0, {0, 0, 0, 0, 0, 0}, 0, {0, 0}, {0, 0}, {7, 45}},
    {1, true, false, false, 0, "MLB", 142, "Melbourne", 60, {2, 10, 30, 90, 160}, 50, {14, 41}, {18, 42}, {6, 42}},
    {2, false, false, false, 0, "\0", 0, "DANA UMUM", 0, {0, 0, 0, 0, 0, 0}, 0, {0, 0}, {0, 0}, {6, 38}},
    {3, true, false, false, 0, "SYD", 142, "Sydney", 60, {4, 20, 60, 180, 320}, 50, {14, 33}, {18, 34}, {6, 34}},

};

HANDLE hConsole;

int main()
{
  // inisialisasi
  hConsole = GetStdHandle(STD_OUTPUT_HANDLE); // handle console (untuk mengubah warna teks)
  SetConsoleOutputCP(CP_UTF8);                // agar dapat print utf-8
  srand(time(NULL));                          // inisialisasi random

  renderLogo();
  char startNotSelectedMenuString[][30] = {"     START  ",
                                           "  HOW TO PLAY  ",
                                           "     QUIT  "};
  char startSelectedMenuString[][30] = {"   ▶ START ◀",
                                        "▶ HOW TO PLAY ◀",
                                        "   ▶ QUIT ◀"};
  int selection = selectionMenu(startNotSelectedMenuString, startSelectedMenuString, 1, 32, 7);
  system("cls");
  if (selection == 0) // start
  {
    printBoard();
  }
  else if (selection == 1) // how to play
  {
    printf("tesssss");
  }

  getchar();
  return 0;
}

void gotoxy(int x, int y)
{
  COORD coord;
  coord.X = x;
  coord.Y = y;
  SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void delay(int ms)
{
  clock_t start = clock();
  while (clock() < start + ms)
  {
    ;
  }
}

// void setColor(int color)
// {

//   SetConsoleTextAttribute(hConsole, color);
// }

void setColor(int color)
{
  SetConsoleTextAttribute(hConsole, color);
}

void printWithColor(int color, char text[], ...)
{
  va_list args;
  va_start(args, text);
  setColor(color);
  vprintf(text, args);
  va_end(args);
  setColor(7);
}

void setBold(bool isBold)
{
  if (isBold)
  {
    printf("\033[1m");
  }
  else
  {
    printf("\033[0m");
  }
}

int rotateIndex(int currentIndex, int maxIndex, bool isPlus)
{
  if (isPlus)
  {
    currentIndex++;
    if (currentIndex > maxIndex)
    {
      currentIndex = 0;
    }
  }
  else
  {
    currentIndex--;
    if (currentIndex < 0)
    {
      currentIndex = maxIndex;
    }
  }
  return currentIndex;
}

void renderLogo()
{
  /*
 __  __    ___    _  _     ___      ___    ___     _     __   __
|  \/  |  |_ _|  | \| |   |_ _|    | _ \  / _ \   | |    \ \ / /
| |\/| |   | |   | .` |    | |     |  _/ | (_) |  | |__   \ V /
|_|__|_|  |___|  |_|\_|   |___|   _|_|_   \___/   |____|  _|_|_
_|"""""|_|"""""|_|"""""|_|"""""|_| """ |_|"""""|_|"""""|_| """ |
"`-0-0-'"`-0-0-'"`-0-0-'"`-0-0-'"`-0-0-'"`-0-0-'"`-0-0-'"`-0-0-'
  */
  char minipolyTrainText[][80] = {"             __  __    ___    _  _     ___      ___    ___     _     __   __ ",
                                  "            |  \\/  |  |_ _|  | \\| |   |_ _|    | _ \\  / _ \\   | |    \\ \\ / / ",
                                  "            | |\\/| |   | |   | .` |    | |     |  _/ | (_) |  | |__   \\ V /  ",
                                  "            |_|__|_|  |___|  |_|\\_|   |___|   _|_|_   \\___/   |____|  _|_|_  ",
                                  "            _|\"\"\"\"\"|_|\"\"\"\"\"|_|\"\"\"\"\"|_|\"\"\"\"\"|_| \"\"\" |_|\"\"\"\"\"|_|\"\"\"\"\"|_| \"\"\" | ",
                                  "            \"`-0-0-'\"`-0-0-'\"`-0-0-'\"`-0-0-'\"`-0-0-'\"`-0-0-'\"`-0-0-'\"`-0-0-' "};
  for (int i = 0; i <= 72; i++)
  {
    gotoxy(0, 0);
    for (int j = 0; j < 6; j++)
    {
      if (j == 4 || j == 5)
      {
        printWithColor(9, "%s\n", &(minipolyTrainText[j][strlen(minipolyTrainText[j]) - i]));
      }
      else
      {
        setColor(11);
        printWithColor(11, "%s\n", &(minipolyTrainText[j][strlen(minipolyTrainText[j]) - i]));
      }
    }
    delay(25);
  }
}

int startMenuSelection()
{
  int selected = 0;
  renderStartSelectionMenu(selected);
  while (1)
  {
    int keyboardResult = keyboardInputHandler();
    if (keyboardResult == ARROW_UP)
    {
      selected = rotateIndex(selected, 1, false);
    }
    else if (keyboardResult == ARROW_DOWN)
    {
      selected = rotateIndex(selected, 1, true);
    }
    else if (keyboardResult == 13)
    {
      return selected;
    }
    renderStartSelectionMenu(selected);
  }
  return 0;
}

void renderStartSelectionMenu(int index)
{
  char notSelectedStrings[][30] = {"     START  \n",
                                   "  HOW TO PLAY  \n"};
  char selectedStrings[][30] = {"   ▶ START ◀\n",
                                "▶ HOW TO PLAY ◀\n"};

  gotoxy(0, 7);
  for (int i = 0; i < 2; i++)
  {
    if (i == index)
    {
      setBold(true);
      printWithColor(14, "%s", selectedStrings[i]);
      setBold(false);
    }
    else
    {
      setColor(7);
      printf("%s", notSelectedStrings[i]);
    }
  }
}

int selectionMenu(char notSelectedStrings[][30], char selectedStrings[][30], int maxIndex, int x, int y)
{
  int selected = 0;
  renderSelectionMenu(notSelectedStrings, selectedStrings, selected, maxIndex, x, y);
  while (1)
  {
    int keyboardResult = keyboardInputHandler();
    if (keyboardResult == ARROW_UP)
    {
      selected = rotateIndex(selected, 1, false);
    }
    else if (keyboardResult == ARROW_DOWN)
    {
      selected = rotateIndex(selected, 1, true);
    }
    else if (keyboardResult == KEY_ENTER)
    {
      break;
    }
    renderSelectionMenu(notSelectedStrings, selectedStrings, selected, maxIndex, x, y);
  }
  return selected;
}
int renderSelectionMenu(char notSelectedStrings[][30], char selectedStrings[][30], int selected, int maxIndex, int x, int y)
{
  for (int i = 0; i < 2; i++)
  {
    gotoxy(x, y + i);
    i == selected ? printWithColor(14, "%s", selectedStrings[i]) : printf("%s", notSelectedStrings[i]);
  }
}

void startGame()
{
  printBoard();
}

void printBoard()
{
  char board[] = "╔══════════════╦════════╦════════╦════════╦════════╦════════╦════════╦════════╦════════╦════════╦══════════════╗\n"
                 "║  ║ HANYA  ║  ║  GZH   ║  PLN   ║  BJG   ║  SHN   ║  GBG   ║  MSL   ║  DANA  ║  BDX   ║  PAR   ║    PARKIR    ║\n"
                 "║ MENGUNJUNGI  ║        ║        ║        ║        ║        ║        ║  UMUM  ║        ║        ║    GRATIS    ║\n"
                 "║  ║PENJARA ║  ║        ║        ║        ║        ║        ║        ║        ║        ║        ║              ║\n"
                 "║              ║        ║        ║        ║        ║        ║        ║        ║        ║        ║      __      ║\n"
                 "║  ║  ║  ║  ║  ║  $140  ║        ║  $140  ║  $160  ║        ║  $180  ║        ║  $180  ║  $200  ║    _| =\\__   ║\n"
                 "║  ║  ║  ║  ║  ║████████║  $150  ║████████║████████║  $200  ║████████║  $$$$  ║████████║████████║   /o____o_\\  ║\n"
                 "╠══════════════╬════════╩════════╩════════╩════════╩════════╩════════╩════════╩════════╩════════╬══════════════╣\n"
                 "║    TYO     ██║                                                                                ║██    RTD     ║\n"
                 "║            ██║                                                                                ║██            ║\n"
                 "║    $120    ██║       ┌──────────────────────────┐                                             ║██    $220    ║\n"
                 "╠══════════════╣       │                          ├┐                                            ╠══════════════╣\n"
                 "║    KYT     ██║       │                          ││                                            ║  KESEMPATAN  ║\n"
                 "║            ██║       │         DANA UMUM        ││                                            ║              ║\n"
                 "║    $100    ██║       │                          ││                                            ║     ????     ║\n"
                 "╠══════════════╣       │                          ││                                            ╠══════════════╣\n"
                 "║  KESEMPATAN  ║       └┬─────────────────────────┘│                                            ║██    DHG     ║\n"
                 "║              ║        └──────────────────────────┘                                            ║██            ║\n"
                 "║     ????     ║                                                                                ║██    $220    ║\n"
                 "╠══════════════╣                                                                                ╠══════════════╣\n"
                 "║    OSA     ██║                                                                                ║██    AMS     ║\n"
                 "║            ██║                                                                                ║██            ║\n"
                 "║    $100    ██║                                                                                ║██    $240    ║\n"
                 "╠══════════════╣           ███╗   ███╗██╗███╗   ██╗██╗██████╗  ██████╗ ██╗  ██╗   ██╗           ╠══════════════╣\n"
                 "║     PSN      ║           ████╗ ████║██║████╗  ██║██║██╔══██╗██╔═══██╗██║  ╚██╗ ██╔╝           ║     STH      ║\n"
                 "║              ║           ██╔████╔██║██║██╔██╗ ██║██║██████╔╝██║   ██║██║   ╚████╔╝            ║              ║\n"
                 "║     $200     ║           ██║╚██╔╝██║██║██║╚██╗██║██║██╔═══╝ ██║   ██║██║    ╚██╔╝             ║     $200     ║\n"
                 "╠══════════════╣           ██║ ╚═╝ ██║██║██║ ╚████║██║██║     ╚██████╔╝███████╗██║              ╠══════════════╣\n"
                 "║  INCOME TAX  ║           ╚═╝     ╚═╝╚═╝╚═╝  ╚═══╝╚═╝╚═╝      ╚═════╝ ╚══════╝╚═╝              ║██    TPN     ║\n"
                 "║              ║                                                                                ║██            ║\n"
                 "║   PAY $200   ║                                                                                ║██    $260    ║\n"
                 "╠══════════════╣                                                                                ╠══════════════╣\n"
                 "║    SYD     ██║                                                                                ║██    JRG     ║\n"
                 "║            ██║                                                                                ║██            ║\n"
                 "║    $60     ██║                                             ┌──────────────────────────┐       ║██    $260    ║\n"
                 "╠══════════════╣                                             │                          ├┐      ╠══════════════╣\n"
                 "║  DANA UMUM   ║                                             │                          ││      ║     WTR      ║\n"
                 "║              ║                                             │        KESEMPATAN        ││      ║              ║\n"
                 "║    $$$$      ║                                             │                          ││      ║     $150     ║\n"
                 "╠══════════════╣                                             │                          ││      ╠══════════════╣\n"
                 "║    MLB     ██║                                             └┬─────────────────────────┘│      ║██    SGP     ║\n"
                 "║            ██║                                              └──────────────────────────┘      ║██            ║\n"
                 "║    $60     ██║                                                                                ║██    $280    ║\n"
                 "╠══════════════╬════════╦════════╦════════╦════════╦════════╦════════╦════════╦════════╦════════╬══════════════╣\n"
                 "║              ║████████║ LUXURY ║████████║ KESEM- ║  GBR   ║████████║  DANA  ║████████║████████║  ║PERGI KE║  ║\n"
                 "║              ║  JKT   ║  TAX   ║  BDG   ║ PATAN  ║        ║  NYC   ║  UMUM  ║  LAG   ║  CCG   ║  ║PENJARA ║  ║\n"
                 "║ ▒█▀▀█ ▒█▀▀▀█ ║        ║        ║        ║        ║        ║        ║        ║        ║        ║  ║  ║  ║  ║  ║\n"
                 "║ ▒█░▄▄ ▒█░░▒█ ║        ║        ║        ║        ║        ║        ║        ║        ║        ║              ║\n"
                 "║ ▒█▄▄█ ▒█▄▄▄█ ║        ║        ║        ║        ║        ║        ║        ║        ║        ║  ║  ║  ║  ║  ║\n"
                 "║     $200     ║  $400  ║PAY $75 ║  $350  ║  ????  ║  $200  ║  $320  ║  $$$$  ║  $300  ║  $300  ║  ║  ║  ║  ║  ║\n"
                 "╚══════════════╩════════╩════════╩════════╩════════╩════════╩════════╩════════╩════════╩════════╩══════════════╝";
  printf("%s", board);
  // TODO print dengan warna header setiap negara
  for (int i = 0; i < 3; i++)
  {
    int blockId = listStep[i].blockId;
    if (listStep[i].)
      if (blockId == 0 || blockId == 1 || blockId == 4 || blockId == 5)
      {
        printf(" ");
      }
  }
}

int keyboardInputHandler()
{
  int ch = _getch();
  if (ch == 224)
  {
    ch = _getch();
    switch (ch)
    {
    case ARROW_UP:
      return ARROW_UP;
    case ARROW_DOWN:
      return ARROW_DOWN;
    case ARROW_LEFT:
      return ARROW_LEFT;
    case ARROW_RIGHT:
      return ARROW_RIGHT;
    default:
      return 0;
    }
  }
  else if (ch == KEY_ENTER)
  {
    return KEY_ENTER;
  }
}

dadu kocokDadu()
{
  dadu tempDadu;
  tempDadu.dadu1 = rand() % 6 + 1;
  tempDadu.dadu2 = rand() % 6 + 1;
  return tempDadu;
}

void resetGame()
{
}

void removeProperty(playerProperty properties[], int jumlahProperty, int index)
{
  // remove an element from properties array
  for (int i = index; i < jumlahProperty - 1; i++)
  {
    properties[i] = properties[i + 1];
  }
}
