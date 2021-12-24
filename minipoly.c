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

#define MULTIPLIER_AMBIL_ALIH 1.5
#define MULTIPLIER_JUAL_SEPIHAK 0.5

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
  char *jenisProperty;                      // jenis properti seperti PROPERTI_KOTA, PROPERTY_STASION, PROPERTY_PERUSAHAAN
  int kompleksId;                           // id block yang sejenis
  char kodeKota[3];                         // kode nama kota
  char namaKota[30];                        // nama dari kota
  int hargaBeli;                            // harga beli property yang ada di papan
  int hargaSewa[6];                         // harga sewa untuk setiap tingkatan (sewa, 1 rumah, 2 rumah, 3 rumah, 4 rumah, hotel)
  int hargaUpgrade;                         // harga rumah & hotel
  int warna;                                // warna pada kompleks
  bool hanyaDua;                            // true jika property hanya terdapat 2 buah property pada 1 kompleks
  struct ScreenCoordinate tandaKepemilikan; // tempat kepemilikan (untuk render)
  struct ScreenCoordinate tandaWarna;       // tempat warna (untuk render)
  struct ScreenCoordinate lokasiRumah;      // lokasi rumah di layar (untuk render)
} KartuProperty;

typedef struct Petak
{
  int id;                               // id step (0-39)
  struct ScreenCoordinate lokasiPlayer; // lokasi player
  char special[30];                     // jika petak special, maka tidak ada kartu property
  struct KartuProperty *kartuProperty;  // jika ada kartu property (bukan termasuk petak special)
} Petak;

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
  int sisaTurnPenjara;         // jumlah turn yang tersisa
  int jumlahProperty;          // jumlah property yang dimiliki
  bool properties[40];         // setiap index merujuk kepada nilai kebenaran kepemilikan property pada petak index oleh player. Contoh, apabila player memiliki property di jakarta, maka properties[39] = true
} Player;

typedef struct PlayerProperty
{
  Player *pemilik;
  int level; // 0 tanpa rumah, 1 rumah, 2 rumah, 3 rumah, 4 rumah, 5 hotel
} PlayerProperty;

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
void playGame();
void startGame();
void howToPlay();
void quitGame();

void startMenu();
void renderHowToPlay();
void renderLogo();
int selectionMenu(char notSelectedStrings[][50], char selectedStrings[][50], int maxIndex, int x, int y);
int renderSelectionMenu(char notSelectedStrings[][50], char selectedStrings[][50], int selectedIndex, int maxIndex, int x, int y, int chosen);
void renderBoard();
int playerSelectionMenu(char selections[][50], int maxIndex); // untuk player memilih ketika sedang berada dalam in-game
void printGiliranText(Player p);
void clearGiliranText();
void clearPlayerSelectionText();
// dadu
Dadu kocokDadu();
Dadu randomDadu();
void renderDadu(Dadu d, bool last);
int getJumlahDadu(Dadu d);
bool isDouble(Dadu d);

void initGame(Player *players, int *playerCount);
void startGame(Player *players, int *playerCount, PlayerProperty playerProperties[40]);

void initPlayerStats(Player players[], int playerCount);
void renderPlayerStats(Player players[], int playerCount);
int changePlayerMoney(Player *player, int jumlahUang); // positif kalo nambah, negatif kalo kurang
void renderPlayerMoney(Player p, int color);

// player
void majuNLangkah(Player *p, int N);                  // berfungsi untuk memajukan player dari posisi sekarang ke posisi sekarang + N
void lompatKe(Player *p, int index);                  // berfungsi untuk melompatkan player tanpa berjalan memutar
void majuHingga(Player *p, int index);                // berfungsi untuk memajukan player hingga posisi N
void majuHinggaJenisProperty(Player *p, char *jenis); // berfungsi untuk memajukan player hingga posisi yang memiliki tipe property tertentu
void cekMenyentuhStart(Player *p);                    // berfungsi untuk mengecek apakah player sudah menyentuh start
void renderPosisiPlayer(Player *p, int sebelum);      // menghapus posisi player sebelumnya dan menampilkan yang baru
void lompatKePenjara(Player *p);
bool cekUangCukup(long long int uang, long long int harga);
void turnManager(Player *players, int *playerCount, PlayerProperty playerProperties[40]);

// jual beli properti
void beliProperty(Player *p, PlayerProperty *playerProperty, int posisi);
void jualProperty(Player *p, PlayerProperty *playerProperty, int posisi);
void ambilAlihProperty(Player *p, PlayerProperty *playerProperty, int posisi);
void renderProperty(PlayerProperty playerProperty, int posisi);

// kartu kesempatan dan dana umum
bool kartuKesempatanHandler(Player players[], int giliran, int playerCount, PlayerProperty playerProperties[40]);
bool kartuDanaUmumHandler(Player players[], int giliran, int playerCount, PlayerProperty playerProperties[40]);

// render
void renderKartuKesempatan(char *text);
void renderKartuDanaUmum(char *text);

// kartu property
void renderKartuProperty(KartuProperty *kartuProperty);

// clear untuk kartu property dan kartu dana umum/kesempatan
void clearKartu();

// handler di petak
void propertyHandler(Player *players, int giliran, int posisi, PlayerProperty playerProperties[40]);
bool specialHandler(Player *players, int giliran, char special[30], int playerCount, PlayerProperty playerProperties[40]);
bool penjaraHandler(Player *p);

void renderRumahHotel(KartuProperty *kartuProperty, int level);

void renderKeyboardInstruction();
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

Petak listPetak[40] = {
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

int grupkompleks[][3] = {
    {1, 3},
    {6, 8, 9},
    {11, 13, 14},
    {16, 18, 19},
    {21, 23, 24},
    {26, 27, 29},
    {31, 32, 34},
    {37, 39}};

HANDLE hConsole;

int main()
{
  // inisialisasi
  SetConsoleTitle("MiniPoly");                // set judul
  hConsole = GetStdHandle(STD_OUTPUT_HANDLE); // handle console (untuk mengubah warna teks)
  SetConsoleOutputCP(CP_UTF8);                // agar dapat print utf-8
  // ShowWindow(GetConsoleWindow(), SW_SHOWMAXIMIZED); // untuk membuka console secara full screen
  srand(time(NULL)); // inisialisasi random
  hideCursor();

  // renderKeyboardInstruction();

  int selection = 0;
  while (selection != 2)
  {
    renderLogo();
    char startNotSelectedMenuString[][50] = {"     START  ",
                                             "  HOW TO PLAY  ",
                                             "     QUIT  "};
    char startSelectedMenuString[][50] = {"   ▶ START ◀",
                                          "▶ HOW TO PLAY ◀",
                                          "   ▶ QUIT ◀"};
    selection = selectionMenu(startNotSelectedMenuString, startSelectedMenuString, 2, 32, 7);
    if (selection == 0) // start
    {
      playGame();
    }
    else if (selection == 1) // how to play
    {
      howToPlay();
    }
    else // quit
    {
      quitGame();
    }
  }
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

void playGame()
{
  Player players[4];
  PlayerProperty playerProperties[40]; // untuk menyimpan properti yang dimiliki player
  for (int i = 0; i < 40; i++)
  {
    playerProperties[i].pemilik = NULL;
    playerProperties[i].level = 0;
  }
  int playerCount = 0;
  initGame(players, &playerCount);

  // main game
  startGame(players, &playerCount, playerProperties);
}
void howToPlay()
{
  renderHowToPlay();
}
void quitGame()
{
  system("cls");
  printf("quitting the game...");
  delay(1000);
  exit(0);
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
        printWithColor(11, "%s\n", &(minipolyTrainText[j][strlen(minipolyTrainText[j]) - i]));
      }
    }
    delay(25);
  }
}
void renderHowToPlay()
{
  system("cls");
  FILE *fp;
  char line[256];
  int i = 0;
  fp = fopen("howtoplay.txt", "r");
  if (fp == NULL)
  {
    printf("File hilang!\n");
  }
  else
  {
    char c = fgetc(fp);
    while (c != EOF)
    {
      printf("%c", c);
      c = fgetc(fp);
    }
  }
  fclose(fp);
  printf("\n\nTekan apapun untuk kembali ke main menu...");
  getchar();
  system("cls");
}
int selectionMenu(char notSelectedStrings[][50], char selectedStrings[][50], int maxIndex, int x, int y)
{
  fflush(stdin);
  int selected = 0;
  renderSelectionMenu(notSelectedStrings, selectedStrings, selected, maxIndex, x, y, false);
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
      renderSelectionMenu(notSelectedStrings, selectedStrings, selected, maxIndex, x, y, true);
      break;
    }
    renderSelectionMenu(notSelectedStrings, selectedStrings, selected, maxIndex, x, y, false);
  }
  return selected;
}
int renderSelectionMenu(char notSelectedStrings[][50], char selectedStrings[][50], int selectedIndex, int maxIndex, int x, int y, int chosen)
{
  for (int i = 0; i <= maxIndex; i++)
  {
    gotoxy(x, y + i);
    if (i == selectedIndex)
    {
      if (chosen)
      {
        printWithColor(9, "%s", selectedStrings[i]);
      }
      else
      {
        printWithColor(14, "%s", selectedStrings[i]);
      }
    }
    else
    {
      printf("%s", notSelectedStrings[i]);
    }
  }
}

void renderBoard()
{
  char board[] = "╔══════════════╦════════╦════════╦════════╦════════╦════════╦════════╦════════╦════════╦════════╦══════════════╗\n"
                 "║  ║ HANYA  ║  ║  GZH   ║  PLN   ║  BJG   ║  SHN   ║  GBG   ║  MSL   ║  DANA  ║  BDX   ║  PAR   ║    PARKIR    ║\n"
                 "║ MENGUNJUNGI  ║        ║        ║        ║        ║        ║        ║  UMUM  ║        ║        ║    BEBAS     ║\n"
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
    if (listPetak[i].kartuProperty != NULL && listPetak[i].kartuProperty->jenisProperty == PROPERTY_KOTA)
    {
      ScreenCoordinate tempScreenCoordinate = listPetak[i].kartuProperty->tandaWarna;
      if (i < 10 || (i > 20 && i < 30))
      {
        for (int j = 0; j < 3; j++)
        {
          gotoxy(tempScreenCoordinate.x, tempScreenCoordinate.y + j);
          printWithColor(listPetak[i].kartuProperty->warna, "  ");
        }
      }
      else
      {
        gotoxy(tempScreenCoordinate.x, tempScreenCoordinate.y);
        printWithColor(listPetak[i].kartuProperty->warna, "        ");
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
    renderDadu(randomDadu(), false);
    delay(100);
  }
  Dadu hasilDadu = randomDadu();
  renderDadu(hasilDadu, true);
  return hasilDadu;
}
Dadu randomDadu()
{
  Dadu tempDadu;
  tempDadu.dadu1 = rand() % 6 + 1;
  tempDadu.dadu2 = rand() % 6 + 1;
  return tempDadu;
}
void renderDadu(Dadu d, bool last)
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

void initGame(Player *players, int *playerCount)
{
  char NotSelectedPlayerCount[][50] = {"  2 PLAYER  ",
                                       "  3 PLAYER  ",
                                       "  4 PLAYER  "};
  char SelectedPlayerCount[][50] = {"▶ 2 PLAYER ◀",
                                    "▶ 3 PLAYER ◀",
                                    "▶ 4 PLAYER ◀"};
  int selection = selectionMenu(NotSelectedPlayerCount, SelectedPlayerCount, 2, 34, 7);
  *playerCount = selection + 2;
  clearArea(0, 7, 72, 10);

  char notSelectedBotCount[][50] = {"  Tanpa Bot  ",
                                    "    1 BOT     ",
                                    "    2 BOT     ",
                                    "    3 BOT     "};
  char SelectedBotCount[][50] = {"▶ Tanpa Bot ◀",
                                 "  ▶ 1 BOT ◀   ",
                                 "  ▶ 2 BOT ◀   ",
                                 "  ▶ 3 BOT ◀   "};
  int selectionBot = selectionMenu(notSelectedBotCount, SelectedBotCount, selection + 1, 33, 7);
  clearArea(0, 7, 72, 10);
  gotoxy(12, 7);
  printf("Tentukan nama pemain manusia (max 25 karakter)");
  for (int i = 1; i <= *playerCount - selectionBot; i++)
  {
    players[i - 1].isBot = false;

    gotoxy(12, 8 + i);
    printf("Nama pemain %d: ", i);
    showCursor();
    scanf("%[^\n]", players[i - 1].nama);
    getchar();
    hideCursor();
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
  // untuk mereset garbage value
  for (int i = 0; i < *playerCount; i++)
  {
    players[i].uang = 1500;
    players[i].urutanBermain = i;
    players[i].posisi = 0;
    players[i].punyaKartuBebasPenjara = false;
    players[i].isBangkrut = false;
    players[i].jumlahProperty = 0;
    players[i].sisaTurnPenjara = 0;
    for (int j = 0; j < 40; j++)
    {
      players[i].properties[j] = false;
    }
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

void startGame(Player *players, int *playerCount, PlayerProperty playerProperties[40])
{
  system("cls");
  gotoxy(0, 0);
  renderBoard();
  initPlayerStats(players, *playerCount);
  for (int i = 0; i < *playerCount; i++)
  {
    renderPosisiPlayer(&players[i], 0);
  }
  turnManager(players, playerCount, playerProperties);
  // TODO render pemenang
  system("cls");
  printf("Anda menang!!!!!");
  getchar();
}

void turnManager(Player *players, int *playerCount, PlayerProperty playerProperties[40])
{
  int giliran = 0;
  int doubleCount = 0;
  int pemainTersisa = *playerCount;
  while (pemainTersisa > 1)
  {
    // kalau giliran sekarang pemain sudah bangkrut maka langsung continue ke pemain selanjutnya
    if (players[giliran].isBangkrut)
    {
      giliran = rotateIndex(giliran, *playerCount - 1, true);
      continue;
    }

    clearKartu();
    printGiliranText(players[giliran]);
    char selections[][50] = {"Kocok Dadu"};
    int selection = playerSelectionMenu(selections, 0);

    bool turnFinished = false; // untuk menandakan apakah turn sudah selesai atau belum
    // TODO cek player lagi di penjara ato engga
    if (players[giliran].sisaTurnPenjara > 0)
    {
      turnFinished = penjaraHandler(&players[giliran]);
      if (turnFinished) // kalau belum keluar dari penjara maka turn finished = true
      {
        giliran = rotateIndex(giliran, *playerCount - 1, true);
        continue;
      }
    }

    Dadu hasilDadu = kocokDadu();
    int doubleCount = isDouble(hasilDadu) ? doubleCount + 1 : 0;

    // cek player triple double
    if (doubleCount == 3)
    {
      doubleCount = 0;
      lompatKePenjara(&players[giliran]);
      continue;
    }

    majuNLangkah(&players[giliran], getJumlahDadu(hasilDadu));
    while (!turnFinished)
    {
      clearKartu();
      if (strcmp(listPetak[players[giliran].posisi].special, "") == 0)
      {
        propertyHandler(players, giliran, players[giliran].posisi, playerProperties);
        turnFinished = true;
      }
      else
      {
        turnFinished = specialHandler(players, giliran, listPetak[players[giliran].posisi].special, *playerCount, playerProperties);
      }
    }

    // cek apakah player sekarang bangkrut
    if (players[giliran].uang < 0)
    {
      players[giliran].isBangkrut = true;
      renderPlayerMoney(players[giliran], 4);
      pemainTersisa--;
    }

    // kalo engga double pindah giliran ke player selanjutnya
    if (doubleCount == 0)
    {
      giliran = rotateIndex(giliran, *playerCount - 1, true);
    }
  }
}

void printGiliranText(Player p)
{
  clearGiliranText();
  gotoxy(117, 32);
  printf("Giliran %s (%s)", p.nama, p.simbol);
}
void clearGiliranText()
{
  gotoxy(117, 32);
  printf("                                                  ");
}
void clearPlayerSelectionText()
{
  for (int i = 0; i < 6; i++)
  {
    gotoxy(117, 33 + i);
    printf("                                                    ");
  }
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
  printf("                         ");
  gotoxy(118, 2 + (p.urutanBermain * 3));
  if (p.uang > 0)
  {
    printWithColor(color, "$%lld", p.uang);
  }
  else
  {
    printWithColor(color, "BANGKRUT");
  }
}

void majuNLangkah(Player *p, int N)
{
  for (int i = 0; i < abs(N); i++)
  {
    delay(200);
    int posisiSebelum = p->posisi;
    if (N > 0)
    {
      p->posisi = rotateIndex(p->posisi, 39, true);
    }
    else
    {
      p->posisi = rotateIndex(p->posisi, 39, false);
    }
    renderPosisiPlayer(p, posisiSebelum);
    cekMenyentuhStart(p);
  }
}
void lompatKe(Player *p, int index)
{
  int posisiSebelum = p->posisi;
  p->posisi = index;
  renderPosisiPlayer(p, posisiSebelum);
  cekMenyentuhStart(p);
}
void majuHingga(Player *p, int index)
{
  while (p->posisi != index)
  {
    majuNLangkah(p, 1);
  }
}
void majuHinggaJenisProperty(Player *p, char *jenis)
{
  bool sudahSampai = false;
  while (!sudahSampai)
  {
    majuNLangkah(p, 1);
    if (strcmp(listPetak[p->posisi].special, "") == 0)
    {
      if (strcmp(listPetak[p->posisi].kartuProperty->jenisProperty, jenis) == 0)
      {
        sudahSampai = true;
      }
    }
  }
}
void lompatKePenjara(Player *p)
{
  lompatKe(p, 10);
  p->sisaTurnPenjara = 3;
}
bool cekUangCukup(long long int uang, long long int harga)
{
  return uang >= harga;
}

void cekMenyentuhStart(Player *p)
{
  if (p->posisi == 0)
  {
    changePlayerMoney(p, 200);
  }
}
void renderPosisiPlayer(Player *p, int sebelum)
{
  gotoxy(listPetak[sebelum].lokasiPlayer.x + p->urutanBermain, listPetak[sebelum].lokasiPlayer.y);
  printf(" ");
  gotoxy(listPetak[p->posisi].lokasiPlayer.x + p->urutanBermain, listPetak[p->posisi].lokasiPlayer.y);
  printWithColor(240, "%s", p->simbol);
}

void propertyHandler(Player *players, int giliran, int posisi, PlayerProperty playerProperties[40])
{
  KartuProperty *kartuProperty = listPetak[posisi].kartuProperty;
  renderKartuProperty(kartuProperty);
  /*kalau belum dimiliki:
  - beli
  - sudahi giliran

  kalau sudah dimiliki player lain:
  - bayar sewa
  - bayar sewa dan ambil alih (99999)
  - kalo pemain lainnya udah punya ketiga kotanya, maka ambil alihnya harus tiga-tiganya

  kalau sudah dimiliki sendiri:
  - beli rumah/hotel
  - jual properti
  - sudahi giliran
  */

  // property belum dimiliki oleh siapapun
  if (playerProperties[posisi].pemilik == NULL)
  {
    char selectionText[][50] = {
        "Akhiri Giliran",
        "Beli Property"};
    int hargaBeliProperty = kartuProperty->hargaBeli;
    // kalau uang cukup untuk membeli properti
    sprintf(selectionText[1], "Beli Property ($%d)", kartuProperty->hargaBeli);
    bool uangCukupUntukBeli = cekUangCukup(players[giliran].uang, hargaBeliProperty);
    int selection = playerSelectionMenu(selectionText, uangCukupUntukBeli ? 1 : 0);
    // TODO pemain bisa membeli property
    switch (selection)
    {
    case 0:
      // TODO error disini
      break;
    case 1:
      beliProperty(&players[giliran], &playerProperties[posisi], posisi);
      break;
    }
  }

  // artinya property sudah dimiliki oleh orang lain
  else if (playerProperties[posisi].pemilik->urutanBermain != giliran)
  {
    char selectionText[][50] = {
        "Bayar sewa",
        "Bayar sewa dan ambil alih"};
    int hargaSewa = kartuProperty->hargaSewa[playerProperties[posisi].level];
    int hargaAmbilAlih = MULTIPLIER_AMBIL_ALIH * (kartuProperty->hargaBeli + playerProperties[posisi].level * kartuProperty->hargaUpgrade);
    sprintf(selectionText[0], "Bayar Sewa ($%d)", hargaSewa);
    sprintf(selectionText[1], "Bayar Sewa dan ambil alih ($%d)", hargaSewa + hargaAmbilAlih);
    bool uangCukupUntukAmbilAlih = cekUangCukup(players[giliran].uang, hargaSewa + hargaAmbilAlih);
    int selection = playerSelectionMenu(selectionText, uangCukupUntukAmbilAlih ? 1 : 0);

    // pasti bayar sewa, jadi bisa diletakkan di luar switch
    changePlayerMoney(&players[giliran], -hargaSewa);
    changePlayerMoney(playerProperties[posisi].pemilik, hargaSewa);
    switch (selection)
    {
    case 0:
      break;
    case 1:
      changePlayerMoney(&players[giliran], -hargaAmbilAlih);
      changePlayerMoney(playerProperties[posisi].pemilik, hargaAmbilAlih);
      playerProperties[posisi].pemilik->properties[posisi] = false;
      playerProperties[posisi].pemilik = &players[giliran];
      players[giliran].properties[posisi] = true;
      renderProperty(playerProperties[posisi], posisi);
      break;
    }
  }
  // menggunakan if karena setelah dibeli, pemilik yang baru dapat langsung upgrade jika sudah memiliki ketiga/kedua kota di kompleks yang sama
  if (playerProperties[posisi].pemilik != NULL && playerProperties[posisi].pemilik->urutanBermain == giliran)
  {
    bool pilihanSelesai = false;
    while (!pilihanSelesai)
    {
      char selectionText[][50] = {
          "Akhiri Giliran",
          "Jual property",
          "Upgrade rumah/hotel"};
      int hargaJualProperty = MULTIPLIER_JUAL_SEPIHAK * (kartuProperty->hargaBeli + playerProperties[posisi].level * kartuProperty->hargaUpgrade);
      sprintf(selectionText[1], "Jual property ($%d)", hargaJualProperty);
      int bisaUpgradeLagi = playerProperties[posisi].level < 5;
      // kalau perusahaan atau stasion tidak dapat di upgrade
      bool bisaUpgrade = listPetak[posisi].kartuProperty->jenisProperty == PROPERTY_KOTA;
      int selection = playerSelectionMenu(selectionText, bisaUpgrade ? 2 : 1);
      switch (selection)
      {
      case 0:
        pilihanSelesai = true;
        break;
      case 1:
        changePlayerMoney(playerProperties[posisi].pemilik, hargaJualProperty);
        playerProperties[posisi].pemilik->properties[posisi] = false;
        playerProperties[posisi].pemilik = NULL;
        playerProperties[posisi].level = 0;
        renderProperty(playerProperties[posisi], posisi);
        pilihanSelesai = true;
        break;
      case 2:
        // TODO handle upgrade rumah/hotel
        if (playerProperties[posisi].level < 4)
        {
          char selectionTextUpgrade[][50] = {
              "Kembali",
              "1 Rumah",
              "2 Rumah",
              "3 Rumah",
              "4 Rumah"};
          int selectionUpgrade = playerSelectionMenu(selectionTextUpgrade, 4 - playerProperties[posisi].level);
          if (selectionUpgrade == 0)
          {
            break;
          }
          else
          {
            changePlayerMoney(&players[giliran], -(kartuProperty->hargaUpgrade * selectionUpgrade));
            playerProperties[posisi].level += selectionUpgrade;
            renderProperty(playerProperties[posisi], posisi);
            pilihanSelesai = true;
          }
        }
        else
        {
          char selectionTextUpgrade[][50] = {"Kembali",
                                             "1 Hotel"};
          int selectionUpgrade = playerSelectionMenu(selectionTextUpgrade, 1);
          if (selectionUpgrade == 0)
          {
            break;
          }
          else
          {
            changePlayerMoney(&players[giliran], -(kartuProperty->hargaUpgrade * selectionUpgrade));
            playerProperties[posisi].level += selectionUpgrade;
            renderProperty(playerProperties[posisi], posisi);
            pilihanSelesai = true;
          }
        }
        // jika belum memiliki rumah sama sekali
      }
    }
  }
}

void beliProperty(Player *p, PlayerProperty *playerProperty, int posisi)
{
  KartuProperty *kartuProperty = listPetak[posisi].kartuProperty;
  int hargaBeliProperty = kartuProperty->hargaBeli;
  changePlayerMoney(p, -hargaBeliProperty);
  playerProperty->pemilik = p;
  playerProperty->level = 0;
  p->properties[posisi] = true;
  renderProperty(*playerProperty, posisi);
}

void jualProperty(Player *p, PlayerProperty *playerProperty, int posisi)
{
  KartuProperty *kartuProperty = listPetak[posisi].kartuProperty;
  int hargaJualProperty = MULTIPLIER_JUAL_SEPIHAK * (kartuProperty->hargaBeli + playerProperty->level * kartuProperty->hargaUpgrade);
  changePlayerMoney(p, hargaJualProperty);
  p->properties[posisi] = false;
  playerProperty->pemilik = NULL;
  playerProperty->level = 0;
  renderProperty(*playerProperty, posisi);
}

void ambilAlihProperty(Player *p, PlayerProperty *playerProperty, int posisi)
{
  KartuProperty *kartuProperty = listPetak[posisi].kartuProperty;
  int hargaAmbilAlih = MULTIPLIER_AMBIL_ALIH * (kartuProperty->hargaBeli + playerProperty->level * kartuProperty->hargaUpgrade);
  changePlayerMoney(p, -hargaAmbilAlih);
  changePlayerMoney(playerProperty->pemilik, hargaAmbilAlih);
  playerProperty->pemilik->properties[posisi] = false;
  p->properties[posisi] = true;
  playerProperty->pemilik = p;
  renderProperty(*playerProperty, posisi);
}

void renderProperty(PlayerProperty playerProperty, int posisi)
{
  KartuProperty *tempKartuProperty = listPetak[posisi].kartuProperty;
  gotoxy(tempKartuProperty->tandaKepemilikan.x, tempKartuProperty->tandaKepemilikan.y);
  if (playerProperty.pemilik != NULL)
  {
    printf("%s", playerProperty.pemilik->simbol);
  }
  else
  {
    printf(" ");
  }
  gotoxy(tempKartuProperty->lokasiRumah.x, tempKartuProperty->lokasiRumah.y);
  if (playerProperty.level == 0)
  {
    printWithColor(tempKartuProperty->warna, "  ");
  }
  else
  {
    if (playerProperty.level <= 4)
    {
      printWithColor(tempKartuProperty->warna, "%dR", playerProperty.level);
    }
    else
    {
      printWithColor(tempKartuProperty->warna, "1H");
    }
  }
}

bool specialHandler(Player *players, int giliran, char special[30], int playerCount, PlayerProperty playerProperties[40])
{
  if (strcmp(special, "DANA_UMUM") == 0)
  {
    return kartuDanaUmumHandler(players, giliran, playerCount, playerProperties);
  }
  else if (strcmp(special, "KESEMPATAN") == 0)
  {
    return kartuKesempatanHandler(players, giliran, playerCount, playerProperties);
  }
  else if (strcmp(special, "INCOME_TAX") == 0)
  {
    char selectionText[][50] = {"Bayar Income Tax $200"};
    int selection = playerSelectionMenu(selectionText, 0);
    if (selection == 0)
    {
      changePlayerMoney(&players[giliran], -200);
    }
    return true;
  }
  else if (strcmp(special, "LUXURY_TAX") == 0)
  {
    char selectionText[][50] = {"Bayar Luxury Tax $75"};
    int selection = playerSelectionMenu(selectionText, 0);
    if (selection == 0)
    {
      changePlayerMoney(&players[giliran], -75);
    }
    return true;
  }
  else if (strcmp(special, "PARKIR_GRATIS") == 0)
  {
    clearPlayerSelectionText();
    gotoxy(117, 33);
    printf("Gunakan ↑ dan ↓ pada keyboard untuk memilih lokasi tujuan.");
    bool isSelected = false;

    int posisiDipilih = 20;
    gotoxy(listPetak[posisiDipilih].lokasiPlayer.x, listPetak[posisiDipilih].lokasiPlayer.y);
    showCursor();
    while (!isSelected)
    {
      int keyboardResult = keyboardArrowEnterHandler();
      if (keyboardResult == ARROW_UP)
      {
        posisiDipilih = rotateIndex(posisiDipilih, 39, true);
      }
      else if (keyboardResult == ARROW_DOWN)
      {
        posisiDipilih = rotateIndex(posisiDipilih, 39, false);
      }
      else if (keyboardResult == KEY_ENTER)
      {
        isSelected = true;
        hideCursor();
        gotoxy(117, 33);
        printf("                                                               ");
      }
      gotoxy(listPetak[posisiDipilih].lokasiPlayer.x, listPetak[posisiDipilih].lokasiPlayer.y);
    }
    majuHingga(&players[giliran], posisiDipilih);
    return false;
  }
  else if (strcmp(special, "PERGI_KE_PENJARA") == 0)
  {
    lompatKePenjara(&players[giliran]);
    return true;
  }
  else
  {
    // untuk hanya mengunjungi penjara
    return true;
  }
}

bool penjaraHandler(Player *p)
{
  char selectionText[][50] = {"Kocok dadu (jika double keluar penjara)",
                              "Bayar $50 langsung keluar penjara",
                              "Gunakan kartu bebas penjara"};
  int selection = playerSelectionMenu(selectionText, p->punyaKartuBebasPenjara ? 2 : 1);
  switch (selection)
  {
  case 0:
    if (isDouble(kocokDadu()))
    {
      p->sisaTurnPenjara = 0;
      clearPlayerSelectionText();
      gotoxy(117, 33);
      printf("Anda berhasil keluar dari penjara.");
      gotoxy(117, 34);
      printf("Tekan ENTER untuk melanjutkan...");
      waitForEnter();
      gotoxy(117, 33);
      printf("                                   ");
      gotoxy(117, 34);
      printf("                                   ");
      return false;
    }
    else
    {
      clearPlayerSelectionText();
      gotoxy(117, 33);
      printf("Anda gagal keluar dari penjara.");
      gotoxy(117, 34);
      printf("Tekan ENTER untuk melanjutkan...");
      waitForEnter();
      gotoxy(117, 33);
      printf("                                   ");
      gotoxy(117, 34);
      printf("                                   ");
      return true;
    }
    break;
  case 1:
    changePlayerMoney(p, -50);
    p->sisaTurnPenjara = 0;
    return false;
    break;
  case 2:
    p->punyaKartuBebasPenjara = false;
    p->sisaTurnPenjara = 0;
    return false;
    break;
  }
}

// return true jika kartu kesempatan sudah selesai menghandle pemain, misalnya player hanya membayar uang
// return false jika kartu kesempatan hanya mengubah posisi player, namun pembayaran dan sebagaimancamnya tidak di handle
bool kartuKesempatanHandler(Player players[], int giliran, int playerCount, PlayerProperty playerProperties[40])
{
  char kesempatan[][200] = {"Pergi ke Guangzhou. Jika melewati\n\"GO\" ambil $200",                                  // false
                            "Kartu bebas penjara. Kartu ini\ndapat disimpan hingga dibutuhkan",                      // true
                            "Pergi ke Stasion Pasar Senen.\nJika melewati \"GO\" ambil $200",                        // false
                            "Pergi ke stasion terdekat.",                                                            // false
                            "Pergi ke Amsterdam. Jika melewati\n\"GO\" ambil $200",                                  // false
                            "Ada biaya perbaikan rumah, bayar\n$25 untuk setiap rumah dan\n$100 untuk setiap hotel", // true
                            "Pergi ke \"GO\". Ambil $200",                                                           // true
                            "Bank memberikan Anda gratifikasi $50",                                                  // true
                            "Bayar pajak untuk orang miskin $15",                                                    // true
                            "Pergi ke perusahaan terdekat.",                                                         // false
                            "Anda masuk penjara, tidak melewati\n\"GO\", tidak mendapatkan $200",                    // true
                            "Anda terpilih sebagai ketua dari\npapan ini. Bayar setiap pemain $50",                  // true
                            "Jalan-jalan ke Jakarta.\nPergi ke Jakarta",                                             // false
                            "Mundur 3 langkah",                                                                      // false
                            "Bangunan dan pinjaman anda sudah\njatuh tempo. Ambil $150 dari Bank",                   // true
                            "Saatnya jalan-jalan.\nLompat ke Parkir Bebas"};                                         // false
  // random dari 0 ke 15
  int random = rand() % 15;
  // render kartu
  renderKartuKesempatan(kesempatan[random]);

  char selectionText[][50] = {"Lanjut"};
  int selection = playerSelectionMenu(selectionText, 0);
  switch (random)
  {
  case 0:
    majuHingga(&players[giliran], 11);
    break;
  case 1:
    players[giliran].punyaKartuBebasPenjara = true;
    break;
  case 2:
    majuHingga(&players[giliran], 5);
    break;
  case 3:
    majuHinggaJenisProperty(&players[giliran], PROPERTY_STASION);
    break;
  case 4:
    majuHingga(&players[giliran], 24);
    break;
  case 5:
    for (int i = 0; i < 40; i++)
    {
      if (players[giliran].properties[i] == true)
      {
        if (playerProperties[i].level > 0 && playerProperties[i].level < 5)
        {
          changePlayerMoney(&players[giliran], -25 * playerProperties[i].level);
        }
        else
        {
          changePlayerMoney(&players[giliran], -100);
        }
      }
    }
    break;
  case 6:
    majuHingga(&players[giliran], 0);
    break;
  case 7:
    changePlayerMoney(&players[giliran], 50);
    break;
  case 8:
    changePlayerMoney(&players[giliran], -15);
    break;
  case 9:
    majuHinggaJenisProperty(&players[giliran], PROPERTY_PERUSAHAAN);
    break;
  case 10:
    lompatKePenjara(&players[giliran]);
    break;
  case 11:
  {
    int playerTersisa = 0;
    for (int i = 0; i < playerCount; i++)
    {
      if (i != giliran && !players[i].isBangkrut)
      {
        playerTersisa++;
        changePlayerMoney(&players[i], 50);
      }
    }
    changePlayerMoney(&players[giliran], -50 * playerTersisa);
    break;
  }
  case 12:
    majuHingga(&players[giliran], 39);
    break;
  case 13:
    majuNLangkah(&players[giliran], -3);
    break;
  case 14:
    changePlayerMoney(&players[giliran], 150);
    break;
  case 15:
    lompatKe(&players[giliran], 20);
    break;
  }
  if (random == 1 || random == 5 || random == 6 || random == 7 || random == 8 || random == 10 || random == 11 || random == 14)
  {
    return true;
  }
  else
  {
    return false;
  }
}
// semua kartu dana umum akan selesai menghandle pemain
bool kartuDanaUmumHandler(Player players[], int giliran, int playerCount, PlayerProperty playerProperties[40])
{

  char danaUmum[][200] = {"Bayar pajak sekolah $150",
                          "Grand Opera Opening.\nSetiap pemain membayar Anda $50",
                          "Anda mewarisi $100",
                          "Anda baru saja melahirkan.\nBayar biaya rumah sakit $100",
                          "Pengembalian dana pajak pendapatan.\nAmbil $20",
                          "Anda masuk penjara, tidak melewati\n\"GO\", tidak mendapatkan $200",
                          "Kartu bebas penjara. Kartu ini\ndapat disimpan hingga dibutuhkan",
                          "Bayar biaya perbaikan jalan.\nBayar $40 per rumah dan\n$115 per hotel",
                          "Bank error, Anda mendapatkan $200",
                          "Pergi ke \"GO\". Ambil $200",
                          "Anda mendapat hadiah natal $100",
                          "Anda kecelakaan.\nBayar perawatan dokter $50",
                          "Dari penjualan saham,\nAnda mendapatkan $45",
                          "Dapatkan $25 karena Anda telah\nmenjadi pelayan",
                          "Jatuh tempo dana asuransi jiwa.\nAnda mendapat $100",
                          "Juara kedua kontes kecantikan.\nAnda mendapat $10"};
  // random dari 0 ke 15
  int random = rand() % 15;
  // render kartu
  renderKartuDanaUmum(danaUmum[random]);

  char selectionText[][50] = {"Lanjut"};
  int selection = playerSelectionMenu(selectionText, 0);

  switch (random)
  {
  case 0:
    changePlayerMoney(&players[giliran], -150);
    break;
  case 1:
  {
    int playerTersisa = 0;
    for (int i = 0; i < playerCount; i++)
    {
      if (i != giliran && !players[i].isBangkrut)
      {
        playerTersisa++;
        changePlayerMoney(&players[i], 50);
      }
    }
    changePlayerMoney(&players[giliran], -50 * playerTersisa);
    break;
  }
  case 2:
    changePlayerMoney(&players[giliran], -100);
    break;
  case 3:
    changePlayerMoney(&players[giliran], -100);
    break;
  case 4:
    changePlayerMoney(&players[giliran], 20);
    break;
  case 5:
    lompatKePenjara(&players[giliran]);
    break;
  case 6:
    players[giliran].punyaKartuBebasPenjara = true;
    break;
  case 7:
    for (int i = 0; i < 40; i++)
    {
      if (players[giliran].properties[i] == true)
      {
        if (playerProperties[i].level > 0 && playerProperties[i].level < 5)
        {
          changePlayerMoney(&players[giliran], -40 * playerProperties[i].level);
        }
        else
        {
          changePlayerMoney(&players[giliran], -115);
        }
      }
    }
    break;
  case 8:
    changePlayerMoney(&players[giliran], 200);
    break;
  case 9:
    changePlayerMoney(&players[giliran], 200);
    break;
  case 10:
    changePlayerMoney(&players[giliran], 100);
    break;
  case 11:
    changePlayerMoney(&players[giliran], -50);
    break;
  case 12:
    changePlayerMoney(&players[giliran], 45);
    break;
  case 13:
    changePlayerMoney(&players[giliran], 25);
    break;
  case 14:
    changePlayerMoney(&players[giliran], 100);
    break;
  case 15:
    changePlayerMoney(&players[giliran], 10);
    break;
  }
  return true;
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
                                     "│ Secara sepihak :            │",
                                     "│ Ambil alih     :            │",
                                     "└─────────────────────────────┘"};
    for (int i = 0; i < 17; i++)
    {
      gotoxy(116, 14 + i);
      printf("%s", kartuPropertyText[i]);
    }
    printTengah(kartuProperty->namaKota, 117, 15, 29, kartuProperty->warna);
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
    // print multiplier harga jual ambil alih dan sepihak
    gotoxy(135, 28);
    printf("%.0f%%", MULTIPLIER_JUAL_SEPIHAK * 100.0);
    gotoxy(135, 29);
    printf("%.0f%%", MULTIPLIER_AMBIL_ALIH * 100.0);
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
                                     "│ Secara sepihak :            │",
                                     "│ Ambil alih     :            │",
                                     "│                             │",
                                     "│                             │",
                                     "└─────────────────────────────┘"};
    for (int i = 0; i < 17; i++)
    {
      gotoxy(116, 14 + i);
      printf("%s", kartuPropertyText[i]);
    }
    printTengah(kartuProperty->namaKota, 117, 15, 29, kartuProperty->warna);
    for (int i = 0; i < 4; i++)
    {
      gotoxy(131, 19 + i);
      printf("%d", kartuProperty->hargaSewa[i]);
    }
    // print multiplier harga jual ambil alih dan sepihak
    gotoxy(135, 26);
    printf("%.0f%%", MULTIPLIER_JUAL_SEPIHAK * 100.0);
    gotoxy(135, 27);
    printf("%.0f%%", MULTIPLIER_AMBIL_ALIH * 100.0);
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
                                     "│ Secara sepihak :            │",
                                     "│ Ambil alih     :            │",
                                     "└─────────────────────────────┘"};
    for (int i = 0; i < 17; i++)
    {
      gotoxy(116, 14 + i);
      printf("%s", kartuPropertyText[i]);
    }
    printTengah(kartuProperty->namaKota, 117, 15, 29, kartuProperty->warna);
    // print multiplier harga jual ambil alih dan sepihak
    gotoxy(135, 28);
    printf("%.0f%%", MULTIPLIER_JUAL_SEPIHAK * 100.0);
    gotoxy(135, 29);
    printf("%.0f%%", MULTIPLIER_AMBIL_ALIH * 100.0);
  }
}
int playerSelectionMenu(char selections[][50], int maxIndex)
{
  clearPlayerSelectionText();
  char notSelectedStrings[10][50];
  for (int i = 0; i <= maxIndex; i++)
  {
    sprintf(notSelectedStrings[i], "  %s", selections[i]);
  }
  char selectedStrings[10][50];
  for (int i = 0; i <= maxIndex; i++)
  {
    sprintf(selectedStrings[i], "▶ %s", selections[i]);
  }
  return selectionMenu(notSelectedStrings, selectedStrings, maxIndex, 117, 33);
}
void clearKartu()
{
  clearArea(116, 14, 156, 30);
}

void shufflePlayer(Player *players, int playerCount)
{
  // metode fisher yates shuffle
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