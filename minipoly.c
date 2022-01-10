/*
Author: Jovan Shelomo & Rahma Alia Latifa
Latest ver: 1.2.0 / 10 jan 2022
compiler: gcc 17
*/

#define _CRT_RAND_S

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
#define KEY_SPACE 32

// mendefinisikan properti
#define PROPERTY_CITY "PROPERTY_CITY"
#define PROPERTY_STATION "PROPERTY_STATION"
#define PROPERTY_COMPANY "PROPERTY_COMPANY"

#define MULTIPLIER_RENT_COMPLETED 2      // jika pemilik sudah memiliki kota lengkap, maka multiplier sewanya menjadi 2x nya
#define MULTIPLIER_TAKE_OVER 1.5         // jika player belum memiliki kota/stasion/perusahaan lengkap, maka multiplier ini digunakan
#define MULTIPLIER_TAKE_OVER_COMPLETED 2 // jika player sudah memiliki kota/stasion/perusahaan lengkap, maka multiplier ini yang digunakan
#define MULTIPLIER_SELL_UNILATERALLY 0.5 // ketika menjual sepihak, maka multiplier ini yang digunakan

#define BOT_DELAY 800 // dalam ms

#define FILE_NAME_HOW_TO_PLAY "howtoplay.txt"
#define FILE_NAME_LEADERBOARD "leaderboard"

typedef struct Dice
{
  int dice1;
  int dice2;
} Dice;

typedef struct ScreenCoordinate
{
  int x;
  int y;
} ScreenCoordinate;

typedef struct PropertyCard
{
  char *propertyType;                          // jenis properti seperti PROPERTI_KOTA, PROPERTY_STASION, PROPERTY_COMPANY
  int clusterId;                               // id kompleks yang sejenis
  char cityCode[3];                            // kode nama kota
  char name[30];                               // nama dari kota
  int buyPrice;                                // harga beli property yang ada di papan
  int rentPrice[6];                            // harga sewa untuk setiap tingkatan (sewa, 1 rumah, 2 rumah, 3 rumah, 4 rumah, hotel)
  int upgradePrice;                            // harga rumah & hotel
  int color;                                   // warna pada kompleks
  bool onlyTwo;                                // true jika property hanya terdapat 2 buah property pada 1 kompleks
  struct ScreenCoordinate ownershipCoordinate; // tempat kepemilikan (untuk render)
  struct ScreenCoordinate colorCoordinate;     // tempat warna (untuk render)
  struct ScreenCoordinate houseCoordinate;     // lokasi rumah di layar (untuk render)
} PropertyCard;

typedef struct Plot
{
  int id;                                   // id step (0-39)
  struct ScreenCoordinate playerCoordinate; // lokasi player
  char special[30];                         // jika petak special, maka tidak ada kartu property
  char specialName[30];                     // jika petak special, maka ada namanya, jika tidak maka nama diambil dari kartu property
  struct PropertyCard *propertyCard;        // jika ada kartu property (bukan termasuk petak special)
} Plot;

typedef struct Player
{
  char symbol[3];            //"Ω", "Θ", "Δ", "Π", "Σ", "Ξ", "Φ", "Ψ"
  char name[26];             // nama player
  int turnOrder;             // urutan bermain
  bool isBot;                // true jika player bot
  int botDifficulty;         // level bot (0-2) easy, medium, hard
  bool isBankrupt;           // true jika player sudah bangkrut
  int money;                 // jumlah uang player
  int position;              // posisi di papan (0-39)
  bool haveGetOutOfJailCard; // true jika punya kartu bebas penjara
  int jailTurnsLeft;         // jumlah turn yang tersisa ketika di penjara
  bool properties[40];       // setiap index merujuk kepada nilai kebenaran kepemilikan property pada petak index oleh player. Contoh, apabila player memiliki property di jakarta, maka properties[39] = true
} Player;

typedef struct Duration
{
  int hours;
  int minutes;
  int seconds;
} Duration;

typedef struct Date
{
  int year;
  int month;
  int day;
  int hour;
  int minute;
  int second;
} Date;

typedef struct PlayerLeaderboard
{
  struct Duration gameDuration;
  Date date;
  char name[26];
  int money;
} PlayerLeaderboard;

typedef struct PlayerProperty
{
  struct Player *owner; // pointer ke pemilik property
  int level;            // 0 tanpa rumah, 1 rumah, 2 rumah, 3 rumah, 4 rumah, 5 hotel
} PlayerProperty;

// list modul tidak terkait dengan game
void gotoxy(int x, int y);                                         // pergi ke koordinat x,y
void delay(int ms);                                                // melakukan delay
int randomInt(int min, int max);                                   // untuk mengenerate angka random dari min sampai max inklusif
int randFrequency(int freqArr[], int maxIndex);                    // random menggunakan frekuensi dari sebuah array
void setColor(int color);                                          // menyetel warna untuk print
void hideCursor();                                                 // menyembunyikan cursor
void showCursor();                                                 // menampilkan cursor
void clearArea(int xStart, int yStart, int xEnd, int yEnd);        // berfungsi untuk menghapus area di layar
int rotateIndex(int currentIndex, int maxIndex, bool isPlus);      // berfungsi untuk merotasi index jika sudah terakhir atau ke paling atas
void printCenter(char *text, int x, int y, int lebar, int color);  // untuk memprint teks di tengah
void printMultiLineAlignLeft(char *text, int x, int y, int color); // untuk print multi line teks rata kiri
int keyboardEventHandler();                                        // menghandle keyboard arrow dan enter
void waitForEnter();                                               // menunggu keyboard enter
void flushInput();                                                 // membersihkan input keyboard

// list modul terkait dengan game
void attentionBeforePlay();
void playGame();
void startGame();
void howToPlay();
void quitGame();

void renderWinner(Player p);

// leaderboard
Date getcurrentDate();
int getLeaderboardDataCount();
void addToLeaderboard(Player p, Duration gameDuration);
void dateToString(Date date, char *str);
void renderLeaderboard();

// dalam permainan
void renderHowToPlay();
void renderLogo();
int selectionMenu(char notSelectedStrings[][50], char selectedStrings[][50], int maxIndex, int x, int y);
int renderSelectionMenu(char notSelectedStrings[][50], char selectedStrings[][50], int selectedIndex, int maxIndex, int x, int y, int selected);
void renderBoard();
int playerSelectionHuman(char selections[][50], int maxIndex, int yOffset); // untuk player memilih ketika sedang berada dalam in-game
int playerSelectionManager(Player p, char selections[][50], int maxIndex, int yOffset, char botContext[], PlayerProperty playerProperties[40]);
void printTurnText(Player p);
void clearGiliranText();
void clearPlayerSelectionText();

// dadu
Dice rollDice();
Dice randomDice();
void renderDice(Dice d, bool last);
int getTotalDice(Dice d);
bool isDouble(Dice d);

// awal
void initGame(Player *players, int *playerCount);
void startGame(Player *players, int *playerCount, PlayerProperty playerProperties[40]);

void initPlayerStats(Player players[], int playerCount);

// player
void advanceNSteps(Player *p, int N);                                                     // berfungsi untuk memajukan player dari posisi sekarang ke posisi sekarang + N
void jumpTo(Player *p, int index);                                                        // berfungsi untuk melompatkan player tanpa berjalan memutar
void advanceTo(Player *p, int index);                                                     // berfungsi untuk memajukan player hingga posisi N
void advanceToPropertyType(Player *p, char *type);                                        // berfungsi untuk memajukan player hingga posisi yang memiliki tipe property tertentu
void checkIsInStart(Player *p);                                                           // berfungsi untuk mengecek apakah player sudah menyentuh start
void renderPlayerPosition(Player p, int previous);                                        // menghapus posisi player sebelumnya dan menampilkan yang baru
void jumpToJail(Player *p);                                                               // berfungsi untuk melompatkan player ke penjara
bool checkSufficientMoney(int money, int amount);                                         // berfungsi untuk mengecek apakah uang cukup untuk harga yang ditentukan
void turnManager(Player *players, int *playerCount, PlayerProperty playerProperties[40]); // berfungsi untuk mengatur turn
void setPlayerAsBankrupt(Player *p);                                                      // berfungsi untuk mengubah status player menjadi bangkrut

int changePlayerMoney(Player *player, int amount); // positif kalo nambah, negatif kalo kurang
void renderPlayerMoney(Player p, int color);       // ngerender jumlah uang playernya

// jual beli property
void buyProperty(Player *p, PlayerProperty *playerProperty, int position);
void sellProperty(Player *p, PlayerProperty *playerProperty, int position);
void takeOverProperty(Player *p, PlayerProperty *playerProperty, int position);
// menambah menghapus property
void addPropertyOwnership(Player *p, PlayerProperty *playerProperty, int position);
void removePropertyOwnership(PlayerProperty *playerProperty, int position);
void transferPropertyOwnership(Player *newOwner, PlayerProperty *playerProperty, int position);

void renderProperty(PlayerProperty playerProperty, int position);

// untuk mengecek
bool isClusterCompleted(Player p, int position);
int totalPropertyOwnedInCluster(Player p, int position);
int totalStationOwned(Player p);
int totalCompanyOwned(Player p);

// kartu kesempatan dan dana umum
bool chanceCardHandler(Player players[], int turn, int playerCount, PlayerProperty playerProperties[40]);
bool communityChestCardHandler(Player players[], int turn, int playerCount, PlayerProperty playerProperties[40]);

// render kartu kesempatan dan dana umum
void renderChanceCard(char *text);
void renderCommunityChestCard(char *text);

// render kartu property
void renderPropertyCard(PropertyCard *propertyCard);

// clear untuk kartu property dan kartu dana umum/kesempatan
void clearRenderedCard();

// handler di petak
void propertyHandler(Player *players, int turn, int position, PlayerProperty playerProperties[40]);
bool specialHandler(Player *players, int turn, char special[30], int playerCount, PlayerProperty playerProperties[40]);
bool jailHandler(Player *p);

int getPropertySellPrice(PlayerProperty playerProperty, PropertyCard propertyCard);

// ketika uang negatif, maka player harus menjual asetnya atau bangkrut
bool insufficientMoneyHandler(Player *p, PlayerProperty playerProperties[40]);

// shuffle
void shuffleBotName(Player *players, int playerCount, int startBotIndex);
void shufflePlayer(Player *players, int playerCount);
void shuffleSymbol(Player *players, int playerCount);

// untuk pemain bot
int playerSelectionBot(Player p, int maxIndex, int yOffset, char botContext[], PlayerProperty playerProperties[40]);
int botLevelEasy(Player p, int maxIndex, int yOffset, char botContext[], PlayerProperty playerProperties[40]);
int botLevelMedium(Player p, int maxIndex, int yOffset, char botContext[], PlayerProperty playerProperties[40]);
int botLevelHard(Player p, int maxIndex, int yOffset, char botContext[], PlayerProperty playerProperties[40]);
int FreeParkingBotHandler(Player *p, PlayerProperty playerProperties[40]);
void insufficientMoneyBotHandler(Player *p, PlayerProperty playerProperties[40], int totalOwnedProperties, int listOwnedProperty[28], bool selectedPositions[], int *totalMoneyFromSelectedProperties);

PropertyCard propertyCards[28] = {
    {PROPERTY_CITY, 0, "MLB", "Melbourne", 60, {2, 10, 30, 90, 160, 250}, 50, 31, true, {17, 41}, {13, 40}, {13, 41}},
    {PROPERTY_CITY, 0, "SYD", "Sydney", 60, {4, 20, 60, 180, 320, 450}, 50, 31, true, {17, 33}, {13, 32}, {13, 33}},
    {PROPERTY_STATION, 8, "PSN", "Pasar Senen", 200, {25, 50, 100, 200}, 0, 249, false, {17, 25}, {}, {}},
    {PROPERTY_CITY, 1, "OSA", "Osaka", 100, {6, 30, 90, 270, 400, 550}, 50, 47, false, {17, 21}, {13, 20}, {13, 21}},
    {PROPERTY_CITY, 1, "KYO", "Kyoto", 100, {6, 30, 90, 270, 400, 550}, 50, 47, false, {17, 13}, {13, 12}, {13, 13}},
    {PROPERTY_CITY, 1, "TYO", "Tokyo", 120, {8, 40, 100, 300, 450, 600}, 50, 47, false, {17, 9}, {13, 8}, {13, 9}},
    {PROPERTY_CITY, 2, "GZH", "Guangzhou", 140, {10, 50, 150, 450, 625, 750}, 100, 224, false, {19, 8}, {16, 6}, {19, 6}},
    {PROPERTY_COMPANY, 9, "PLN", "Perusahaan Listrik Negara", 150, {}, 0, 249, false, {28, 8}, {}, {}},
    {PROPERTY_CITY, 2, "BJG", "Beijing", 140, {10, 50, 150, 450, 625, 750}, 100, 224, false, {37, 8}, {34, 6}, {37, 6}},
    {PROPERTY_CITY, 2, "SHG", "Shanghai", 160, {12, 60, 180, 500, 700, 900}, 100, 224, false, {46, 8}, {43, 6}, {46, 6}},
    {PROPERTY_STATION, 8, "GBG", "Gubeng", 200, {25, 50, 100, 200}, 0, 249, false, {55, 8}, {}, {}},
    {PROPERTY_CITY, 3, "MSL", "Marseille", 180, {14, 70, 200, 550, 750, 950}, 100, 79, false, {64, 8}, {61, 6}, {64, 6}},
    {PROPERTY_CITY, 3, "BDX", "Bordeaux", 180, {14, 70, 200, 550, 750, 950}, 100, 79, false, {82, 8}, {79, 6}, {82, 6}},
    {PROPERTY_CITY, 3, "PAR", "Paris", 200, {16, 80, 220, 600, 800, 1000}, 100, 79, false, {91, 8}, {88, 6}, {91, 6}},
    {PROPERTY_CITY, 4, "RTD", "Rotterdam", 220, {18, 90, 250, 700, 875, 1050}, 150, 95, false, {94, 9}, {97, 8}, {97, 9}},
    {PROPERTY_CITY, 4, "DHG", "Den Haag", 220, {18, 90, 250, 700, 875, 1050}, 150, 95, false, {94, 17}, {97, 16}, {97, 17}},
    {PROPERTY_CITY, 4, "AMS", "Amsterdam", 240, {20, 100, 300, 750, 925, 1100}, 150, 95, false, {94, 21}, {97, 20}, {97, 21}},
    {PROPERTY_STATION, 8, "STH", "ST. Hall", 200, {25, 50, 100, 200}, 0, 249, false, {94, 25}, {}, {}},
    {PROPERTY_CITY, 5, "TPN", "Tampines", 260, {22, 110, 330, 800, 975, 1150}, 150, 143, false, {94, 29}, {97, 28}, {97, 29}},
    {PROPERTY_CITY, 5, "JRG", "Jurong", 260, {22, 110, 330, 800, 975, 1150}, 150, 143, false, {94, 33}, {97, 32}, {97, 33}},
    {PROPERTY_COMPANY, 9, "WTR", "Perusahaan Daerah Air Minum", 150, {}, 0, 249, false, {94, 37}, {}, {}},
    {PROPERTY_CITY, 5, "SGP", "Singapore", 280, {24, 120, 360, 850, 1025, 1200}, 150, 143, false, {94, 41}, {97, 40}, {97, 41}},
    {PROPERTY_CITY, 6, "CCG", "Chicago", 300, {26, 130, 390, 900, 1100, 1275}, 200, 63, false, {91, 42}, {88, 44}, {91, 44}},
    {PROPERTY_CITY, 6, "LAG", "Los Angeles", 300, {26, 130, 390, 900, 1100, 1275}, 200, 63, false, {82, 42}, {79, 44}, {82, 44}},
    {PROPERTY_CITY, 6, "NYC", "New York", 320, {28, 150, 450, 1000, 1200, 1400}, 200, 63, false, {65, 42}, {61, 44}, {64, 44}},
    {PROPERTY_STATION, 8, "GBR", "Gambir", 200, {25, 50, 100, 200}, 0, 249, false, {55, 42}, {}},
    {PROPERTY_CITY, 7, "BDG", "Bandung", 350, {35, 175, 500, 1100, 1300, 1500}, 200, 111, true, {37, 42}, {34, 44}, {37, 44}},
    {PROPERTY_CITY, 7, "JKT", "Jakarta", 400, {40, 185, 550, 1200, 1500, 1700}, 200, 111, true, {19, 42}, {16, 44}, {19, 44}}};

Plot plotList[40] = {
    {0, {6, 44}, "GO", "GO", NULL},
    {1, {5, 41}, "", "", &propertyCards[0]},
    {2, {5, 37}, "DANA_UMUM", "Dana Umum", NULL},
    {3, {5, 33}, "", "", &propertyCards[1]},
    {4, {5, 29}, "INCOME_TAX", "Income Tax", NULL},
    {5, {6, 25}, "", "", &propertyCards[2]},
    {6, {5, 21}, "", "", &propertyCards[3]},
    {7, {6, 17}, "KESEMPATAN", "Kesempatan", NULL},
    {8, {5, 13}, "", "", &propertyCards[4]},
    {9, {5, 9}, "", "", &propertyCards[5]},
    {10, {6, 4}, "MENGUNJUNGI_PENJARA", "Hanya Mengunjungi Penjara", NULL},
    {11, {18, 3}, "", "", &propertyCards[6]},
    {12, {27, 3}, "", "", &propertyCards[7]},
    {13, {36, 3}, "", "", &propertyCards[8]},
    {14, {45, 3}, "", "", &propertyCards[9]},
    {15, {54, 3}, "", "", &propertyCards[10]},
    {16, {63, 3}, "", "", &propertyCards[11]},
    {17, {72, 4}, "DANA_UMUM", "Dana Umum", NULL},
    {18, {81, 3}, "", "", &propertyCards[12]},
    {19, {90, 3}, "", "", &propertyCards[13]},
    {20, {102, 3}, "PARKIR_BEBAS", "Parkir Bebas", NULL},
    {21, {103, 9}, "", "", &propertyCards[14]},
    {22, {102, 13}, "KESEMPATAN", "Kesempatan", NULL},
    {23, {103, 17}, "", "", &propertyCards[15]},
    {24, {103, 21}, "", "", &propertyCards[16]},
    {25, {102, 25}, "", "", &propertyCards[17]},
    {26, {103, 29}, "", "", &propertyCards[18]},
    {27, {103, 33}, "", "", &propertyCards[19]},
    {28, {102, 37}, "", "", &propertyCards[20]},
    {29, {103, 41}, "", "", &propertyCards[21]},
    {30, {102, 47}, "MASUK_PENJARA", "Masuk Penjara", NULL},
    {31, {90, 47}, "", "", &propertyCards[22]},
    {32, {81, 47}, "", "", &propertyCards[23]},
    {33, {72, 47}, "DANA_UMUM", "Dana Umum", NULL},
    {34, {63, 47}, "", "", &propertyCards[24]},
    {35, {54, 47}, "", "", &propertyCards[25]},
    {36, {45, 47}, "KESEMPATAN", "Kesempatan", NULL},
    {37, {36, 47}, "", "", &propertyCards[26]},
    {38, {27, 47}, "LUXURY_TAX", "Luxury Tax", NULL},
    {39, {18, 47}, "", "", &propertyCards[27]}};

int clustersGroup[10][4] = {
    {1, 3},
    {6, 8, 9},
    {11, 13, 14},
    {16, 18, 19},
    {21, 23, 24},
    {26, 27, 29},
    {31, 32, 34},
    {37, 39},
    {5, 15, 25, 35}, // stasion
    {12, 28}};       // perusahaan

HANDLE hConsole;

int main()
{
  // inisialisasi
  SetConsoleTitle("MiniPoly");                // set judul
  hConsole = GetStdHandle(STD_OUTPUT_HANDLE); // handle console (untuk mengubah warna teks)
  SetConsoleOutputCP(CP_UTF8);                // agar dapat print utf-8
  // ShowWindow(GetConsoleWindow(), SW_SHOWMAXIMIZED); // untuk membuka console secara full screen
  attentionBeforePlay();

  hideCursor();
  int selection = 0;
  while (selection != 3)
  {
    renderLogo();
    char startNotSelectedMenuString[][50] = {"     START  ",
                                             "  HOW TO PLAY  ",
                                             "  LEADERBOARD  ",
                                             "     QUIT  "};
    char startSelectedMenuString[][50] = {"   ⯈ START ⯇",
                                          "⯈ HOW TO PLAY ⯇",
                                          "⯈ LEADERBOARD ⯇",
                                          "   ⯈ QUIT ⯇"};
    selection = selectionMenu(startNotSelectedMenuString, startSelectedMenuString, 3, 32, 7);
    if (selection == 0) // start
    {
      playGame();
    }
    else if (selection == 1) // how to play
    {
      howToPlay();
    }
    else if (selection == 2)
    {
      renderLeaderboard();
    }
    else if (selection == 3) // quit
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
int randomInt(int min, int max)
{
  unsigned int result;
  errno_t err;
  err = rand_s(&result);
  while (err != 0)
  {
    err = rand_s(&result);
  }
  return (int)((double)result / ((double)UINT_MAX + 1) * (double)(max - min + 1)) + min;
}
int randFrequency(int freqArr[], int maxIndex)
{
  int total = 0;
  for (int i = 0; i <= maxIndex; i++)
  {
    total += freqArr[i];
  }
  int r = randomInt(1, total);
  for (int i = 0; i <= maxIndex; i++)
  {
    if (r <= freqArr[i])
    {
      return i;
    }
    else
    {
      r -= freqArr[i];
    }
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
void printWithColor(int color, char text[], ...)
{
  va_list args;
  va_start(args, text);
  setColor(color);
  vprintf(text, args);
  setColor(7);
  va_end(args);
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
void printCenter(char *text, int x, int y, int width, int color)
{
  gotoxy(x, y);
  int textLength = strlen(text);
  int leftSpace = (width - textLength) / 2;
  int rightSpace = width - textLength - leftSpace;
  for (int i = 0; i < leftSpace; i++)
  {
    printWithColor(color, " ");
  }
  printWithColor(color, "%s", text);
  for (int i = 0; i < rightSpace; i++)
  {
    printWithColor(color, " ");
  }
}
void printMultiLineAlignLeft(char *text, int x, int y, int color)
{
  char *token = strtok(text, "\n");
  int currentLine = 0;
  while (token != NULL)
  {
    gotoxy(x, y + currentLine);
    printWithColor(color, token);
    token = strtok(NULL, "\n");
    currentLine++;
  }
}

void waitForEnter()
{
  while (_getch() != KEY_ENTER)
    ;
}

void flushInput()
{
  while (_kbhit())
  {
    _getch();
  }
}

Date getcurrentDate()
{
  time_t t = time(NULL);
  struct tm *tm = localtime(&t);
  Date date;
  date.year = tm->tm_year + 1900;
  date.month = tm->tm_mon + 1;
  date.day = tm->tm_mday;
  date.hour = tm->tm_hour;
  date.minute = tm->tm_min;
  date.second = tm->tm_sec;
  return date;
}

void renderDate(Date date)
{
  char month[][10] = {"Januari", "Februari", "Maret", "April", "Mei", "Juni", "Juli", "Agustus", "September", "Oktober", "November", "Desember"};
  printf("%d %s %d %02d:%02d:%02d", date.day, month[date.month - 1], date.year, date.hour, date.minute, date.second);
}

void attentionBeforePlay()
{
  printf("════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════\n");
  printWithColor(12, "! PERHATIAN SEBELUM MEMULAI !\n\n");
  printf("Pastikan window Anda FULL SCREEN dengan garis atas serta garis bawah terlihat dengan jelas dan tidak ter-wrap (terlihat 2 atau lebih baris).\n"
         "jika garis ter-wrap atau garis bawah tidak terlihat tetapi sudah full screen, kecilkan ukuran font dengan cara\n");
  printWithColor(11, "CTRL + scroll mouse bawah\n\n");
  printf("Tekan ENTER jika Anda sudah yakin"
         "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
  printf("════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════");
  waitForEnter();
  system("cls");
}

void playGame()
{
  Player players[4];
  PlayerProperty playerProperties[40]; // untuk menyimpan properti yang dimiliki player
  for (int i = 0; i < 40; i++)
  {
    playerProperties[i].owner = NULL;
    playerProperties[i].level = 0;
  }
  int playerCount = 0;
  initGame(players, &playerCount);

  // main game dan menghitung lama permainan
  struct timespec start, end;
  clock_gettime(CLOCK_MONOTONIC, &start);

  startGame(players, &playerCount, playerProperties);

  clock_gettime(CLOCK_MONOTONIC, &end);
  Duration gameDuration;
  gameDuration.hours = (end.tv_sec - start.tv_sec) / 3600;
  gameDuration.minutes = ((end.tv_sec - start.tv_sec) % 3600) / 60;
  gameDuration.seconds = ((end.tv_sec - start.tv_sec) % 3600) % 60;

  int winner;
  // cek siapa pemenangnya
  for (int i = 0; i < playerCount; i++)
  {
    if (!players[i].isBankrupt)
    {
      winner = i;
    }
  }
  // menambahkan pemain ke leaderboard jika bukan bot
  if (!players[winner].isBot)
  {
    addToLeaderboard(players[winner], gameDuration);
  }
  renderWinner(players[winner]);
}

void howToPlay()
{
  renderHowToPlay();
}

int getLeaderboardDataCount()
{
  FILE *leaderboardFile = fopen(FILE_NAME_LEADERBOARD, "rb");
  if (leaderboardFile == NULL)
  {
    return 0;
  }
  int dataCount = 0;
  fseek(leaderboardFile, 0, SEEK_END);
  dataCount = ftell(leaderboardFile) / sizeof(PlayerLeaderboard);
  fclose(leaderboardFile);
  return dataCount;
}

void addToLeaderboard(Player p, Duration gameDuration)
{
  int structSize = sizeof(PlayerLeaderboard);
  // open file
  FILE *leaderboardFile = fopen(FILE_NAME_LEADERBOARD, "rb+");
  if (leaderboardFile == NULL)
  {
    fclose(leaderboardFile);
    // create file
    leaderboardFile = fopen(FILE_NAME_LEADERBOARD, "ab+");
  }

  // menghitung jumlah data yang ada pada file
  int dataCount = getLeaderboardDataCount();

  // membuat struct playerLeaderboard
  PlayerLeaderboard pl;
  strcpy(pl.name, p.name);
  pl.money = p.money;
  pl.gameDuration = gameDuration;
  pl.date = getcurrentDate();

  // mengecek nama yang sama, kalau ada maka timpa
  PlayerLeaderboard temp;
  PlayerLeaderboard temp2;
  rewind(leaderboardFile);
  bool isSameName = false;
  while (fread(&temp, structSize, 1, leaderboardFile))
  {
    if (strcmp(temp.name, pl.name) == 0)
    {
      isSameName = true;
      break;
    }
  }
  if (isSameName)
  {
    if (pl.money > temp.money)
    { // langsung timpa
      fseek(leaderboardFile, -structSize, SEEK_CUR);
      fwrite(&pl, structSize, 1, leaderboardFile);
    }
  }
  else
  {
    // append data ke akhir file
    fseek(leaderboardFile, 0, SEEK_END);
    fwrite(&pl, structSize, 1, leaderboardFile);
    dataCount++;
  }

  // sort data menggunakan bubble sort
  if (dataCount > 1)
  {
    rewind(leaderboardFile);
    for (int i = 0; i < dataCount; i++)
    {
      for (int j = 0; j < dataCount - i - 1; j++)
      {
        fread(&temp, sizeof(PlayerLeaderboard), 1, leaderboardFile);
        fread(&temp2, sizeof(PlayerLeaderboard), 1, leaderboardFile);
        if (temp.money < temp2.money)
        {
          fseek(leaderboardFile, -(sizeof(PlayerLeaderboard) * 2), SEEK_CUR);
          fwrite(&temp2, sizeof(PlayerLeaderboard), 1, leaderboardFile);
          fwrite(&temp, sizeof(PlayerLeaderboard), 1, leaderboardFile);
        }
        fseek(leaderboardFile, -sizeof(PlayerLeaderboard), SEEK_CUR);
      }
      rewind(leaderboardFile);
    }
  }

  // jika data melebihi 10, hapus data terakhir
  if (dataCount > 10)
  {
    rewind(leaderboardFile);
    FILE *tempFile = fopen("temp", "wb");
    for (int i = 0; i < 10; i++)
    {
      fread(&temp, sizeof(PlayerLeaderboard), 1, leaderboardFile);
      fwrite(&temp, sizeof(PlayerLeaderboard), 1, tempFile);
    }
    fclose(leaderboardFile);
    fclose(tempFile);
    remove(FILE_NAME_LEADERBOARD);
    rename("temp", FILE_NAME_LEADERBOARD);
  }
  else
  {
    fclose(leaderboardFile);
  }
}

void dateToString(Date date, char *str)
{
  // change month to string
  char month[12][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
  sprintf(str, "%d %s %d %02d:%02d:%02d", date.day, month[date.month - 1], date.year, date.hour, date.minute, date.second);
}

void renderLeaderboard()
{
  system("cls");

  char templateLeaderboard[] = "╔════╦═════════════════════════════╦══════════════════╦═══════════════════════════════╦════════════════╗\n"
                               "║    ║                             ║                  ║                               ║                ║\n"
                               "║ No ║         Nama Player         ║       Uang       ║             Tanggal           ║  Lama Bermain  ║\n"
                               "║    ║                             ║                  ║                               ║                ║\n";
  char templateIfEmpty[] = "╠════╩═════════════════════════════╩══════════════════╩═══════════════════════════════╩════════════════╣\n"
                           "║                                                                                                      ║\n"
                           "║                                   Belum Ada Pemain di Leaderboard                                    ║\n"
                           "║                                                                                                      ║\n"
                           "╚══════════════════════════════════════════════════════════════════════════════════════════════════════╝\n";

  printf(templateLeaderboard);
  FILE *leaderboardFile = fopen(FILE_NAME_LEADERBOARD, "rb+");
  if (leaderboardFile == NULL || getLeaderboardDataCount() == 0)
  {
    printf(templateIfEmpty);
  }
  else
  {
    PlayerLeaderboard pl;
    int count = 1;
    while (fread(&pl, sizeof(PlayerLeaderboard), 1, leaderboardFile))
    {
      char date[30];
      dateToString(pl.date, date);
      printf("╠════╬═════════════════════════════╬══════════════════╬═══════════════════════════════╬════════════════╣\n");
      printf("║ %-2d ║ %-27s ║ $%-15d ║ %-29s ║ %02d:%02d:%02d       ║\n", count, pl.name, pl.money, date, pl.gameDuration.hours, pl.gameDuration.minutes, pl.gameDuration.seconds);
      count++;
    }
    printf("╚════╩═════════════════════════════╩══════════════════╩═══════════════════════════════╩════════════════╝\n");
  }
  fclose(leaderboardFile);

  printf("\n\nTekan ENTER untuk kembali ke main menu...");
  waitForEnter();
  system("cls");
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
  fp = fopen(FILE_NAME_HOW_TO_PLAY, "r");
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
  printf("\n\nTekan ENTER untuk kembali ke main menu...");
  waitForEnter();
  system("cls");
}
int selectionMenu(char notSelectedStrings[][50], char selectedStrings[][50], int maxIndex, int x, int y)
{
  flushInput();
  int selected = 0;
  renderSelectionMenu(notSelectedStrings, selectedStrings, selected, maxIndex, x, y, false);
  while (1)
  {
    int keyboardResult = keyboardEventHandler();
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
int renderSelectionMenu(char notSelectedStrings[][50], char selectedStrings[][50], int selectedIndex, int maxIndex, int x, int y, int selected)
{
  for (int i = 0; i <= maxIndex; i++)
  {
    gotoxy(x, y + i);
    if (i == selectedIndex)
    {
      if (selected)
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
                 "║    HANYA     ║  GZH   ║  PLN   ║  BJG   ║  SHN   ║  GBG   ║  MSL   ║  DANA  ║  BDX   ║  PAR   ║    PARKIR    ║\n"
                 "║ MENGUNJUNGI  ║        ║        ║        ║        ║        ║        ║  UMUM  ║        ║        ║    BEBAS     ║\n"
                 "║    ╔═════════╣        ║        ║        ║        ║        ║        ║        ║        ║        ║              ║\n"
                 "║    ║ PENJARA ║        ║        ║        ║        ║        ║        ║        ║        ║        ║      __      ║\n"
                 "║    ║         ║  $140  ║        ║  $140  ║  $160  ║        ║  $180  ║        ║  $180  ║  $200  ║    _| =\\__   ║\n"
                 "║    ║ ║  ║  ║ ║████████║  $150  ║████████║████████║  $200  ║████████║  $$$$  ║████████║████████║   /o____o_\\  ║\n"
                 "╠════╩═════════╬════════╩════════╩════════╩════════╩════════╩════════╩════════╩════════╩════════╬══════════════╣\n"
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
    if (plotList[i].propertyCard != NULL && plotList[i].propertyCard->propertyType == PROPERTY_CITY)
    {
      ScreenCoordinate tempScreenCoordinate = plotList[i].propertyCard->colorCoordinate;
      if (i < 10 || (i > 20 && i < 30))
      {
        for (int j = 0; j < 3; j++)
        {
          gotoxy(tempScreenCoordinate.x, tempScreenCoordinate.y + j);
          printWithColor(plotList[i].propertyCard->color, "  ");
        }
      }
      else
      {
        gotoxy(tempScreenCoordinate.x, tempScreenCoordinate.y);
        printWithColor(plotList[i].propertyCard->color, "        ");
      }
    }
  }
}

int keyboardEventHandler()
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
  else
  {
    return ch;
  }
}

Dice rollDice()
{
  int randomRollCount = randomInt(14, 21);
  for (int i = 0; i < randomRollCount; i++)
  {
    renderDice(randomDice(), false);
    delay(100);
  }
  Dice diceResult = randomDice();
  renderDice(diceResult, true);
  return diceResult;
}
Dice randomDice()
{
  Dice tempDice;
  tempDice.dice1 = randomInt(1, 6);
  tempDice.dice2 = randomInt(1, 6);
  return tempDice;
}
void renderDice(Dice d, bool last)
{
  char dice[6][5][40] = {{"┌─────────┐",
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
      printWithColor(3, "%s", dice[d.dice1 - 1][i]);
    }
    else
    {
      printf("%s", dice[d.dice1 - 1][i]);
    }
  }
  // print dadu kedua
  for (int i = 0; i < 5; i++)
  {
    gotoxy(38, 36 + i);
    if (last)
    {
      printWithColor(3, "%s", dice[d.dice2 - 1][i]);
    }
    else
    {
      printf("%s", dice[d.dice2 - 1][i]);
    }
  }
}
int getTotalDice(Dice d)
{
  return d.dice1 + d.dice2;
}
bool isDouble(Dice d)
{
  return d.dice1 == d.dice2;
}

void initGame(Player *players, int *playerCount)
{
  clearArea(0, 7, 77, 12);

  // jumlah player
  printCenter("Tentukan Jumlah Player (termasuk bot)", 7, 7, 64, 10);
  char NotSelectedPlayerCount[][50] = {"  2 PLAYER  ",
                                       "  3 PLAYER  ",
                                       "  4 PLAYER  "};
  char SelectedPlayerCount[][50] = {"⯈ 2 PLAYER ⯇",
                                    "⯈ 3 PLAYER ⯇",
                                    "⯈ 4 PLAYER ⯇"};
  int selectionPlayerCount = selectionMenu(NotSelectedPlayerCount, SelectedPlayerCount, 2, 33, 8);
  *playerCount = selectionPlayerCount + 2;
  delay(300);
  clearArea(0, 7, 77, 12);

  // jumlah bot
  printCenter("Tentukan Jumlah Player Bot", 7, 7, 64, 10);
  char notSelectedBotCount[][50] = {"  Tanpa Bot  ",
                                    "    1 BOT     ",
                                    "    2 BOT     ",
                                    "    3 BOT     ",
                                    "    4 BOT     "};
  char SelectedBotCount[][50] = {"⯈ Tanpa Bot ⯇",
                                 "  ⯈ 1 BOT ⯇   ",
                                 "  ⯈ 2 BOT ⯇   ",
                                 "  ⯈ 3 BOT ⯇   ",
                                 "  ⯈ 4 BOT ⯇   "};
  // selectionPlayerCount + 1 artinya minimal 1 pemain manusia , selectionPlayerCount +2 artinya minimal 0 pemain manusia
  int selectionBotCount = selectionMenu(notSelectedBotCount, SelectedBotCount, selectionPlayerCount + 2, 32, 8);
  delay(300);
  clearArea(0, 7, 77, 13);

  // menentukan tingkat kesulitan setiap bot
  int startBotIndex = *playerCount - selectionBotCount;
  if (selectionBotCount != 0)
  {
    for (int i = startBotIndex; i < *playerCount; i++)
    {
      char tempTextBotDifficulty[] = "Tentukan Tingkat Kesulitan Bot";
      sprintf(tempTextBotDifficulty, "%s %d", tempTextBotDifficulty, i - startBotIndex + 1);
      printCenter(tempTextBotDifficulty, 7, 7, 64, 10);
      char notSelectedBotDifficulty[][50] = {"   EASY    ",
                                             "  MEDIUM   ",
                                             "   HARD    "};
      char SelectedBotDifficulty[][50] = {" ⯈ EASY ⯇  ",
                                          "⯈ MEDIUM ⯇ ",
                                          " ⯈ HARD ⯇  "};
      players[i].botDifficulty = selectionMenu(notSelectedBotDifficulty, SelectedBotDifficulty, 2, 34, 8);
      players[i].isBot = true;
      delay(300);
      clearArea(0, 7, 77, 10);
    }
  }
  if (*playerCount != selectionBotCount)
  {
    gotoxy(12, 7);
    printWithColor(10, "Tentukan nama pemain manusia (max 25 karakter)");
    for (int i = 1; i <= *playerCount - selectionBotCount; i++)
    {
      players[i - 1].isBot = false;
      gotoxy(12, 8 + i);
      printf("Nama pemain %d: ", i);
      showCursor();

      // untuk membatasi input user menjadi 25 dan memotong sisanya
      scanf("%25[^\n]", players[i - 1].name);
      getchar();
      hideCursor();
    }
  }

  shuffleBotName(players, *playerCount, startBotIndex);
  gotoxy(12, 9 + *playerCount - selectionBotCount);
  printf("Mengacak urutan permainan...\n");
  shufflePlayer(players, *playerCount);
  shuffleSymbol(players, *playerCount);
  delay(800);
  // untuk mereset garbage value player
  for (int i = 0; i < *playerCount; i++)
  {
    players[i].money = 1500;
    players[i].turnOrder = i;
    players[i].position = 0;
    players[i].haveGetOutOfJailCard = false;
    players[i].isBankrupt = false;
    players[i].jailTurnsLeft = 0;
    for (int j = 0; j < 40; j++)
    {
      players[i].properties[j] = false;
    }
  }

  gotoxy(12, 10 + *playerCount - selectionBotCount);
  printf("urutan pemain:");
  for (int i = 0; i < *playerCount; i++)
  {
    gotoxy(12, 11 + *playerCount - selectionBotCount + i);
    printf("%d. (%s) %s\n", i + 1, players[i].symbol, players[i].name);
  }
  printf("\n");
  gotoxy(12, 11 + *playerCount * 2 - selectionBotCount);
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
    renderPlayerPosition(players[i], 0);
  }
  turnManager(players, playerCount, playerProperties);
  // setelah turn manager selesai, artinya game sudah selesai dan 1 pemain sudah menang
  system("cls");
}

void renderWinner(Player p)
{
  char template[] = "\n\n"
                    "                ███████████████████████████████████████████████████████████████████████████████████████\n"
                    "                █▄ █▀▀▀█ ▄█▄ ▄█▄ ▀█▄ ▄█▄ ▀█▄ ▄█▄ ▄▄ █▄ ▄▄▀███▄ █▀▀▀█ ▄█▄ ▄█▄ ▀█▄ ▄█▄ ▀█▄ ▄█▄ ▄▄ █▄ ▄▄▀█\n"
                    "                ██ █ █ █ ███ ███ █▄▀ ███ █▄▀ ███ ▄█▀██ ▄ ▄████ █ █ █ ███ ███ █▄▀ ███ █▄▀ ███ ▄█▀██ ▄ ▄█\n"
                    "                ██▄▄▄█▄▄▄██▄▄▄█▄▄▄██▄▄█▄▄▄██▄▄█▄▄▄▄▄█▄▄█▄▄████▄▄▄█▄▄▄██▄▄▄█▄▄▄██▄▄█▄▄▄██▄▄█▄▄▄▄▄█▄▄█▄▄█\n"
                    "                ▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀\n"
                    "         ██████╗██╗  ██╗██╗ ██████╗██╗  ██╗███████╗███╗   ██╗    ██████╗ ██╗███╗   ██╗███╗   ██╗███████╗██████╗ \n"
                    "        ██╔════╝██║  ██║██║██╔════╝██║ ██╔╝██╔════╝████╗  ██║    ██╔══██╗██║████╗  ██║████╗  ██║██╔════╝██╔══██╗\n"
                    "        ██║     ███████║██║██║     █████╔╝ █████╗  ██╔██╗ ██║    ██║  ██║██║██╔██╗ ██║██╔██╗ ██║█████╗  ██████╔╝\n"
                    "        ██║     ██╔══██║██║██║     ██╔═██╗ ██╔══╝  ██║╚██╗██║    ██║  ██║██║██║╚██╗██║██║╚██╗██║██╔══╝  ██╔══██╗\n"
                    "        ╚██████╗██║  ██║██║╚██████╗██║  ██╗███████╗██║ ╚████║    ██████╔╝██║██║ ╚████║██║ ╚████║███████╗██║  ██║\n"
                    "         ╚═════╝╚═╝  ╚═╝╚═╝ ╚═════╝╚═╝  ╚═╝╚══════╝╚═╝  ╚═══╝    ╚═════╝ ╚═╝╚═╝  ╚═══╝╚═╝  ╚═══╝╚══════╝╚═╝  ╚═╝\n";
  printWithColor(14, "%s", template);
  printCenter(p.name, 8, 15, 104, 11);
  printCenter("Tekan ENTER untuk melanjutkan", 8, 17, 104, 7);
  waitForEnter();
  system("cls");
}

void turnManager(Player *players, int *playerCount, PlayerProperty playerProperties[40])
{
  int turn = 0;
  int doubleCount = 0;
  int playersLeft = *playerCount;
  while (playersLeft > 1)
  {
    // kalau giliran sekarang pemain sudah bangkrut maka langsung continue ke pemain selanjutnya
    if (players[turn].isBankrupt)
    {
      turn = rotateIndex(turn, *playerCount - 1, true);
      continue;
    }

    clearRenderedCard();
    printTurnText(players[turn]);

    bool turnFinished = false; // untuk menandakan apakah turn sudah selesai atau belum

    if (players[turn].jailTurnsLeft > 0)
    {
      // jika pemain terkirim ke penjara dan sebelumnya me roll dadu double, pemain tidak akan mendapat turn tambahan
      if (doubleCount > 0 && players[turn].jailTurnsLeft == 4)
      {
        doubleCount = 0;
        turn = rotateIndex(turn, *playerCount - 1, true);
        continue;
      }

      turnFinished = jailHandler(&players[turn]);
      if (turnFinished) // kalau belum keluar dari penjara maka turn finished = true
      {
        turn = rotateIndex(turn, *playerCount - 1, true);
        continue;
      }
    }

    char selectionText[][50] = {"Kocok Dadu"};
    int selection = playerSelectionManager(players[turn], selectionText, 0, 0, "KOCOK_DADU", playerProperties);

    Dice diceResult = rollDice();
    int doubleCount = isDouble(diceResult) ? doubleCount + 1 : 0;

    // cek player triple double
    if (doubleCount == 3)
    {
      doubleCount = 0;
      gotoxy(117, 33);
      char text[] = "Anda telah mendapatkan double sebanyak 3 kali.\n"
                    "Anda masuk penjara!";
      printMultiLineAlignLeft(text, 117, 33, 7);
      char selectionText[][50] = {"Lanjut"};
      int selectionIndex = playerSelectionManager(players[turn], selectionText, 0, 2, "DOUBLE_3X_PENJARA", playerProperties);
      clearArea(117, 33, 187, 35);
      jumpToJail(&players[turn]);
      turn = rotateIndex(turn, *playerCount - 1, true);
      continue;
    }

    advanceNSteps(&players[turn], getTotalDice(diceResult));
    while (!turnFinished)
    {
      clearRenderedCard();
      if (strcmp(plotList[players[turn].position].special, "") == 0)
      {
        propertyHandler(players, turn, players[turn].position, playerProperties);
        turnFinished = true;
      }
      else
      {
        turnFinished = specialHandler(players, turn, plotList[players[turn].position].special, *playerCount, playerProperties);
      }
    }

    // cek apakah player sekarang bangkrut
    if (players[turn].money < 0)
    {
      bool playerSudahBangkrut = insufficientMoneyHandler(&players[turn], playerProperties);
      if (playerSudahBangkrut)
      {
        setPlayerAsBankrupt(&players[turn]);
        playersLeft--;
      }
    }

    // kalo engga double pindah giliran ke player selanjutnya
    if (doubleCount == 0)
    {
      turn = rotateIndex(turn, *playerCount - 1, true);
    }
  }
}

void printTurnText(Player p)
{
  clearGiliranText();
  gotoxy(117, 32);
  printf("Giliran %s (%s)", p.name, p.symbol);
}
void clearGiliranText()
{
  clearArea(117, 32, 167, 32);
}
void clearPlayerSelectionText()
{
  clearArea(117, 33, 187, 40);
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
    gotoxy(118, 1 + (players[i].turnOrder * 3));
    printf("%s", players[i].name);
    gotoxy(144, 1 + (players[i].turnOrder * 3));
    printf("(%s)", players[i].symbol);
    renderPlayerMoney(players[i], 7);
  }
}

int changePlayerMoney(Player *player, int amount)
{
  int moneyBefore = player->money;
  int moneyAfter = moneyBefore + amount;
  if (amount < 0)
  {
    while (player->money > moneyAfter)
    {
      if (player->money - 4 >= moneyAfter)
      {
        player->money -= 4;
      }
      else
      {
        player->money--;
      }
      renderPlayerMoney(*player, 4);
      delay(1);
    }
  }
  else
  {
    while (player->money < moneyAfter)
    {
      if (player->money + 4 <= moneyAfter)
      {
        player->money += 4;
      }
      else
      {
        player->money++;
      }
      renderPlayerMoney(*player, 2);
      delay(1);
    }
  }
  renderPlayerMoney(*player, 7);
  return player->money;
}

void renderPlayerMoney(Player p, int color)
{
  gotoxy(118, 2 + (p.turnOrder * 3));
  printf("                         ");
  gotoxy(118, 2 + (p.turnOrder * 3));
  if (!p.isBankrupt)
  {
    printWithColor(color, "$%d", p.money);
  }
  else
  {
    printWithColor(4, "BANGKRUT");
  }
}

void advanceNSteps(Player *p, int N)
{
  for (int i = 0; i < abs(N); i++)
  {
    delay(200);
    int previousPosition = p->position;
    if (N > 0)
    {
      p->position = rotateIndex(p->position, 39, true);
    }
    else
    {
      p->position = rotateIndex(p->position, 39, false);
    }
    renderPlayerPosition(*p, previousPosition);
    checkIsInStart(p);
  }
}
void jumpTo(Player *p, int index)
{
  int previousPosition = p->position;
  p->position = index;
  renderPlayerPosition(*p, previousPosition);
  checkIsInStart(p);
}
void advanceTo(Player *p, int index)
{
  while (p->position != index)
  {
    advanceNSteps(p, 1);
  }
}
void advanceToPropertyType(Player *p, char *type)
{
  bool isArrived = false;
  while (!isArrived)
  {
    advanceNSteps(p, 1);
    if (strcmp(plotList[p->position].special, "") == 0)
    {
      if (plotList[p->position].propertyCard->propertyType == type)
      {
        isArrived = true;
      }
    }
  }
}
void jumpToJail(Player *p)
{
  p->jailTurnsLeft = 4;
  jumpTo(p, 10);
}
bool checkSufficientMoney(int money, int amount)
{
  return money >= amount;
}

void checkIsInStart(Player *p)
{
  if (p->position == 0)
  {
    changePlayerMoney(p, 200);
  }
}
void renderPlayerPosition(Player p, int previous)
{
  // hapus hasil render player di posisi sebelumnya
  if (previous == 10)
  {
    // special case jika penjara
    gotoxy(8 + p.turnOrder, 5);
    printf(" ");
    gotoxy(2 + (p.turnOrder % 2), 4 + (p.turnOrder > 1 ? 1 : 0));
    printf(" ");
  }
  else
  {
    gotoxy(plotList[previous].playerCoordinate.x + p.turnOrder, plotList[previous].playerCoordinate.y);
    printf(" ");
  }

  // render posisi baru player
  if (!p.isBankrupt)
  {
    // special case jika penjara
    if (p.position == 10)
    {
      // cek apakah sedang dalam penjara
      if (p.jailTurnsLeft > 0)
      {
        gotoxy(8 + p.turnOrder, 5);
      }
      // kalau tidak
      else
      {
        gotoxy(2 + (p.turnOrder % 2), 4 + (p.turnOrder > 1 ? 1 : 0));
      }
    }
    else
    {
      gotoxy(plotList[p.position].playerCoordinate.x + p.turnOrder, plotList[p.position].playerCoordinate.y);
    }
    printWithColor(240, "%s", p.symbol);
  }
}

void setPlayerAsBankrupt(Player *p)
{
  p->isBankrupt = true;
  renderPlayerMoney(*p, 4);
  renderPlayerPosition(*p, p->position);
}

void propertyHandler(Player *players, int turn, int position, PlayerProperty playerProperties[40])
{
  PropertyCard *propertyCard = plotList[position].propertyCard;
  renderPropertyCard(propertyCard);
  /*kalau belum dimiliki:
  - Akhiri giliran
  - beli

  kalau sudah dimiliki player lain:
  - bayar sewa
  - bayar sewa dan ambil alih

  kalau sudah dimiliki sendiri:
  - Akhiri giliran
  - jual properti
  - beli rumah/hotel
  */

  // property belum dimiliki oleh siapapun
  if (playerProperties[position].owner == NULL)
  {
    char selectionText[][50] = {
        "Akhiri Giliran",
        "Beli Property"};
    int buyPropertyPrice = propertyCard->buyPrice;
    // kalau uang cukup untuk membeli properti
    sprintf(selectionText[1], "Beli Property ($%d)", propertyCard->buyPrice);
    bool moneySufficientToBuy = checkSufficientMoney(players[turn].money, buyPropertyPrice);
    int selection = playerSelectionManager(players[turn], selectionText, moneySufficientToBuy ? 1 : 0, 0, "AKHIRI_BELI_PROPERTY", playerProperties);
    switch (selection)
    {
    case 0:
      break;
    case 1:
      buyProperty(&players[turn], &playerProperties[position], position);
      break;
    }
  }

  // artinya property sudah dimiliki oleh orang lain
  else if (playerProperties[position].owner->turnOrder != turn)
  {
    char selectionText[][50] = {
        "Bayar sewa",
        "Bayar sewa dan ambil alih"};
    int rentPrice, takeOverPrice;
    // harga sewa menjadi MULTIPLIER_SEWA_LENGKAPx jika pemain memiliki semua property pada kompleks yang sama
    //  harga ambil alih MULTIPLIER_AMBIL_ALIH_LENGKAPx lipat jika pemain memiliki semua property pada kompleks yang sama
    //  harga biaya sewa tergantung jenis property
    if (plotList[position].propertyCard->propertyType == PROPERTY_CITY)
    {
      rentPrice = propertyCard->rentPrice[playerProperties[position].level];
      if (isClusterCompleted(*playerProperties[position].owner, position))
      {
        rentPrice = MULTIPLIER_RENT_COMPLETED * rentPrice;
        takeOverPrice = MULTIPLIER_TAKE_OVER_COMPLETED * (propertyCard->buyPrice + playerProperties[position].level * propertyCard->upgradePrice);
      }
      else
      {
        takeOverPrice = MULTIPLIER_TAKE_OVER * (propertyCard->buyPrice + playerProperties[position].level * propertyCard->upgradePrice);
      }
    }
    else if (plotList[position].propertyCard->propertyType == PROPERTY_STATION)
    {
      int ownedStationsCount = totalStationOwned(*playerProperties[position].owner);
      rentPrice = propertyCard->rentPrice[ownedStationsCount - 1];
      if (ownedStationsCount == 4)
      {
        takeOverPrice = MULTIPLIER_TAKE_OVER_COMPLETED * (propertyCard->buyPrice);
      }
      else
      {
        takeOverPrice = MULTIPLIER_TAKE_OVER * (propertyCard->buyPrice);
      }
    }
    else if (plotList[position].propertyCard->propertyType == PROPERTY_COMPANY)
    {
      int companyCount = totalCompanyOwned(*playerProperties[position].owner);
      takeOverPrice = MULTIPLIER_TAKE_OVER * (propertyCard->buyPrice);
      int diceRollCount = 4;
      if (companyCount == 2)
      {
        diceRollCount = 10;
        takeOverPrice = MULTIPLIER_TAKE_OVER_COMPLETED * (propertyCard->buyPrice);
      }

      rentPrice = 0;
      // roll dadu untuk mengetahui berapa harga yang harus dibayar
      int rollCount = diceRollCount / 2;
      Dice dice;
      for (int i = 0; i < rollCount; i++)
      {
        gotoxy(117, 33);
        printf("Sedang melempar dadu sebanyak %d kali. Harga sewa sekarang: %d", diceRollCount, rentPrice);
        dice = rollDice();
        rentPrice += getTotalDice(dice);
        delay(300);
      }
      clearArea(117, 33, 187, 33);
    }
    sprintf(selectionText[0], "Bayar Sewa ($%d)", rentPrice);
    sprintf(selectionText[1], "Bayar Sewa dan ambil alih ($%d)", rentPrice + takeOverPrice);
    bool moneySufficientToTakeOver = checkSufficientMoney(players[turn].money, rentPrice + takeOverPrice);
    int selection = playerSelectionManager(players[turn], selectionText, moneySufficientToTakeOver ? 1 : 0, 0, "SEWA_AMBIL_ALIH_PROPERTY", playerProperties);

    switch (selection)
    {
    case 0:
      changePlayerMoney(&players[turn], -rentPrice);
      changePlayerMoney(playerProperties[position].owner, rentPrice);
      break;
    case 1:
      takeOverProperty(&players[turn], &playerProperties[position], position);
      break;
    }
  }
  // kalau else ditambah, player tidak dapat langsung membeli rumah setelah membeli properti, harus setelah turn berikutnya
  if (playerProperties[position].owner != NULL && playerProperties[position].owner->turnOrder == turn)
  {
    bool selectionFinished = false;
    while (!selectionFinished)
    {
      char selectionText[][50] = {
          "Akhiri Giliran",
          "Jual property",
          "Upgrade rumah/hotel"};
      int propertySellPrice = getPropertySellPrice(playerProperties[position], *propertyCard);
      sprintf(selectionText[1], "Jual property ($%d)", propertySellPrice);
      int canUpgradeAgain = playerProperties[position].level < 5;
      // kalau perusahaan atau stasion tidak dapat di upgrade
      // kalau level property sudah hotel tidak dapat di upgrade
      bool canUpgrade = propertyCard->propertyType == PROPERTY_CITY && playerProperties[position].level < 5;
      int selection = playerSelectionManager(players[turn], selectionText, canUpgrade ? 2 : 1, 0, "AKHIRI_JUAL_UPGRADE_PROPERTY", playerProperties);
      switch (selection)
      {
      case 0:
        selectionFinished = true;
        break;
      case 1:
        sellProperty(&players[turn], &playerProperties[position], position);
        selectionFinished = true;
        break;
      case 2:
        if (playerProperties[position].level < 4)
        {
          char selectionTextUpgrade[][50] = {
              "Kembali",
              "1 Rumah",
              "2 Rumah",
              "3 Rumah",
              "4 Rumah"};
          sprintf(selectionTextUpgrade[1], "1 Rumah ($%d)", propertyCard->upgradePrice);
          sprintf(selectionTextUpgrade[2], "2 Rumah ($%d)", propertyCard->upgradePrice * 2);
          sprintf(selectionTextUpgrade[3], "3 Rumah ($%d)", propertyCard->upgradePrice * 3);
          sprintf(selectionTextUpgrade[4], "4 Rumah ($%d)", propertyCard->upgradePrice * 4);
          int maxIndexSelection = 4 - playerProperties[position].level;
          for (int i = maxIndexSelection; i >= 0; i--)
          {
            if (propertyCard->upgradePrice * i < players[turn].money)
            {
              maxIndexSelection = i;
              break;
            }
          }
          int selectionUpgrade = playerSelectionManager(players[turn], selectionTextUpgrade, maxIndexSelection, 0, "UPGRADE_RUMAH_PROPERTY", playerProperties);
          if (selectionUpgrade == 0)
          {
            break;
          }
          else
          {
            changePlayerMoney(&players[turn], -(propertyCard->upgradePrice * selectionUpgrade));
            playerProperties[position].level += selectionUpgrade;
            renderProperty(playerProperties[position], position);
            selectionFinished = true;
          }
        }
        else
        {
          char selectionTextUpgrade[][50] = {"Kembali",
                                             "1 Hotel"};
          sprintf(selectionTextUpgrade[1], "1 Hotel ($%d)", propertyCard->upgradePrice);
          int selectionUpgrade = playerSelectionManager(players[turn], selectionTextUpgrade, 1, 0, "UPGRADE_HOTEL_PROPERTY", playerProperties);
          if (selectionUpgrade == 0)
          {
            break;
          }
          else
          {
            changePlayerMoney(&players[turn], -propertyCard->upgradePrice);
            playerProperties[position].level += 1;
            renderProperty(playerProperties[position], position);
            selectionFinished = true;
          }
        }
      }
    }
  }
}

bool isClusterCompleted(Player p, int position)
{
  bool isHaveAll = true;
  int clusterId = plotList[position].propertyCard->clusterId;
  int maxProperty = plotList[position].propertyCard->onlyTwo ? 2 : 3;
  for (int i = 0; i < maxProperty; i++)
  {
    if (!p.properties[clustersGroup[clusterId][i]])
    {
      isHaveAll = false;
      break;
    }
  }
  return isHaveAll;
}
int totalPropertyOwnedInCluster(Player p, int position)
{
  int total = 0;
  int clusterId = plotList[position].propertyCard->clusterId;
  int maxProperty = plotList[position].propertyCard->onlyTwo ? 2 : 3;
  for (int i = 0; i < maxProperty; i++)
  {
    if (p.properties[clustersGroup[clusterId][i]])
    {
      total++;
    }
  }
  return total;
}
int totalStationOwned(Player p)
{
  int total = 0;
  for (int i = 0; i < 4; i++)
  {
    if (p.properties[clustersGroup[8][i]])
    {
      total++;
    }
  }
  return total;
}

int totalCompanyOwned(Player p)
{
  int total = 0;
  for (int i = 0; i < 2; i++)
  {
    if (p.properties[clustersGroup[9][i]])
    {
      total++;
    }
  }
  return total;
}

void buyProperty(Player *p, PlayerProperty *playerProperty, int position)
{
  PropertyCard *propertyCard = plotList[position].propertyCard;
  int buyPropertyPrice = propertyCard->buyPrice;
  changePlayerMoney(p, -buyPropertyPrice);
  addPropertyOwnership(p, playerProperty, position);
}

void sellProperty(Player *p, PlayerProperty *playerProperty, int position)
{
  /*        changePlayerMoney(playerProperties[position].owner, propertySellPrice);
          playerProperties[position].owner->properties[position] = false;
          playerProperties[position].owner = NULL;
          playerProperties[position].level = 0;
          renderProperty(playerProperties[position], position);
          selectionFinished = true;*/

  PropertyCard *propertyCard = plotList[position].propertyCard;
  int hargaJualProperty = getPropertySellPrice(*playerProperty, *propertyCard); // MULTIPLIER_SELL_UNILATERALLY * (propertyCard->buyPrice + playerProperty->level * propertyCard->upgradePrice);
  changePlayerMoney(p, hargaJualProperty);
  removePropertyOwnership(playerProperty, position);
}

void takeOverProperty(Player *p, PlayerProperty *playerProperty, int position)
{
  PropertyCard *propertyCard = plotList[position].propertyCard;
  int hargaAmbilAlih = MULTIPLIER_TAKE_OVER * (propertyCard->buyPrice + playerProperty->level * propertyCard->upgradePrice);
  changePlayerMoney(p, -hargaAmbilAlih);
  changePlayerMoney(playerProperty->owner, hargaAmbilAlih);
  transferPropertyOwnership(p, playerProperty, position);
}

void addPropertyOwnership(Player *p, PlayerProperty *playerProperty, int position)
{
  playerProperty->owner = p;
  playerProperty->level = 0;
  p->properties[position] = true;
  renderProperty(*playerProperty, position);
}

void removePropertyOwnership(PlayerProperty *playerProperty, int position)
{
  playerProperty->owner->properties[position] = false;
  playerProperty->owner = NULL;
  playerProperty->level = 0;
  renderProperty(*playerProperty, position);
}
void transferPropertyOwnership(Player *newOwner, PlayerProperty *playerProperty, int position)
{
  playerProperty->owner->properties[position] = false;
  newOwner->properties[position] = true;
  playerProperty->owner = newOwner;
  renderProperty(*playerProperty, position);
}

void renderProperty(PlayerProperty playerProperty, int position)
{
  PropertyCard *tempPropertyCard = plotList[position].propertyCard;
  gotoxy(tempPropertyCard->ownershipCoordinate.x, tempPropertyCard->ownershipCoordinate.y);
  if (playerProperty.owner != NULL)
  {
    printf("%s", playerProperty.owner->symbol);
  }
  else
  {
    printf(" ");
  }
  // cek apakah property merupakan kota atau bukan, kalau iya render rumah, kalau tidak jangan
  if (tempPropertyCard->propertyType == PROPERTY_CITY)
  {
    gotoxy(tempPropertyCard->houseCoordinate.x, tempPropertyCard->houseCoordinate.y);
    if (playerProperty.level == 0)
    {
      printWithColor(tempPropertyCard->color, "  ");
    }
    else if (playerProperty.level <= 4)
    {
      printWithColor(tempPropertyCard->color, "%dR", playerProperty.level);
    }
    else
    {
      printWithColor(tempPropertyCard->color, "1H");
    }
  }
}

bool specialHandler(Player *players, int turn, char special[30], int playerCount, PlayerProperty playerProperties[40])
{
  if (strcmp(special, "DANA_UMUM") == 0)
  {
    return communityChestCardHandler(players, turn, playerCount, playerProperties);
  }
  else if (strcmp(special, "KESEMPATAN") == 0)
  {
    return chanceCardHandler(players, turn, playerCount, playerProperties);
  }
  else if (strcmp(special, "INCOME_TAX") == 0)
  {
    char selectionText[][50] = {"Bayar Income Tax $200"};
    int selection = playerSelectionManager(players[turn], selectionText, 0, 0, "BAYAR_INCOME_TAX", playerProperties);
    if (selection == 0)
    {
      changePlayerMoney(&players[turn], -200);
    }
    return true;
  }
  else if (strcmp(special, "LUXURY_TAX") == 0)
  {
    char selectionText[][50] = {"Bayar Luxury Tax $75"};
    int selection = playerSelectionManager(players[turn], selectionText, 0, 0, "BAYAR_LUXURY_TAX", playerProperties);
    if (selection == 0)
    {
      changePlayerMoney(&players[turn], -75);
    }
    return true;
  }
  else if (strcmp(special, "PARKIR_BEBAS") == 0)
  {
    int selectedPosition;
    if (players[turn].isBot)
    {
      // bot
      selectedPosition = FreeParkingBotHandler(&players[turn], playerProperties);
    }
    else
    {
      // player manusia
      selectedPosition = 20;
      clearPlayerSelectionText();
      gotoxy(117, 33);
      printf("Gunakan ↑ dan ↓ pada keyboard untuk memilih lokasi tujuan.");
      bool isSelected = false;
      gotoxy(plotList[selectedPosition].playerCoordinate.x, plotList[selectedPosition].playerCoordinate.y);
      showCursor();
      while (!isSelected)
      {
        int keyboardResult = keyboardEventHandler();
        if (keyboardResult == ARROW_UP)
        {
          selectedPosition = rotateIndex(selectedPosition, 39, true);
        }
        else if (keyboardResult == ARROW_DOWN)
        {
          selectedPosition = rotateIndex(selectedPosition, 39, false);
        }
        else if (keyboardResult == KEY_ENTER)
        {
          isSelected = true;
          hideCursor();
          clearArea(117, 33, 187, 33);
        }
        gotoxy(plotList[selectedPosition].playerCoordinate.x, plotList[selectedPosition].playerCoordinate.y);
      }
    }
    advanceTo(&players[turn], selectedPosition);
    return false;
  }
  else if (strcmp(special, "MASUK_PENJARA") == 0)
  {
    jumpToJail(&players[turn]);
    return true;
  }
  else
  {
    // untuk hanya mengunjungi penjara
    return true;
  }
}

bool jailHandler(Player *p)
{
  char selectionText[][50] = {"Bayar $50 langsung keluar penjara",
                              "Kocok dadu (jika double keluar penjara)",
                              "Gunakan kartu bebas penjara"};
  int selection = playerSelectionManager(*p, selectionText, p->jailTurnsLeft == 1 ? 0 : p->haveGetOutOfJailCard ? 2
                                                                                                                : 1,
                                         0,
                                         "BAYAR_KOCOK_KARTU_PENJARA", NULL);
  switch (selection)
  {
  case 0:
    changePlayerMoney(p, -50);
    p->jailTurnsLeft = 0;
    return false;
    break;
  case 1:
    if (isDouble(rollDice()))
    {
      p->jailTurnsLeft = 0;
      clearPlayerSelectionText();
      char text[] = "Anda berhasil keluar dari penjara.";
      printMultiLineAlignLeft(text, 117, 33, 7);
      char selectionText[][50] = {"Lanjut"};
      int selection = playerSelectionManager(*p, selectionText, 0, 1, "BERHASIL_KELUAR_PENJARA", NULL);
      clearPlayerSelectionText();
      return false;
    }
    else
    {
      p->jailTurnsLeft = p->jailTurnsLeft - 1;
      clearPlayerSelectionText();
      char text[] = "Anda gagal keluar dari penjara.";
      printMultiLineAlignLeft(text, 117, 33, 7);
      char selectionText[][50] = {"Lanjut"};
      int selection = playerSelectionManager(*p, selectionText, 0, 1, "GAGAL_KELUAR_PENJARA", NULL);
      clearPlayerSelectionText();
      return true;
    }
    break;
  case 2:
    p->haveGetOutOfJailCard = false;
    p->jailTurnsLeft = 0;
    return false;
    break;
  }
}

int getPropertySellPrice(PlayerProperty playerProperty, PropertyCard propertyCard)
{
  return MULTIPLIER_SELL_UNILATERALLY * (propertyCard.buyPrice + playerProperty.level * propertyCard.upgradePrice);
}

// return true jika kartu kesempatan sudah selesai menghandle pemain, misalnya player hanya membayar uang
// return false jika kartu kesempatan hanya mengubah position player, namun pembayaran dan sebagaimancamnya tidak di handle
bool chanceCardHandler(Player players[], int turn, int playerCount, PlayerProperty playerProperties[40])
{
  char chances[][200] = {"Pergi ke Guangzhou. Jika melewati\n\"GO\" ambil $200",                                  // false
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
  int random = randomInt(0, 15);
  // render kartu
  renderChanceCard(chances[random]);

  char selectionText[][50] = {"Lanjut"};
  int selection = playerSelectionManager(players[turn], selectionText, 0, 0, "LANJUT_KARTU_KESEMPATAN", playerProperties);
  switch (random)
  {
  case 0:
    advanceTo(&players[turn], 11);
    break;
  case 1:
    players[turn].haveGetOutOfJailCard = true;
    break;
  case 2:
    advanceTo(&players[turn], 5);
    break;
  case 3:
    advanceToPropertyType(&players[turn], PROPERTY_STATION);
    break;
  case 4:
    advanceTo(&players[turn], 24);
    break;
  case 5:
    for (int i = 0; i < 40; i++)
    {
      if (players[turn].properties[i] == true)
      {
        if (playerProperties[i].level > 0)
        {
          if (playerProperties[i].level < 5)
          {
            changePlayerMoney(&players[turn], -25 * playerProperties[i].level);
          }
          else
          {
            changePlayerMoney(&players[turn], -100);
          }
        }
      }
    }
    break;
  case 6:
    advanceTo(&players[turn], 0);
    break;
  case 7:
    changePlayerMoney(&players[turn], 50);
    break;
  case 8:
    changePlayerMoney(&players[turn], -15);
    break;
  case 9:
    advanceToPropertyType(&players[turn], PROPERTY_COMPANY);
    break;
  case 10:
    jumpToJail(&players[turn]);
    break;
  case 11:
  {
    int playersLeft = 0;
    for (int i = 0; i < playerCount; i++)
    {
      if (i != turn && !players[i].isBankrupt)
      {
        playersLeft++;
        changePlayerMoney(&players[i], 50);
      }
    }
    changePlayerMoney(&players[turn], -50 * playersLeft);
    break;
  }
  case 12:
    advanceTo(&players[turn], 39);
    break;
  case 13:
    advanceNSteps(&players[turn], -3);
    break;
  case 14:
    changePlayerMoney(&players[turn], 150);
    break;
  case 15:
    jumpTo(&players[turn], 20);
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
bool communityChestCardHandler(Player players[], int turn, int playerCount, PlayerProperty playerProperties[40])
{

  char communityChests[][200] = {"Bayar pajak sekolah $150",
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
  int random = randomInt(0, 15);
  // render kartu
  renderCommunityChestCard(communityChests[random]);

  char selectionText[][50] = {"Lanjut"};
  int selection = playerSelectionManager(players[turn], selectionText, 0, 0, "LANJUT_KARTU_DANA_UMUM", playerProperties);

  switch (random)
  {
  case 0:
    changePlayerMoney(&players[turn], -150);
    break;
  case 1:
  {
    int playersLeft = 0;
    for (int i = 0; i < playerCount; i++)
    {
      if (i != turn && !players[i].isBankrupt)
      {
        playersLeft++;
        changePlayerMoney(&players[i], 50);
      }
    }
    changePlayerMoney(&players[turn], -50 * playersLeft);
    break;
  }
  case 2:
    changePlayerMoney(&players[turn], -100);
    break;
  case 3:
    changePlayerMoney(&players[turn], -100);
    break;
  case 4:
    changePlayerMoney(&players[turn], 20);
    break;
  case 5:
    jumpToJail(&players[turn]);
    break;
  case 6:
    players[turn].haveGetOutOfJailCard = true;
    break;
  case 7:
    for (int i = 0; i < 40; i++)
    {
      if (players[turn].properties[i] == true)
      {
        if (playerProperties[i].level > 0)
        {
          if (playerProperties[i].level < 5)
          {
            changePlayerMoney(&players[turn], -40 * playerProperties[i].level);
          }
          else
          {
            changePlayerMoney(&players[turn], -115);
          }
        }
      }
    }
    break;
  case 8:
    changePlayerMoney(&players[turn], 200);
    break;
  case 9:
    advanceTo(&players[turn], 0);
    break;
  case 10:
    changePlayerMoney(&players[turn], 100);
    break;
  case 11:
    changePlayerMoney(&players[turn], -50);
    break;
  case 12:
    changePlayerMoney(&players[turn], 45);
    break;
  case 13:
    changePlayerMoney(&players[turn], 25);
    break;
  case 14:
    changePlayerMoney(&players[turn], 100);
    break;
  case 15:
    changePlayerMoney(&players[turn], 10);
    break;
  }
  return true;
}

// return true jika player dinyatakan bangkrut(tidak dapat membayar hingga uangnya >= 0)
bool insufficientMoneyHandler(Player *p, PlayerProperty playerProperties[40])
{
  // pertama, semua property milik pemain tersebut akan di list
  // jika pemain memiliki 0 peroperti, maka langsung bangkrut
  // jika pemain memiliki setidaknya 1 properti dan jumlah harga seluruh propertinya cukup untuk membayar, maka dia dapat memilih properti yang ingin dijual
  // juka pemain memiliki properti dan jumlah semua propertinya tidak cukup untuk membayar, maka pemain bangkrut
  int totalOwnedProperty = 0;
  int totalPropertyPrice = 0;
  int listOwnedProperty[28];

  bool selectedPositions[28];
  int totalMoneyFromSelectedProperties = 0;
  for (int i = 0; i < 28; i++)
  {
    selectedPositions[i] = false;
  }

  // semua property milik pemain tersebut di list
  for (int i = 0; i < 40; i++)
  {
    if (p->properties[i])
    {
      totalPropertyPrice += getPropertySellPrice(playerProperties[i], *plotList[i].propertyCard);
      listOwnedProperty[totalOwnedProperty] = i;
      totalOwnedProperty++;
    }
  }

  if (totalOwnedProperty == 0)
  {
    clearPlayerSelectionText();
    char text[] = "Anda kehabisan uang dan Anda tidak memiliki property.\n"
                  "Anda dinyatakan bangkrut.";
    printMultiLineAlignLeft(text, 117, 33, 7);
    char selectionText[][50] = {"Lanjut"};
    int selection = playerSelectionManager(*p, selectionText, 0, 2, "LANJUT_BANGKRUT", playerProperties);
    clearPlayerSelectionText();
    return true;
  }
  else if (totalPropertyPrice + p->money < 0)
  {
    clearPlayerSelectionText();
    char text[190];
    sprintf(text, "Anda kehabisan uang dan total harga jual property Anda\n"
                  "sebesar $%d tidak cukup untuk menutupi kekurangan\n"
                  "uang Anda. Anda dinyatakan bangkrut.",
            totalPropertyPrice);
    printMultiLineAlignLeft(text, 117, 33, 7);
    char selectionText[][50] = {"Lanjut"};
    int selection = playerSelectionManager(*p, selectionText, 0, 3, "LANJUT_BANGKRUT", playerProperties);
    for (int i = 0; i < totalOwnedProperty; i++)
    {
      removePropertyOwnership(&playerProperties[listOwnedProperty[i]], listOwnedProperty[i]);
    }
    clearPlayerSelectionText();
    return true;
  }
  else
  {
    if (p->isBot)
    {
      insufficientMoneyBotHandler(p, playerProperties, totalOwnedProperty, listOwnedProperty, selectedPositions, &totalMoneyFromSelectedProperties);
    }
    else
    {
      clearPlayerSelectionText();
      char text[] = "Anda kehabisan uang dan harus menjual property-property milik Anda.\n"
                    "Gunakan ↑ dan ↓ pada keyboard untuk memilih property yang akan dijual.\n"
                    "Tekan SPASI untuk menselect property yang akan dijual\n"
                    "Tekan ENTER untuk menjual semua property yang sudah dipilih";
      printMultiLineAlignLeft(text, 117, 33, 7);
      int currentIndexPosition = 0;

      gotoxy(117, 37);
      printf("Akumulasi harga jual: %d", totalMoneyFromSelectedProperties);
      bool selectionFinished = false;
      gotoxy(plotList[listOwnedProperty[currentIndexPosition]].playerCoordinate.x, plotList[listOwnedProperty[currentIndexPosition]].playerCoordinate.y);
      showCursor();
      while (!selectionFinished)
      {
        int keyboardResult = keyboardEventHandler();
        if (keyboardResult == ARROW_UP)
        {
          currentIndexPosition = rotateIndex(currentIndexPosition, totalOwnedProperty - 1, true);
          clearRenderedCard();
          renderPropertyCard(plotList[listOwnedProperty[currentIndexPosition]].propertyCard);
        }
        else if (keyboardResult == ARROW_DOWN)
        {
          currentIndexPosition = rotateIndex(currentIndexPosition, totalOwnedProperty - 1, false);
          clearRenderedCard();
          renderPropertyCard(plotList[listOwnedProperty[currentIndexPosition]].propertyCard);
        }
        else if (keyboardResult == KEY_SPACE)
        {
          selectedPositions[currentIndexPosition] = !selectedPositions[currentIndexPosition];
          int hargaJualProperty = getPropertySellPrice(playerProperties[listOwnedProperty[currentIndexPosition]], *plotList[listOwnedProperty[currentIndexPosition]].propertyCard);
          if (selectedPositions[currentIndexPosition])
          {
            totalMoneyFromSelectedProperties += hargaJualProperty;
          }
          else
          {
            totalMoneyFromSelectedProperties -= hargaJualProperty;
          }
          // menjadikan posisiKepemilikan menjadi warna biru jika terpilih (warna 23)
          gotoxy(plotList[listOwnedProperty[currentIndexPosition]].propertyCard->ownershipCoordinate.x, plotList[listOwnedProperty[currentIndexPosition]].propertyCard->ownershipCoordinate.y);
          printWithColor(selectedPositions[currentIndexPosition] ? 23 : 7, "%s", p->symbol);
          clearArea(117, 37, 187, 37);
          gotoxy(117, 37);
          printf("Akumulasi harga jual: %d", totalMoneyFromSelectedProperties);
        }
        else if (keyboardResult == KEY_ENTER)
        {
          if (totalMoneyFromSelectedProperties + p->money < 0)
          {
            clearArea(117, 37, 187, 37);
            gotoxy(117, 37);
            printWithColor(12, "property yang dipilih belum cukup untuk menutupi kekurangan uang!");
            delay(1000);
            clearArea(117, 37, 187, 37);
            gotoxy(117, 37);
            printf("Akumulasi harga jual: %d", totalMoneyFromSelectedProperties);
          }
          else
          {
            selectionFinished = true;
          }
        }
        gotoxy(plotList[listOwnedProperty[currentIndexPosition]].playerCoordinate.x, plotList[listOwnedProperty[currentIndexPosition]].playerCoordinate.y);
      }
    }
    hideCursor();
    clearArea(117, 33, 187, 37);
    for (int i = 0; i < 28; i++)
    {
      if (selectedPositions[i])
      {
        removePropertyOwnership(&playerProperties[listOwnedProperty[i]], listOwnedProperty[i]);
      }
    }
    changePlayerMoney(p, totalMoneyFromSelectedProperties);

    return false;
  }
}

void renderChanceCard(char *text)
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
    printCenter(token, 118, 20 + currentLine, 36, 7);
    token = strtok(NULL, "\n");
    currentLine++;
  }
}
void renderCommunityChestCard(char *text)
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
    printCenter(token, 118, 20 + currentLine, 36, 7);
    token = strtok(NULL, "\n");
    currentLine++;
  }
}

void renderPropertyCard(PropertyCard *propertyCard)
{
  if (strcmp(propertyCard->propertyType, PROPERTY_CITY) == 0)
  {
    char propertyCardText[][100] = {"┌─────────────────────────────┐",
                                    "│                             │",
                                    "├─────────────────────────────┤",
                                    "│                             │",
                                    "│     Jika sudah memiliki     │",
                                    "│ 1 Rumah : $                 │",
                                    "│ 2 Rumah : $                 │",
                                    "│ 3 Rumah : $                 │",
                                    "│ 4 Rumah : $                 │",
                                    "│ 1 Hotel : $                 │",
                                    "│ Sewa jika lengkap :         │",
                                    "│ Harga rumah & hotel : $     │",
                                    "│                             │",
                                    "│ Harga jual dari total asset │",
                                    "│ Secara sepihak :            │",
                                    "│ Ambil alih     :            │",
                                    "└─────────────────────────────┘"};
    for (int i = 0; i < 17; i++)
    {
      gotoxy(116, 14 + i);
      printf("%s", propertyCardText[i]);
    }
    printCenter(propertyCard->name, 117, 15, 29, propertyCard->color);
    char rentText[15];
    sprintf(rentText, "Sewa $%d", propertyCard->rentPrice[0]);
    printCenter(rentText, 117, 17, 29, 7);
    for (int i = 1; i <= 5; i++)
    {
      gotoxy(129, 18 + i);
      printf("%d", propertyCard->rentPrice[i]);
    }
    gotoxy(141, 25);
    printf("%d", propertyCard->upgradePrice);
    // print multiplier harga jual ambil alih dan sepihak
    gotoxy(138, 24);
    printf("%.0f%%", MULTIPLIER_RENT_COMPLETED * 100.0);
    gotoxy(135, 28);
    printf("%.0f%%", MULTIPLIER_SELL_UNILATERALLY * 100.0);
    gotoxy(135, 29);
    printf("%.0f%%/%.0f%%", MULTIPLIER_TAKE_OVER * 100.0, MULTIPLIER_TAKE_OVER_COMPLETED * 100.0);
  }
  else if (strcmp(propertyCard->propertyType, PROPERTY_STATION) == 0)
  {
    char propertyCardText[][100] = {"┌─────────────────────────────┐",
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
      printf("%s", propertyCardText[i]);
    }
    printCenter(propertyCard->name, 117, 15, 29, propertyCard->color);
    for (int i = 0; i < 4; i++)
    {
      gotoxy(131, 19 + i);
      printf("%d", propertyCard->rentPrice[i]);
    }
    // print multiplier harga jual ambil alih dan sepihak
    gotoxy(135, 26);
    printf("%.0f%%", MULTIPLIER_SELL_UNILATERALLY * 100.0);
    gotoxy(135, 27);
    printf("%.0f%%/%.0f%%", MULTIPLIER_TAKE_OVER * 100.0, MULTIPLIER_TAKE_OVER_COMPLETED * 100.0);
  }
  else if (strcmp(propertyCard->propertyType, PROPERTY_COMPANY) == 0)
  {
    char propertyCardText[][100] = {"┌─────────────────────────────┐",
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
      printf("%s", propertyCardText[i]);
    }
    printCenter(propertyCard->name, 117, 15, 29, propertyCard->color);
    // print multiplier harga jual ambil alih dan sepihak
    gotoxy(135, 28);
    printf("%.0f%%", MULTIPLIER_SELL_UNILATERALLY * 100.0);
    gotoxy(135, 29);
    printf("%.0f%%/%.0f%%", MULTIPLIER_TAKE_OVER * 100.0, MULTIPLIER_TAKE_OVER_COMPLETED * 100.0);
  }
}

int playerSelectionManager(Player p, char selections[][50], int maxIndex, int yOffset, char botContext[], PlayerProperty playerProperties[40])
{
  if (yOffset == 0)
  {
    clearPlayerSelectionText();
  }
  return p.isBot ? playerSelectionBot(p, maxIndex, yOffset, botContext, playerProperties) : playerSelectionHuman(selections, maxIndex, yOffset);
}

int playerSelectionHuman(char selections[][50], int maxIndex, int yOffset)
{
  char notSelectedStrings[10][50];
  for (int i = 0; i <= maxIndex; i++)
  {
    sprintf(notSelectedStrings[i], "  %s", selections[i]);
  }
  char selectedStrings[10][50];
  for (int i = 0; i <= maxIndex; i++)
  {
    sprintf(selectedStrings[i], "⯈ %s", selections[i]);
  }
  return selectionMenu(notSelectedStrings, selectedStrings, maxIndex, 117, 33 + yOffset);
}

int playerSelectionBot(Player p, int maxIndex, int yOffset, char botContext[], PlayerProperty playerProperties[40])
{

  int (*botLevelFunction)(Player p, int maxIndex, int yOffset, char botContext[], PlayerProperty playerProperties[40]) = NULL;
  switch (p.botDifficulty)
  {
  case 0:
    botLevelFunction = botLevelEasy;
    break;
  case 1:
    botLevelFunction = botLevelMedium;
    break;
  case 2:
    botLevelFunction = botLevelHard;
    break;
  }

  int result;
  gotoxy(117, 33 + yOffset);
  if (strcmp(botContext, "KOCOK_DADU") == 0)
  {
    result = 0;
    printf("Bot sedang mengocok dadu...");
  }
  else if (strcmp(botContext, "AKHIRI_BELI_PROPERTY") == 0)
  {
    result = botLevelFunction(p, maxIndex, yOffset, botContext, playerProperties);
    switch (result)
    {
    case 0:
      printf("Bot mengakhiri giliran");
      break;
    case 1:
      printf("Bot membeli properti");
      break;
    }
  }
  else if (strcmp(botContext, "SEWA_AMBIL_ALIH_PROPERTY") == 0)
  {
    result = botLevelFunction(p, maxIndex, yOffset, botContext, playerProperties);
    switch (result)
    {
    case 0:
      printf("Bot membayar sewa");
      break;
    case 1:
      printf("Bot membayar sewa dan mengambil alih properti");
      break;
    }
  }
  else if (strcmp(botContext, "AKHIRI_JUAL_UPGRADE_PROPERTY") == 0)
  {
    result = botLevelFunction(p, maxIndex, yOffset, botContext, playerProperties);
    switch (result)
    {
    case 0:
      printf("Bot mengakhiri giliran");
      break;
    case 1:
      printf("Bot menjual properti");
      break;
    case 2:
      printf("Bot ingin mengupgrade properti");
      break;
    }
  }
  else if (strcmp(botContext, "UPGRADE_RUMAH_PROPERTY") == 0)
  {
    result = botLevelFunction(p, maxIndex, yOffset, botContext, playerProperties);
    switch (result)
    {
    case 0:
      printf("Bot tidak jadi megupgrade properti");
      break;
    default:
      printf("Bot membeli %d rumah", result);
      break;
    }
  }
  else if (strcmp(botContext, "UPGRADE_HOTEL_PROPERTY") == 0)
  {
    int result = botLevelFunction(p, maxIndex, yOffset, botContext, playerProperties);
    switch (result)
    {
    case 0:
      printf("Bot tidak jadi megupgrade properti");
      break;
    case 1:
      printf("Bot membeli 1 hotel");
      break;
    }
  }
  else if (strcmp(botContext, "BAYAR_INCOME_TAX") == 0)
  {
    result = 0;
    printf("Bot membayar income tax");
  }
  else if (strcmp(botContext, "BAYAR_LUXURY_TAX") == 0)
  {
    result = 0;
    printf("Bot membayar luxury tax");
  }
  else if (strcmp(botContext, "BAYAR_KOCOK_KARTU_PENJARA") == 0)
  {
    result = botLevelFunction(p, maxIndex, yOffset, botContext, playerProperties);
    switch (result)
    {
    case 0:
      printf("Bot membayar $50 untuk keluar penjara");
      break;
    case 1:
      printf("Bot mengocok dadu untuk keluar penjara");
      break;
    case 2:
      printf("Bot menggunakan kartu bebas penjara");
      break;
    }
  }
  else if (strcmp(botContext, "LANJUT_KARTU_KESEMPATAN") == 0)
  {
    delay(1000);
    result = 0;
    printf("Bot melaksanakan kartu kesempatan");
  }
  else if (strcmp(botContext, "LANJUT_KARTU_DANA_UMUM") == 0)
  {
    delay(1000);
    result = 0;
    printf("Bot melaksanakan kartu dana umum");
  }
  else if (strcmp(botContext, "DOUBLE_3X_PENJARA") == 0)
  {
    result = 0;
    printf("Bot masuk penjara karena double 3x");
  }
  else if (strcmp(botContext, "BERHASIL_KELUAR_PENJARA") == 0)
  {
    result = 0;
    printf("Bot berhasil keluar dari penjara");
  }
  else if (strcmp(botContext, "GAGAL_KELUAR_PENJARA") == 0)
  {
    result = 0;
    printf("Bot gagal keluar dari penjara");
  }
  else if (strcmp(botContext, "LANJUT_BANGKRUT") == 0)
  {
    result = 0;
    printf("Bot bangkrut");
  }
  delay(BOT_DELAY);
  return result;
}

int botLevelEasy(Player p, int maxIndex, int yOffset, char botContext[], PlayerProperty playerProperties[40])
{
  int result;
  if (strcmp(botContext, "AKHIRI_BELI_PROPERTY") == 0)
  {
    int probabilities[] = {1, 9999};
    result = randFrequency(probabilities, maxIndex);
  }
  else if (strcmp(botContext, "SEWA_AMBIL_ALIH_PROPERTY") == 0)
  {
    int probabilities[] = {1, 2};
    result = randFrequency(probabilities, maxIndex);
  }
  else if (strcmp(botContext, "AKHIRI_JUAL_UPGRADE_PROPERTY") == 0)
  {
    int probabilities[] = {1, 1, 9999};
    if (p.money > plotList[p.position].propertyCard->upgradePrice)
    {
      probabilities[2] = 1;
    }
    result = randFrequency(probabilities, maxIndex);
  }
  else if (strcmp(botContext, "UPGRADE_RUMAH_PROPERTY") == 0)
  {
    int probabilities[] = {1, 9999, 1, 1, 1};
    result = randFrequency(probabilities, maxIndex);
  }
  else if (strcmp(botContext, "UPGRADE_HOTEL_PROPERTY") == 0)
  {
    int probabilities[] = {1, 1};
    int hasil = randFrequency(probabilities, maxIndex);
  }
  else if (strcmp(botContext, "BAYAR_KOCOK_KARTU_PENJARA") == 0)
  {
    int probabilities[] = {1, 1, 1};
    result = randFrequency(probabilities, maxIndex);
  }
  return result;
}
int botLevelMedium(Player p, int maxIndex, int yOffset, char botContext[], PlayerProperty playerProperties[40])
{
  int result;
  if (strcmp(botContext, "AKHIRI_BELI_PROPERTY") == 0)
  {
    int probabilities[] = {1, 9};
    result = randFrequency(probabilities, maxIndex);
  }
  else if (strcmp(botContext, "SEWA_AMBIL_ALIH_PROPERTY") == 0)
  {
    int probabilities[] = {7, 3};
    result = randFrequency(probabilities, maxIndex);
  }
  else if (strcmp(botContext, "AKHIRI_JUAL_UPGRADE_PROPERTY") == 0)
  {
    int probabilities[] = {1, 0, 0};
    if (p.money > plotList[p.position].propertyCard->upgradePrice)
    {
      probabilities[2] = 2;
    }
    result = randFrequency(probabilities, maxIndex);
  }
  else if (strcmp(botContext, "UPGRADE_RUMAH_PROPERTY") == 0)
  {
    int probabilities[] = {1, 5, 6, 4, 4};
    result = randFrequency(probabilities, maxIndex);
  }
  else if (strcmp(botContext, "UPGRADE_HOTEL_PROPERTY") == 0)
  {
    int probabilities[] = {1, 10};
    int hasil = randFrequency(probabilities, maxIndex);
  }
  else if (strcmp(botContext, "BAYAR_KOCOK_KARTU_PENJARA") == 0)
  {
    int probabilities[] = {1, 3, 999};
    result = randFrequency(probabilities, maxIndex);
  }
  return result;
}
int botLevelHard(Player p, int maxIndex, int yOffset, char botContext[], PlayerProperty playerProperties[40])
{
  int result;
  if (strcmp(botContext, "AKHIRI_BELI_PROPERTY") == 0)
  {
    PropertyCard *tempPropertyCard = plotList[p.position].propertyCard;
    int probabilities[] = {1, 1};
    if (p.money - tempPropertyCard->buyPrice > 200)
    {
      probabilities[1] += 2;
    }
    // jika uang banyak atau sudah memiliki property pada kompleks yang sama
    int totalPropertiesInCluster = totalPropertyOwnedInCluster(p, p.position);
    if (p.money > tempPropertyCard->buyPrice * 2 || totalPropertiesInCluster > 0)
    {
      probabilities[1] += 5;
    }
    if ((plotList[p.position].propertyCard->onlyTwo && totalPropertiesInCluster == 1) || (!plotList[p.position].propertyCard->onlyTwo && totalPropertiesInCluster == 2))
    {
      probabilities[1] += 10;
    }

    result = randFrequency(probabilities, maxIndex);
  }
  else if (strcmp(botContext, "SEWA_AMBIL_ALIH_PROPERTY") == 0)
  {
    int probabilities[] = {7, 3};
    // jika sudah memiliki property pada kompleks yang sama
    int totalPropertiesInCluster = totalPropertyOwnedInCluster(p, p.position);
    if (totalPropertiesInCluster > 0)
    {
      probabilities[1] = 15;
      if ((plotList[p.position].propertyCard->onlyTwo && totalPropertiesInCluster == 1) || (!plotList[p.position].propertyCard->onlyTwo && totalPropertiesInCluster == 2))
      {
        probabilities[1] = 90;
      }
    }
    result = randFrequency(probabilities, maxIndex);
  }
  else if (strcmp(botContext, "AKHIRI_JUAL_UPGRADE_PROPERTY") == 0)
  {
    PropertyCard *tempPropertyCard = plotList[p.position].propertyCard;
    int probabilities[] = {1, 0, 0};
    if (p.money > tempPropertyCard->upgradePrice)
    {
      probabilities[2] = 3;
      if (p.money > tempPropertyCard->upgradePrice * 2)
      {
        probabilities[2] = 7;
      }
    }
    result = randFrequency(probabilities, maxIndex);
  }
  else if (strcmp(botContext, "UPGRADE_RUMAH_PROPERTY") == 0)
  {
    PropertyCard *tempPropertyCard = plotList[p.position].propertyCard;
    int probabilities[] = {1, 6, 5, 4, 4};
    if (p.money - tempPropertyCard->upgradePrice * 4 > 500)
    {
      probabilities[4] = 7;
    }
    else if (p.money - tempPropertyCard->upgradePrice * 3 > 500)
    {
      probabilities[3] = 7;
    }
    else if (p.money - tempPropertyCard->upgradePrice * 2 > 500)
    {
      probabilities[2] = 7;
    }
    else if (p.money - tempPropertyCard->upgradePrice > 500)
    {
      probabilities[1] = 7;
    }
    result = randFrequency(probabilities, maxIndex);
  }
  else if (strcmp(botContext, "UPGRADE_HOTEL_PROPERTY") == 0)
  {
    int probabilities[] = {1, 10};
    result = randFrequency(probabilities, maxIndex);
  }
  else if (strcmp(botContext, "BAYAR_KOCOK_KARTU_PENJARA") == 0)
  {
    int probabilities[] = {1, 3, 999};
    if (p.money - 50 > 300)
    {
      probabilities[0] = 3;
    }
    result = randFrequency(probabilities, maxIndex);
  }
  return result;
}

int FreeParkingBotHandler(Player *p, PlayerProperty playerProperties[40])
{
  int result;
  if (p->botDifficulty == 0)
  {
    result = randomInt(0, 39);
  }
  else if (p->botDifficulty == 1)
  {
    result = randomInt(0, 39);
    while (result == 20 || result == 4, result == 30 || result == 38 || (playerProperties[result].owner != p && playerProperties[result].owner != NULL))
    {
      result = randomInt(0, 39);
    }
  }
  else if (p->botDifficulty == 2)
  {
    int plotScore[] = {1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1};
    for (int i = 0; i < 40; i++)
    {
      // kalo di property yang udah dimiliki orang lain jangan didaratin
      if (playerProperties[i].owner != p && playerProperties[i].owner != NULL)
      {
        plotScore[i] = 0;
        continue;
      }
      // kalo di property setelah GO tambah poin 1
      if (i < 20)
      {
        plotScore[i]++;
      }
      // kalo ada kartu property nya
      if (plotList[i].propertyCard != NULL)
      {
        if (plotList[i].propertyCard->propertyType == PROPERTY_CITY)
        {
          int totalPropertiesInCluster = totalPropertyOwnedInCluster(*p, i);
          // kalo kita pemiliknya dan level < 5 dan uang > harga upgrade
          if (playerProperties[i].owner == p && playerProperties[i].level < 5 && p->money > plotList[i].propertyCard->upgradePrice)
          {
            plotScore[i]++;
          }
          // kalo di property yang belum ada pemilik
          if (playerProperties[i].owner == NULL)
          {
            // kalo player punya property di kompleks itu
            if (totalPropertiesInCluster > 0)
            {
              // kalo uang > harga beli
              if (p->money > plotList[i].propertyCard->buyPrice)
              {
                // kalo property player sudah hampir lengkap
                if (((plotList[i].propertyCard->onlyTwo && totalPropertiesInCluster == 1) || (!plotList[i].propertyCard->onlyTwo && totalPropertiesInCluster == 2)))
                {
                  plotScore[i] += 5;
                }
                // kalo cuma punya aja
                else
                {
                  plotScore[i] += 2;
                }
              }
            }
          }
        }
        else if (plotList[i].propertyCard->propertyType == PROPERTY_COMPANY)
        {
          int ownedCompaniesCount = totalCompanyOwned(*p);
          // kalo perusahaan belum punya pemilik tapi player sudah memiliki perusahaan
          if (playerProperties[i].owner == NULL && ownedCompaniesCount > 0)
          {
            plotScore[i] += 5;
          }
          // jika perusahaan sudah dimiliki
          else if (playerProperties[i].owner == p)
          {
            plotScore[i] = 0;
          }
        }
        else if (plotList[i].propertyCard->propertyType == PROPERTY_STATION)
        {
          int ownedStationsCount = totalStationOwned(*p);
          // kalo stasion belum punya pemilik tapi player sudah memiliki stasion
          if (playerProperties[i].owner == NULL && totalStationOwned(*p) > 0)
          {
            plotScore[i] += 5;
          }
          // jika stasion sudah dimiliki
          else if (playerProperties[i].owner == p)
          {
            plotScore[i] = 0;
          }
        }
      }
    }
    result = randFrequency(plotScore, 39);
  }
  gotoxy(117, 33);
  printf("Bot ingin pergi ke %s", plotList[result].propertyCard != NULL ? plotList[result].propertyCard->name : plotList[result].specialName);
  delay(BOT_DELAY);
  return result;
}

void insufficientMoneyBotHandler(Player *p, PlayerProperty playerProperties[40], int totalOwnedProperties, int listOwnedProperty[28], bool selectedPositions[], int *totalMoneyFromSelectedProperties)
{
  gotoxy(117, 33);
  printf("Bot sudah memilih properti untuk dijual");
  delay(BOT_DELAY);
  for (int i = 0; i < totalOwnedProperties; i++)
  {
    selectedPositions[i] = true;
    (*totalMoneyFromSelectedProperties) += getPropertySellPrice(playerProperties[listOwnedProperty[i]], *plotList[listOwnedProperty[i]].propertyCard);
    if ((*totalMoneyFromSelectedProperties) + p->money >= 0)
    {
      break;
    }
  }
}

void clearRenderedCard()
{
  clearArea(116, 14, 156, 30);
}

void shuffleBotName(Player *players, int playerCount, int startBotIndex)
{
  char botNames[][25] = {"Melon Mask", "bread Pitt", "Coolman Paris", "Nangomong Makarim", "Mike Swarovski", "Keanu Leaves", "Ariana Venti", "Rick Pastry", "Emma What Son", "Taylor Drift", "Kanye East"};
  char difficulty[][25] = {"Easy", "Med", "Hard"};
  int nameIndexes[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  // shuffle namaIndex
  for (int i = 10; i > 0; i--)
  {
    int random = randomInt(0, i);
    int temp = nameIndexes[i];
    nameIndexes[i] = nameIndexes[random];
    nameIndexes[random] = temp;
  }
  for (int i = startBotIndex; i < playerCount; i++)
  {
    sprintf(players[i].name, "%s - %s", botNames[nameIndexes[i - startBotIndex]], difficulty[players[i].botDifficulty]);
  }
}
void shufflePlayer(Player *players, int playerCount)
{
  // metode fisher yates shuffle
  for (int i = playerCount - 1; i > 0; i--)
  {
    int random = randomInt(0, i);
    Player temp = players[i];
    players[i] = players[random];
    players[random] = temp;
  }
}
void shuffleSymbol(Player *players, int playerCount)
{
  char symbols[][3] = {"Ω", "Θ", "Δ", "Π", "Σ", "Ξ", "Φ", "Ψ"};
  int symbolIndexes[] = {0, 1, 2, 3, 4, 5, 6, 7};
  // random simbol index
  for (int i = 7; i > 0; i--)
  {
    int random = randomInt(0, i);
    int temp = symbolIndexes[i];
    symbolIndexes[i] = symbolIndexes[random];
    symbolIndexes[random] = temp;
  }
  for (int i = 0; i < playerCount; i++)
  {
    strcpy(players[i].symbol, symbols[symbolIndexes[i]]);
  }
}
