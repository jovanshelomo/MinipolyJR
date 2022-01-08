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
#define PROPERTY_KOTA "PROPERTY_KOTA"
#define PROPERTY_STASION "PROPERTY_STASION"
#define PROPERTY_PERUSAHAAN "PROPERTY_PERUSAHAAN"

#define MULTIPLIER_SEWA_LENGKAP 2       // jika pemilik sudah memiliki kota lengkap, maka multiplier sewanya menjadi 1.5x nya
#define MULTIPLIER_AMBIL_ALIH 1.5       // jika player belum memiliki kota/stasion/perusahaan lengkap, maka multiplier ini digunakan
#define MULTIPLIER_AMBIL_ALIH_LENGKAP 2 // jika player sudah memiliki kota/stasion/perusahaan lengkap, maka multiplier ini yang digunakan
#define MULTIPLIER_JUAL_SEPIHAK 0.5     // ketika menjual sepihak, maka multiplier ini yang digunakan

#define BOT_DELAY 800 // dalam ms

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
  char namaSpecial[30];                 // jika petak special, maka ada namanya, jika tidak maka nama diambil dari kartu property
  struct KartuProperty *kartuProperty;  // jika ada kartu property (bukan termasuk petak special)
} Petak;

typedef struct Player
{
  char simbol[3];              //"Ω", "Θ", "Δ", "Π", "Σ", "Ξ", "Φ", "Ψ"
  char nama[26];               // nama player
  int urutanBermain;           // urutan bermain
  bool isBot;                  // true jika player bot
  int botDifficulty;           // level bot (0-2) easy, medium, hard
  bool isBangkrut;             // true jika player sudah bangkrut
  int uang;                    // jumlah uang player
  int posisi;                  // posisi di papan (0-39)
  bool punyaKartuBebasPenjara; // true jika punya kartu bebas penjara
  int sisaTurnPenjara;         // jumlah turn yang tersisa
  int jumlahProperty;          // jumlah property yang dimiliki
  bool properties[40];         // setiap index merujuk kepada nilai kebenaran kepemilikan property pada petak index oleh player. Contoh, apabila player memiliki property di jakarta, maka properties[39] = true
} Player;

typedef struct PlayerProperty
{
  Player *pemilik; // pointer ke pemilik property
  int level;       // 0 tanpa rumah, 1 rumah, 2 rumah, 3 rumah, 4 rumah, 5 hotel
} PlayerProperty;

// list modul tidak terkait dengan game
void gotoxy(int x, int y);                                        // pergi ke koordinat x,y
void delay(int ms);                                               // melakukan delay
int randomInt(int min, int max);                                  // untuk mengenerate angka random dari min sampai max inklusif
int randFrequency(int freqArr[], int maxIndex);                   // random menggunakan frekuensi dari sebuah array
void setColor(int color);                                         // menyetel warna untuk print
void hideCursor();                                                // menyembunyikan cursor
void showCursor();                                                // menampilkan cursor
void clearArea(int xStart, int yStart, int xEnd, int yEnd);       // berfungsi untuk menghapus area di layar
int rotateIndex(int currentIndex, int maxIndex, bool isPlus);     // berfungsi untuk merotasi index jika sudah terakhir atau ke paling atas
void printTengah(char *teks, int x, int y, int lebar, int color); // untuk memprint teks di tengah
void printMultiLineRataKiri(char *teks, int x, int y, int color); // untuk print multi line teks rata kiri
int keyboardEventHandler();                                       // menghandle keyboard arrow dan enter
void waitForEnter();                                              // menunggu keyboard enter
void flushInput();                                                // membersihkan input keyboard

// list modul terkait dengan game
void attentionBeforePlay();
void playGame();
void startGame();
void howToPlay();
void quitGame();

void renderPemenang(Player p);

void startMenu();
void renderHowToPlay();
void renderLogo();
int selectionMenu(char notSelectedStrings[][50], char selectedStrings[][50], int maxIndex, int x, int y);
int renderSelectionMenu(char notSelectedStrings[][50], char selectedStrings[][50], int selectedIndex, int maxIndex, int x, int y, int terpilih);
void renderBoard();
int playerSelectionMenu(char selections[][50], int maxIndex, int yOffset); // untuk player memilih ketika sedang berada dalam in-game
int playerSelectionManager(Player p, char selections[][50], int maxIndex, int yOffset, char botContext[], PlayerProperty playerProperties[40]);
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

// player
void majuNLangkah(Player *p, int N);                                                      // berfungsi untuk memajukan player dari posisi sekarang ke posisi sekarang + N
void lompatKe(Player *p, int index);                                                      // berfungsi untuk melompatkan player tanpa berjalan memutar
void majuHingga(Player *p, int index);                                                    // berfungsi untuk memajukan player hingga posisi N
void majuHinggaJenisProperty(Player *p, char *jenis);                                     // berfungsi untuk memajukan player hingga posisi yang memiliki tipe property tertentu
void cekMenyentuhStart(Player *p);                                                        // berfungsi untuk mengecek apakah player sudah menyentuh start
void renderPosisiPlayer(Player p, int sebelum);                                           // menghapus posisi player sebelumnya dan menampilkan yang baru
void lompatKePenjara(Player *p);                                                          // berfungsi untuk melompatkan player ke penjara
bool cekUangCukup(int uang, int harga);                                                   // berfungsi untuk mengecek apakah uang cukup untuk harga yang ditentukan
void turnManager(Player *players, int *playerCount, PlayerProperty playerProperties[40]); // berfungsi untuk mengatur turn
void bangkrutkanPlayer(Player *p);                                                        // berfungsi untuk mengubah status player menjadi bangkrut

int changePlayerMoney(Player *player, int jumlahUang); // positif kalo nambah, negatif kalo kurang
void renderPlayerMoney(Player p, int color);           // ngerender jumlah uang playernya

// jual beli properti
void beliProperty(Player *p, PlayerProperty *playerProperty, int posisi);
void jualProperty(Player *p, PlayerProperty *playerProperty, int posisi);
void ambilAlihProperty(Player *p, PlayerProperty *playerProperty, int posisi);

void tambahKepemilikanProperty(Player *p, PlayerProperty *playerProperty, int posisi);
void hapusKepemilikanProperty(PlayerProperty *playerProperty, int posisi);
void PindahKepemilikanProperty(Player *pemilikBaru, PlayerProperty *playerProperty, int posisi);

void renderProperty(PlayerProperty playerProperty, int posisi);

// untuk mengecek
bool kompleksLengkap(Player p, int posisi);
int jumlahPropertyDimilikiDiKompleks(Player p, int posisi);
int jumlahStasionDimiliki(Player p);
int jumlahPerusahaanDimiliki(Player p);

// kartu kesempatan dan dana umum
bool kartuKesempatanHandler(Player players[], int giliran, int playerCount, PlayerProperty playerProperties[40]);
bool kartuDanaUmumHandler(Player players[], int giliran, int playerCount, PlayerProperty playerProperties[40]);

// render kartu kesempatan dan dana umum
void renderKartuKesempatan(char *text);
void renderKartuDanaUmum(char *text);

// render kartu property
void renderKartuProperty(KartuProperty *kartuProperty);

// clear untuk kartu property dan kartu dana umum/kesempatan
void clearKartu();

// handler di petak
void propertyHandler(Player *players, int giliran, int posisi, PlayerProperty playerProperties[40]);
bool specialHandler(Player *players, int giliran, char special[30], int playerCount, PlayerProperty playerProperties[40]);
bool penjaraHandler(Player *p);

int getHargaJualProperty(PlayerProperty playerProperty, KartuProperty kartuProperty);

// ketika uang habis, maka player harus menjual asetnya atau bangkrut
bool uangHabisHandler(Player *p, PlayerProperty playerProperties[40]);

// shuffle
void shuffleBotName(Player *players, int playerCount, int startBotIndex);
void shufflePlayer(Player *players, int playerCount);
void shuffleSymbol(Player *players, int playerCount);

// untuk pemain bot
int botHandler(Player p, int maxIndex, int yOffset, char botContext[], PlayerProperty playerProperties[40]);
int botLevelEasy(Player p, int maxIndex, int yOffset, char botContext[], PlayerProperty playerProperties[40]);
int botLevelMedium(Player p, int maxIndex, int yOffset, char botContext[], PlayerProperty playerProperties[40]);
int botLevelHard(Player p, int maxIndex, int yOffset, char botContext[], PlayerProperty playerProperties[40]);
int botParkirBebasHandler(Player *p, PlayerProperty playerProperties[40]);
void botUangHabisHandler(Player *p, PlayerProperty playerProperties[40], int jumlahPropertyDimiliki, int listPropertyDimiliki[28], bool posisiTerpilih[], int *jumlahUangHasilTerpilih);

KartuProperty kartuProp[28] = {
    {PROPERTY_KOTA, 0, "MLB", "Melbourne", 60, {2, 10, 30, 90, 160, 250}, 50, 31, true, {17, 41}, {13, 40}, {13, 41}},
    {PROPERTY_KOTA, 0, "SYD", "Sydney", 60, {4, 20, 60, 180, 320, 450}, 50, 31, true, {17, 33}, {13, 32}, {13, 33}},
    {PROPERTY_STASION, 8, "PSN", "Pasar Senen", 200, {25, 50, 100, 200}, 0, 249, false, {17, 25}, {}, {}},
    {PROPERTY_KOTA, 1, "OSA", "Osaka", 100, {6, 30, 90, 270, 400, 550}, 50, 47, false, {17, 21}, {13, 20}, {13, 21}},
    {PROPERTY_KOTA, 1, "KYO", "Kyoto", 100, {6, 30, 90, 270, 400, 550}, 50, 47, false, {17, 13}, {13, 12}, {13, 13}},
    {PROPERTY_KOTA, 1, "TYO", "Tokyo", 120, {8, 40, 100, 300, 450, 600}, 50, 47, false, {17, 9}, {13, 8}, {13, 9}},
    {PROPERTY_KOTA, 2, "GZH", "Guangzhou", 140, {10, 50, 150, 450, 625, 750}, 100, 224, false, {19, 8}, {16, 6}, {19, 6}},
    {PROPERTY_PERUSAHAAN, 9, "PLN", "Perusahaan Listrik Negara", 150, {}, 0, 249, false, {28, 8}, {}, {}},
    {PROPERTY_KOTA, 2, "BJG", "Beijing", 140, {10, 50, 150, 450, 625, 750}, 100, 224, false, {37, 8}, {34, 6}, {37, 6}},
    {PROPERTY_KOTA, 2, "SHG", "Shanghai", 160, {12, 60, 180, 500, 700, 900}, 100, 224, false, {46, 8}, {43, 6}, {46, 6}},
    {PROPERTY_STASION, 8, "GBG", "Gubeng", 200, {25, 50, 100, 200}, 0, 249, false, {55, 8}, {}, {}},
    {PROPERTY_KOTA, 3, "MSL", "Marseille", 180, {14, 70, 200, 550, 750, 950}, 100, 79, false, {64, 8}, {61, 6}, {64, 6}},
    {PROPERTY_KOTA, 3, "BDX", "Bordeaux", 180, {14, 70, 200, 550, 750, 950}, 100, 79, false, {82, 8}, {79, 6}, {82, 6}},
    {PROPERTY_KOTA, 3, "PAR", "Paris", 200, {16, 80, 220, 600, 800, 1000}, 100, 79, false, {91, 8}, {88, 6}, {91, 6}},
    {PROPERTY_KOTA, 4, "RTD", "Rotterdam", 220, {18, 90, 250, 700, 875, 1050}, 150, 95, false, {94, 9}, {97, 8}, {97, 9}},
    {PROPERTY_KOTA, 4, "DHG", "Den Haag", 220, {18, 90, 250, 700, 875, 1050}, 150, 95, false, {94, 17}, {97, 16}, {97, 17}},
    {PROPERTY_KOTA, 4, "AMS", "Amsterdam", 240, {20, 100, 300, 750, 925, 1100}, 150, 95, false, {94, 21}, {97, 20}, {97, 21}},
    {PROPERTY_STASION, 8, "STH", "ST. Hall", 200, {25, 50, 100, 200}, 0, 249, false, {94, 25}, {}, {}},
    {PROPERTY_KOTA, 5, "TPN", "Tampines", 260, {22, 110, 330, 800, 975, 1150}, 150, 143, false, {94, 29}, {97, 28}, {97, 29}},
    {PROPERTY_KOTA, 5, "JRG", "Jurong", 260, {22, 110, 330, 800, 975, 1150}, 150, 143, false, {94, 33}, {97, 32}, {97, 33}},
    {PROPERTY_PERUSAHAAN, 9, "WTR", "Perusahaan Daerah Air Minum", 150, {}, 0, 249, false, {94, 37}, {}, {}},
    {PROPERTY_KOTA, 5, "SGP", "Singapore", 280, {24, 120, 360, 850, 1025, 1200}, 150, 143, false, {94, 41}, {97, 40}, {97, 41}},
    {PROPERTY_KOTA, 6, "CCG", "Chicago", 300, {26, 130, 390, 900, 1100, 1275}, 200, 63, false, {91, 42}, {88, 44}, {91, 44}},
    {PROPERTY_KOTA, 6, "LAG", "Los Angeles", 300, {26, 130, 390, 900, 1100, 1275}, 200, 63, false, {82, 42}, {79, 44}, {82, 44}},
    {PROPERTY_KOTA, 6, "NYC", "New York", 320, {28, 150, 450, 1000, 1200, 1400}, 200, 63, false, {65, 42}, {61, 44}, {64, 44}},
    {PROPERTY_STASION, 8, "GBR", "Gambir", 200, {25, 50, 100, 200}, 0, 249, false, {55, 42}, {}},
    {PROPERTY_KOTA, 7, "BDG", "Bandung", 350, {35, 175, 500, 1100, 1300, 1500}, 200, 111, true, {37, 42}, {34, 44}, {37, 44}},
    {PROPERTY_KOTA, 7, "JKT", "Jakarta", 400, {40, 185, 550, 1200, 1500, 1700}, 200, 111, true, {19, 42}, {16, 44}, {19, 44}}};

Petak listPetak[40] = {
    {0, {6, 44}, "GO", "GO", NULL},
    {1, {5, 41}, "", "", &kartuProp[0]},
    {2, {5, 37}, "DANA_UMUM", "Dana Umum", NULL},
    {3, {5, 33}, "", "", &kartuProp[1]},
    {4, {5, 29}, "INCOME_TAX", "Income Tax", NULL},
    {5, {6, 25}, "", "", &kartuProp[2]},
    {6, {5, 21}, "", "", &kartuProp[3]},
    {7, {6, 17}, "KESEMPATAN", "Kesempatan", NULL},
    {8, {5, 13}, "", "", &kartuProp[4]},
    {9, {5, 9}, "", "", &kartuProp[5]},
    {10, {6, 4}, "MENGUNJUNGI_PENJARA", "Hanya Mengunjungi Penjara", NULL},
    {11, {18, 3}, "", "", &kartuProp[6]},
    {12, {27, 3}, "", "", &kartuProp[7]},
    {13, {36, 3}, "", "", &kartuProp[8]},
    {14, {45, 3}, "", "", &kartuProp[9]},
    {15, {54, 3}, "", "", &kartuProp[10]},
    {16, {63, 3}, "", "", &kartuProp[11]},
    {17, {72, 4}, "DANA_UMUM", "Dana Umum", NULL},
    {18, {81, 3}, "", "", &kartuProp[12]},
    {19, {90, 3}, "", "", &kartuProp[13]},
    {20, {102, 3}, "PARKIR_BEBAS", "Parkir Bebas", NULL},
    {21, {103, 9}, "", "", &kartuProp[14]},
    {22, {102, 13}, "KESEMPATAN", "Kesempatan", NULL},
    {23, {103, 17}, "", "", &kartuProp[15]},
    {24, {103, 21}, "", "", &kartuProp[16]},
    {25, {102, 25}, "", "", &kartuProp[17]},
    {26, {103, 29}, "", "", &kartuProp[18]},
    {27, {103, 33}, "", "", &kartuProp[19]},
    {28, {102, 37}, "", "", &kartuProp[20]},
    {29, {103, 41}, "", "", &kartuProp[21]},
    {30, {102, 47}, "MASUK_PENJARA", "Masuk Penjara", NULL},
    {31, {90, 47}, "", "", &kartuProp[22]},
    {32, {81, 47}, "", "", &kartuProp[23]},
    {33, {72, 47}, "DANA_UMUM", "Dana Umum", NULL},
    {34, {63, 47}, "", "", &kartuProp[24]},
    {35, {54, 47}, "", "", &kartuProp[25]},
    {36, {45, 47}, "KESEMPATAN", "Kesempatan", NULL},
    {37, {36, 47}, "", "", &kartuProp[26]},
    {38, {27, 47}, "LUXURY_TAX", "Luxury Tax", NULL},
    {39, {18, 47}, "", "", &kartuProp[27]}};

int grupkompleks[10][4] = {
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
  while (selection != 2)
  {
    renderLogo();
    char startNotSelectedMenuString[][50] = {"     START  ",
                                             "  HOW TO PLAY  ",
                                             "     QUIT  "};
    char startSelectedMenuString[][50] = {"   ⯈ START ⯇",
                                          "⯈ HOW TO PLAY ⯇",
                                          "   ⯈ QUIT ⯇"};
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
int randomInt(int min, int max)
{
  unsigned int hasil;
  errno_t err;
  err = rand_s(&hasil);
  while (err != 0)
  {
    err = rand_s(&hasil);
  }
  return (int)((double)hasil / ((double)UINT_MAX + 1) * (double)(max - min + 1)) + min;
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
void printMultiLineRataKiri(char *teks, int x, int y, int color)
{
  char *token = strtok(teks, "\n");
  int currentLine = 0;
  while (token != NULL)
  {
    gotoxy(x, y + currentLine);
    printWithColor(color, token);
    token = strtok(NULL, "\n");
    currentLine++;
  }
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
int renderSelectionMenu(char notSelectedStrings[][50], char selectedStrings[][50], int selectedIndex, int maxIndex, int x, int y, int terpilih)
{
  for (int i = 0; i <= maxIndex; i++)
  {
    gotoxy(x, y + i);
    if (i == selectedIndex)
    {
      if (terpilih)
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

Dadu kocokDadu()
{
  int randomJumlahKocok = randomInt(15, 21);
  for (int i = 0; i < randomJumlahKocok; i++)
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
  tempDadu.dadu1 = randomInt(1, 6);
  tempDadu.dadu2 = randomInt(1, 6);
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
  clearArea(0, 7, 77, 12);

  // jumlah player
  printTengah("Tentukan Jumlah Player (termasuk bot)", 7, 7, 64, 10);
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
  printTengah("Tentukan Jumlah Player Bot", 7, 7, 64, 10);
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
      printTengah(tempTextBotDifficulty, 7, 7, 64, 10);
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
      scanf("%25[^\n]", players[i - 1].nama);
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

  gotoxy(12, 10 + *playerCount - selectionBotCount);
  printf("urutan pemain:");
  for (int i = 0; i < *playerCount; i++)
  {
    gotoxy(12, 11 + *playerCount - selectionBotCount + i);
    printf("%d. (%s) %s\n", i + 1, players[i].simbol, players[i].nama);
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
    renderPosisiPlayer(players[i], 0);
  }
  turnManager(players, playerCount, playerProperties);
  system("cls");
  int pemenang;
  for (int i = 0; i < *playerCount; i++)
  {
    if (!players[i].isBangkrut)
    {
      pemenang = i;
    }
  }
  renderPemenang(players[pemenang]);
}

void renderPemenang(Player p)
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
  printTengah(p.nama, 8, 15, 104, 11);
  printTengah("Tekan ENTER untuk melanjutkan", 8, 17, 104, 7);
  waitForEnter();
  system("cls");
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

    bool turnFinished = false; // untuk menandakan apakah turn sudah selesai atau belum

    if (players[giliran].sisaTurnPenjara > 0)
    {
      // jika pemain terkirim ke penjara dan sebelumnya me roll dadu double, pemain tidak akan mendapat turn tambahan
      if (doubleCount > 0 && players[giliran].sisaTurnPenjara == 4)
      {
        doubleCount = 0;
        giliran = rotateIndex(giliran, *playerCount - 1, true);
        continue;
      }

      turnFinished = penjaraHandler(&players[giliran]);
      if (turnFinished) // kalau belum keluar dari penjara maka turn finished = true
      {
        giliran = rotateIndex(giliran, *playerCount - 1, true);
        continue;
      }
    }

    char selectionText[][50] = {"Kocok Dadu"};
    int selection = playerSelectionManager(players[giliran], selectionText, 0, 0, "KOCOK_DADU", playerProperties);

    Dadu hasilDadu = kocokDadu();
    int doubleCount = isDouble(hasilDadu) ? doubleCount + 1 : 0;

    // cek player triple double
    if (doubleCount == 3)
    {
      doubleCount = 0;
      gotoxy(117, 33);
      char teks[] = "Anda telah mendapatkan double sebanyak 3 kali.\n"
                    "Anda masuk penjara!";
      printMultiLineRataKiri(teks, 117, 33, 7);
      char selectionText[][50] = {"Lanjut"};
      int selectionIndex = playerSelectionManager(players[giliran], selectionText, 0, 2, "DOUBLE_3X_PENJARA", playerProperties);
      clearArea(117, 33, 187, 35);
      lompatKePenjara(&players[giliran]);
      giliran = rotateIndex(giliran, *playerCount - 1, true);
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
      bool playerSudahBangkrut = uangHabisHandler(&players[giliran], playerProperties);
      if (playerSudahBangkrut)
      {
        bangkrutkanPlayer(&players[giliran]);
        pemainTersisa--;
      }
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
  if (!p.isBangkrut)
  {
    printWithColor(color, "$%d", p.uang);
  }
  else
  {
    printWithColor(4, "BANGKRUT");
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
    renderPosisiPlayer(*p, posisiSebelum);
    cekMenyentuhStart(p);
  }
}
void lompatKe(Player *p, int index)
{
  int posisiSebelum = p->posisi;
  p->posisi = index;
  renderPosisiPlayer(*p, posisiSebelum);
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
      if (listPetak[p->posisi].kartuProperty->jenisProperty == jenis)
      {
        sudahSampai = true;
      }
    }
  }
}
void lompatKePenjara(Player *p)
{
  p->sisaTurnPenjara = 4;
  lompatKe(p, 10);
}
bool cekUangCukup(int uang, int harga)
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
void renderPosisiPlayer(Player p, int sebelum)
{
  // hapus hasil render player di posisi sebelumnya
  if (sebelum == 10)
  {
    // special case jika penjara
    gotoxy(8 + p.urutanBermain, 5);
    printf(" ");
    gotoxy(2 + (p.urutanBermain % 2), 4 + (p.urutanBermain > 1 ? 1 : 0));
    printf(" ");
  }
  else
  {
    gotoxy(listPetak[sebelum].lokasiPlayer.x + p.urutanBermain, listPetak[sebelum].lokasiPlayer.y);
    printf(" ");
  }

  // render posisi baru player
  if (!p.isBangkrut)
  {
    // special case jika penjara
    if (p.posisi == 10)
    {
      // cek apakah sedang dalam penjara
      if (p.sisaTurnPenjara > 0)
      {
        gotoxy(8 + p.urutanBermain, 5);
      }
      // kalau tidak
      else
      {
        gotoxy(2 + (p.urutanBermain % 2), 4 + (p.urutanBermain > 1 ? 1 : 0));
      }
    }
    else
    {
      gotoxy(listPetak[p.posisi].lokasiPlayer.x + p.urutanBermain, listPetak[p.posisi].lokasiPlayer.y);
    }
    printWithColor(240, "%s", p.simbol);
  }
}

void bangkrutkanPlayer(Player *p)
{
  p->isBangkrut = true;
  renderPlayerMoney(*p, 4);
  renderPosisiPlayer(*p, p->posisi);
}

void propertyHandler(Player *players, int giliran, int posisi, PlayerProperty playerProperties[40])
{
  KartuProperty *kartuProperty = listPetak[posisi].kartuProperty;
  renderKartuProperty(kartuProperty);
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
  if (playerProperties[posisi].pemilik == NULL)
  {
    char selectionText[][50] = {
        "Akhiri Giliran",
        "Beli Property"};
    int hargaBeliProperty = kartuProperty->hargaBeli;
    // kalau uang cukup untuk membeli properti
    sprintf(selectionText[1], "Beli Property ($%d)", kartuProperty->hargaBeli);
    bool uangCukupUntukBeli = cekUangCukup(players[giliran].uang, hargaBeliProperty);
    int selection = playerSelectionManager(players[giliran], selectionText, uangCukupUntukBeli ? 1 : 0, 0, "AKHIRI_BELI_PROPERTY", playerProperties);
    switch (selection)
    {
    case 0:
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
    int hargaSewa, hargaAmbilAlih;
    // harga sewa menjadi MULTIPLIER_SEWA_LENGKAPx jika pemain memiliki semua property pada kompleks yang sama
    //  harga ambil alih MULTIPLIER_AMBIL_ALIH_LENGKAPx lipat jika pemain memiliki semua property pada kompleks yang sama
    //  harga biaya sewa tergantung jenis property
    if (listPetak[posisi].kartuProperty->jenisProperty == PROPERTY_KOTA)
    {
      hargaSewa = kartuProperty->hargaSewa[playerProperties[posisi].level];
      if (kompleksLengkap(*playerProperties[posisi].pemilik, posisi))
      {
        hargaSewa = MULTIPLIER_SEWA_LENGKAP * hargaSewa;
        hargaAmbilAlih = MULTIPLIER_AMBIL_ALIH_LENGKAP * (kartuProperty->hargaBeli + playerProperties[posisi].level * kartuProperty->hargaUpgrade);
      }
      else
      {
        hargaAmbilAlih = MULTIPLIER_AMBIL_ALIH * (kartuProperty->hargaBeli + playerProperties[posisi].level * kartuProperty->hargaUpgrade);
      }
    }
    else if (listPetak[posisi].kartuProperty->jenisProperty == PROPERTY_STASION)
    {
      int jumlahStasion = jumlahStasionDimiliki(*playerProperties[posisi].pemilik);
      hargaSewa = kartuProperty->hargaSewa[jumlahStasion - 1];
      if (jumlahStasion == 4)
      {
        hargaAmbilAlih = MULTIPLIER_AMBIL_ALIH_LENGKAP * (kartuProperty->hargaBeli);
      }
      else
      {
        hargaAmbilAlih = MULTIPLIER_AMBIL_ALIH * (kartuProperty->hargaBeli);
      }
    }
    else if (listPetak[posisi].kartuProperty->jenisProperty == PROPERTY_PERUSAHAAN)
    {
      int jumlahPerusahaan = jumlahPerusahaanDimiliki(*playerProperties[posisi].pemilik);
      hargaAmbilAlih = MULTIPLIER_AMBIL_ALIH * (kartuProperty->hargaBeli);
      int jumlahLemparanDadu = 4;
      if (jumlahPerusahaan == 2)
      {
        jumlahLemparanDadu = 10;
        hargaAmbilAlih = MULTIPLIER_AMBIL_ALIH_LENGKAP * (kartuProperty->hargaBeli);
      }

      hargaSewa = 0;
      // roll dadu untuk mengetahui berapa harga yang harus dibayar
      int jumlahRoll = jumlahLemparanDadu / 2;
      Dadu dadu;
      for (int i = 0; i < jumlahRoll; i++)
      {
        gotoxy(117, 33);
        printf("Sedang melempar dadu sebanyak %d kali. Harga sewa sekarang: %d", jumlahLemparanDadu, hargaSewa);
        dadu = kocokDadu();
        hargaSewa += getJumlahDadu(dadu);
        delay(300);
      }
      clearArea(117, 33, 187, 33);
    }
    sprintf(selectionText[0], "Bayar Sewa ($%d)", hargaSewa);
    sprintf(selectionText[1], "Bayar Sewa dan ambil alih ($%d)", hargaSewa + hargaAmbilAlih);
    bool uangCukupUntukAmbilAlih = cekUangCukup(players[giliran].uang, hargaSewa + hargaAmbilAlih);
    int selection = playerSelectionManager(players[giliran], selectionText, uangCukupUntukAmbilAlih ? 1 : 0, 0, "SEWA_AMBIL_ALIH_PROPERTY", playerProperties);

    switch (selection)
    {
    case 0:
      changePlayerMoney(&players[giliran], -hargaSewa);
      changePlayerMoney(playerProperties[posisi].pemilik, hargaSewa);
      break;
    case 1:
      ambilAlihProperty(&players[giliran], &playerProperties[posisi], posisi);
      break;
    }
  }
  // kalau else ditambah, player tidak dapat langsung membeli rumah setelah membeli properti, harus setelah turn berikutnya
  if (playerProperties[posisi].pemilik != NULL && playerProperties[posisi].pemilik->urutanBermain == giliran)
  {
    bool pilihanSelesai = false;
    while (!pilihanSelesai)
    {
      char selectionText[][50] = {
          "Akhiri Giliran",
          "Jual property",
          "Upgrade rumah/hotel"};
      int hargaJualProperty = getHargaJualProperty(playerProperties[posisi], *kartuProperty);
      sprintf(selectionText[1], "Jual property ($%d)", hargaJualProperty);
      int bisaUpgradeLagi = playerProperties[posisi].level < 5;
      // kalau perusahaan atau stasion tidak dapat di upgrade
      // kalau level property sudah hotel tidak dapat di upgrade
      bool bisaUpgrade = kartuProperty->jenisProperty == PROPERTY_KOTA && playerProperties[posisi].level < 5;
      int selection = playerSelectionManager(players[giliran], selectionText, bisaUpgrade ? 2 : 1, 0, "AKHIRI_JUAL_UPGRADE_PROPERTY", playerProperties);
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
        if (playerProperties[posisi].level < 4)
        {
          char selectionTextUpgrade[][50] = {
              "Kembali",
              "1 Rumah",
              "2 Rumah",
              "3 Rumah",
              "4 Rumah"};
          sprintf(selectionTextUpgrade[1], "1 Rumah ($%d)", kartuProperty->hargaUpgrade);
          sprintf(selectionTextUpgrade[2], "2 Rumah ($%d)", kartuProperty->hargaUpgrade * 2);
          sprintf(selectionTextUpgrade[3], "3 Rumah ($%d)", kartuProperty->hargaUpgrade * 3);
          sprintf(selectionTextUpgrade[4], "4 Rumah ($%d)", kartuProperty->hargaUpgrade * 4);
          int maxIndexSelection = 4 - playerProperties[posisi].level;
          for (int i = maxIndexSelection; i >= 0; i--)
          {
            if (kartuProperty->hargaUpgrade * i < players[giliran].uang)
            {
              maxIndexSelection = i;
              break;
            }
          }
          int selectionUpgrade = playerSelectionManager(players[giliran], selectionTextUpgrade, maxIndexSelection, 0, "UPGRADE_RUMAH_PROPERTY", playerProperties);
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
          sprintf(selectionTextUpgrade[1], "1 Hotel ($%d)", kartuProperty->hargaUpgrade);
          int selectionUpgrade = playerSelectionManager(players[giliran], selectionTextUpgrade, 1, 0, "UPGRADE_HOTEL_PROPERTY", playerProperties);
          if (selectionUpgrade == 0)
          {
            break;
          }
          else
          {
            changePlayerMoney(&players[giliran], -kartuProperty->hargaUpgrade);
            playerProperties[posisi].level += 1;
            renderProperty(playerProperties[posisi], posisi);
            pilihanSelesai = true;
          }
        }
      }
    }
  }
}

bool kompleksLengkap(Player p, int posisi)
{
  bool punyaSemua = true;
  int kompleksId = listPetak[posisi].kartuProperty->kompleksId;
  int maxProperty = listPetak[posisi].kartuProperty->hanyaDua ? 2 : 3;
  for (int i = 0; i < maxProperty; i++)
  {
    if (!p.properties[grupkompleks[kompleksId][i]])
    {
      punyaSemua = false;
      break;
    }
  }
  return punyaSemua;
}
int jumlahPropertyDimilikiDiKompleks(Player p, int posisi)
{
  int jumlah = 0;
  int kompleksId = listPetak[posisi].kartuProperty->kompleksId;
  int maxProperty = listPetak[posisi].kartuProperty->hanyaDua ? 2 : 3;
  for (int i = 0; i < maxProperty; i++)
  {
    if (p.properties[grupkompleks[kompleksId][i]])
    {
      jumlah++;
    }
  }
  return jumlah;
}
int jumlahStasionDimiliki(Player p)
{
  int jumlah = 0;
  for (int i = 0; i < 4; i++)
  {
    if (p.properties[grupkompleks[8][i]])
    {
      jumlah++;
    }
  }
  return jumlah;
}

int jumlahPerusahaanDimiliki(Player p)
{
  int jumlah = 0;
  for (int i = 0; i < 2; i++)
  {
    if (p.properties[grupkompleks[9][i]])
    {
      jumlah++;
    }
  }
  return jumlah;
}

void beliProperty(Player *p, PlayerProperty *playerProperty, int posisi)
{
  KartuProperty *kartuProperty = listPetak[posisi].kartuProperty;
  int hargaBeliProperty = kartuProperty->hargaBeli;
  changePlayerMoney(p, -hargaBeliProperty);
  tambahKepemilikanProperty(p, playerProperty, posisi);
}

void jualProperty(Player *p, PlayerProperty *playerProperty, int posisi)
{
  KartuProperty *kartuProperty = listPetak[posisi].kartuProperty;
  int hargaJualProperty = MULTIPLIER_JUAL_SEPIHAK * (kartuProperty->hargaBeli + playerProperty->level * kartuProperty->hargaUpgrade);
  changePlayerMoney(p, hargaJualProperty);
  hapusKepemilikanProperty(playerProperty, posisi);
}

void ambilAlihProperty(Player *p, PlayerProperty *playerProperty, int posisi)
{
  KartuProperty *kartuProperty = listPetak[posisi].kartuProperty;
  int hargaAmbilAlih = MULTIPLIER_AMBIL_ALIH * (kartuProperty->hargaBeli + playerProperty->level * kartuProperty->hargaUpgrade);
  changePlayerMoney(p, -hargaAmbilAlih);
  changePlayerMoney(playerProperty->pemilik, hargaAmbilAlih);
  PindahKepemilikanProperty(p, playerProperty, posisi);
}

void tambahKepemilikanProperty(Player *p, PlayerProperty *playerProperty, int posisi)
{
  playerProperty->pemilik = p;
  playerProperty->level = 0;
  p->properties[posisi] = true;
  renderProperty(*playerProperty, posisi);
}

void hapusKepemilikanProperty(PlayerProperty *playerProperty, int posisi)
{
  playerProperty->pemilik->properties[posisi] = false;
  playerProperty->pemilik = NULL;
  playerProperty->level = 0;
  renderProperty(*playerProperty, posisi);
}
void PindahKepemilikanProperty(Player *pemilikBaru, PlayerProperty *playerProperty, int posisi)
{
  playerProperty->pemilik->properties[posisi] = false;
  pemilikBaru->properties[posisi] = true;
  playerProperty->pemilik = pemilikBaru;
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
  // cek apakah property merupakan kota atau bukan, kalau iya render rumah, kalau tidak jangan
  if (tempKartuProperty->jenisProperty == PROPERTY_KOTA)
  {
    gotoxy(tempKartuProperty->lokasiRumah.x, tempKartuProperty->lokasiRumah.y);
    if (playerProperty.level == 0)
    {
      printWithColor(tempKartuProperty->warna, "  ");
    }
    else if (playerProperty.level <= 4)
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
    int selection = playerSelectionManager(players[giliran], selectionText, 0, 0, "BAYAR_INCOME_TAX", playerProperties);
    if (selection == 0)
    {
      changePlayerMoney(&players[giliran], -200);
    }
    return true;
  }
  else if (strcmp(special, "LUXURY_TAX") == 0)
  {
    char selectionText[][50] = {"Bayar Luxury Tax $75"};
    int selection = playerSelectionManager(players[giliran], selectionText, 0, 0, "BAYAR_LUXURY_TAX", playerProperties);
    if (selection == 0)
    {
      changePlayerMoney(&players[giliran], -75);
    }
    return true;
  }
  else if (strcmp(special, "PARKIR_BEBAS") == 0)
  {
    int posisiDipilih;
    if (players[giliran].isBot)
    {
      // bot
      posisiDipilih = botParkirBebasHandler(&players[giliran], playerProperties);
    }
    else
    {
      // player manusia
      posisiDipilih = 20;
      clearPlayerSelectionText();
      gotoxy(117, 33);
      printf("Gunakan ↑ dan ↓ pada keyboard untuk memilih lokasi tujuan.");
      bool isSelected = false;
      gotoxy(listPetak[posisiDipilih].lokasiPlayer.x, listPetak[posisiDipilih].lokasiPlayer.y);
      showCursor();
      while (!isSelected)
      {
        int keyboardResult = keyboardEventHandler();
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
          clearArea(117, 33, 187, 33);
        }
        gotoxy(listPetak[posisiDipilih].lokasiPlayer.x, listPetak[posisiDipilih].lokasiPlayer.y);
      }
    }
    majuHingga(&players[giliran], posisiDipilih);
    return false;
  }
  else if (strcmp(special, "MASUK_PENJARA") == 0)
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
  char selectionText[][50] = {"Bayar $50 langsung keluar penjara",
                              "Kocok dadu (jika double keluar penjara)",
                              "Gunakan kartu bebas penjara"};
  int selection = playerSelectionManager(*p, selectionText, p->sisaTurnPenjara == 1 ? 0 : p->punyaKartuBebasPenjara ? 2
                                                                                                                    : 1,
                                         0,
                                         "BAYAR_KOCOK_KARTU_PENJARA", NULL);
  switch (selection)
  {
  case 0:
    changePlayerMoney(p, -50);
    p->sisaTurnPenjara = 0;
    return false;
    break;
  case 1:
    if (isDouble(kocokDadu()))
    {
      p->sisaTurnPenjara = 0;
      clearPlayerSelectionText();
      char teks[] = "Anda berhasil keluar dari penjara.";
      printMultiLineRataKiri(teks, 117, 33, 7);
      char selectionText[][50] = {"Lanjut"};
      int selection = playerSelectionManager(*p, selectionText, 0, 1, "BERHASIL_KELUAR_PENJARA", NULL);
      clearPlayerSelectionText();
      return false;
    }
    else
    {
      p->sisaTurnPenjara = p->sisaTurnPenjara - 1;
      clearPlayerSelectionText();
      char teks[] = "Anda gagal keluar dari penjara.";
      printMultiLineRataKiri(teks, 117, 33, 7);
      char selectionText[][50] = {"Lanjut"};
      int selection = playerSelectionManager(*p, selectionText, 0, 1, "GAGAL_KELUAR_PENJARA", NULL);
      clearPlayerSelectionText();
      return true;
    }
    break;
  case 2:
    p->punyaKartuBebasPenjara = false;
    p->sisaTurnPenjara = 0;
    return false;
    break;
  }
}

int getHargaJualProperty(PlayerProperty playerProperty, KartuProperty kartuProperty)
{
  return MULTIPLIER_JUAL_SEPIHAK * (kartuProperty.hargaBeli + playerProperty.level * kartuProperty.hargaUpgrade);
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
  int random = randomInt(0, 15);
  // render kartu
  renderKartuKesempatan(kesempatan[random]);

  char selectionText[][50] = {"Lanjut"};
  int selection = playerSelectionManager(players[giliran], selectionText, 0, 0, "LANJUT_KARTU_KESEMPATAN", playerProperties);
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
        if (playerProperties[i].level > 0)
        {
          if (playerProperties[i].level < 5)
          {
            changePlayerMoney(&players[giliran], -25 * playerProperties[i].level);
          }
          else
          {
            changePlayerMoney(&players[giliran], -100);
          }
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
  int random = randomInt(0, 15);
  // render kartu
  renderKartuDanaUmum(danaUmum[random]);

  char selectionText[][50] = {"Lanjut"};
  int selection = playerSelectionManager(players[giliran], selectionText, 0, 0, "LANJUT_KARTU_DANA_UMUM", playerProperties);

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
        if (playerProperties[i].level > 0)
        {
          if (playerProperties[i].level < 5)
          {
            changePlayerMoney(&players[giliran], -40 * playerProperties[i].level);
          }
          else
          {
            changePlayerMoney(&players[giliran], -115);
          }
        }
      }
    }
    break;
  case 8:
    changePlayerMoney(&players[giliran], 200);
    break;
  case 9:
    majuHingga(&players[giliran], 0);
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

// return true jika player dinyatakan bangkrut(tidak dapat membayar hingga uangnya >= 0)
bool uangHabisHandler(Player *p, PlayerProperty playerProperties[40])
{
  // pertama, semua property milik pemain tersebut akan di list
  // jika pemain memiliki 0 peroperti, maka langsung bangkrut
  // jika pemain memiliki setidaknya 1 properti dan jumlah harga seluruh propertinya cukup untuk membayar, maka dia dapat memilih properti yang ingin dijual
  // juka pemain memiliki properti dan jumlah semua propertinya tidak cukup untuk membayar, maka pemain bangkrut
  int jumlahPropertyDimiliki = 0;
  int totalHargaProperty = 0;
  int listPropertyDimiliki[28];

  bool posisiTerpilih[28];
  int jumlahUangHasilTerpilih = 0;
  for (int i = 0; i < 28; i++)
  {
    posisiTerpilih[i] = false;
  }

  // semua property milik pemain tersebut di list
  for (int i = 0; i < 40; i++)
  {
    if (p->properties[i])
    {
      totalHargaProperty += getHargaJualProperty(playerProperties[i], *listPetak[i].kartuProperty);
      listPropertyDimiliki[jumlahPropertyDimiliki] = i;
      jumlahPropertyDimiliki++;
    }
  }

  if (jumlahPropertyDimiliki == 0)
  {
    clearPlayerSelectionText();
    char teks[] = "Anda kehabisan uang dan Anda tidak memiliki property.\n"
                  "Anda dinyatakan bangkrut.";
    printMultiLineRataKiri(teks, 117, 33, 7);
    char selectionText[][50] = {"Lanjut"};
    int selection = playerSelectionManager(*p, selectionText, 0, 2, "LANJUT_BANGKRUT", playerProperties);
    clearPlayerSelectionText();
    return true;
  }
  else if (totalHargaProperty + p->uang < 0)
  {
    clearPlayerSelectionText();
    char teks[190];
    sprintf(teks, "Anda kehabisan uang dan total harga jual property Anda\n"
                  "sebesar $%d tidak cukup untuk menutupi kekurangan\n"
                  "uang Anda. Anda dinyatakan bangkrut.",
            totalHargaProperty);
    printMultiLineRataKiri(teks, 117, 33, 7);
    char selectionText[][50] = {"Lanjut"};
    int selection = playerSelectionManager(*p, selectionText, 0, 3, "LANJUT_BANGKRUT", playerProperties);
    for (int i = 0; i < jumlahPropertyDimiliki; i++)
    {
      hapusKepemilikanProperty(&playerProperties[listPropertyDimiliki[i]], listPropertyDimiliki[i]);
    }
    clearPlayerSelectionText();
    return true;
  }
  else
  {
    if (p->isBot)
    {
      botUangHabisHandler(p, playerProperties, jumlahPropertyDimiliki, listPropertyDimiliki, posisiTerpilih, &jumlahUangHasilTerpilih);
    }
    else
    {
      clearPlayerSelectionText();
      char teks[] = "Anda kehabisan uang dan harus menjual property-property milik Anda.\n"
                    "Gunakan ↑ dan ↓ pada keyboard untuk memilih property yang akan dijual.\n"
                    "Tekan SPASI untuk menselect property yang akan dijual\n"
                    "Tekan ENTER untuk menjual semua property yang sudah dipilih";
      printMultiLineRataKiri(teks, 117, 33, 7);
      int indexPosisiSekarang = 0;

      gotoxy(117, 37);
      printf("Akumulasi harga jual: %d", jumlahUangHasilTerpilih);
      bool pilihanSelesai = false;
      gotoxy(listPetak[listPropertyDimiliki[indexPosisiSekarang]].lokasiPlayer.x, listPetak[listPropertyDimiliki[indexPosisiSekarang]].lokasiPlayer.y);
      showCursor();
      while (!pilihanSelesai)
      {
        int keyboardResult = keyboardEventHandler();
        if (keyboardResult == ARROW_UP)
        {
          indexPosisiSekarang = rotateIndex(indexPosisiSekarang, jumlahPropertyDimiliki - 1, true);
          clearKartu();
          hideCursor();
          renderKartuProperty(listPetak[listPropertyDimiliki[indexPosisiSekarang]].kartuProperty);
        }
        else if (keyboardResult == ARROW_DOWN)
        {
          indexPosisiSekarang = rotateIndex(indexPosisiSekarang, jumlahPropertyDimiliki - 1, false);
          clearKartu();
          hideCursor();
          renderKartuProperty(listPetak[listPropertyDimiliki[indexPosisiSekarang]].kartuProperty);
        }
        else if (keyboardResult == KEY_SPACE)
        {
          posisiTerpilih[indexPosisiSekarang] = !posisiTerpilih[indexPosisiSekarang];
          int hargaJualProperty = getHargaJualProperty(playerProperties[listPropertyDimiliki[indexPosisiSekarang]], *listPetak[listPropertyDimiliki[indexPosisiSekarang]].kartuProperty);
          if (posisiTerpilih[indexPosisiSekarang])
          {
            jumlahUangHasilTerpilih += hargaJualProperty;
          }
          else
          {
            jumlahUangHasilTerpilih -= hargaJualProperty;
          }
          // menjadikan posisiKepemilikan menjadi warna biru jika terpilih (warna 23)
          showCursor();
          gotoxy(listPetak[listPropertyDimiliki[indexPosisiSekarang]].kartuProperty->tandaKepemilikan.x, listPetak[listPropertyDimiliki[indexPosisiSekarang]].kartuProperty->tandaKepemilikan.y);
          printWithColor(posisiTerpilih[indexPosisiSekarang] ? 23 : 7, "%s", p->simbol);
          clearArea(117, 37, 187, 37);
          gotoxy(117, 37);
          printf("Akumulasi harga jual: %d", jumlahUangHasilTerpilih);
        }
        else if (keyboardResult == KEY_ENTER)
        {
          if (jumlahUangHasilTerpilih + p->uang < 0)
          {
            clearArea(117, 37, 187, 37);
            gotoxy(117, 37);
            printWithColor(12, "property yang dipilih belum cukup untuk menutupi kekurangan uang!");
            delay(1000);
            clearArea(117, 37, 187, 37);
            gotoxy(117, 37);
            printf("Akumulasi harga jual: %d", jumlahUangHasilTerpilih);
          }
          else
          {
            pilihanSelesai = true;
          }
        }
        gotoxy(listPetak[listPropertyDimiliki[indexPosisiSekarang]].lokasiPlayer.x, listPetak[listPropertyDimiliki[indexPosisiSekarang]].lokasiPlayer.y);
      }
    }
    hideCursor();
    clearArea(117, 33, 187, 37);
    for (int i = 0; i < 28; i++)
    {
      if (posisiTerpilih[i])
      {
        hapusKepemilikanProperty(&playerProperties[listPropertyDimiliki[i]], listPropertyDimiliki[i]);
      }
    }
    changePlayerMoney(p, jumlahUangHasilTerpilih);

    return false;
  }
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
    gotoxy(138, 24);
    printf("%.0f%%", MULTIPLIER_SEWA_LENGKAP * 100.0);
    gotoxy(135, 28);
    printf("%.0f%%", MULTIPLIER_JUAL_SEPIHAK * 100.0);
    gotoxy(135, 29);
    printf("%.0f%%/%.0f%%", MULTIPLIER_AMBIL_ALIH * 100.0, MULTIPLIER_AMBIL_ALIH_LENGKAP * 100.0);
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
    printf("%.0f%%/%.0f%%", MULTIPLIER_AMBIL_ALIH * 100.0, MULTIPLIER_AMBIL_ALIH_LENGKAP * 100.0);
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
    printf("%.0f%%/%.0f%%", MULTIPLIER_AMBIL_ALIH * 100.0, MULTIPLIER_AMBIL_ALIH_LENGKAP * 100.0);
  }
}

int playerSelectionManager(Player p, char selections[][50], int maxIndex, int yOffset, char botContext[], PlayerProperty playerProperties[40])
{
  if (yOffset == 0)
  {
    clearPlayerSelectionText();
  }
  return p.isBot ? botHandler(p, maxIndex, yOffset, botContext, playerProperties) : playerSelectionMenu(selections, maxIndex, yOffset);
}

int playerSelectionMenu(char selections[][50], int maxIndex, int yOffset)
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

int botHandler(Player p, int maxIndex, int yOffset, char botContext[], PlayerProperty playerProperties[40])
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

  int hasil;
  gotoxy(117, 33 + yOffset);
  if (strcmp(botContext, "KOCOK_DADU") == 0)
  {
    hasil = 0;
    printf("Bot sedang mengocok dadu...");
  }
  else if (strcmp(botContext, "AKHIRI_BELI_PROPERTY") == 0)
  {
    hasil = botLevelFunction(p, maxIndex, yOffset, botContext, playerProperties);
    switch (hasil)
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
    hasil = botLevelFunction(p, maxIndex, yOffset, botContext, playerProperties);
    switch (hasil)
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
    hasil = botLevelFunction(p, maxIndex, yOffset, botContext, playerProperties);
    switch (hasil)
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
    hasil = botLevelFunction(p, maxIndex, yOffset, botContext, playerProperties);
    switch (hasil)
    {
    case 0:
      printf("Bot tidak jadi megupgrade properti");
      break;
    default:
      printf("Bot membeli %d rumah", hasil);
      break;
    }
  }
  else if (strcmp(botContext, "UPGRADE_HOTEL_PROPERTY") == 0)
  {
    int hasil = botLevelFunction(p, maxIndex, yOffset, botContext, playerProperties);
    switch (hasil)
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
    hasil = 0;
    printf("Bot membayar income tax");
  }
  else if (strcmp(botContext, "BAYAR_LUXURY_TAX") == 0)
  {
    hasil = 0;
    printf("Bot membayar luxury tax");
  }
  else if (strcmp(botContext, "BAYAR_KOCOK_KARTU_PENJARA") == 0)
  {
    hasil = botLevelFunction(p, maxIndex, yOffset, botContext, playerProperties);
    switch (hasil)
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
    hasil = 0;
    printf("Bot melaksanakan kartu kesempatan");
  }
  else if (strcmp(botContext, "LANJUT_KARTU_DANA_UMUM") == 0)
  {
    delay(1000);
    hasil = 0;
    printf("Bot melaksanakan kartu dana umum");
  }
  else if (strcmp(botContext, "DOUBLE_3X_PENJARA") == 0)
  {
    hasil = 0;
    printf("Bot masuk penjara karena double 3x");
  }
  else if (strcmp(botContext, "BERHASIL_KELUAR_PENJARA") == 0)
  {
    hasil = 0;
    printf("Bot berhasil keluar dari penjara");
  }
  else if (strcmp(botContext, "GAGAL_KELUAR_PENJARA") == 0)
  {
    hasil = 0;
    printf("Bot gagal keluar dari penjara");
  }
  else if (strcmp(botContext, "LANJUT_BANGKRUT") == 0)
  {
    hasil = 0;
    printf("Bot bangkrut");
  }
  delay(BOT_DELAY);
  return hasil;
}

int botLevelEasy(Player p, int maxIndex, int yOffset, char botContext[], PlayerProperty playerProperties[40])
{
  int hasil;
  if (strcmp(botContext, "AKHIRI_BELI_PROPERTY") == 0)
  {
    int probabilities[] = {2, 1};
    hasil = randFrequency(probabilities, maxIndex);
  }
  else if (strcmp(botContext, "SEWA_AMBIL_ALIH_PROPERTY") == 0)
  {
    int probabilities[] = {1, 0};
    hasil = randFrequency(probabilities, maxIndex);
  }
  else if (strcmp(botContext, "AKHIRI_JUAL_UPGRADE_PROPERTY") == 0)
  {
    int probabilities[] = {1, 1, 0};
    hasil = randFrequency(probabilities, maxIndex);
  }
  else if (strcmp(botContext, "UPGRADE_RUMAH_PROPERTY") == 0)
  {
    int probabilities[] = {1, 0, 0, 0, 0};
    hasil = randFrequency(probabilities, maxIndex);
  }
  else if (strcmp(botContext, "UPGRADE_HOTEL_PROPERTY") == 0)
  {
    int probabilities[] = {1, 0};
    int hasil = randFrequency(probabilities, maxIndex);
  }
  else if (strcmp(botContext, "BAYAR_KOCOK_KARTU_PENJARA") == 0)
  {
    int probabilities[] = {1, 1, 1};
    hasil = randFrequency(probabilities, maxIndex);
  }
  return hasil;
}
int botLevelMedium(Player p, int maxIndex, int yOffset, char botContext[], PlayerProperty playerProperties[40])
{
  int hasil;
  if (strcmp(botContext, "AKHIRI_BELI_PROPERTY") == 0)
  {
    int probabilities[] = {1, 9};
    hasil = randFrequency(probabilities, maxIndex);
  }
  else if (strcmp(botContext, "SEWA_AMBIL_ALIH_PROPERTY") == 0)
  {
    int probabilities[] = {7, 3};
    hasil = randFrequency(probabilities, maxIndex);
  }
  else if (strcmp(botContext, "AKHIRI_JUAL_UPGRADE_PROPERTY") == 0)
  {
    int probabilities[] = {1, 0, 0};
    if (p.uang > listPetak[p.posisi].kartuProperty->hargaUpgrade)
    {
      probabilities[2] = 2;
    }
    hasil = randFrequency(probabilities, maxIndex);
  }
  else if (strcmp(botContext, "UPGRADE_RUMAH_PROPERTY") == 0)
  {
    int probabilities[] = {1, 5, 6, 4, 4};
    hasil = randFrequency(probabilities, maxIndex);
  }
  else if (strcmp(botContext, "UPGRADE_HOTEL_PROPERTY") == 0)
  {
    int probabilities[] = {1, 10};
    int hasil = randFrequency(probabilities, maxIndex);
  }
  else if (strcmp(botContext, "BAYAR_KOCOK_KARTU_PENJARA") == 0)
  {
    int probabilities[] = {1, 3, 999};
    hasil = randFrequency(probabilities, maxIndex);
  }
  return hasil;
}
int botLevelHard(Player p, int maxIndex, int yOffset, char botContext[], PlayerProperty playerProperties[40])
{
  int hasil;
  if (strcmp(botContext, "AKHIRI_BELI_PROPERTY") == 0)
  {
    KartuProperty *tempKartuProperty = listPetak[p.posisi].kartuProperty;
    int probabilities[] = {1, 1};
    if (p.uang - tempKartuProperty->hargaBeli > 200)
    {
      probabilities[1] += 2;
    }
    // jika uang banyak atau sudah memiliki property pada kompleks yang sama
    int jumlahPropertyDiKompleks = jumlahPropertyDimilikiDiKompleks(p, p.posisi);
    if (p.uang > tempKartuProperty->hargaBeli * 2 || jumlahPropertyDiKompleks > 0)
    {
      probabilities[1] += 5;
    }
    if ((listPetak[p.posisi].kartuProperty->hanyaDua && jumlahPropertyDiKompleks == 1) || (!listPetak[p.posisi].kartuProperty->hanyaDua && jumlahPropertyDiKompleks == 2))
    {
      probabilities[1] += 10;
    }

    hasil = randFrequency(probabilities, maxIndex);
  }
  else if (strcmp(botContext, "SEWA_AMBIL_ALIH_PROPERTY") == 0)
  {
    int probabilities[] = {7, 3};
    // jika sudah memiliki property pada kompleks yang sama
    int jumlahPropertyDiKompleks = jumlahPropertyDimilikiDiKompleks(p, p.posisi);
    if (jumlahPropertyDiKompleks > 0)
    {
      probabilities[1] = 15;
      if ((listPetak[p.posisi].kartuProperty->hanyaDua && jumlahPropertyDiKompleks == 1) || (!listPetak[p.posisi].kartuProperty->hanyaDua && jumlahPropertyDiKompleks == 2))
      {
        probabilities[1] = 90;
      }
    }
    hasil = randFrequency(probabilities, maxIndex);
  }
  else if (strcmp(botContext, "AKHIRI_JUAL_UPGRADE_PROPERTY") == 0)
  {
    KartuProperty *tempKartuProperty = listPetak[p.posisi].kartuProperty;
    int probabilities[] = {1, 0, 0};
    if (p.uang > tempKartuProperty->hargaUpgrade)
    {
      probabilities[2] = 3;
      if (p.uang > tempKartuProperty->hargaUpgrade * 2)
      {
        probabilities[2] = 7;
      }
    }
    hasil = randFrequency(probabilities, maxIndex);
  }
  else if (strcmp(botContext, "UPGRADE_RUMAH_PROPERTY") == 0)
  {
    KartuProperty *tempKartuProperty = listPetak[p.posisi].kartuProperty;
    int probabilities[] = {1, 6, 5, 4, 4};
    if (p.uang - tempKartuProperty->hargaUpgrade * 4 > 500)
    {
      probabilities[4] = 7;
    }
    else if (p.uang - tempKartuProperty->hargaUpgrade * 3 > 500)
    {
      probabilities[3] = 7;
    }
    else if (p.uang - tempKartuProperty->hargaUpgrade * 2 > 500)
    {
      probabilities[2] = 7;
    }
    else if (p.uang - tempKartuProperty->hargaUpgrade > 500)
    {
      probabilities[1] = 7;
    }
    hasil = randFrequency(probabilities, maxIndex);
  }
  else if (strcmp(botContext, "UPGRADE_HOTEL_PROPERTY") == 0)
  {
    int probabilities[] = {1, 10};
    int hasil = randFrequency(probabilities, maxIndex);
  }
  else if (strcmp(botContext, "BAYAR_KOCOK_KARTU_PENJARA") == 0)
  {
    int probabilities[] = {1, 3, 999};
    if (p.uang - 50 > 300)
    {
      probabilities[0] = 3;
    }
    hasil = randFrequency(probabilities, maxIndex);
  }
  return hasil;
}

int botParkirBebasHandler(Player *p, PlayerProperty playerProperties[40])
{
  int hasil;
  if (p->botDifficulty == 0)
  {
    hasil = randomInt(0, 39);
  }
  else if (p->botDifficulty == 1)
  {
    hasil = randomInt(0, 39);
    while (hasil == 20 || hasil == 4, hasil == 30 || hasil == 38 || (playerProperties[hasil].pemilik != p && playerProperties[hasil].pemilik != NULL))
    {
      hasil = randomInt(0, 39);
    }
  }
  else if (p->botDifficulty == 2)
  {
    int petakScore[] = {1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1};
    for (int i = 0; i < 40; i++)
    {
      // kalo di property yang udah dimiliki orang lain jangan didaratin
      if (playerProperties[i].pemilik != p && playerProperties[i].pemilik != NULL)
      {
        petakScore[i] = 0;
        continue;
      }
      // kalo di property setelah GO tambah poin 1
      if (i < 20)
      {
        petakScore[i]++;
      }
      // kalo ada kartu property nya
      if (listPetak[i].kartuProperty != NULL)
      {
        if (listPetak[i].kartuProperty->jenisProperty == PROPERTY_KOTA)
        {
          int jumlahPropertyDiKompleks = jumlahPropertyDimilikiDiKompleks(*p, i);
          // kalo kita pemiliknya dan level < 5 dan uang > harga upgrade
          if (playerProperties[i].pemilik == p && playerProperties[i].level < 5 && p->uang > listPetak[i].kartuProperty->hargaUpgrade)
          {
            petakScore[i]++;
          }
          // kalo di property yang belum ada pemilik
          if (playerProperties[i].pemilik == NULL)
          {
            // kalo player punya property di kompleks itu
            if (jumlahPropertyDiKompleks > 0)
            {
              // kalo uang > harga beli
              if (p->uang > listPetak[i].kartuProperty->hargaBeli)
              {
                // kalo property player sudah hampir lengkap
                if (((listPetak[i].kartuProperty->hanyaDua && jumlahPropertyDiKompleks == 1) || (!listPetak[i].kartuProperty->hanyaDua && jumlahPropertyDiKompleks == 2)))
                {
                  petakScore[i] += 5;
                }
                // kalo cuma punya aja
                else
                {
                  petakScore[i] += 2;
                }
              }
            }
          }
        }
        else if (listPetak[i].kartuProperty->jenisProperty == PROPERTY_PERUSAHAAN)
        {
          int jumlahPerusahaan = jumlahPerusahaanDimiliki(*p);
          // kalo perusahaan belum punya pemilik tapi player sudah memiliki perusahaan
          if (playerProperties[i].pemilik == NULL && jumlahPerusahaan > 0)
          {
            petakScore[i] += 5;
          }
          // jika perusahaan sudah dimiliki
          else if (playerProperties[i].pemilik == p)
          {
            petakScore[i] = 0;
          }
        }
        else if (listPetak[i].kartuProperty->jenisProperty == PROPERTY_STASION)
        {
          int jumlahStasion = jumlahStasionDimiliki(*p);
          // kalo stasion belum punya pemilik tapi player sudah memiliki stasion
          if (playerProperties[i].pemilik == NULL && jumlahStasionDimiliki(*p) > 0)
          {
            petakScore[i] += 5;
          }
          // jika stasion sudah dimiliki
          else if (playerProperties[i].pemilik == p)
          {
            petakScore[i] = 0;
          }
        }
      }
    }
    hasil = randFrequency(petakScore, 39);
  }
  gotoxy(117, 33);
  printf("Bot ingin pergi ke %s", listPetak[hasil].kartuProperty != NULL ? listPetak[hasil].kartuProperty->namaKota : listPetak[hasil].namaSpecial);
  delay(BOT_DELAY);
  return hasil;
}

void botUangHabisHandler(Player *p, PlayerProperty playerProperties[40], int jumlahPropertyDimiliki, int listPropertyDimiliki[28], bool posisiTerpilih[], int *jumlahUangHasilTerpilih)
{
  gotoxy(117, 33);
  printf("Bot sudah memilih properti untuk dijual");
  delay(BOT_DELAY);
  for (int i = 0; i < jumlahPropertyDimiliki; i++)
  {
    posisiTerpilih[i] = true;
    (*jumlahUangHasilTerpilih) += getHargaJualProperty(playerProperties[listPropertyDimiliki[i]], *listPetak[listPropertyDimiliki[i]].kartuProperty);
    if ((*jumlahUangHasilTerpilih) + p->uang >= 0)
    {
      break;
    }
  }
}

void clearKartu()
{
  clearArea(116, 14, 156, 30);
}

void shuffleBotName(Player *players, int playerCount, int startBotIndex)
{
  char namaBot[][25] = {"Melon Mask", "bread Pitt", "Coolman Paris", "Nangomong Makarim", "Mike Swarovski", "Keanu Leaves", "Ariana Venti", "Rick Pastry", "Emma What Son", "Taylor Drift", "Kanye East"};
  char difficulty[][25] = {"Easy", "Med", "Hard"};
  int namaIndex[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  // shuffle namaIndex
  for (int i = 10; i > 0; i--)
  {
    int random = randomInt(0, i);
    int temp = namaIndex[i];
    namaIndex[i] = namaIndex[random];
    namaIndex[random] = temp;
  }
  for (int i = startBotIndex; i < playerCount; i++)
  {
    sprintf(players[i].nama, "%s - %s", namaBot[namaIndex[i - startBotIndex]], difficulty[players[i].botDifficulty]);
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
  char simbol[][3] = {"Ω", "Θ", "Δ", "Π", "Σ", "Ξ", "Φ", "Ψ"};
  int simbolIndex[] = {0, 1, 2, 3, 4, 5, 6, 7};
  // random simbol index
  for (int i = 7; i > 0; i--)
  {
    int random = randomInt(0, i);
    int temp = simbolIndex[i];
    simbolIndex[i] = simbolIndex[random];
    simbolIndex[random] = temp;
  }
  for (int i = 0; i < playerCount; i++)
  {
    strcpy(players[i].simbol, simbol[simbolIndex[i]]);
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