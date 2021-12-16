#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <Windows.h>
#include <stdbool.h>

// mendefinisikan key pada keyboard untuk _getch()
#define ARROW_UP 72
#define ARROW_DOWN 80
#define ARROW_LEFT 75
#define ARROW_RIGHT 77
#define KEY_ENTER 13

// mendefinisikan properti
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
  char *jenisProperty; // jenis properti seperti PROPERTI_KOTA, PROPERTY_STASION, PROPERTY_PERUSAHAAN
  int kompleksId;      // id block yang sejenis
  char kodeKota[3];
  char namaKota[30];
  int hargaBeli;                            // harga beli property yang ada di papan
  int hargaSewa[6];                         // harga sewa untuk setiap tingkatan (sewa, 1 rumah, 2 rumah, 3 rumah, 4 rumah, hotel)
  int hargaUpgrade;                         // harga rumah & hotel
  int color;                                // warna pada kompleks
  bool hanyaDua;                            // true jika property hanya terdapat 2 buah pada 1 kompleks
  struct ScreenCoordinate tandaKepemilikan; // tempat kepemilikan
  struct ScreenCoordinate tandaWarna;
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
  struct KartuProperty *kartuProperty;
  int jumlahRumah;
} playerProperty;

typedef struct Player
{
  char simbol[3];              //"Ω", "Θ", "Δ", "Π", "Σ", "Ξ", "Φ", "Ψ"
  char nama[25];               // nama player
  int urutanBermain;           // urutan bermain
  bool isBot;                  // true jika player bot
  bool isBangkrut;             // true jika player sudah bangkrut
  long long int uang;          // jumlah uang player
  int posisi;                  // posisi di papan (0-39)
  bool punyaKartuBebasPenjara; // true jika punya kartu bebas penjara
  int jumlahProperty;          // jumlah property yang dimiliki
  playerProperty *properties;  // property yang dimiliki
} Player;

// list semua module yang terdapat pada program

// list modul tidak terkait dengan game
void gotoxy(int x, int y);
void delay(int ms);
void setColor(int color);
void hideCursor();
void showCursor();
int getScreenBottomIndex();
int getScreenRightIndex();
void clearArea(int xStart, int yStart, int xEnd, int yEnd);
int rotateIndex(int currentIndex, int maxIndex, bool isPlus);     // berfungsi untuk merotasi index jika sudah terakhir atau ke paling atas
void printTengah(char *teks, int x, int y, int lebar, int color); // untuk memprint teks di tengah
int keyboardArrowEnterHandler();                                  // menghandle keyboard arrow dan enter
void waitForEnter();

// list modul terkait dengan game
void renderLogo();
int startMenuSelection();
void renderStartSelectionMenu();
int selectionMenu(char notSelectedStrings[][30], char selectedStrings[][30], int maxIndex, int x, int y);
int renderSelectionMenu(char notSelectedStrings[][30], char selectedStrings[][30], int selected, int maxIndex, int x, int y);
void printBoard();

void waitForKey(int key);

// dadu
Dadu kocokDadu();
Dadu randomDadu();
void printDadu(Dadu d, bool last);
int getJumlahDadu(Dadu d);
bool isDouble(Dadu d);

void resetGame();
void initGame(Player *players, int *playerCount);
void startGame(Player *players, int *playerCount, int propertyOwnership[40]);

void initPlayerStats(Player players[], int playerCount);
void renderPlayerStats(Player players[], int playerCount);
int changePlayerMoney(Player *player, int jumlahUang); // positif kalo nambah, negatif kalo kurang
void renderPlayerMoney(Player p, int color);

// player
void majuNLangkah(Player *p, int N);             // berfungsi untuk memajukan player dari posisi sekarang ke posisi sekarang + N
void lompatKe(Player *p, int index);             // berfungsi untuk melompatkan player tanpa berjalan memutar
void majuHingga(Player *p, int index);           // berfungsi untuk memajukan player hingga posisi N
void renderPosisiPlayer(Player *p, int sebelum); // menghapus posisi player sebelumnya dan menampilkan yang baru
int setLokasiPlayer(Player *p, int indexLokasi);
void lompatKePenjara(Player *p);

// kartu kesempatan dan dana umum
void kartuKesempatanHandler(Player *p);
void kartuDanaUmumHandler(Player *p);
void renderKartuKesempatan(char *text);
void renderKartuDanaUmum(char *text);

// dalam permainan
void turnManager(Player *p);

// handler di petak
void propertyHandler(Player *players, int giliran, int step, int *propertyOwnership);
void specialHandler(Player *players, int giliran, char special[30]);

// property
void renderKartuProperty(KartuProperty *kartuProperty);
void clearKartu();

void renderRumahHotel(KartuProperty *kartuProperty, int level);

void renderKeyboardInstruction();
void removeProperty(playerProperty properties[], int jumlahProperty, int index);
void shufflePlayer(Player *players, int playerCount);
void shuffleSymbol(Player *players, int playerCount);

KartuProperty kartuProp[28] = {
    {PROPERTY_KOTA, 0, "MLB", "Melbourne", 60, {2, 10, 30, 90, 160, 250}, 50, 31, true, {17, 41}, {13, 40}, {13, 41}},
    {PROPERTY_KOTA, 0, "SYD", "Sydney", 60, {4, 20, 60, 180, 320, 450}, 50, 31, false, {17, 33}, {13, 32}, {13, 33}},
    {PROPERTY_STASION, -1, "PSN", "Pasar Senen", 200, {25, 50, 100, 200}, 0, 249, false, {17, 25}, {}, {}},
    {PROPERTY_KOTA, 1, "OSA", "Osaka", 100, {6, 30, 90, 270, 400, 550}, 50, 47, false, {17, 21}, {13, 20}, {13, 21}},
    {PROPERTY_KOTA, 1, "KYO", "Kyoto", 100, {6, 30, 90, 270, 400, 550}, 50, 47, false, {17, 13}, {13, 12}, {13, 13}},
    {PROPERTY_KOTA, 1, "TYO", "Tokyo", 120, {8, 40, 100, 300, 450, 600}, 50, 47, false, {17, 9}, {13, 8}, {13, 9}},
    {PROPERTY_KOTA, 2, "GZH", "Guangzhou", 140, {10, 50, 150, 450, 625, 750}, 100, 224, false, {19, 8}, {16, 6}, {19, 6}},
    {PROPERTY_PERUSAHAAN, -1, "PLN", "Perusahaan Listrik Negara", 150, {}, 0, 249, false, {28, 8}, {}, {}},
    {PROPERTY_KOTA, 2, "BJG", "Beijing", 140, {10, 50, 150, 450, 625, 750}, 100, 224, false, {37, 8}, {34, 6}, {37, 6}},
    {PROPERTY_KOTA, 2, "SHG", "Shanghai", 160, {12, 60, 180, 500, 700, 900}, 100, 224, false, {46, 8}, {43, 6}, {46, 6}},
    {PROPERTY_STASION, -1, "GBG", "Gubeng", 200, {25, 50, 100, 200}, 0, 249, false, {55, 8}, {}, {}},
    {PROPERTY_KOTA, 3, "MSL", "Marseille", 180, {14, 70, 200, 550, 750, 950}, 100, 79, false, {64, 8}, {61, 6}, {64, 6}},
    {PROPERTY_KOTA, 3, "BDX", "Bordeaux", 180, {14, 70, 200, 550, 750, 950}, 100, 79, false, {82, 8}, {79, 6}, {82, 6}},
    {PROPERTY_KOTA, 3, "PAR", "Paris", 200, {16, 80, 220, 600, 800, 1000}, 100, 79, false, {91, 8}, {88, 6}, {91, 6}},
    {PROPERTY_KOTA, 4, "RTD", "Rotterdam", 220, {18, 90, 250, 700, 875, 1050}, 150, 95, false, {94, 9}, {97, 8}, {97, 9}},
    {PROPERTY_KOTA, 4, "DHG", "Den Haag", 220, {18, 90, 250, 700, 875, 1050}, 150, 95, false, {94, 17}, {97, 16}, {97, 17}},
    {PROPERTY_KOTA, 4, "AMS", "Amsterdam", 240, {20, 100, 300, 750, 925, 1100}, 150, 95, false, {94, 21}, {97, 20}, {97, 21}},
    {PROPERTY_STASION, -1, "STH", "ST. Hall", 200, {25, 50, 100, 200}, 0, 249, false, {94, 25}, {}, {}},
    {PROPERTY_KOTA, 5, "TPN", "Tampines", 260, {22, 110, 330, 800, 975, 1150}, 150, 143, false, {94, 29}, {97, 28}, {97, 29}},
    {PROPERTY_KOTA, 5, "JRG", "Jurong", 260, {22, 110, 330, 800, 975, 1150}, 150, 143, false, {94, 34}, {97, 32}, {97, 33}},
    {PROPERTY_PERUSAHAAN, -1, "WTR", "Perusahaan Daerah Air Minum", 150, {}, 0, 249, false, {94, 37}, {}, {}},
    {PROPERTY_KOTA, 5, "SGP", "Singapore", 280, {24, 120, 360, 850, 1025, 1200}, 150, 143, false, {94, 41}, {97, 40}, {97, 41}},
    {PROPERTY_KOTA, 6, "CCG", "Chicago", 300, {26, 130, 390, 900, 1100, 1275}, 200, 63, false, {91, 42}, {88, 44}, {91, 44}},
    {PROPERTY_KOTA, 6, "LAG", "Los Angeles", 300, {26, 130, 390, 900, 1100, 1275}, 200, 63, false, {82, 42}, {79, 44}, {82, 44}},
    {PROPERTY_KOTA, 6, "NYC", "New York", 320, {28, 150, 450, 1000, 1200, 1400}, 200, 63, false, {65, 42}, {61, 44}, {64, 44}},
    {PROPERTY_STASION, -1, "GBR", "Gambir", 200, {25, 50, 100, 200}, 0, 249, false, {55, 42}, {}},
    {PROPERTY_KOTA, 7, "BDG", "Bandung", 350, {35, 175, 500, 1100, 1300, 1500}, 200, 111, false, {37, 42}, {34, 44}, {37, 44}},
    {PROPERTY_KOTA, 7, "JKT", "Jakarta", 400, {40, 185, 550, 1200, 1500, 1700}, 200, 111, true, {19, 42}, {16, 44}, {19, 44}}};

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
    {20, {102, 3}, "PARKIR_GRATIS", NULL},
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
  SetConsoleTitle("MiniPoly");
  hConsole = GetStdHandle(STD_OUTPUT_HANDLE); // handle console (untuk mengubah warna teks)
  SetConsoleOutputCP(CP_UTF8);                // agar dapat print utf-8
  srand(time(NULL));                          // inisialisasi random
  hideCursor();
  renderLogo();
  renderKeyboardInstruction();

  char startNotSelectedMenuString[][30] = {"     START  ",
                                           "  HOW TO PLAY  ",
                                           "     QUIT  "};
  char startSelectedMenuString[][30] = {"   ▶ START ◀",
                                        "▶ HOW TO PLAY ◀",
                                        "   ▶ QUIT ◀"};
  int selection = selectionMenu(startNotSelectedMenuString, startSelectedMenuString, 2, 32, 7);
  if (selection == 0) // start
  {
    Player players[4];
    int propertyOwnership[40];
    for (int i = 0; i < 40; i++)
    {
      propertyOwnership[i] = -1;
    }
    int playerCount = 0;
    initGame(players, &playerCount);

    // main game
    startGame(players, &playerCount, propertyOwnership);
  }
  else if (selection == 1) // how to play
  {
    printf("ini how to play");
  }
  else // quit
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
  Sleep(ms);
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
int getScreenBottomIndex()
{
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
  return csbi.srWindow.Bottom - csbi.srWindow.Top;
}
int getScreenRightIndex()
{
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
  return csbi.srWindow.Right - csbi.srWindow.Left;
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
  gotoxy(x, y);
  int panjangTeks = strlen(teks);
  int spasiKiri = (lebar - panjangTeks) / 2;
  int spasiKanan = lebar - panjangTeks - spasiKiri;
  for (int i = 0; i < spasiKiri; i++)
  {
    printWithColor(color, " ");
  }
  printWithColor(color, "%s", teks);
  for (int i = 0; i < spasiKanan; i++)
  {
    printWithColor(color, " ");
  }
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
    int keyboardResult = keyboardArrowEnterHandler();
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
    if (i == selected)
    {
      printWithColor(14, "%s", selectedStrings[i]);
    }
    else
    {
      printf("%s", notSelectedStrings[i]);
    }
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
      ScreenCoordinate tempScreenCoordinate = listStep[i].kartuProperty->tandaWarna;
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

int keyboardArrowEnterHandler()
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

void clearArea(int xStart, int yStart, int xEnd, int yEnd)
{
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
int getJumlahDadu(Dadu d)
{
  return d.dadu1 + d.dadu2;
}
bool isDouble(Dadu d)
{
  return d.dadu1 == d.dadu2;
}

void resetGame()
{
  // TODO reset stats player nya
}

void initGame(Player *players, int *playerCount)
{
  char NotSelectedPlayerCount[][30] = {"  2 PLAYER  ",
                                       "  3 PLAYER  ",
                                       "  4 PLAYER  "};
  char SelectedPlayerCount[][30] = {"▶ 2 PLAYER ◀",
                                    "▶ 3 PLAYER ◀",
                                    "▶ 4 PLAYER ◀"};
  int selection = selectionMenu(NotSelectedPlayerCount, SelectedPlayerCount, 2, 34, 7);
  *playerCount = selection + 2;
  clearArea(0, 7, 72, 10);

  char notSelectedBotCount[][30] = {"  Tanpa Bot  ",
                                    "    1 BOT     ",
                                    "    2 BOT     ",
                                    "    3 BOT     "};
  char SelectedBotCount[][30] = {"▶ Tanpa Bot ◀",
                                 "  ▶ 1 BOT ◀   ",
                                 "  ▶ 2 BOT ◀   ",
                                 "  ▶ 3 BOT ◀   "};
  int selectionBot = selectionMenu(notSelectedBotCount, SelectedBotCount, selection + 1, 33, 7);
  clearArea(0, 7, 72, 10);
  gotoxy(12, 7);
  printf("Tentukan nama pemain manusia");
  for (int i = 1; i <= *playerCount - selectionBot; i++)
  {
    players[i - 1].isBot = false;

    gotoxy(12, 8 + i);
    printf("Nama pemain %d: ", i);
    scanf("%[^\n]", players[i - 1].nama);
    getchar();
  }
  for (int i = *playerCount - selectionBot + 1; i <= *playerCount; i++)
  {
    players[i - 1].isBot = true;

    snprintf(players[i - 1].nama, sizeof(players[i - 1].nama), "Bot %d", i);
  }
  gotoxy(12, 9 + *playerCount - selectionBot);
  printf("Mengacak urutan permainan...\n");
  shufflePlayer(players, *playerCount);
  shuffleSymbol(players, *playerCount);
  delay(800);
  for (int i = 0; i < *playerCount; i++)
  {
    players[i].uang = 1500;
    players[i].urutanBermain = i;
    players[i].posisi = 0;
    players[i].punyaKartuBebasPenjara = false;
    players[i].isBangkrut = false;
    players[i].jumlahProperty = 0;
    players[i].properties = (playerProperty *)malloc(sizeof(playerProperty) * 28); // set properties untuk tiap player sebanyak max 28 karena properties hanya ada 28
  }
  gotoxy(12, 10 + *playerCount - selectionBot);
  printf("urutan pemain:");
  for (int i = 0; i < *playerCount; i++)
  {
    gotoxy(12, 11 + *playerCount - selectionBot + i);
    printf("%d. (%s) %s\n", i + 1, players[i].simbol, players[i].nama);
  }
  printf("\n");
  gotoxy(12, 11 + *playerCount * 2 - selectionBot);
  printf("Tekan ENTER untuk melanjutkan...");
  waitForEnter();
}

void startGame(Player *players, int *playerCount, int propertyOwnership[40])
{
  system("cls");
  gotoxy(0, 0);
  printBoard();
  initPlayerStats(players, *playerCount);
  for (int i = 0; i < *playerCount; i++)
  {
    renderPosisiPlayer(&players[i], 0);
  }
  int pemainTersisa = *playerCount;
  int giliran = 0;
  int doubleCount = 0;
  while (pemainTersisa > 1)
  {
    clearKartu();

    // TODO cek player lagi di penjara ato engga
    Dadu hasilDadu = kocokDadu();
    doubleCount = isDouble(hasilDadu) ? doubleCount + 1 : 0;
    // cek player triple double
    if (doubleCount == 3)
    {
      doubleCount = 0;
      // TODO player masuk penjara
    }
    else
    {
      majuNLangkah(&players[giliran], getJumlahDadu(hasilDadu));
      if (strcmp(listStep[players[giliran].posisi].special, "") == 0)
      {
        propertyHandler(players, giliran, players[giliran].posisi, propertyOwnership);
      }
      else
      {
        specialHandler(players, giliran, listStep[players[giliran].posisi].special);
      }
    }

    if (doubleCount == 0)
    {
      giliran = rotateIndex(giliran, *playerCount - 1, true);
    }
    delay(3000);
  }
  getchar();
}

void initPlayerStats(Player players[], int playerCount)
{
  char statsTemplate[][100] = {"┌─────────────STATS─────────────┐",
                               "│                               │",
                               "│                               │",
                               "│                               │",
                               "│                               │",
                               "│                               │",
                               "│                               │",
                               "│                               │",
                               "│                               │",
                               "│                               │",
                               "│                               │",
                               "│                               │",
                               "└───────────────────────────────┘"};

  for (int i = 0; i < 13; i++)
  {
    gotoxy(116, 0 + i);
    printf("%s", statsTemplate[i]);
  }
  for (int i = 0; i < playerCount; i++)
  {
    gotoxy(118, 1 + (players[i].urutanBermain * 3));
    printf("%s", players[i].nama);
    gotoxy(144, 1 + (players[i].urutanBermain * 3));
    printf("(%s)", players[i].simbol);
    renderPlayerMoney(players[i], 7);
  }
}

int changePlayerMoney(Player *player, int jumlah)
{
  int uangSebelum = player->uang;
  int uangSetelah = uangSebelum + jumlah;
  if (jumlah < 0)
  {
    while (player->uang > uangSetelah)
    {
      if (player->uang - 4 >= uangSetelah)
      {
        player->uang -= 4;
      }
      else
      {
        player->uang--;
      }
      renderPlayerMoney(*player, 4);
      delay(1);
    }
  }
  else
  {
    while (player->uang < uangSetelah)
    {
      if (player->uang + 4 <= uangSetelah)
      {
        player->uang += 4;
      }
      else
      {
        player->uang++;
      }
      renderPlayerMoney(*player, 2);
      delay(1);
    }
  }
  renderPlayerMoney(*player, 7);
  return player->uang;
}

void renderPlayerMoney(Player p, int color)
{
  gotoxy(118, 2 + (p.urutanBermain * 3));
  printWithColor(color, "$%lld", p.uang);
}

void majuNLangkah(Player *p, int N)
{
  for (int i = 0; i < N; i++)
  {
    delay(200);
    int posisiSebelum = p->posisi;
    p->posisi = rotateIndex(p->posisi, 39, true);
    renderPosisiPlayer(p, posisiSebelum);
    if (p->posisi == 0)
    {
      changePlayerMoney(p, 200);
    }
  }
}
void lompatKe(Player *p, int index)
{
  p->posisi = index;
}

void renderPosisiPlayer(Player *p, int sebelum)
{
  gotoxy(listStep[sebelum].lokasiPlayer.x + p->urutanBermain, listStep[sebelum].lokasiPlayer.y);
  printf(" ");
  gotoxy(listStep[p->posisi].lokasiPlayer.x + p->urutanBermain, listStep[p->posisi].lokasiPlayer.y);
  printWithColor(240, "%s", p->simbol);
}

void propertyHandler(Player *players, int giliran, int step, int *propertyOwnership)
{
  KartuProperty *kartuProperty = listStep[step].kartuProperty;
  renderKartuProperty(kartuProperty);
  // artinya property belum dimiliki oleh siapapun
  if (propertyOwnership[step] == -1)
  {

    // TODO pemain bisa membeli property
  }
  // artinya property udah dimiliki
  else
  {
    // TODO pemain lain udah ada yang beli, pemain ini harus dibayar oleh yang nginep dan bisa ambil alih
  }
}
void specialHandler(Player *players, int giliran, char special[30])
{
  if (strcmp(special, "DANA_UMUM") == 0)
  {
    kartuDanaUmumHandler(&players[giliran]);
  }
  else if (strcmp(special, "KESEMPATAN") == 0)
  {
    kartuKesempatanHandler(&players[giliran]);
  }
  else if (strcmp(special, "INCOME_TAX") == 0)
  {
  }
  else if (strcmp(special, "LUXURY_TAX") == 0)
  {
  }
  else if (strcmp(special, "PARKIR_GRATIS") == 0)
  {
  }
  else if (strcmp(special, "PERGI_KE_PENJARA") == 0)
  {
  }
}
void kartuKesempatanHandler(Player *p)
{
  char kesempatan[][200] = {"Pergi ke Guangzhou. Jika melewati\n\"GO\" ambil $200",
                            "Kartu bebas penjara. Kartu ini\ndapat disimpan hingga dibutuhkan",
                            "Pergi ke Stasiun Pasar Senen.\nJika melewatu \"GO\" ambil $200",
                            "Pergi ke stasiun terdekat.\nJika sudah dimiliki pemain lain,\nbayar 2 kali lipat\nharga sewanya",
                            "Pergi ke Amsterdam. Jika melewati\n\"GO\" ambil $200",
                            "Ada biaya perbaikan rumah, bayar\n$25 untuk setiap rumah dan\n$100 untuk setiap hotel",
                            "Pergi ke \"GO\". Ambil $200",
                            "Bank memberikan Anda gratifikasi $50",
                            "Bayar pajak untuk orang miskin $15",
                            "Pergi ke perusahaan terdekat.\nJika sudah dimiliki pemain lain,\nbayar pemilik sebanyak 10x jumlah\nlemparan dadu",
                            "Anda masuk penjara, tidak melewati\n\"GO\", tidak mendapatkan $200",
                            "Anda terpilih sebagai ketua dari\npapan ini. Bayar setiap pemain $50",
                            "Jalan-jalan ke Jakarta.\nPergi ke Jakarta",
                            "Mundur 3 langkah",
                            "Bangunan dan pinjaman anda sudah\njatuh tempo. Ambil $150 dari Bank",
                            "Saatnya jalan-jalan.\nPergi ke Parkir Bebas"};
  // random dari 0 ke 15
  int random = rand() % 15;
  // render kartu
  renderKartuKesempatan(kesempatan[random]);
  // TODO buat handlernya
}
void kartuDanaUmumHandler(Player *p)
{

  char danaUmum[][200] = {"Bayar pajak sekolah $150",
                          "Grand Opening Opera.\nSetiap pemain membayar Anda $50",
                          "Anda mewarisi $100",
                          "Bayar biaya rumah sakit $100",
                          "Pengembalian dana pajak pendapatan.\nAmbil $20",
                          "Anda masuk penjara, tidak melewati\n\"GO\", tidak mendapatkan $200",
                          "Kartu bebas penjara. Kartu ini\ndapat disimpan hingga dibutuhkan",
                          "Bayar biaya perbaikan jalan.\nBayar $40 per rumah dan\n$115 per hotel",
                          "Bank error, Anda mendapatkan $200",
                          "Pergi ke \"GO\". Ambil $200",
                          "Anda mendapat hadiah natal $100",
                          "Biaya dokter, bayar $50",
                          "Dari penjualan saham,\nAnda mendapatkan $45",
                          "Dapatkan $25 karena Anda telah\nmenjadi pelayan",
                          "Jatuh tempo dana asuransi jiwa.\nAnda mendapat $100"};
  // random dari 0 ke 15
  int random = rand() % 15;
  // render kartu
  renderKartuDanaUmum(danaUmum[random]);
  // TODO buat handlernya
}

void renderKartuKesempatan(char *text)
{
  char template[][121] = {"┌──────────────────────────────────────┐",
                          "│              KESEMPATAN              │",
                          "├──────────────────────────────────────┤",
                          "│                                      │",
                          "│                                      │",
                          "│                                      │",
                          "│                                      │",
                          "│                                      │",
                          "│                                      │",
                          "│                                      │",
                          "│                                      │",
                          "│                                      │",
                          "└──────────────────────────────────────┘"};
  for (int i = 0; i < 13; i++)
  {
    gotoxy(116, 14 + i);
    printf("%s", template[i]);
  }

  char *token = strtok(text, "\n");
  int currentLine = 0;
  while (token != NULL)
  {
    printTengah(token, 118, 20 + currentLine, 36, 7);
    token = strtok(NULL, "\n");
    currentLine++;
  }
}
void renderKartuDanaUmum(char *text)
{
  char template[][121] = {"┌──────────────────────────────────────┐",
                          "│              DANA UMUM               │",
                          "├──────────────────────────────────────┤",
                          "│                                      │",
                          "│                                      │",
                          "│                                      │",
                          "│                                      │",
                          "│                                      │",
                          "│                                      │",
                          "│                                      │",
                          "│                                      │",
                          "│                                      │",
                          "└──────────────────────────────────────┘"};
  for (int i = 0; i < 13; i++)
  {
    gotoxy(116, 14 + i);
    printf("%s", template[i]);
  }

  char *token = strtok(text, "\n");
  int currentLine = 0;
  while (token != NULL)
  {
    printTengah(token, 118, 20 + currentLine, 36, 7);
    token = strtok(NULL, "\n");
    currentLine++;
  }
}

void renderKartuProperty(KartuProperty *kartuProperty)
{
  if (strcmp(kartuProperty->jenisProperty, PROPERTY_KOTA) == 0)
  {
    char kartuPropertyText[][100] = {"┌─────────────────────────────┐",
                                     "│                             │",
                                     "├─────────────────────────────┤",
                                     "│                             │",
                                     "│ Jika sudah memiliki:        │",
                                     "│ 1 Rumah : $                 │",
                                     "│ 2 Rumah : $                 │",
                                     "│ 3 Rumah : $                 │",
                                     "│ 4 Rumah : $                 │",
                                     "│ 1 Hotel : $                 │",
                                     "│                             │",
                                     "│ Harga rumah & hotel : $     │",
                                     "│                             │",
                                     "│ Harga jual dari total asset │",
                                     "│ Secara sepihak : 70%        │",
                                     "│ Ambil alih     : 150%       │",
                                     "└─────────────────────────────┘"};
    for (int i = 0; i < 17; i++)
    {
      gotoxy(116, 14 + i);
      printf("%s", kartuPropertyText[i]);
    }
    printTengah(kartuProperty->namaKota, 117, 15, 29, kartuProperty->color);
    char teksSewa[15];
    sprintf(teksSewa, "Sewa $%d", kartuProperty->hargaSewa[0]);
    printTengah(teksSewa, 117, 17, 29, 7);
    for (int i = 1; i <= 5; i++)
    {
      gotoxy(129, 18 + i);
      printf("%d", kartuProperty->hargaSewa[i]);
    }
    gotoxy(141, 25);
    printf("%d", kartuProperty->hargaUpgrade);
  }
  else if (strcmp(kartuProperty->jenisProperty, PROPERTY_STASION) == 0)
  {
    char kartuPropertyText[][100] = {"┌─────────────────────────────┐",
                                     "│                             │",
                                     "├─────────────────────────────┤",
                                     "│         Harga Sewa          │",
                                     "│     Jika sudah memiliki     │",
                                     "│ 1 Stasion : $               │",
                                     "│ 2 Stasion : $               │",
                                     "│ 3 Stasion : $               │",
                                     "│ 4 Stasion : $               │",
                                     "│                             │",
                                     "│                             │",
                                     "│    Harga jual 1 stasion     │",
                                     "│ Secara sepihak : 70%        │",
                                     "│ Ambil alih     : 150%       │",
                                     "│                             │",
                                     "│                             │",
                                     "└─────────────────────────────┘"};
    for (int i = 0; i < 17; i++)
    {
      gotoxy(116, 14 + i);
      printf("%s", kartuPropertyText[i]);
    }
    printTengah(kartuProperty->namaKota, 117, 15, 29, kartuProperty->color);
    for (int i = 0; i < 4; i++)
    {
      gotoxy(131, 19 + i);
      printf("%d", kartuProperty->hargaSewa[i]);
    }
  }
  else if (strcmp(kartuProperty->jenisProperty, PROPERTY_PERUSAHAAN) == 0)
  {
    char kartuPropertyText[][100] = {"┌─────────────────────────────┐",
                                     "│                             │",
                                     "├─────────────────────────────┤",
                                     "│         Harga Sewa          │",
                                     "│                             │",
                                     "│ jika 1 perusahaan dimiliki: │",
                                     "│ harga sewa sejumlah 4x      │",
                                     "│ lempar dadu                 │",
                                     "│                             │",
                                     "│ jika 2 perusahaan dimiliki: │",
                                     "│ harga sewa sejumlah 10x     │",
                                     "│ lempar dadu                 │",
                                     "│                             │",
                                     "│   Harga jual 1 perusahaan   │",
                                     "│ Secara sepihak : 70%        │",
                                     "│ Ambil alih     : 150%       │",
                                     "└─────────────────────────────┘"};
    for (int i = 0; i < 17; i++)
    {
      gotoxy(116, 14 + i);
      printf("%s", kartuPropertyText[i]);
    }
    printTengah(kartuProperty->namaKota, 117, 15, 29, kartuProperty->color);
  }
}
void clearKartu()
{
  clearArea(116, 14, 156, 30);
}

void removeProperty(playerProperty properties[], int jumlahProperty, int index)
{
  // remove an element from properties array
  for (int i = index; i < jumlahProperty - 1; i++)
  {
    properties[i] = properties[i + 1];
  }
}

void shufflePlayer(Player *players, int playerCount)
{
  for (int i = 0; i < playerCount; i++)
  {
    int random = rand() % playerCount;
    Player temp = players[i];
    players[i] = players[random];
    players[random] = temp;
  }
}

void shuffleSymbol(Player *players, int playerCount)
{
  char simbol[][3] = {"Ω", "Θ", "Δ", "Π", "Σ", "Ξ", "Φ", "Ψ"};
  int randomIndex[] = {0, 1, 2, 3, 4, 5, 6, 7};
  for (int i = 0; i < 7; i++)
  {
    int random = rand() % 8;
    int temp = randomIndex[i];
    randomIndex[i] = randomIndex[random];
    randomIndex[random] = temp;
  }
  for (int i = 0; i < playerCount; i++)
  {
    strcpy(players[i].simbol, simbol[randomIndex[i]]);
  }
}

void renderKeyboardInstruction()
{
  char instructionText[] = "↑ ↓: UP / DOWN      ENTER: SELECT      ESC: BACK";
  gotoxy(0, getScreenBottomIndex());
  for (int i = 0; i <= getScreenRightIndex(); i++)
  {
    if (i < 53)
    {
      printWithColor(112, "%c", instructionText[i]);
    }
    else
    {
      printWithColor(112, " ");
    }
  }
}

void waitForEnter()
{
  while (1)
  {
    int ch = _getch();
    if (ch == KEY_ENTER)
    {
      break;
    }
  }
}