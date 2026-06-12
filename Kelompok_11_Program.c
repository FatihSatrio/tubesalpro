#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// KONSTANTA //
#define MAX_PELANGGAN  200
#define FILE_DATA_AWAL "Kelompok_11_Data.txt"
#define FILE_SIMPAN    "data_pelanggan.txt"

// STRUKTUR DATA PELANGGAN //
typedef struct {
    char  idPelanggan[15], namaPelanggan[70];
    char  jenisPaket[30];       // Nama ISP: Citranet, Biznet, dsb //
    float tagihanBulanan, tunggakan;
    int   bulanTertunggak;
    char  statusPembayaran[15]; // LUNAS / MENUNGGAK //
} Pelanggan;

// VARIABEL GLOBAL //
Pelanggan daftar[MAX_PELANGGAN];
int jumlah = 0;

// PROTOTIPE FUNGSI //
void tampilMenu(void), tambahData(void), tampilData(void), editData(void);
void hapusData(void), cariData(void), urutkanData(void), laporanMenunggak(void);
void simpanFile(void), bacaFileSimpan(void), bacaDataAwal(void), tampilMenuISP(void);
float hitungTagihan(const char *isp);
void updateStatus(int i);
int  cariIndex(const char *id);
void garis(int n), bersihBuffer(void), trim(char *s);

static const char  *ISP_NAME[]  = {"Citranet","MyRepublik","Biznet","Indihome","Oxygen","XLHome","Firts Media","Mega Data"};
static const float  ISP_HARGA[] = {200000,250000,350000,300000,180000,220000,400000,150000};

// MAIN //
int main(void) {
    // Coba baca data tersimpan dulu, jika tidak ada, baca file awal //
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
            case 0: simpanFile();
            printf("\n  Data disimpan. Program selesai.\n\n"); break;
            default: printf("\n  [!] Pilihan tidak valid.\n");
        }
        if (pilih != 0) { printf("\n  Tekan Enter untuk lanjut..."); getchar(); }
    } while (pilih != 0);
    return 0;
}

// TAMPIL MENU //
void tampilMenu(void) {
    printf("\n"); garis(62);
    printf("   SISTEM MANAJEMEN DATA PELANGGAN ISP LOKAL\n");
    printf("   Telkom University Purwokerto - Kelompok 11\n");
    garis(62); printf("   Jumlah Pelanggan : %d\n", jumlah); garis(62);
    printf("  [1] Tambah Data Pelanggan\n  [2] Tampilkan Semua Pelanggan\n"
           "  [3] Edit Data Pelanggan\n  [4] Hapus Data Pelanggan\n"
           "  [5] Cari Pelanggan (by ID)\n  [6] Urutkan by Tunggakan\n"
           "  [7] Laporan Pelanggan Menunggak\n  [8] Simpan ke File\n  [0] Keluar\n");
    garis(62);
}

// BACA FILE DATA AWAL (Kelompok_11_Data.txt) //
// Format: NamaISP = NAMA1, NAMA2, NAMA3, ... //
void bacaDataAwal(void) {
    FILE *fp = fopen(FILE_DATA_AWAL, "r");
    if (!fp) { printf("\n  [!] File '%s' tidak ditemukan.\n", FILE_DATA_AWAL); return; }
    char baris[512]; int idCounter = 1;
    while (fgets(baris, sizeof(baris), fp)) {
        // Hapus \r\n //
        baris[strcspn(baris, "\r\n")] = '\0';
        if (!strlen(baris)) continue;
        // Pisahkan NamaISP dan daftar nama dengan '=' //
        char *posEq = strchr(baris, '=');
        if (!posEq) continue;
        // Ambil nama ISP //
        char namaISP[30]; int panjangISP = (int)(posEq - baris);
        strncpy(namaISP, baris, panjangISP); namaISP[panjangISP] = '\0'; trim(namaISP);
        // Ambil bagian nama pelanggan setelah '=' //
        char *bagianNama = posEq + 1; trim(bagianNama);
        // Pecah berdasarkan koma //
        char *token = strtok(bagianNama, ",");
        while (token && jumlah < MAX_PELANGGAN) {
            trim(token);
            if (strlen(token)) {
                Pelanggan p;
                snprintf(p.idPelanggan, sizeof(p.idPelanggan), "ISP-%03d", idCounter++);
                strncpy(p.namaPelanggan, token, sizeof(p.namaPelanggan)-1); p.namaPelanggan[sizeof(p.namaPelanggan)-1]='\0';
                strncpy(p.jenisPaket, namaISP, sizeof(p.jenisPaket)-1);    p.jenisPaket[sizeof(p.jenisPaket)-1]='\0';
                p.tagihanBulanan=hitungTagihan(namaISP); p.bulanTertunggak=0; p.tunggakan=0;
                strcpy(p.statusPembayaran, "LUNAS");
                daftar[jumlah++] = p;
            }
            token = strtok(NULL, ",");
        }
    }
    fclose(fp);
    printf("\n  [OK] %d pelanggan berhasil dimuat dari '%s'.\n", jumlah, FILE_DATA_AWAL);
}

// SIMPAN KE FILE PROGRAM (data_pelanggan.txt) //
void simpanFile(void) {
    FILE *fp = fopen(FILE_SIMPAN, "w");
    if (!fp) { printf("\n  [!] Gagal menyimpan ke '%s'.\n", FILE_SIMPAN); return; }
    fprintf(fp, "%d\n", jumlah);
    for (int i = 0; i < jumlah; i++)
        fprintf(fp, "%s|%s|%s|%.0f|%d|%.0f|%s\n", daftar[i].idPelanggan, daftar[i].namaPelanggan, daftar[i].jenisPaket, daftar[i].tagihanBulanan, daftar[i].bulanTertunggak, daftar[i].tunggakan, daftar[i].statusPembayaran);
    fclose(fp); printf("\n  [OK] %d data disimpan ke '%s'.\n", jumlah, FILE_SIMPAN);
}

// BACA FILE SIMPANAN (data_pelanggan.txt) //
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
        t=strtok(baris,"|"); if(!t) continue; strncpy(p.idPelanggan, t,sizeof(p.idPelanggan)-1); p.idPelanggan[sizeof(p.idPelanggan)-1]='\0';
        t=strtok(NULL, "|"); if(!t) continue; strncpy(p.namaPelanggan, t,sizeof(p.namaPelanggan)-1); p.namaPelanggan[sizeof(p.namaPelanggan)-1]='\0';
        t=strtok(NULL, "|"); if(!t) continue; strncpy(p.jenisPaket, t,sizeof(p.jenisPaket)-1); p.jenisPaket[sizeof(p.jenisPaket)-1]='\0';
        t=strtok(NULL, "|"); if(!t) continue; p.tagihanBulanan = (float)atof(t);
        t=strtok(NULL, "|"); if(!t) continue; p.bulanTertunggak = atoi(t);
        t=strtok(NULL, "|"); if(!t) continue; p.tunggakan = (float)atof(t);
        t=strtok(NULL, "|"); if(!t) continue; strncpy(p.statusPembayaran, t,sizeof(p.statusPembayaran)-1); p.statusPembayaran[sizeof(p.statusPembayaran)-1]='\0';
        daftar[jumlah++] = p;
    }
    fclose(fp);
    if (jumlah > 0) printf("\n  [OK] %d data dimuat dari '%s'.\n", jumlah, FILE_SIMPAN);
}

void tampilMenuISP(void) {
    for (int i = 0; i < 8; i++) printf("    [%d] %-13s Rp %.0f\n", i+1, ISP_NAME[i], ISP_HARGA[i]);
}

// TAMBAH DATA //
void tambahData(void) {
    if (jumlah >= MAX_PELANGGAN) { printf("\n  [!] Data penuh!\n"); return; }
    Pelanggan p;
    printf("\n"); garis(50); printf("  TAMBAH DATA PELANGGAN BARU\n"); garis(50);
    snprintf(p.idPelanggan, sizeof(p.idPelanggan), "ISP-%03d", jumlah + 1);
    printf("  ID Pelanggan (auto) : %s\n  Nama Pelanggan      : ", p.idPelanggan);
    fgets(p.namaPelanggan, sizeof(p.namaPelanggan), stdin);
    p.namaPelanggan[strcspn(p.namaPelanggan, "\n")] = '\0';
    printf("  Pilih ISP / Paket:\n"); tampilMenuISP(); printf("  Pilihan : ");
    int pil; scanf("%d", &pil); bersihBuffer();
    if (pil >= 1 && pil <= 8) strncpy(p.jenisPaket, ISP_NAME[pil-1], sizeof(p.jenisPaket)-1);
    else { printf("  [!] Tidak valid, default Citranet.\n"); strcpy(p.jenisPaket, "Citranet"); }
    p.tagihanBulanan = hitungTagihan(p.jenisPaket);
    printf("  Bulan Tertunggak    : "); scanf("%d", &p.bulanTertunggak); bersihBuffer();
    if (p.bulanTertunggak < 0) p.bulanTertunggak = 0;
    p.tunggakan = p.tagihanBulanan * p.bulanTertunggak;
    strcpy(p.statusPembayaran, p.bulanTertunggak == 0 ? "LUNAS" : "MENUNGGAK");
    daftar[jumlah++] = p;
    printf("\n  [OK] Pelanggan '%s' (%s) ditambahkan.\n       Tagihan: Rp %.0f | Tunggakan: Rp %.0f | Status: %s\n", p.namaPelanggan, p.jenisPaket, p.tagihanBulanan, p.tunggakan, p.statusPembayaran);
}

// TAMPILKAN SEMUA DATA //
void tampilData(void) {
    printf("\n"); garis(92); printf("  DAFTAR PELANGGAN ISP\n"); garis(92);
    if (jumlah == 0) { printf("  Belum ada data.\n"); return; }
    printf("  %-9s %-35s %-13s %-13s %-5s %-13s %-10s\n", "ID","Nama Pelanggan","ISP/Paket","Tagihan/Bln","Tgk","Tunggakan","Status");
    garis(92);
    for (int i = 0; i < jumlah; i++)
        printf("  %-9s %-35s %-13s Rp%-11.0f %-5d Rp%-11.0f %-10s\n", daftar[i].idPelanggan, daftar[i].namaPelanggan, daftar[i].jenisPaket, daftar[i].tagihanBulanan, daftar[i].bulanTertunggak, daftar[i].tunggakan, daftar[i].statusPembayaran);
    garis(92); printf("  Total: %d pelanggan\n", jumlah);
}

// EDIT DATA //
void editData(void) {
    char idInput[15];
    printf("\n"); garis(50); printf("  EDIT DATA PELANGGAN\n"); garis(50);
    printf("  ID Pelanggan yang diedit : ");
    fgets(idInput, sizeof(idInput), stdin);
    idInput[strcspn(idInput, "\n")] = '\0'; trim(idInput);
    int idx = cariIndex(idInput);
    if (idx == -1) { printf("\n  [!] ID '%s' tidak ditemukan.\n", idInput); return; }
    Pelanggan *p = &daftar[idx];
    printf("  Data ditemukan: %s | %s | Tunggakan: %d bulan\n", p->namaPelanggan, p->jenisPaket, p->bulanTertunggak);
    printf("  Edit: [1] Nama  [2] ISP/Paket  [3] Bulan Tertunggak : ");
    int pil; scanf("%d", &pil); bersihBuffer();
    if (pil == 1) {
        printf("  Nama baru : ");
        fgets(p->namaPelanggan, sizeof(p->namaPelanggan), stdin);
        p->namaPelanggan[strcspn(p->namaPelanggan, "\n")] = '\0';
    } else if (pil == 2) {
        printf("  Pilih ISP baru:\n"); tampilMenuISP(); printf("  Pilihan : ");
        int pi; scanf("%d", &pi); bersihBuffer();
        if (pi >= 1 && pi <= 8) { strncpy(p->jenisPaket, ISP_NAME[pi-1], sizeof(p->jenisPaket)-1); p->tagihanBulanan = hitungTagihan(p->jenisPaket); }
        p->tunggakan = p->tagihanBulanan * p->bulanTertunggak;
    } else if (pil == 3) {
        printf("  Bulan tertunggak baru : "); scanf("%d", &p->bulanTertunggak); bersihBuffer();
        if (p->bulanTertunggak < 0) p->bulanTertunggak = 0;
        p->tunggakan = p->tagihanBulanan * p->bulanTertunggak;
    } else { printf("  [!] Pilihan tidak valid.\n"); return; }
    // Update status //
    strcpy(p->statusPembayaran, p->bulanTertunggak == 0 ? "LUNAS" : "MENUNGGAK");
    printf("\n  [OK] Data berhasil diperbarui.\n");
}

// HAPUS DATA //
void hapusData(void) {
    char idInput[15], konfirm;
    printf("\n"); garis(50); printf("  HAPUS DATA PELANGGAN\n"); garis(50);
    printf("  ID Pelanggan yang dihapus : ");
    fgets(idInput, sizeof(idInput), stdin);
    idInput[strcspn(idInput, "\n")] = '\0'; trim(idInput);
    int idx = cariIndex(idInput);
    if (idx == -1) { printf("\n  [!] ID '%s' tidak ditemukan.\n", idInput); return; }
    printf("  Hapus '%s' (%s)? (y/n) : ", daftar[idx].namaPelanggan, daftar[idx].jenisPaket);
    scanf("%c", &konfirm); bersihBuffer();
    if (konfirm == 'y' || konfirm == 'Y') {
        for (int i = idx; i < jumlah - 1; i++) daftar[i] = daftar[i+1];
        jumlah--; printf("\n  [OK] Data berhasil dihapus.\n");
    } else printf("\n  [i] Dibatalkan.\n");
}

// CARI DATA — Sequential Search by ID //
void cariData(void) {
    char idInput[15];
    printf("\n"); garis(50); printf("  PENCARIAN PELANGGAN (Sequential Search)\n"); garis(50);
    printf("  Masukkan ID Pelanggan : ");
    fgets(idInput, sizeof(idInput), stdin);
    idInput[strcspn(idInput, "\n")] = '\0'; trim(idInput);
    printf("  Mencari ID '%s'...\n", idInput);
    // Sequential Search //
    int idx = cariIndex(idInput);
    if (idx == -1) { printf("\n  [!] Pelanggan dengan ID '%s' tidak ditemukan.\n", idInput); return; }
    printf("\n  [OK] Ditemukan di posisi ke-%d!\n", idx + 1); garis(50);
    printf("  ID            : %s\n  Nama          : %s\n  ISP / Paket   : %s\n"
           "  Tagihan/Bulan : Rp %.0f\n  Tgk (bulan)   : %d\n"
           "  Tunggakan     : Rp %.0f\n  Status        : %s\n",
           daftar[idx].idPelanggan, daftar[idx].namaPelanggan, daftar[idx].jenisPaket,
           daftar[idx].tagihanBulanan, daftar[idx].bulanTertunggak,
           daftar[idx].tunggakan, daftar[idx].statusPembayaran);
    garis(50);
}

// URUTKAN DATA — Selection Sort by Tunggakan //
void urutkanData(void) {
    if (jumlah == 0) { printf("\n  [!] Belum ada data.\n"); return; }
    printf("\n"); garis(50); printf("  PENGURUTAN DATA (Selection Sort)\n"); garis(50);
    printf("  [1] Tunggakan Terbesar -> Terkecil\n  [2] Tunggakan Terkecil -> Terbesar\n  Pilihan : ");
    int arah; scanf("%d", &arah); bersihBuffer();
    /* Selection Sort */
    for (int i = 0; i < jumlah - 1; i++) {
        int pos = i;
        for (int j = i+1; j < jumlah; j++)
            if ((arah==1) ? (daftar[j].tunggakan > daftar[pos].tunggakan)
                          : (daftar[j].tunggakan < daftar[pos].tunggakan)) pos = j;
        if (pos != i) { Pelanggan tmp = daftar[i]; daftar[i] = daftar[pos]; daftar[pos] = tmp; }
    }
    printf("\n  [OK] Data berhasil diurutkan!\n"); tampilData();
}

// LAPORAN PELANGGAN MENUNGGAK //
void laporanMenunggak(void) {
    float total = 0; int cnt = 0;
    printf("\n"); garis(75); printf("  LAPORAN PELANGGAN YANG BELUM MEMBAYAR\n"); garis(75);
    printf("  %-9s %-35s %-13s %-6s %-13s\n", "ID","Nama","ISP","Bln","Tunggakan");
    garis(75);
    for (int i = 0; i < jumlah; i++) {
        if (strcmp(daftar[i].statusPembayaran, "MENUNGGAK") == 0) {
            printf("  %-9s %-35s %-13s %-6d Rp %.0f\n", daftar[i].idPelanggan, daftar[i].namaPelanggan, daftar[i].jenisPaket, daftar[i].bulanTertunggak, daftar[i].tunggakan);
            total += daftar[i].tunggakan; cnt++;
        }
    }
    garis(75);
    if (cnt == 0) printf("  Semua pelanggan sudah LUNAS.\n");
    else printf("  Pelanggan menunggak  : %d orang\n  Total tagihan macet  : Rp %.0f\n", cnt, total);
}

// HITUNG TAGIHAN BERDASARKAN NAMA ISP //
float hitungTagihan(const char *isp) {
    for (int i = 0; i < 8; i++) if (strcmp(isp, ISP_NAME[i]) == 0) return ISP_HARGA[i];
    return 200000.0f; // default //
}

// UPDATE STATUS PEMBAYARAN (helper) //
void updateStatus(int i) {
    strcpy(daftar[i].statusPembayaran, daftar[i].bulanTertunggak == 0 ? "LUNAS" : "MENUNGGAK");
}

// CARI INDEX BERDASARKAN ID (Sequential Search, return -1 jika tidak ada) //
int cariIndex(const char *id) {
    for (int i = 0; i < jumlah; i++) if (strcmp(daftar[i].idPelanggan, id) == 0) return i;
    return -1;
}

// UTILITAS: GARIS PEMISAH //
void garis(int n) { for (int i=0;i<n;i++) printf("="); printf("\n"); }

// UTILITAS: BERSIHKAN INPUT BUFFER //
void bersihBuffer(void) { int c; while((c=getchar())!='\n'&&c!=EOF); }

// UTILITAS: TRIM SPASI DAN \r DI AWAL & AKHIR STRING //
void trim(char *s) {
    if (!s) return;
    // Kiri //
    int start = 0;
    while (s[start]==' '||s[start]=='\t'||s[start]=='\r') start++;
    if (start > 0) memmove(s, s+start, strlen(s)-start+1);
    // Kanan //
    int end = (int)strlen(s)-1;
    while (end>=0&&(s[end]==' '||s[end]=='\t'||s[end]=='\r')) s[end--]='\0';
}