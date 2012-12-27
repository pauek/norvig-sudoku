#include <iostream>
#include <vector>
#include <assert.h>
using namespace std;

const vector<bool> TODOS(10, true);

class Posibles {
   vector<bool> _bits;
public:
   Posibles() : _bits(TODOS) {}
   bool activo(int i) const { return _bits[i]; }
   void elimina(int i) { _bits[i] = false; }
   string str() const {
      string s;
      for (int i = 1; i < 10; i++) {
         if (_bits[i]) s += '0' + i;
      }
      return s;
   }
   pair<int, int> valores() const {
      int c = 0, v;
      for (int i = 1; i <= 9; i++) {
         if (_bits[i]) v = i, c++;
      }
      return make_pair(c, v);
   }
   int num_activos() const { return valores().first; }
};

class Sudoku {
   vector<Posibles> _casillas;

   static vector< vector<int> > _grupo, _vecinos, _grupos_de;

   bool elimina(int k, int valor);
public:
   Sudoku(string s);
   bool asigna(int k, int valor);
   Posibles posibles(int k) const { return _casillas[k]; }
   int menos_posibilidades() const;
   void escribe(ostream& o) const;
   bool resuelto() const;

   static void inicializa();
};

bool Sudoku::resuelto() const {
   for (int i = 0; i < _casillas.size(); i++) {
      if (_casillas[i].num_activos() != 1) {
         return false;
      }
   }
   return true;
}

void Sudoku::escribe(ostream& o) const {
   int ancho = 1;
   for (int k = 0; k < _casillas.size(); k++) {
      pair<int, int> cv = _casillas[k].valores();
      ancho = max(ancho, 1 + cv.first);
   }
   string sep(ancho, '-');
   for (int i = 0; i < 9; i++) {
      if (i == 3 || i == 6) {
         string s(3*ancho, '-');
         o << s << "+-" << s << "+" << s << endl;
      }
      for (int j = 0; j < 9; j++) {
         if (j == 3 || j == 6) o << "| ";
         int k = i*9 + j;
         string s = _casillas[k].str();
         s.resize(ancho, ' ');
         o << s;
      }
      o << endl;
   }
}

vector< vector<int> > Sudoku::_grupo(27), Sudoku::_vecinos(81), Sudoku::_grupos_de(81);

void Sudoku::inicializa() {
   for (int i = 0; i < 9; i++) {
      for (int j = 0; j < 9; j++) {
         int k = i*9 + j;
         int x[3] = {i, 9 + j, 18 + (i/3)*3 + j/3};
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

bool Sudoku::asigna(int k, int valor) {
   for (int i = 1; i <= 9; i++) {
      if (i != valor) {
         if (!elimina(k, i)) return false;
      }
   }
   return true;
}

bool Sudoku::elimina(int k, int valor) {
   if (!_casillas[k].activo(valor)) {
      return true;
   }
   _casillas[k].elimina(valor);
   pair<int, int> cv = _casillas[k].valores();
   if (cv.first == 0) {
      return false;
   } else if (cv.first == 1) {
      int v = cv.second;
      for (int i = 0; i < _vecinos[k].size(); i++) {
         if (!elimina(_vecinos[k][i], v)) return false;
      }
   }
   for (int i = 0; i < _grupos_de[k].size(); i++) {
      int x = _grupos_de[k][i];
      vector<int> Ks;
      for (int j = 0; j < 9; j++) {
         int p = _grupo[x][j];
         if (_casillas[p].activo(valor)) {
            Ks.push_back(p);
         }
      }
      if (Ks.size() == 0) {
         return false;
      } else if (Ks.size() == 1) {
         if (!asigna(Ks[0], valor)) return false;
      }
   }
   return true;
}

int Sudoku::menos_posibilidades() const {
   int k = -1, min;
   for (int i = 0; i < _casillas.size(); i++) {
      int m = _casillas[i].num_activos();
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
  : _casillas(81) 
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
      if (!p.activo(i)) {
         continue;
      }
      Sudoku *S1 = new Sudoku(*S);
      if (!S1->asigna(k, i)) {
         delete S1;
         continue;
      }
      Sudoku *S2 = soluciona(S1);
      if (S2 != S1) {
         delete S1;
      } 
      if (S2 != NULL) {
         return S2;
      }
   }
   return NULL;
}

int main() {
   Sudoku::inicializa();
   string line;
   while (getline(cin, line)) {
      Sudoku* S = new Sudoku(line);
      S = soluciona(S);
      if (S != NULL) {
         S->escribe(cout);
      } else {
         cout << "No hay solución";
      }
      cout << endl;
   }
}
