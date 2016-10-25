/*
 * mrhyde.cpp
 * Copyright (C) 2012 - Giovan Battista Rolandi
 * www.glgprograms.it - giomba@live.it
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 * Scopo del programma
 * Il programma nasconde messaggi di testo in immagini bitmap con profondità
 * di colore di 24bpp, ed è in grado di decodificarli successivamente.
 */

#include <fstream>    /* flusso su file */
#include <iostream>   /* flusso standard */
#include <cmath>      /* funzioni matematiche */
using namespace std;

/* Prototipi delle funzioni */
/* Inserisce nella matrice binario[] il carattere passato come argomento in binario */
void byte(unsigned char);
/* Restituisce il carattere ASCII contenuto nella matrice binario[] */
char reverse(void);
/* Legge unsigned int in little-endian dal file passato come argomento e ne restituisce il valore */
unsigned int read32(fstream &); /* Intero a 32bit */
unsigned int read16(fstream &); /* Intero a 16bit */
/* Scrivi sul file indicato, a partire dal byte indicato, un messaggio di cui si conosce la lunghezza */
bool scrivi(fstream&, unsigned int, unsigned int);
/* Legge dal file indicato, a partire dal byte indicato, un messaggio di cui si conosce la lunghezza */
bool leggi(fstream&, unsigned int, unsigned int);
/* Chiede all'utente se si vuole continuare in seguito ad un errore fatale e in caso negativo restituisce true */
bool stop(void);

/* Dichiarazione delle variabili globali */
bool binario[8];  /* Contiene 8 bit per le operazioni di codifica/decodifica */

int main()
{
  char nome[256];
  clog << "Nome dell'immagine? "; cin.getline(nome, 255);

  fstream file(nome, ios::in | ios::out | ios::binary);
  if (! file) { cerr << "Impossibile aprire il file '" << nome << "'" << endl; return 1; }

  /* Controllo tipo di file */
  char imgtype[2];
  file.seekg(0, ios::beg); file.read(imgtype, 2);
  if (imgtype[0] == 'B' && imgtype[1] == 'M')
    { clog << "File bitmap valido." << endl; }
  else
    { cerr << "File bitmap non valido! " << endl; if (stop()) return 1; }

  /* Lettura dimensione file */
  unsigned int filesize;
  file.seekg(2, ios::beg); filesize = read32(file);
  clog << "Dimensioni dichiarate: " << filesize << " bytes" << endl;

  /* Controllo profondità di colore */
  unsigned int depth;
  file.seekg(28, ios::beg); depth = read16(file);
  if (depth == 24)
    { clog << "Profondità di colore valida: " << depth << "bpp" << endl; }
  else
    { cerr << "Profondità di colore non valida: " << depth << "bpp" << endl; if (stop()) return 1; }

  /* Lettura spazio disponibile */
  unsigned int imgsize, msgsize;
  file.seekg(34, ios::beg); imgsize = read32(file);
  msgsize = imgsize / 8;
  clog << "Bytes utili: " << imgsize << " (circa " << msgsize << " caratteri)" << endl;

  /* Lettura start-bit */
  unsigned int startbit;
  file.seekg(10, ios::beg); startbit = read32(file);
  clog << "Startbit dichiarato: " << startbit << endl;
  clog << endl;

  char scelta[2];
  clog << "Scegliere un'operazione: (S)crivi (L)eggi (E)sci" << endl;
  cin.getline(scelta, 2);

  switch (scelta[0])
  {
    case 'e': case 'E': break;
    case 's': case 'S': scrivi(file, startbit, msgsize); break;
    case 'l': case 'L': leggi(file, startbit, msgsize); break;
    default: cerr << "Operazione sconosciuta!"; return 1; break;
  }

  clog << endl << "Programma terminato!" << endl;
  return 0;
}

void byte(unsigned char numero)
{
  short int v;
  for (v = 0; v <= 7; v++)
   { binario[v] = false; }

  for (v = 7; v >= 0; v--)
  {
    binario[v] = (bool) (numero % 2);
    numero = (int) numero / 2;
  }
}

unsigned int read32(fstream &flusso)
{
  char aa, bb, cc, dd; unsigned char a, b, c, d;
  flusso.get(aa); flusso.get(bb); flusso.get(cc); flusso.get(dd);
  a = aa; b = bb; c = cc; d = dd;

  unsigned int numero = 0;
  numero = a + b * pow(2, 8) + c * pow(2, 16) + d * pow(2, 24);

  return numero;
}

unsigned int read16(fstream &flusso)
{
  char aa, bb; unsigned char a, b;
  flusso.get(aa); flusso.get(bb);
  a = aa; b = bb;

  unsigned int numero = 0;
  numero = a + b * pow(2, 8);

  return numero;
}

bool scrivi(fstream &file, unsigned int startbit, unsigned int msgsize)
{
  clog << "Inserire messaggio:" << endl;
  char messaggio[msgsize]; cin.getline(messaggio, msgsize);

  unsigned char carattere; unsigned int v = 0, n = 0;
  char ssample; char sample;
  unsigned long int pos = startbit;
  file.seekg(startbit, ios::beg); file.seekp(startbit, ios::beg);
  while (1)
  {
    carattere = messaggio[v]; v++;
    byte(carattere);
    for (n = 0; n <= 7; n++)
    {
      file.seekg(pos, ios::beg);
      file.get(ssample); sample = ssample;
      if ((sample % 2) == 0)
      { if (binario[n] != 0) { sample++; } }
      else
      { if (binario[n] != 1) { sample--; } }
      file.seekp(pos, ios::beg); file.put(sample);
      pos++;
    }
    if (carattere == '\0') break;
  }
  clog << "Messaggio salvato!" << endl;

  return true;

}

bool leggi(fstream &file, unsigned int startbit, unsigned int msgsize)
{
  clog << "Decodifica:" << endl;

  unsigned int v, n; char carattere, c;
  file.seekg(startbit, ios::beg);

  for (v = 0; v < msgsize; v++)
    {
      for (n = 0; n <= 7; n++)
      {
        file.get(carattere); binario[n] = carattere % 2;
      }
      c = reverse();
      if (c == 0) break;
      cout << c;
    }

  return true;
}

char reverse(void)
{
  short int v; char ascii = 0;
  for (v = 0; v <= 7; v++)
  {
    ascii = ascii + binario[v] * pow(2, 7 - v);
  }
  return ascii;
}

bool stop(void)
{
  char scelta[2];
  cerr << "Errore fatale. Continuare comunque? (s/N)" << endl;
  cin.getline(scelta, 2);
  if (scelta[0] == 's' || scelta[0] == 'S') return false;
    else return true;
}

