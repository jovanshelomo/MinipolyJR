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

#define PROPERTY_KOTA "PROPERTY_KOTA"
#define PROPERTY_STASION "PROPERTY_STASION"
#define PROPERTY_PERUSAHAAN "PROPERTY_PERUSAHAAN"

typedef struct Dadu
{
  int dadu1;
  int dadu2;
} Dadu;

typedef struct ScreenCoordinate
{
  int x;
  int y;
} ScreenCoordinate;

typedef struct KartuProperty
{
  char *jenisProperty;
  int blockId;
  char code[3];
  char nama[30];
  int hargaBeli; // harga beli property yang ada di papan
  int hargaSewa[6];
  int hargaUpgrade;
  int color;
  struct ScreenCoordinate tandaKepemilikan; // tempat kepemilikan
  struct ScreenCoordinate lokasiRumah;
} KartuProperty;

typedef struct Step
{
  int id;                               // id step (0-39)
  struct ScreenCoordinate lokasiPlayer; // lokasi player
  char special[30];
  struct KartuProperty *kartuProperty;
} Step;

typedef struct playerProperty
{
  struct KartuProperty kartuProperty;
  int propertyGroup;
  int jumlahRumah;
} playerProperty;

typedef struct Player
{
  char simbol; //ΩΘΔΠΣ
  char nama[25];
  long long int uang;
  int posisi; // 0-39
  ScreenCoordinate posisiLayar;
  bool punyaKartuBebasPenjara;
  int jumlahProperty;
  playerProperty properties[];
} Player;

// list semua module yang terdapat pada program
void gotoxy(int x, int y);

void delay(int ms);
void setColor(int color);
void hideCursor();
void showCursor();

int rotateIndex(int currentIndex, int maxIndex, bool isPlus); // berfungsi untuk merotasi index jika sudah terakhir atau ke paling atas
void renderLogo();
int startMenuSelection();
void renderStartSelectionMenu();
void printTengah(char *teks, int x, int y, int lebar, int color);
int selectionMenu(char notSelectedStrings[][30], char selectedStrings[][30], int maxIndex, int x, int y);
int renderSelectionMenu(char notSelectedStrings[][30], char selectedStrings[][30], int selected, int maxIndex, int x, int y);
void printBoard();
int keyboardInputHandler();
void clearArea(int xStart, int yStart, int xEnd, int yEnd);

// dadu
Dadu kocokDadu();
Dadu randomDadu();
void printDadu(Dadu d, bool last);

void resetGame();
void initGame();
void startGame();

// player
void majuNLangkah(Player *p, int N);                         // berfungsi untuk memajukan player dari posisi sekarang ke posisi sekarang + N
void lompatKe(Player *p, int index);                         // berfungsi untuk melompatkan player tanpa berjalan memutar
void majuHingga(Player *p, int index);                       // berfungsi untuk memajukan player hingga posisi N
void renderPosisiPlayer(Player *p, int sebelum, int posisi); // menghapus posisi player sebelumnya dan menampilkan yang baru
int setLokasiPlayer(Player *p, int indexLokasi);

void removeProperty(playerProperty properties[], int jumlahProperty, int index);

// warna: 23, 39, 232, 71, 87, 103, 55, 135
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
KartuProperty kartuProp[28] = {
    {PROPERTY_KOTA, 0, "MLB", "Melbourne", 60, {2, 10, 30, 90, 160, 250}, 50, 23, {17, 41}, {13, 40}},
    {PROPERTY_KOTA, 0, "SYD", "Sydney", 60, {4, 20, 60, 180, 320, 450}, 50, 23, {17, 33}, {13, 32}},
    {PROPERTY_STASION, -1, "PSN", "Pasar Senen", 200, {}, 0, 0, {17, 25}, {}},
    {PROPERTY_KOTA, 1, "OSA", "Osaka", 100, {6, 30, 90, 270, 400, 550}, 50, 39, {17, 21}, {13, 20}},
    {PROPERTY_KOTA, 1, "KYO", "Kyoto", 100, {6, 30, 90, 270, 400, 550}, 50, 39, {17, 13}, {13, 12}},
    {PROPERTY_KOTA, 1, "TYO", "Tokyo", 120, {8, 40, 100, 300, 450, 600}, 50, 39, {17, 9}, {13, 8}},
    {PROPERTY_KOTA, 2, "GZH", "Guangzhou", 140, {10, 50, 150, 450, 625, 750}, 100, 232, {19, 8}, {16, 6}},
    {PROPERTY_PERUSAHAAN, -1, "PLN", "Perusahaan Listrik", 150, {}, 0, 0, {28, 8}, {}},
    {PROPERTY_KOTA, 2, "BJG", "Beijing", 140, {10, 50, 150, 450, 625, 750}, 100, 232, {37, 8}, {34, 6}},
    {PROPERTY_KOTA, 2, "SHG", "Shanghai", 160, {12, 60, 180, 500, 700, 900}, 100, 232, {46, 8}, {43, 6}},
    {PROPERTY_STASION, -1, "GBG", "Gubeng", 200, {}, 0, 0, {55, 8}, {}},
    {PROPERTY_KOTA, 3, "MSL", "Marseille", 180, {14, 70, 200, 550, 750, 950}, 100, 71, {64, 8}, {61, 6}},
    {PROPERTY_KOTA, 3, "BDX", "Bordeaux", 180, {14, 70, 200, 550, 750, 950}, 100, 71, {82, 8}, {79, 6}},
    {PROPERTY_KOTA, 3, "PAR", "Paris", 200, {16, 80, 220, 600, 800, 1000}, 100, 71, {91, 8}, {88, 6}},
    {PROPERTY_KOTA, 4, "RTD", "Rotterdam", 220, {18, 90, 250, 700, 875, 1050}, 150, 87, {94, 9}, {97, 8}},
    {PROPERTY_KOTA, 4, "DHG", "Den Haag", 220, {18, 90, 250, 700, 875, 1050}, 150, 87, {94, 17}, {97, 16}},
    {PROPERTY_KOTA, 4, "AMS", "Amsterdam", 240, {20, 100, 300, 750, 925, 1100}, 150, 87, {94, 21}, {97, 20}},
    {PROPERTY_STASION, -1, "STH", "ST. Hall", 200, {}, 0, 0, {94, 25}, {}},
    {PROPERTY_KOTA, 5, "TPN", "Tampines", 260, {22, 110, 330, 800, 975, 1150}, 150, 103, {94, 29}, {97, 28}},
    {PROPERTY_KOTA, 5, "JRG", "Jurong", 260, {22, 110, 330, 800, 975, 1150}, 150, 103, {94, 34}, {97, 32}},
    {PROPERTY_PERUSAHAAN, -1, "WTR", "PDAM", 150, {}, 0, 0, {94, 37}, {}},
    {PROPERTY_KOTA, 5, "SGP", "Singapore", 280, {24, 120, 360, 850, 1025, 1200}, 150, 103, {94, 41}, {97, 40}},
    {PROPERTY_KOTA, 6, "CCG", "Chicago", 300, {26, 130, 390, 900, 1100, 1275}, 200, 55, {91, 42}, {88, 44}},
    {PROPERTY_KOTA, 6, "LAG", "Los Angeles", 300, {26, 130, 390, 900, 1100, 1275}, 200, 55, {82, 42}, {79, 44}},
    {PROPERTY_KOTA, 6, "NYC", "New York", 320, {28, 150, 450, 1000, 1200, 1400}, 200, 55, {65, 42}, {61, 44}},
    {PROPERTY_STASION, -1, "GBR", "Gambir", 200, {}, 0, 0, {55, 42}, {}},
    {PROPERTY_KOTA, 7, "BDG", "Bandung", 350, {35, 175, 500, 1100, 1300, 1500}, 200, 135, {37, 42}, {34, 44}},
    {PROPERTY_KOTA, 7, "JKT", "Jakarta", 400, {40, 185, 550, 1200, 1500, 1700}, 200, 135, {19, 42}, {16, 44}}};

Step listStep[40] = {
    {0, {6, 44}, "GO", NULL},
    {1, {5, 41}, "", &kartuProp[0]},
    {2, {5, 37}, "DANA_UMUM", NULL},
    {3, {5, 33}, "", &kartuProp[1]},
    {4, {5, 29}, "INCOME_TAX", NULL},
    {5, {6, 25}, "", &kartuProp[2]},
    {6, {5, 21}, "", &kartuProp[3]},
    {7, {6, 17}, "KESEMPATAN", NULL},
    {8, {5, 13}, "", &kartuProp[4]},
    {9, {5, 9}, "", &kartuProp[5]},
    {10, {6, 4}, "MENGUNJUNGI_PENJARA", NULL},
    {11, {18, 3}, "", &kartuProp[6]},
    {12, {27, 3}, "", &kartuProp[7]},
    {13, {36, 3}, "", &kartuProp[8]},
    {14, {45, 3}, "", &kartuProp[9]},
    {15, {54, 3}, "", &kartuProp[10]},
    {16, {63, 3}, "", &kartuProp[11]},
    {17, {72, 4}, "DANA_UMUM", NULL},
    {18, {81, 3}, "", &kartuProp[12]},
    {19, {90, 3}, "", &kartuProp[13]},
    {20, {102, 4}, "PARKIR_GRATIS", NULL},
    {21, {103, 9}, "", &kartuProp[14]},
    {22, {102, 13}, "KESEMPATAN", NULL},
    {23, {103, 17}, "", &kartuProp[15]},
    {24, {103, 21}, "", &kartuProp[16]},
    {25, {102, 25}, "", &kartuProp[17]},
    {26, {103, 29}, "", &kartuProp[18]},
    {27, {103, 33}, "", &kartuProp[19]},
    {28, {102, 37}, "", &kartuProp[20]},
    {29, {103, 41}, "", &kartuProp[21]},
    {30, {102, 47}, "PERGI_KE_PENJARA", NULL},
    {31, {90, 47}, "", &kartuProp[22]},
    {32, {81, 47}, "", &kartuProp[23]},
    {33, {72, 47}, "DANA_UMUM", NULL},
    {34, {63, 47}, "", &kartuProp[24]},
    {35, {54, 47}, "", &kartuProp[25]},
    {36, {45, 47}, "KESEMPATAN", NULL},
    {37, {36, 47}, "", &kartuProp[26]},
    {38, {27, 47}, "LUXURY_TAX", NULL},
    {39, {18, 47}, "", &kartuProp[27]}};

HANDLE hConsole;

int main()
{
  // inisialisasi
  hConsole = GetStdHandle(STD_OUTPUT_HANDLE); // handle console (untuk mengubah warna teks)
  SetConsoleOutputCP(CP_UTF8);                // agar dapat print utf-8
  srand(time(NULL));                          // inisialisasi random
  hideCursor();
  renderLogo();
  char startNotSelectedMenuString[][30] = {"     START  ",
                                           "  HOW TO PLAY  ",
                                           "     QUIT  "};
  char startSelectedMenuString[][30] = {"   ▶ START ◀",
                                        "▶ HOW TO PLAY ◀",
                                        "   ▶ QUIT ◀"};
  int selection = selectionMenu(startNotSelectedMenuString, startSelectedMenuString, 2, 32, 7);
  if (selection == 0) // start
  {
    initGame();
  }
  else if (selection == 1) // how to play
  {
    printf("tesssss");
  }
  else
  {
    printf("quitting the game...");
    delay(1000);
    return 0;
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

void setColor(int color)
{
  SetConsoleTextAttribute(hConsole, color);
}

void hideCursor()
{
  CONSOLE_CURSOR_INFO info;
  info.dwSize = 100;
  info.bVisible = FALSE;
  SetConsoleCursorInfo(hConsole, &info);
}

void showCursor()
{
  CONSOLE_CURSOR_INFO info;
  info.dwSize = 100;
  info.bVisible = TRUE;
  SetConsoleCursorInfo(hConsole, &info);
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

void printTengah(char *teks, int x, int y, int lebar, int color)
{
  int panjang = strlen(teks);
  int xAwal = x - (panjang / 2);
  int xAkhir = xAwal + panjang;
  int yAwal = y - (lebar / 2);
  int yAkhir = yAwal + lebar;
  gotoxy(xAwal, yAwal);
  printWithColor(color, "%s", teks);
  setColor(7);
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

int selectionMenu(char notSelectedStrings[][30], char selectedStrings[][30], int maxIndex, int x, int y)
{
  int selected = 0;
  renderSelectionMenu(notSelectedStrings, selectedStrings, selected, maxIndex, x, y);
  while (1)
  {
    int keyboardResult = keyboardInputHandler();
    if (keyboardResult == ARROW_UP)
    {
      selected = rotateIndex(selected, maxIndex, false);
    }
    else if (keyboardResult == ARROW_DOWN)
    {
      selected = rotateIndex(selected, maxIndex, true);
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
  for (int i = 0; i <= maxIndex; i++)
  {
    gotoxy(x, y + i);
    i == selected ? printWithColor(14, "%s", selectedStrings[i]) : printf("%s", notSelectedStrings[i]);
  }
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
                 "║              ║████████║ LUXURY ║████████║ KESEM- ║  GBR   ║████████║  DANA  ║████████║████████║  ║ MASUK  ║  ║\n"
                 "║              ║  JKT   ║  TAX   ║  BDG   ║ PATAN  ║        ║  NYC   ║  UMUM  ║  LAG   ║  CCG   ║  ║PENJARA ║  ║\n"
                 "║ ▒█▀▀█ ▒█▀▀▀█ ║        ║        ║        ║        ║        ║        ║        ║        ║        ║  ║  ║  ║  ║  ║\n"
                 "║ ▒█░▄▄ ▒█░░▒█ ║        ║        ║        ║        ║        ║        ║        ║        ║        ║              ║\n"
                 "║ ▒█▄▄█ ▒█▄▄▄█ ║        ║        ║        ║        ║        ║        ║        ║        ║        ║  ║  ║  ║  ║  ║\n"
                 "║     $200     ║  $400  ║PAY $75 ║  $350  ║  ????  ║  $200  ║  $320  ║  $$$$  ║  $300  ║  $300  ║  ║  ║  ║  ║  ║\n"
                 "╚══════════════╩════════╩════════╩════════╩════════╩════════╩════════╩════════╩════════╩════════╩══════════════╝";
  printf("%s", board);
  // print header setiap block kota dengan warnanya masing-masing
  for (int i = 0; i < 40; i++)
  {
    if (listStep[i].kartuProperty != NULL && listStep[i].kartuProperty->jenisProperty == PROPERTY_KOTA)
    {
      ScreenCoordinate tempScreenCoordinate = listStep[i].kartuProperty->lokasiRumah;
      if (i < 10 || (i > 20 && i < 30))
      {
        for (int j = 0; j < 3; j++)
        {
          gotoxy(tempScreenCoordinate.x, tempScreenCoordinate.y + j);
          printWithColor(listStep[i].kartuProperty->color, "  ");
        }
      }
      else
      {
        gotoxy(tempScreenCoordinate.x, tempScreenCoordinate.y);
        printWithColor(listStep[i].kartuProperty->color, "        ");
      }
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

void clearArea(int xStart, int yStart, int xEnd, int yEnd){
  for (int i = yStart; i <= yEnd; i++)
  {
    gotoxy(xStart, i);
    for (int j = xStart; j <= xEnd; j++)
    {
      printf(" ");
    }
  }
}

Dadu kocokDadu()
{
  for (int i = 0; i < 20; i++)
  {
    printDadu(randomDadu(), false);
    delay(100);
  }
  Dadu hasilDadu = randomDadu();
  printDadu(hasilDadu, true);
  return hasilDadu;
}

Dadu randomDadu()
{
  Dadu tempDadu;
  tempDadu.dadu1 = rand() % 6 + 1;
  tempDadu.dadu2 = rand() % 6 + 1;
  return tempDadu;
}

void printDadu(Dadu d, bool last)
{
  char dadu[6][5][40] = {{"┌─────────┐",
                          "│         │",
                          "│    o    │",
                          "│         │",
                          "└─────────┘"},
                         {"┌─────────┐",
                          "│  o      │",
                          "│         │",
                          "│      o  │",
                          "└─────────┘"},
                         {"┌─────────┐",
                          "│  o      │",
                          "│    o    │",
                          "│      o  │",
                          "└─────────┘"},
                         {"┌─────────┐",
                          "│  o   o  │",
                          "│         │",
                          "│  o   o  │",
                          "└─────────┘"},
                         {"┌─────────┐",
                          "│  o   o  │",
                          "│    o    │",
                          "│  o   o  │",
                          "└─────────┘"},
                         {"┌─────────┐",
                          "│  o   o  │",
                          "│  o   o  │",
                          "│  o   o  │",
                          "└─────────┘"}

  };
  // print dadu pertama
  for (int i = 0; i < 5; i++)
  {
    gotoxy(24, 36 + i);
    if (last)
    {
      printWithColor(3, "%s", dadu[d.dadu1 - 1][i]);
    }
    else
    {
      printf("%s", dadu[d.dadu1 - 1][i]);
    }
  }
  // print dadu kedua
  for (int i = 0; i < 5; i++)
  {
    gotoxy(38, 36 + i);
    if (last)
    {
      printWithColor(3, "%s", dadu[d.dadu2 - 1][i]);
    }
    else
    {
      printf("%s", dadu[d.dadu2 - 1][i]);
    }
  }
}

void resetGame()
{
  // TODO reset stats player nya
}

void initGame()
{
  char NotSelectedPlayerCount[][30] = {"  2 PLAYER  ",
                                       "  3 PLAYER  ",
                                       "  4 PLAYER  "};
  char SelectedPlayerCount[][30] = {"▶ 2 PLAYER ◀",
                                    "▶ 3 PLAYER ◀",
                                    "▶ 4 PLAYER ◀"};
  int selection = selectionMenu(NotSelectedPlayerCount, SelectedPlayerCount, 2, 32, 7);
  // TODO minta jumlah player, minta nama player, random player
}

void startGame()
{
  printBoard();
  // TODO print player nya
}

void removeProperty(playerProperty properties[], int jumlahProperty, int index)
{
  // remove an element from properties array
  for (int i = index; i < jumlahProperty - 1; i++)
  {
    properties[i] = properties[i + 1];
  }
}
