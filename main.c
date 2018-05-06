#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void get_avg_val(char data[][6][100], float *res, int n, int kolom);
void export(int n, int kolom, char source[][6][100], FILE *file, int *res);
void parse_data(FILE *file, char res[][6][100]);
void get_num_rows_cols(FILE *file, int *ndata, int *kolom);

int main(int argc, char const *argv[])
{
	int r;
	float phi;
	char filename[100];
	
	printf("Isikan r : ");
    scanf("%d", &r);
	
	printf("Isikan phi : ");
    scanf("%f", &phi);

    printf("Isikan nama file source (.csv): ");
    scanf("%s", &filename);

	FILE *file;

	// Load File
	file = fopen(filename, "r");

	// Inisiasi kolom dan banyak data
	int kolom = 1;
	int ndata = 0;

	// insisiasi pointer untuk menampung alamat memori variabel kolom dan ndata
	int *pt_kolom = &kolom;
	int *pt_ndata = &ndata;

	// menghitung jumlah baris dan kolom
	get_num_rows_cols(file, pt_ndata, pt_kolom);

    // inisiasi array dengan menerima panjang karakter (diasumsikan) adalah 100
    char data[ndata][kolom][100];

    // inisiasi untuk menampung pengganti missing value
    float m_val[] = {0,0,0,0};

    // Buka file dengan flag "r" yang artinya read atau hanya membaca
    file = fopen(filename, "r");

    // memasukkan data ke array
    parse_data(file, data);

    // mencari missing values dan mencari rata-ratanya
    get_avg_val(data, m_val, ndata, kolom);

    // inisiasi array untuk menampung semua nilai pada kolom (dijadikan tipe data float)
    float extracted[ndata][kolom];

    // loop pada seluruh baris data
    for (int i = 0; i < ndata; ++i)
    {
    	// loop pada seluruh kolom di setiap baris data kecuali 2 kolom terakhir (karena berisi string)
    	for (int j = 0; j < kolom-2; ++j)
    	{	
    		if (data[i][j][0] == '?') // jika data berisi karakter tanda tanya
    		{
    			extracted[i][j] = m_val[j]/(ndata-1); // isikan pengganti nilai missing values, ndata dikurangi satu karena baris pertama adalah header tabel
    		} 
    		else 
    		{
    			extracted[i][j] = atof(data[i][j]);	// isikan nilai dari data string
    		}
    	}
    }

    // inisiasi array untuk menampung distance tiap data
	float dist[ndata][ndata];
	
	// Loop pada semua baris data
	for (int i = 1; i < ndata; ++i) {
		// Loop pada semua baris data
		for (int j = 1; j < ndata; ++j) {
			float nilai = 0.0;  // variabel untuk menampung hasil absolute pengurangan tiap kolom
			for (int k = 0; k < kolom-2; ++k) {
				// jika data yang dikurangkan berada pada baris yang tidak sama (data yang berbeda)
				if (i != j) {		
					// tambahkan dengan hasil dari nilai sebelumnya
					nilai += (abs(extracted[i][k] - extracted[j][k]));
				}
			}

			// jika data adalah berbeda maka masukkan hasil penghitungan distance nya kedalam array
			if (i != j) {
				dist[i][j] = nilai;
			} else {
				// jika tidak maka isikan dengan -1 (anggap null)
				dist[i][j] = -1;
			}
		}
	}

	// print header tabel yang didapat dari file
	for (int i = 0; i < 1; ++i)
	{
		for (int j = 0; j < kolom; ++j)
		{
			if (j == kolom-1) {
				printf("\t");
			}

			printf("%s\t\t|", data[i][j]);
		}
		printf("\toutlier\t|\tnilai\t\n");
		printf("========================================================================\n");
	}

	int is_outlier[ndata];

	// Loop dari 1 (karena indeks 0 adalah header kolom) hingga banyaknya baris
	for (int i = 1; i < ndata; ++i) {
		// inisiasi counter 0 untuk menghitung banyak data yang lebih besar dari r
		int counter = 0;

		// Loop pada kolom data
		for (int j = 1; j < ndata; ++j) {

			// untuk menampilkan data lain
			if (j < 5) {
				printf("%f\t| ", extracted[i][j-1]);
			} else if (j >= 5 && j <= 6) {
				printf("\t%s\t| ", data[i][j-1]);
			}
			

			// jika data lebih kecil dari r dan lebih besar atau sama dengan 0 maka,
			if (dist[i][j] <= r && dist[i][j] >= 0) {
				// increment counter
				counter++;
			}
		}

		// jika counter dibagi banyaknya data lebih kecil dari phi maka,
		if (counter / (float)ndata <= phi) {
			// print jika data tersebut outlier
			printf("outlier\t| %f\n", counter/(float)ndata);
			is_outlier[i] = 1;
		} else {
			// selain itu data bukan outlier
			printf("bukan outlier\t| %f\n", counter/(float)ndata);
			is_outlier[i] = 0;
		}
	}

	fclose(file);

	export(ndata, kolom, data, file, is_outlier);

	return 0;
}

void get_num_rows_cols(FILE *file, int *ndata, int *kolom)
{

	// variabel untuk menampung karakter pada file
	int cstream;

	// Loop pada seluruh karakter pada file
	while ((cstream = fgetc(file)) != EOF) {

		// jika karakter adalah titik koma maka jumlah kolom + 1, cukup di baris pertama saja
		if (((char)cstream == ';' || (char)cstream == ',') && *ndata == 0) {
			++*kolom;
		}

		// jika adalah \n atau enter maka jumlah ndata + 1
        if ((char)cstream == '\n') {
        	++*ndata;
        }
    }
    // tutup file
	fclose(file);
}

void get_avg_val(char data[][6][100], float *res, int n, int kolom) {

	// inisiasi array untuk meanmpung hasil mengubah dari string ke float
	float extracted[n][kolom-2][100];
	
	// loop pada seluruh baris data
    for (int i = 0; i < n; ++i)
    {
    	// loop pada seluruh kolom di setiap baris data kecuali 2 kolom terakhir (karena berisi string)
    	for (int j = 0; j < kolom-2; ++j)
    	{	
    		// Menambahkan semua data tiap kolom
    		res[j] += atof(data[i][j]);
    	}
    }	
}

void export(int n, int kolom, char source[][6][100], FILE *file, int *res) {
	
	char filename_export[100];

	printf("Isikan nama file hasil eksport (.csv) : ");
	scanf("%s", &filename_export);
	 
	file = fopen(filename_export, "w+");

	for (int i = 0; i < kolom; ++i) {
		fprintf(file, "%s;", source[0][i]);
	}
	
	fprintf(file, "outlier\n");

	for (int i = 1; i < n; ++i)
	{
		for (int j = 0; j < kolom+1; ++j)
		{
			if (j == 6) {
				
				if (res[i] == 1) {
					fprintf(file, "%s;", "true");	
				} else {
					fprintf(file, "%s;", "false");	
				}

			} else {
				fprintf(file, "%s;", source[i][j]);
			}
		}
		fprintf(file, "\n");
	}
	fprintf(file, "\0");
	fclose(file);

	puts("Data exported!");
}

void parse_data(FILE *file, char res[][6][100])
{
	// variabel untuk menampung tiap karakter pada file
    int c;

	// inisiasi row dan col untuk memindahkan teks pada csv ke array
    int row = 0;
    int col = 0;

    // counter untuk memasukkan karakter
    int counter = 0;

    // variabel untuk menampung karakter terakhir yang diisikan pada array
	char prev;

    // Loop pada seluruh karakter pada file
    while ((c = fgetc(file)) != EOF) {    	
		
		// jika bertemu karakter titik koma atau koma maka isikan karakter \0 sebagai penutup suatu string
		if (c == ';' || c == ',') {

			// untuk cek data kosong pada karakter sebelumnya
			if (prev == ';' || prev == ',' || counter == 0)
			{
				res[row][col][counter++] = '?'; // isikan dengan simbol tanda tanya
			}

			res[row][col][counter] = '\0';

			// ganti kolom berikutnya dibaris yang sama
			col++;
			// set counter menjadi 0 agar mengambil karakter yang diperlukan saja (tidak mulai dari awal)
			counter = 0;

		} else if (c == '\n') { // jika bertemu karakter \n atau enter maka isikan karakter \0 sebagai penutup suatu string
			res[row][col][counter] = '\0';

			// ganti baris data selanjutnya
			row++;
			// mulai kolom awal
			col = 0;
			// set counter 0
			counter = 0;
		} else {
			// masukkan tiap karakter ke dalam array
			res[row][col][counter++] = c;
		}
		
		// memasukkan karakter yang ditemukan sebelumnya ke dalam array
		prev = c;
		
    }
    // Tutup filestream
    fclose(file);
}
