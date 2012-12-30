#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

class Posibles {
   vector<bool> _b;
public:
   Posibles() : _b(9, true) {}
   bool   activo(int i)  const { return _b[i-1]; }
   int    num_activos()  const { return count(_b.begin(), _b.end(), true); }
   void   elimina(int i)       { _b[i-1] = false; }
   int    val()          const {
      auto it = find(_b.begin(), _b.end(), true);
      return (it != _b.end() ? 1 + (it - _b.begin()) : -1);
   }
   string str(int ancho) const;
};

string Posibles::str(int ancho) const {
   string s(ancho, ' ');
   int k = 0;
   for (int i = 1; i <= 9; i++) {
      if (activo(i)) s[k++] = '0' + i;
   }
   return s;
} 

class Sudoku {
   vector<Posibles> _celdas;
   static vector< vector<int> > _grupo, _vecinos, _grupos_de;

   bool     elimina(int k, int val);
public:
   Sudoku(string s);
   static void inicializa();

   Posibles posibles(int k) const { return _celdas[k]; }
   bool     resuelto() const;
   bool     asigna(int k, int val);
   int      menos_posibilidades() const;
   void     escribe(ostream& o) const;
};

bool Sudoku::resuelto() const {
   for (int k = 0; k < _celdas.size(); k++) {
      if (_celdas[k].num_activos() != 1) {
         return false;
      }
   }
   return true;
}

void Sudoku::escribe(ostream& o) const {
   int ancho = 1;
   for (int k = 0; k < _celdas.size(); k++) {
      ancho = max(ancho, 1 + _celdas[k].num_activos());
   }
   const string sep(3 * ancho, '-');
   for (int i = 0; i < 9; i++) {
      if (i == 3 || i == 6) {
         o << sep << "+-" << sep << "+" << sep << endl;
      }
      for (int j = 0; j < 9; j++) {
         if (j == 3 || j == 6) o << "| ";
         o << _celdas[i*9 + j].str(ancho);
      }
      o << endl;
   }
}

vector< vector<int> > Sudoku::_grupo(27), Sudoku::_vecinos(81), Sudoku::_grupos_de(81);

void Sudoku::inicializa() {
   for (int i = 0; i < 9; i++) {
      for (int j = 0; j < 9; j++) {
         const int k = i*9 + j;
         const int x[3] = {i, 9 + j, 18 + (i/3)*3 + j/3};
         for (int g = 0; g < 3; g++) {
            _grupo[x[g]].push_back(k);
            _grupos_de[k].push_back(x[g]);
         }
      }
   }
   for (int k = 0; k < _vecinos.size(); k++) {
      for (int x = 0; x < _grupos_de[k].size(); x++) {
         for (int j = 0; j < 9; j++) {
            int k2 = _grupo[_grupos_de[k][x]][j];
            if (k2 != k) _vecinos[k].push_back(k2);
         }
      }
   }
}

bool Sudoku::asigna(int k, int val) {
   for (int i = 1; i <= 9; i++) {
      if (i != val) {
         if (!elimina(k, i)) return false;
      }
   }
   return true;
}

bool Sudoku::elimina(int k, int val) {
   if (!_celdas[k].activo(val)) {
      return true;
   }
   _celdas[k].elimina(val);
   const int N = _celdas[k].num_activos();
   if (N == 0) {
      return false;
   } else if (N == 1) {
      const int v = _celdas[k].val();
      for (int i = 0; i < _vecinos[k].size(); i++) {
         if (!elimina(_vecinos[k][i], v)) return false;
      }
   }
   for (int i = 0; i < _grupos_de[k].size(); i++) {
      const int x = _grupos_de[k][i];
      int n = 0, ks;
      for (int j = 0; j < 9; j++) {
         const int p = _grupo[x][j];
         if (_celdas[p].activo(val)) {
            n++, ks = p;
         }
      }
      if (n == 0) {
         return false;
      } else if (n == 1) {
         if (!asigna(ks, val)) {
            return false;
         }
      }
   }
   return true;
}

int Sudoku::menos_posibilidades() const {
   int k = -1, min;
   for (int i = 0; i < _celdas.size(); i++) {
      int m = _celdas[i].num_activos();
      if (m > 1) {
         if (k == -1 || m < min) {
            min = m;
            k = i;
         }
      }
   }
   return k;
}

Sudoku::Sudoku(string s) 
  : _celdas(81) 
{
   int k = 0;
   for (int i = 0; i < s.size(); i++) {
      if (s[i] >= '1' && s[i] <= '9') {
         if (!asigna(k, s[i] - '0')) {
            cerr << "error" << endl;
            return;
         }
         k++;
      } else if (s[i] == '0' || s[i] == '.') {
         k++;
      }
   }
}

Sudoku* soluciona(Sudoku *S) {
   if (S == NULL || S->resuelto()) {
      return S;
   }
   int k = S->menos_posibilidades();
   Posibles p = S->posibles(k);
   for (int i = 1; i <= 9; i++) {
      if (p.activo(i)) {
         Sudoku *S1 = new Sudoku(*S);
         if (S1->asigna(k, i)) {
            Sudoku *S2 = soluciona(S1);
            if (S2 != NULL) {
               if (S2 != S1) delete S1;
               return S2;
            }
         }
         delete S1;
      }
   }
   return NULL;
}

int main() {
   Sudoku::inicializa();
   string linea;
   while (getline(cin, linea)) {
      Sudoku* S = soluciona(new Sudoku(linea));
      if (S != NULL) {
         S->escribe(cout);
      } else {
         cout << "No hay solución";
      }
      delete S;
      cout << endl;
   }
}
