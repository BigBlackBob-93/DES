// DES.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <iterator> 

using namespace std;

FILE* f;
#define N 64
#define M 32


void FirstCas(int *Mass, int*L, int*R)
{
    int k, FirstCas[N];
    f = fopen("FirstCastlingTable.txt", "r");
    for (int i = 0; i < N; i++)
    {
        fscanf(f, "%d", &k);
        FirstCas[i] = Mass[k - 1];
    }
    fclose(f);
    for (int i = 0; i < N; i++)
    {
        if (i < M) L[i] = FirstCas[i];
        else R[i-M] = FirstCas[i];
    }
}

void SecondCas(int* Mass, int* L, int* R)
{
    for (int i = 0; i < N; i++)
    {
        if (i < M) Mass[i] = L[i];
        else Mass[i] = R[i - 32];
    }

    int k, SecondCas[N];
    f = fopen("SecondCastlingTable.txt", "r");
    for (int i = 0; i < N; i++)
    {
        fscanf(f, "%d", &k);
        SecondCas[i] = Mass[k - 1];
    }
    fclose(f);
    for (int i = 0; i < N; i++) Mass[i] = SecondCas[i];
}

//Циклический сдвиг
void CyclicShift(int* KeyD, int *KeyC, int count)
{
    int n;
    int dopD, dopC, dop2D, dop2C;
    
    switch (count)
    {
    case 0: case 1: case 8: case 15: n = 1; break;
    default: n = 2; break;
    }

    if (n == 1)
    {
        dopD = KeyD[0]; dopC = KeyC[0];

        for (int i = 1; i < 28; i++)
        {
            KeyD[i - 1] = KeyD[i];
            KeyC[i - 1] = KeyC[i];
        }
        KeyD[27] = dopD; KeyC[27] = dopC;
    }
    else
    {
        dopD = KeyD[0]; dopC = KeyC[0];
        dop2D = KeyD[1]; dop2C = KeyC[1];

        for (int i = 2; i < 28; i++)
        {
            KeyD[i - 2] = KeyD[i];
            KeyC[i - 2] = KeyC[i];
        }
        KeyD[26] = dopD; KeyC[26] = dopC;
        KeyD[27] = dop2D; KeyC[27] = dop2C;
    }
}

//Генерация ключей
void KeysGeneration(int** KeysAll, int *Key)
{
    int count = 0, k, FirstKeyCas[56], SecondKeyCas[48];
    int* KeyD = new int[28]; int* KeyC = new int[28];

    //начальная перестановка
    f = fopen("KeyFirstCastlingTable.txt", "r");
    for (int i = 0; i < 56; i++)
    {
        fscanf(f, "%d", &k);
        FirstKeyCas[i] = Key[k - 1];
    }
    fclose(f);
    for (int i = 0; i < 56; i++)
    {
        if (i < 28) KeyD[i] = FirstKeyCas[i];
        else KeyC[i - 28] = FirstKeyCas[i];
    }

    for (int j = 0; j < 16; j++)
    {
        CyclicShift(KeyD, KeyC, count);

        for (int i = 0; i < 56; i++)
        {
            if (i < 28) FirstKeyCas[i] = KeyD[i];
            else FirstKeyCas[i] = KeyC[i - 28];
        }

        //конечная перестановка
        f = fopen("KeySecondCastlingCP.txt", "r");
        for (int i = 0; i < 48; i++)
        {
            fscanf(f, "%d", &k);
            SecondKeyCas[i] = FirstKeyCas[k - 1];
        }
        fclose(f);

        //долгожданный ключ
        for (int i = 0; i < 48; i++) KeysAll[j][i] = SecondKeyCas[i];

        count++;
    }

    delete[] KeyD;
    delete[] KeyC;
}

void Sblocks(int string, int column, int count, int *h)
{
    int a, c;
    int S[4][16];

    switch (count)
    {
    case 1:
        f = fopen("S1.txt", "r");
        break;
    case 2:
        f = fopen("S2.txt", "r");
        break;
    case 3:
        f = fopen("S3.txt", "r");
        break;
    case 4:
        f = fopen("S4.txt", "r");
        break;
    case 5:
        f = fopen("S5.txt", "r");
        break;
    case 6:
        f = fopen("S6.txt", "r");
        break;
    case 7:
        f = fopen("S7.txt", "r");
        break;
    case 8:
        f = fopen("S8.txt", "r");
        break;
    }

    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 16; j++)
            fscanf(f, "%d", &S[i][j]);
    fclose(f);

    a = S[string][column];
    for (int i = 3; i >= 0; i--)
    {
        c = a % 2;
        a /= 2;
        h[i] = c;
    }
}

void DESfunction(int** KeysAll, int* R2, int count)
{
    int Re[48], Rdop[32], k, d, g, z, countS = 1;
    int string, column, h[4];

    //Перестановка, E-расширение до 48 бит
    f = fopen("ExpansionE.txt", "r");
    for (int i = 0; i < 48; i++)
    {
        fscanf(f, "%d", &k);
        Re[i] = R2[k - 1];
        Re[i] ^= KeysAll[count][i]; //XOR
    }
    fclose(f);

    d = 0;
        for (k = 0; k < 48; k+=6)
        {
            string = (Re[k] * 2) + Re[k + 5];
            column = (Re[k + 1] * 8) + (Re[k + 2] * 4) + (Re[k + 3] * 2) + Re[k + 4];

            Sblocks(string, column, countS, h);
            
            for (g = 0, z = d; g < 4; g++, z++)
            {
                Rdop[z] = h[g];
            }
            countS++;
            d += 4;
        }
        //P-перестановка
        f = fopen("CastlingP.txt", "r");
        for (int i = 0; i < 32; i++)
        {
            fscanf(f, "%d", &k);
            R2[i] = Rdop[k - 1];
        }
        fclose(f);
}

//Основная функция шифрования
void EncryptionFullDESfunction(int** KeysAll, int* L, int* R)
{
    int* R2 = new int[32];
    int Rdop[32], count;

    for (int j = 0, count = 0; j < 16; j++, count++)
    {
        for (int i = 0; i < 32; i++)
        {
            R2[i] = R[i];
            Rdop[i] = R[i];
        }

        DESfunction(KeysAll, R2, count);

        if (j!=15)
        { 
            for (int i = 0; i < 32; i++) R[i] = R2[i] ^ L[i];
            for (int i = 0; i < 32; i++) L[i] = Rdop[i];
        }
        else for (int i = 0; i < 32; i++) L[i] ^= R2[i];
    }
    delete[] R2;
}

void DecryptionFullDESfunction(int** KeysAll, int* L, int* R)
{
    int* R2 = new int[32];
    int Rdop[32], count = 15;

    for (int j = 15; j >=0; j--)
    {
        for (int i = 0; i < 32; i++)
        {
            R2[i] = R[i];
            Rdop[i] = R[i];
        }

        DESfunction(KeysAll, R2, count);

        if (j != 0)
        {
            for (int i = 0; i < 32; i++) R[i] = R2[i] ^ L[i];
            for (int i = 0; i < 32; i++) L[i] = Rdop[i];
        }
        else for (int i = 0; i < 32; i++) L[i] ^= R2[i];

        count--;
    }
    delete[] R2;
}


int main()
{   
    int* Mass = new int[64]; int* L = new int[M]; int* R = new int[M];
    int* Key = new int[64]; 
    int** KeysAll;
    
    cout << "Original sequence: ";
    f = fopen("Test.txt", "r");
    for (int i = 0; i < N; i++)
    {
        fscanf(f, "%d", &Mass[i]);
        cout << Mass[i];
    }
    cout <<endl<< "Key: ";
    for (int i = 0; i < N; i++)
    {
        fscanf(f, "%d", &Key[i]);
        cout << Key[i];
    }
    fclose(f);

   FirstCas(Mass, L, R);
   
   KeysAll = new int* [16];
   for (int i = 0; i < 16; i++) KeysAll[i] = new int[48];
   for (int i = 0; i < 16; i++)
       for (int j = 0; j < 48; j++)
           KeysAll[i][j] = 0;

   KeysGeneration(KeysAll, Key);
   EncryptionFullDESfunction(KeysAll, L, R);
   SecondCas(Mass, L, R);
   cout << endl << "Encrypted sequence: ";
   for (int i = 0; i < N; i++) cout << Mass[i];

 /////////////РАСШИФРОВАНИЕ////////////////////////////////////

   FirstCas(Mass, L, R);
   DecryptionFullDESfunction(KeysAll, L, R);
   SecondCas(Mass, L, R);
   cout << endl << "Decrypted sequence: ";
   for (int i = 0; i < N; i++) cout << Mass[i];


    delete[] Mass;
    delete[] L;
    delete[] R;
    delete[] Key;
    for (int i = 0; i < 16; i++) delete[] KeysAll[i];
    delete[] KeysAll; 
}

