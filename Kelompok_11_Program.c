#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Kapasitas maksimal array dan nama file //
#define MAX_PELANGGAN  200
#define FILE_DATA_AWAL "Kelompok_11_Data.txt"
#define FILE_SIMPAN    "data_pelanggan.txt"

// Struct menyimpan satu data pelanggan //
typedef struct {
    char  idPelanggan[15], namaPelanggan[70], jenisPaket[30], statusPembayaran[15];
    float tagihanBulanan, tunggakan;
    int   bulanTertunggak;
} Pelanggan;

// Array global penampung semua pelanggan //
Pelanggan daftar[MAX_PELANGGAN];
int jumlah = 0;

// Daftar nama ISP dan tarif bulanannya //
static const char  *ISP_NAME[]  = {"Citranet","MyRepublik","Biznet","Indihome","Oxygen","XLHome","Firts Media","Mega Data"};
static const float  ISP_HARGA[] = {200000,250000,350000,300000,180000,220000,400000,150000};

// Prototipe semua fungsi //
void  tampilMenu(void), tambahData(void), tampilData(void), editData(void);
void  hapusData(void),  cariData(void),   urutkanData(void), laporanMenunggak(void);
void  simpanFile(void), bacaFileSimpan(void), bacaDataAwal(void), tampilMenuISP(void);
float hitungTagihan(const char *isp);
int   cariIndex(const char *id);
void  garis(int n), bersihBuffer(void), trim(char *s);

// MAIN — titik masuk program //
// Urutan: coba baca simpanan lama -> jika kosong baca file awal //
// loop menu sampai user memilih keluar (0) //
int main(void) {
    bacaFileSimpan();
    if (jumlah == 0) bacaDataAwal();
    int pilih;
    do {
        tampilMenu();
        printf("  Masukkan pilihan : ");
        if (scanf("%d", &pilih) != 1) pilih = -1;
        bersihBuffer();
        switch (pilih) {
            case 1: tambahData(); break;
            case 2: tampilData(); break;
            case 3: editData(); break;
            case 4: hapusData(); break;
            case 5: cariData(); break;
            case 6: urutkanData(); break;
            case 7: laporanMenunggak(); break;
            case 8: simpanFile(); break;
            case 0: simpanFile(); printf("\n  Data disimpan. Program selesai.\n\n"); break;
            default: printf("\n  [!] Pilihan tidak valid.\n");
        }
        if (pilih != 0) { printf("\n  Tekan Enter untuk lanjut..."); getchar(); }
    } while (pilih != 0);
    return 0;
}

// Tampilkan header dan daftar menu utama //
void tampilMenu(void) {
    printf("\n"); garis(62);
    printf("   SISTEM MANAJEMEN DATA PELANGGAN ISP LOKAL\n"
           "   Telkom University Purwokerto - Kelompok 11\n");
    garis(62); printf("   Jumlah Pelanggan : %d\n", jumlah); garis(62);
    printf("  [1] Tambah Data Pelanggan\n  [2] Tampilkan Semua Pelanggan\n"
           "  [3] Edit Data Pelanggan\n  [4] Hapus Data Pelanggan\n"
           "  [5] Cari Pelanggan (by ID)\n  [6] Urutkan by Tunggakan\n"
           "  [7] Laporan Pelanggan Menunggak\n  [8] Simpan ke File\n  [0] Keluar\n");
    garis(62);
}

// Cetak daftar pilihan ISP beserta harganya //
void tampilMenuISP(void) {
    for (int i = 0; i < 8; i++)
        printf("    [%d] %-13s Rp %.0f\n", i+1, ISP_NAME[i], ISP_HARGA[i]);
}

// Kembalikan tarif bulanan sesuai nama ISP; default 200000 jika tidak cocok //
float hitungTagihan(const char *isp) {
    for (int i = 0; i < 8; i++) if (strcmp(isp, ISP_NAME[i]) == 0) return ISP_HARGA[i];
    return 200000.0f;
}

// Sequential search: kembalikan indeks array jika ID ditemukan, -1 jika tidak //
int cariIndex(const char *id) {
    for (int i = 0; i < jumlah; i++) if (strcmp(daftar[i].idPelanggan, id) == 0) return i;
    return -1;
}

// Cetak garis '=' sepanjang n karakter //
void garis(int n)      { for (int i=0;i<n;i++) printf("="); printf("\n"); }

// Buang sisa karakter di buffer stdin agar input berikutnya bersih //
void bersihBuffer(void){ int c; while((c=getchar())!='\n'&&c!=EOF); }

// Hapus spasi/tab/CR di awal dan akhir string s //
void trim(char *s) {
    if (!s) return;
    int start = 0;
    while (s[start]==' '||s[start]=='\t'||s[start]=='\r') start++;
    if (start > 0) memmove(s, s+start, strlen(s)-start+1);
    int end = (int)strlen(s)-1;
    while (end>=0&&(s[end]==' '||s[end]=='\t'||s[end]=='\r')) s[end--]='\0';
}

// bacaDataAwal — parsing Kelompok_11_Data.txt //
// Format tiap baris: NamaISP = NAMA1, NAMA2, ... //
// Setiap nama pelanggan diberi ID otomatis ISP-001, ISP-002, dst. //
// Status awal semua pelanggan: LUNAS (bulan tertunggak = 0) //
void bacaDataAwal(void) {
    FILE *fp = fopen(FILE_DATA_AWAL, "r");
    if (!fp) { printf("\n  [!] File '%s' tidak ditemukan.\n", FILE_DATA_AWAL); return; }
    char baris[512]; int idCounter = 1;
    while (fgets(baris, sizeof(baris), fp)) {
        baris[strcspn(baris, "\r\n")] = '\0';
        if (!strlen(baris)) continue;
        char *posEq = strchr(baris, '=');        // cari pemisah '=' //
        if (!posEq) continue;
        char namaISP[30];
        int panjang = (int)(posEq - baris);
        strncpy(namaISP, baris, panjang); namaISP[panjang] = '\0'; trim(namaISP);
        char *bagian = posEq + 1; trim(bagian);
        char *tok = strtok(bagian, ",");          // pecah nama per koma //
        while (tok && jumlah < MAX_PELANGGAN) {
            trim(tok);
            if (strlen(tok)) {
                Pelanggan p;
                snprintf(p.idPelanggan, sizeof(p.idPelanggan), "ISP-%03d", idCounter++);
                strncpy(p.namaPelanggan, tok, sizeof(p.namaPelanggan)-1); p.namaPelanggan[sizeof(p.namaPelanggan)-1]='\0';
                strncpy(p.jenisPaket,    namaISP, sizeof(p.jenisPaket)-1); p.jenisPaket[sizeof(p.jenisPaket)-1]='\0';
                p.tagihanBulanan=hitungTagihan(namaISP); p.bulanTertunggak=0; p.tunggakan=0;
                strcpy(p.statusPembayaran, "LUNAS");
                daftar[jumlah++] = p;
            }
            tok = strtok(NULL, ",");
        }
    }
    fclose(fp);
    printf("\n  [OK] %d pelanggan dimuat dari '%s'.\n", jumlah, FILE_DATA_AWAL);
}

// simpanFile — tulis seluruh array ke data_pelanggan.txt //
// Format: jumlah\n lalu tiap baris field dipisah '|' //
void simpanFile(void) {
    FILE *fp = fopen(FILE_SIMPAN, "w");
    if (!fp) { printf("\n  [!] Gagal menyimpan.\n"); return; }
    fprintf(fp, "%d\n", jumlah);
    for (int i = 0; i < jumlah; i++)
        fprintf(fp, "%s|%s|%s|%.0f|%d|%.0f|%s\n",
                daftar[i].idPelanggan, daftar[i].namaPelanggan, daftar[i].jenisPaket,
                daftar[i].tagihanBulanan, daftar[i].bulanTertunggak,
                daftar[i].tunggakan, daftar[i].statusPembayaran);
    fclose(fp);
    printf("\n  [OK] %d data disimpan ke '%s'.\n", jumlah, FILE_SIMPAN);
}

// bacaFileSimpan — muat data_pelanggan.txt hasil simpanan //
// Dipanggil pertama kali; jika file tidak ada, fungsi langsung keluar //
// Macro BACA memperpendek pengulangan strncpy untuk field string //
void bacaFileSimpan(void) {
    FILE *fp = fopen(FILE_SIMPAN, "r");
    if (!fp) return;
    int n = 0;
    if (fscanf(fp, "%d\n", &n) != 1) { fclose(fp); return; }
    jumlah = 0; char baris[256];
    for (int i = 0; i < n && jumlah < MAX_PELANGGAN; i++) {
        if (!fgets(baris, sizeof(baris), fp)) break;
        baris[strcspn(baris, "\r\n")] = '\0';
        Pelanggan p; char *t;
        #define BACA(field) t=strtok(i==0?baris:NULL,"|"); if(!t) continue; strncpy(p.field,t,sizeof(p.field)-1); p.field[sizeof(p.field)-1]='\0';
        BACA(idPelanggan) BACA(namaPelanggan) BACA(jenisPaket)
        #undef BACA
        t=strtok(NULL,"|"); if(!t) continue; p.tagihanBulanan =(float)atof(t);
        t=strtok(NULL,"|"); if(!t) continue; p.bulanTertunggak=atoi(t);
        t=strtok(NULL,"|"); if(!t) continue; p.tunggakan      =(float)atof(t);
        t=strtok(NULL,"|"); if(!t) continue; strncpy(p.statusPembayaran,t,sizeof(p.statusPembayaran)-1); p.statusPembayaran[sizeof(p.statusPembayaran)-1]='\0';
        daftar[jumlah++] = p;
    }
    fclose(fp);
    if (jumlah > 0) printf("\n  [OK] %d data dimuat dari '%s'.\n", jumlah, FILE_SIMPAN);
}

// Input data pelanggan baru; ID di-generate otomatis //
void tambahData(void) {
    if (jumlah >= MAX_PELANGGAN) { printf("\n  [!] Data penuh!\n"); return; }
    Pelanggan p;
    printf("\n"); garis(50); printf("  TAMBAH DATA PELANGGAN BARU\n"); garis(50);
    snprintf(p.idPelanggan, sizeof(p.idPelanggan), "ISP-%03d", jumlah+1);
    printf("  ID (auto)        : %s\n  Nama Pelanggan   : ", p.idPelanggan);
    fgets(p.namaPelanggan, sizeof(p.namaPelanggan), stdin);
    p.namaPelanggan[strcspn(p.namaPelanggan, "\n")] = '\0';
    printf("  Pilih ISP:\n"); tampilMenuISP(); printf("  Pilihan : ");
    int pil; scanf("%d", &pil); bersihBuffer();
    strncpy(p.jenisPaket, (pil>=1&&pil<=8)?ISP_NAME[pil-1]:"Citranet", sizeof(p.jenisPaket)-1);
    p.tagihanBulanan = hitungTagihan(p.jenisPaket);
    printf("  Bulan Tertunggak : "); scanf("%d", &p.bulanTertunggak); bersihBuffer();
    if (p.bulanTertunggak < 0) p.bulanTertunggak = 0;
    p.tunggakan = p.tagihanBulanan * p.bulanTertunggak;
    strcpy(p.statusPembayaran, p.bulanTertunggak==0 ? "LUNAS" : "MENUNGGAK");
    daftar[jumlah++] = p;
    printf("\n  [OK] '%s' (%s) | Tagihan: Rp %.0f | Tunggakan: Rp %.0f | %s\n",
           p.namaPelanggan, p.jenisPaket, p.tagihanBulanan, p.tunggakan, p.statusPembayaran);
}

// Cetak seluruh data pelanggan dalam format tabel //
void tampilData(void) {
    printf("\n"); garis(95); printf("  DAFTAR PELANGGAN ISP\n"); garis(95);
    if (jumlah == 0) { printf("  Belum ada data.\n"); return; }
    printf("  %-9s %-35s %-13s %-14s %-5s %-14s %-10s\n",
           "ID","Nama","ISP","Tagihan/Bln","Tgk","Tunggakan","Status");
    garis(95);
    for (int i = 0; i < jumlah; i++)
        printf("  %-9s %-35s %-13s Rp%-12.0f %-5d Rp%-12.0f %-10s\n",
               daftar[i].idPelanggan, daftar[i].namaPelanggan, daftar[i].jenisPaket,
               daftar[i].tagihanBulanan, daftar[i].bulanTertunggak,
               daftar[i].tunggakan, daftar[i].statusPembayaran);
    garis(95); printf("  Total: %d pelanggan\n", jumlah);
}

// Edit nama, ISP, atau bulan tertunggak berdasarkan ID pelanggan //
void editData(void) {
    char idInput[15];
    printf("\n"); garis(50); printf("  EDIT DATA PELANGGAN\n"); garis(50);
    printf("  ID yang diedit : "); fgets(idInput, sizeof(idInput), stdin);
    idInput[strcspn(idInput, "\n")] = '\0'; trim(idInput);
    int idx = cariIndex(idInput);
    if (idx == -1) { printf("\n  [!] ID '%s' tidak ditemukan.\n", idInput); return; }
    Pelanggan *p = &daftar[idx];
    printf("  >> %s | %s | %d bulan tunggak\n", p->namaPelanggan, p->jenisPaket, p->bulanTertunggak);
    printf("  [1] Nama  [2] ISP/Paket  [3] Bulan Tertunggak : ");
    int pil; scanf("%d", &pil); bersihBuffer();
    if (pil == 1) {
        printf("  Nama baru : "); fgets(p->namaPelanggan, sizeof(p->namaPelanggan), stdin);
        p->namaPelanggan[strcspn(p->namaPelanggan, "\n")] = '\0';
    } else if (pil == 2) {
        printf("  Pilih ISP:\n"); tampilMenuISP(); printf("  Pilihan : ");
        int pi; scanf("%d", &pi); bersihBuffer();
        if (pi>=1&&pi<=8) { strncpy(p->jenisPaket, ISP_NAME[pi-1], sizeof(p->jenisPaket)-1); p->tagihanBulanan=hitungTagihan(p->jenisPaket); }
        p->tunggakan = p->tagihanBulanan * p->bulanTertunggak;
    } else if (pil == 3) {
        printf("  Bulan tertunggak baru : "); scanf("%d", &p->bulanTertunggak); bersihBuffer();
        if (p->bulanTertunggak < 0) p->bulanTertunggak = 0;
        p->tunggakan = p->tagihanBulanan * p->bulanTertunggak;
    } else { printf("  [!] Tidak valid.\n"); return; }
    strcpy(p->statusPembayaran, p->bulanTertunggak==0 ? "LUNAS" : "MENUNGGAK"); // update status //
    printf("\n  [OK] Data diperbarui.\n");
}

// Hapus pelanggan dari array; data di kanannya digeser kiri //
void hapusData(void) {
    char idInput[15], konfirm;
    printf("\n"); garis(50); printf("  HAPUS DATA PELANGGAN\n"); garis(50);
    printf("  ID yang dihapus : "); fgets(idInput, sizeof(idInput), stdin);
    idInput[strcspn(idInput, "\n")] = '\0'; trim(idInput);
    int idx = cariIndex(idInput);
    if (idx == -1) { printf("\n  [!] ID '%s' tidak ditemukan.\n", idInput); return; }
    printf("  Hapus '%s' (%s)? (y/n) : ", daftar[idx].namaPelanggan, daftar[idx].jenisPaket);
    scanf("%c", &konfirm); bersihBuffer();
    if (konfirm=='y'||konfirm=='Y') {
        for (int i = idx; i < jumlah-1; i++) daftar[i] = daftar[i+1]; // geser kiri //
        jumlah--; printf("\n  [OK] Data dihapus.\n");
    } else printf("\n  [i] Dibatalkan.\n");
}

// Cari pelanggan by ID menggunakan Sequential Search, lalu tampil detailnya //
void cariData(void) {
    char idInput[15];
    printf("\n"); garis(50); printf("  PENCARIAN (Sequential Search)\n"); garis(50);
    printf("  ID Pelanggan : "); fgets(idInput, sizeof(idInput), stdin);
    idInput[strcspn(idInput, "\n")] = '\0'; trim(idInput);
    int idx = cariIndex(idInput);
    if (idx == -1) { printf("\n  [!] ID '%s' tidak ditemukan.\n", idInput); return; }
    printf("\n  [OK] Ditemukan di posisi ke-%d!\n", idx+1); garis(50);
    printf("  ID       : %s\n  Nama     : %s\n  ISP      : %s\n"
           "  Tagihan  : Rp %.0f\n  Tunggak  : %d bulan\n"
           "  Total    : Rp %.0f\n  Status   : %s\n",
           daftar[idx].idPelanggan, daftar[idx].namaPelanggan, daftar[idx].jenisPaket,
           daftar[idx].tagihanBulanan, daftar[idx].bulanTertunggak,
           daftar[idx].tunggakan, daftar[idx].statusPembayaran);
    garis(50);
}

// urutkanData — Selection Sort berdasarkan tunggakan //
// arah 1: descending (terbesar ke terkecil) //
// arah 2: ascending  (terkecil ke terbesar) //
// Setelah selesai langsung tampilkan hasil urutan //
void urutkanData(void) {
    if (jumlah == 0) { printf("\n  [!] Belum ada data.\n"); return; }
    printf("\n"); garis(50); printf("  PENGURUTAN (Selection Sort)\n"); garis(50);
    printf("  [1] Terbesar -> Terkecil\n  [2] Terkecil -> Terbesar\n  Pilihan : ");
    int arah; scanf("%d", &arah); bersihBuffer();
    for (int i = 0; i < jumlah-1; i++) {
        int pos = i;
        for (int j = i+1; j < jumlah; j++)
            if ((arah==1)?(daftar[j].tunggakan>daftar[pos].tunggakan):(daftar[j].tunggakan<daftar[pos].tunggakan)) pos=j;
        if (pos != i) { Pelanggan tmp=daftar[i]; daftar[i]=daftar[pos]; daftar[pos]=tmp; } // tukar posisi //
    }
    printf("\n  [OK] Diurutkan!\n"); tampilData();
}

// Filter dan tampilkan hanya pelanggan berstatus MENUNGGAK beserta total tagihannya //
void laporanMenunggak(void) {
    float total = 0; int cnt = 0;
    printf("\n"); garis(78); printf("  LAPORAN PELANGGAN MENUNGGAK\n"); garis(78);
    printf("  %-9s %-35s %-13s %-6s %-13s\n", "ID","Nama","ISP","Bln","Tunggakan");
    garis(78);
    for (int i = 0; i < jumlah; i++) {
        if (strcmp(daftar[i].statusPembayaran, "MENUNGGAK") == 0) {
            printf("  %-9s %-35s %-13s %-6d Rp %.0f\n",
                   daftar[i].idPelanggan, daftar[i].namaPelanggan,
                   daftar[i].jenisPaket, daftar[i].bulanTertunggak, daftar[i].tunggakan);
            total += daftar[i].tunggakan; cnt++;
        }
    }
    garis(78);
    if (cnt==0) printf("  Semua pelanggan LUNAS.\n");
    else printf("  Menunggak: %d orang | Total: Rp %.0f\n", cnt, total);
}